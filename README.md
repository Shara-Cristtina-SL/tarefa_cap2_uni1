Explicação: 
Inclusão de Headers:

stdio.h: Para entrada e saída padrão (especialmente a função printf).

pico/stdlib.h: Contém funções específicas do SDK do Raspberry Pi Pico, como inicialização de pinos GPIO (gpio_init, gpio_set_dir, gpio_put, gpio_get) e delays (sleep_ms).

FreeRTOS.h: Header principal do FreeRTOS, o sistema operacional em tempo real que estamos usando.

task.h: Declara funções para criar e gerenciar tarefas no FreeRTOS (xTaskCreate, vTaskDelay).

queue.h: Declara funções para criar e usar filas, que são mecanismos de comunicação entre tarefas (xQueueCreate, xQueueSend, xQueueReceive).

semphr.h: Declara funções para criar e usar semáforos, que são mecanismos de sincronização entre tarefas (xSemaphoreCreateBinary, xSemaphoreGive, xSemaphoreTake).

Definições dos Pinos GPIO:

#define BUTTON_PIN 5: Define uma constante chamada BUTTON_PIN com o valor 5. Isso representa o número do pino GPIO ao qual o botão está conectado. Usar uma constante torna o código mais legível e fácil de modificar.

#define LED_PIN 12: Define uma constante chamada LED_PIN com o valor 12. Isso representa o número do pino GPIO ao qual o LED está conectado.

Variáveis Globais:

bool button_state = false;: Declara uma variável global do tipo booleano chamada button_state e a inicializa com o valor false. Esta variável armazena o estado atual do botão (pressionado ou não pressionado). Ela é global para que todas as tarefas possam acessá-la.

bool led_state = false;: Declara uma variável global do tipo booleano chamada led_state e a inicializa com o valor false. Esta variável armazena o estado atual do LED (aceso ou apagado). Ela é global para que todas as tarefas possam acessá-la.

Handles para Fila e Semáforo:

QueueHandle_t button_queue;: Declara uma variável do tipo QueueHandle_t chamada button_queue. Este handle (ponteiro) será usado para referenciar a fila que as tarefas usarão para se comunicar. As filas são usadas para enviar dados de uma tarefa para outra de forma assíncrona.

SemaphoreHandle_t led_semaphore;: Declara uma variável do tipo SemaphoreHandle_t chamada led_semaphore. Este handle (ponteiro) será usado para referenciar o semáforo que as tarefas usarão para sincronizar suas ações. Semáforos são usados para controlar o acesso a recursos compartilhados e para sinalizar eventos entre tarefas.

task_button_read (Tarefa de Leitura do Botão):

void task_button_read(void *pvParameters): Define a função que implementa a tarefa de leitura do botão. O argumento void *pvParameters é um ponteiro genérico que pode ser usado para passar parâmetros para a tarefa, mas não estamos usando nenhum neste caso.

gpio_init(BUTTON_PIN);: Inicializa o pino GPIO especificado pela constante BUTTON_PIN. Isso configura o pino para ser usado como um pino de entrada/saída.

gpio_set_dir(BUTTON_PIN, GPIO_IN);: Define a direção do pino GPIO como entrada (GPIO_IN). Isso significa que o pino será usado para ler sinais externos, como o estado do botão.

gpio_pull_up(BUTTON_PIN);: Habilita o resistor pull-up interno no pino GPIO. Isso garante que o pino tenha um estado definido quando o botão não está pressionado (HIGH).

while (true): Inicia um loop infinito, fazendo com que a tarefa seja executada repetidamente.

button_state = !gpio_get(BUTTON_PIN);: Lê o estado digital do pino GPIO usando a função gpio_get() e armazena o resultado na variável global button_state. O operador ! inverte o valor lido porque estamos usando um resistor pull-up (o botão pressionado resulta em um sinal LOW).

xQueueSend(button_queue, &button_state, portMAX_DELAY);: Envia o estado do botão (armazenado em button_state) para a fila button_queue. O argumento portMAX_DELAY faz com que a tarefa espere indefinidamente (bloqueie) se a fila estiver cheia. Isso garante que o estado do botão seja eventualmente enviado para a fila, mesmo que a tarefa de processamento esteja ocupada.

vTaskDelay(pdMS_TO_TICKS(100));: Suspende a execução da tarefa por 100 milissegundos. Isso evita que a tarefa consuma toda a CPU e permite que outras tarefas sejam executadas. A função pdMS_TO_TICKS() converte milissegundos em ticks do FreeRTOS.

task_button_process (Tarefa de Processamento do Botão):

void task_button_process(void *pvParameters): Define a função que implementa a tarefa de processamento do botão.

bool received_button_state;: Declara uma variável local do tipo booleano chamada received_button_state. Esta variável armazenará o estado do botão recebido da fila.

while (true): Inicia um loop infinito.

