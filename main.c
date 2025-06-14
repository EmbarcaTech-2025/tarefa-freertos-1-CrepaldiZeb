#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Includes do FreeRTOS
#include "FreeRTOS.h"
#include "task.h"

// --- Definições de Pinos ---
#define RED_LED_PIN     13
#define GREEN_LED_PIN   11
#define BLUE_LED_PIN    12
#define BUZZER_PIN      21
#define BUTTON_A_PIN    5
#define BUTTON_B_PIN    6

// --- Handles das Tarefas ---
// Usados para suspender e resumir as tarefas
TaskHandle_t led_task_handle = NULL;
TaskHandle_t buzzer_task_handle = NULL;

/**
 * @brief Tarefa para controlar o LED RGB.
 * Alterna ciclicamente entre Vermelho, Verde e Azul a cada 500ms.
 */
void led_task(void *pvParameters) {
    const uint led_pins[] = {RED_LED_PIN, GREEN_LED_PIN, BLUE_LED_PIN};
    int current_led = 0;

    for (;;) { // Loop infinito da tarefa
        // Apaga todos os LEDs antes de acender o próximo
        gpio_put(RED_LED_PIN, 0);
        gpio_put(GREEN_LED_PIN, 0);
        gpio_put(BLUE_LED_PIN, 0);

        // Acende o LED da cor atual
        gpio_put(led_pins[current_led], 1);

        // Avança para o próximo LED no ciclo
        current_led = (current_led + 1) % 3;

        // Aguarda 500ms antes da próxima troca de cor
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief Tarefa para controlar o Buzzer.
 * Emite um beep curto (100ms) a cada 1 segundo.
 */
void buzzer_task(void *pvParameters) {
    for (;;) { // Loop infinito da tarefa
        gpio_put(BUZZER_PIN, 1); // Liga o buzzer
        vTaskDelay(pdMS_TO_TICKS(100)); // Duração do beep
        gpio_put(BUZZER_PIN, 0); // Desliga o buzzer
        vTaskDelay(pdMS_TO_TICKS(900)); // Silêncio pelo resto do segundo
    }
}

/**
 * @brief Tarefa para monitorar os botões de controle.
 * Botão A: Suspende/Resume a tarefa do LED.
 * Botão B: Suspende/Resume a tarefa do Buzzer.
 */
void button_task(void *pvParameters) {
    bool led_suspended = false;
    bool buzzer_suspended = false;

    for (;;) { // Loop infinito da tarefa
        // --- Lógica do Botão A (Controle do LED) ---
        // Se o botão A for pressionado e a tarefa do LED não estiver suspensa...
        if (!gpio_get(BUTTON_A_PIN) && !led_suspended) {
            vTaskSuspend(led_task_handle);
            led_suspended = true;
            // Garante que o LED apague ao suspender
            gpio_put(RED_LED_PIN, 0);
            gpio_put(GREEN_LED_PIN, 0);
            gpio_put(BLUE_LED_PIN, 0);
        } 
        // Se o botão A for solto e a tarefa do LED estiver suspensa...
        else if (gpio_get(BUTTON_A_PIN) && led_suspended) {
            vTaskResume(led_task_handle);
            led_suspended = false;
        }

        // --- Lógica do Botão B (Controle do Buzzer) ---
        // Se o botão B for pressionado e a tarefa do buzzer não estiver suspensa...
        if (!gpio_get(BUTTON_B_PIN) && !buzzer_suspended) {
            vTaskSuspend(buzzer_task_handle);
            buzzer_suspended = true;
            // Garante que o buzzer pare ao suspender
            gpio_put(BUZZER_PIN, 0);
        } 
        // Se o botão B for solto e a tarefa do buzzer estiver suspensa...
        else if (gpio_get(BUTTON_B_PIN) && buzzer_suspended) {
            vTaskResume(buzzer_task_handle);
            buzzer_suspended = false;
        }

        // Aguarda 100ms antes de verificar os botões novamente (polling)
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}


int main() {
    stdio_init_all();

    // Inicializa todos os pinos GPIO
    gpio_init(RED_LED_PIN);   gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_init(GREEN_LED_PIN); gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_init(BLUE_LED_PIN);  gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
    gpio_init(BUZZER_PIN);    gpio_set_dir(BUZZER_PIN, GPIO_OUT);
    
    gpio_init(BUTTON_A_PIN);  gpio_set_dir(BUTTON_A_PIN, GPIO_IN); gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);  gpio_set_dir(BUTTON_B_PIN, GPIO_IN); gpio_pull_up(BUTTON_B_PIN);

    // --- Criação das Tarefas ---
    // Parâmetros: função, nome, tamanho da stack, parâmetros, prioridade, handle
    xTaskCreate(led_task, "LED_Task", 256, NULL, 1, &led_task_handle);
    xTaskCreate(buzzer_task, "Buzzer_Task", 256, NULL, 1, &buzzer_task_handle);
    xTaskCreate(button_task, "Button_Task", 256, NULL, 1, NULL); // Não precisa de handle

    // Inicia o escalonador do FreeRTOS
    // O código abaixo desta linha não será executado
    vTaskStartScheduler();

    while (true) {
        // O programa nunca deve chegar aqui
    }

    return 0;
}