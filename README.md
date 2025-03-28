Explicação:

Inclusão de Headers:

stdio.h: Para entrada e saída padrão (printf).

pico/stdlib.h: Para funções específicas do SDK do Pico (inicialização de GPIO, etc.).

FreeRTOS.h: Header principal do FreeRTOS.

task.h: Para gerenciamento de tarefas.

queue.h: Para filas (comunicação entre tarefas).

semphr.h: Para semáforos (sincronização entre tarefas).

Definições de Pinos GPIO:

#define BUTTON_PIN 5 e #define LED_PIN 12: Define os pinos que serão usados para o botão e o LED.

Variáveis Globais:

button_state e led_state: Armazenam o estado do botão e do LED. Note que button_state não é mais usada diretamente, pois o estado do botão é passado pela fila.

Handles para Fila e Semáforo:

button_queue: Um handle (ponteiro) para a fila que será usada para enviar o estado do botão da task_button_read para a task_button_process.

led_semaphore: Um handle para o semáforo binário que será usado para sinalizar a task_led_control quando o botão for pressionado.

task_button_read (Tarefa de Leitura do Botão):

Inicializa o pino do botão como entrada e habilita o pull-up interno.

Entra em um loop infinito:

Lê o estado do botão usando gpio_get(BUTTON_PIN). A leitura é invertida com ! por causa do pull-up (o botão pressionado resulta em LOW).

Envia o estado do botão para a fila button_queue usando xQueueSend(). portMAX_DELAY faz com que a tarefa espere indefinidamente se a fila estiver cheia.

Espera 100 ms usando vTaskDelay().

task_button_process (Tarefa de Processamento do Botão):

Entra em um loop infinito:

Espera receber o estado do botão da fila button_queue usando xQueueReceive(). portMAX_DELAY faz com que a tarefa espere indefinidamente se não houver nada na fila.

Se receber o estado do botão (xQueueReceive() retorna pdTRUE):

Verifica se o botão foi pressionado (received_button_state).

Se o botão foi pressionado, libera o semáforo led_semaphore usando xSemaphoreGive(). Isso sinaliza a task_led_control para acender o LED.

task_led_control (Tarefa de Controle do LED):

Inicializa o pino do LED como saída.

Entra em um loop infinito:

Espera o semáforo led_semaphore ser liberado usando xSemaphoreTake(). portMAX_DELAY faz com que a tarefa espere indefinidamente até que o semáforo seja liberado.

Quando o semáforo é liberado (o que significa que o botão foi pressionado), acende o LED, imprime uma mensagem, espera 1 segundo, apaga o LED, imprime uma mensagem, e espera outro segundo.

main (Função Principal):

Inicializa a entrada e saída padrão (stdio_init_all()).

Cria a fila button_queue usando xQueueCreate(). O primeiro argumento é o tamanho da fila (quantos itens ela pode armazenar) e o segundo é o tamanho de cada item (em bytes).

Cria o semáforo led_semaphore usando xSemaphoreCreateBinary(). Semáforos binários são usados para sinalização (disponível ou não disponível).

Cria as três tarefas usando xTaskCreate():

O primeiro argumento é a função que implementa a tarefa.

O segundo argumento é um nome para a tarefa (para fins de depuração).

O terceiro argumento é o tamanho da stack alocada para a tarefa (use configMINIMAL_STACK_SIZE para começar).

O quarto argumento são parâmetros passados para a tarefa (NULL neste caso).

O quinto argumento é a prioridade da tarefa (quanto maior o número, maior a prioridade). Prioridades mais altas são executadas antes de prioridades mais baixas.

O sexto argumento é um ponteiro para um TaskHandle_t (NULL neste caso).

Inicia o scheduler do FreeRTOS usando vTaskStartScheduler(). Esta função nunca retorna.