if (xQueueReceive(button_queue, &received_button_state, portMAX_DELAY) == pdTRUE): Espera receber o estado do botão da fila button_queue. O argumento portMAX_DELAY faz com que a tarefa espere indefinidamente se a fila estiver vazia. Se a função xQueueReceive() retornar pdTRUE, significa que um item foi recebido com sucesso.

xSemaphoreGive(led_semaphore);: Libera o semáforo led_semaphore, sinalizando para a tarefa task_led_control que o estado do botão foi atualizado.

task_led_control (Tarefa de Controle do LED):

void task_led_control(void *pvParameters): Define a função que implementa a tarefa de controle do LED.

gpio_init(LED_PIN);: Inicializa o pino GPIO especificado pela constante LED_PIN.

gpio_set_dir(LED_PIN, GPIO_OUT);: Define a direção do pino GPIO como saída (GPIO_OUT). Isso significa que o pino será usado para controlar um dispositivo externo, como o LED.

while (true): Inicia um loop infinito.

if( xSemaphoreTake(led_semaphore, portMAX_DELAY) == pdTRUE ): Espera o semáforo led_semaphore ser liberado. O argumento portMAX_DELAY faz com que a tarefa espere indefinidamente se o semáforo não estiver disponível. Se o xSemaphoreTake retorna pdTRUE significa que foi possível "pegar" o semáforo (ou seja, ele foi liberado por outra tarefa).
* if (button_state): Checa a variável global button_state. Se button_state for verdadeiro, significa que o botão está pressionado.

led_state = true;: Define o estado do LED como true (aceso).

gpio_put(LED_PIN, led_state);: Define o estado do pino GPIO do LED como HIGH, acendendo o LED.

printf("LED ligado!\n");: Imprime uma mensagem no console.
* else: Se button_state for falso, significa que o botão não está pressionado.

led_state = false;: Define o estado do LED como false (apagado).

gpio_put(LED_PIN, led_state);: Define o estado do pino GPIO do LED como LOW, apagando o LED.

printf("LED desligado!\n");: Imprime uma mensagem no console.

vTaskDelay(pdMS_TO_TICKS(10));: Suspende a tarefa por 10 milissegundos.

main (Função Principal):

int main(): Define a função principal do programa.

stdio_init_all();: Inicializa a entrada e saída padrão (UART) para que possamos usar a função printf().

button_queue = xQueueCreate(5, sizeof(bool));: Cria a fila button_queue com capacidade para 5 itens, cada um do tamanho de um booleano (1 byte). Se a criação da fila falhar, imprime uma mensagem de erro e entra em um loop infinito.

led_semaphore = xSemaphoreCreateBinary();: Cria o semáforo binário led_semaphore. Semáforos binários são usados para sinalizar a ocorrência de um evento (neste caso, a atualização do estado do botão). Se a criação do semáforo falhar, imprime uma mensagem de erro e entra em um loop infinito.

xTaskCreate(task_button_read, "ButtonRead", configMINIMAL_STACK_SIZE, NULL, 1, NULL);: Cria a tarefa task_button_read com o nome "ButtonRead", tamanho de stack mínimo, sem parâmetros, prioridade 1 e sem handle.

xTaskCreate(task_button_process, "ButtonProcess", configMINIMAL_STACK_SIZE, NULL, 2, NULL);: Cria a tarefa task_button_process.

xTaskCreate(task_led_control, "LEDControl", configMINIMAL_STACK_SIZE, NULL, 3, NULL);: Cria a tarefa task_led_control.

vTaskStartScheduler();: Inicia o scheduler do FreeRTOS, que é responsável por gerenciar a execução das tarefas.

while (true) { printf("Erro! Scheduler finalizado!\n"); }: Este loop infinito nunca deve ser alcançado. Se o scheduler do FreeRTOS terminar, isso indica um erro grave.

Fluxo Geral:

A task_button_read lê o estado do botão periodicamente (a cada 100ms) e envia para a fila button_queue.

A task_button_process espera por um novo estado do botão na fila. Ao receber, libera o semáforo led_semaphore.

A task_led_control espera o semáforo led_semaphore ser liberado. Quando isso acontece, ela checa o estado global do botão (button_state) e acende ou apaga o LED de acordo.

O sistema repete continuamente.


------------------------------------          ------------------------------------
Observação:
Quando Usar o Quê?

Use um Semáforo Binário:

Quando você precisa sinalizar um evento (ex: "dados disponíveis", "operação completa"). 

Quando você precisa sincronizar tarefas, mas não precisa garantir que a mesma tarefa que tomou o recurso o libere.

Use um Mutex:

Quando você precisa proteger um recurso compartilhado e garantir que apenas uma tarefa o acesse por vez.

Quando é importante evitar a inversão de prioridade.

------------------------------------          ------------------------------------

Video no YouTube do projeto em funcionamento: 
https://youtu.be/E4AqXW6z5XE?si=Y3BsEmeDbnzNNuDI
