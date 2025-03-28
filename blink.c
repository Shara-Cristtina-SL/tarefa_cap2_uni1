#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// Definições dos pinos GPIO
#define BUTTON_PIN 5
#define LED_PIN 12

// Variáveis globais para o estado do botão e do LED
bool button_state = false;
bool led_state = false;

// Handles para a fila e o semáforo
QueueHandle_t button_queue;
SemaphoreHandle_t led_semaphore;

// Tarefa 1: Leitura do Botão
void task_button_read(void *pvParameters) {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    while (true) {
        // Lê o estado do botão
        button_state = !gpio_get(BUTTON_PIN); // Inverte a leitura por causa do pull-up
        // Envia o estado do botão para a fila
        xQueueSend(button_queue, &button_state, portMAX_DELAY);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Tarefa 2: Processamento do Botão
void task_button_process(void *pvParameters) {
    bool received_button_state;

    while (true) {
        // Espera receber o estado do botão da fila
        if (xQueueReceive(button_queue, &received_button_state, portMAX_DELAY) == pdTRUE) {
            // Se o botão foi pressionado, libera o semáforo para acionar a Tarefa 3
            // Se não, também libera o semáforo, mas com um valor diferente para indicar que o LED deve apagar
            if (received_button_state) {
                xSemaphoreGive(led_semaphore); // Sinaliza para acender
            } else {
                //Envia um sinal com valor negativo para desligar o led
                 long apagado = -1;
                 xSemaphoreGive(led_semaphore);
            }
        }
    }
}

// Tarefa 3: Controle do LED
void task_led_control(void *pvParameters) {
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    while (true) {
        // Espera o semáforo ser liberado
        if( xSemaphoreTake(led_semaphore, portMAX_DELAY) == pdTRUE ) {

             if (button_state) {  // Se o botão está pressionado acende o led
                led_state = true;
                gpio_put(LED_PIN, led_state);
                printf("LED ligado!\n");
             } else {
                led_state = false;
                gpio_put(LED_PIN, led_state);
                printf("LED desligado!\n");
             }

             vTaskDelay(pdMS_TO_TICKS(10)); // Delay pequeno para não travar a tarefa
        }

    }
}

int main() {
    stdio_init_all();

    // Cria a fila para comunicação entre as tarefas
    button_queue = xQueueCreate(5, sizeof(bool)); // Tamanho da fila: 5, tipo de dado: bool
    if (button_queue == NULL) {
        printf("Erro ao criar a fila do botão!\n");
        while(1);
    }

    // Cria o semáforo para sincronização entre as tarefas
    led_semaphore = xSemaphoreCreateBinary();
    if (led_semaphore == NULL) {
        printf("Erro ao criar o semáforo do LED!\n");
        while(1);
    }

    // Cria as tarefas
    xTaskCreate(task_button_read, "ButtonRead", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(task_button_process, "ButtonProcess", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(task_led_control, "LEDControl", configMINIMAL_STACK_SIZE, NULL, 3, NULL);

    // Inicia o scheduler do FreeRTOS
    vTaskStartScheduler();

    // O código nunca deve chegar aqui
    while (true) {
        printf("Erro! Scheduler finalizado!\n");
    }

    return 0;
}