// Global LED state
volatile int led_state = 0;

// ISR
void IRAM_ATTR button_ISR()
{
    led_state = !led_state;

    // Toggle LED
}

// Main program
void app_main()
{
    // Step 1: Configure GPIO 2 as output

    // Step 2: Configure GPIO 4 as input

    // Step 3: Enable pull-up resistor

    // Step 4: Select falling-edge detection

    // Step 5: Enable interrupt

    // Step 6: Register ISR
$
    // Step 7: Run main application
}

.................................................................................................
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"

#include "soc/soc.h"
#include "soc/gpio_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/interrupts.h"

// Pin definitions
#define LED_PIN       2
#define BUTTON_PIN    4

#define LED_MASK      (1U << LED_PIN)
#define BUTTON_MASK   (1U << BUTTON_PIN)

// GPIO pin register address
#define BUTTON_PIN_REG (GPIO_PIN0_REG + (BUTTON_PIN * 4U))

// Interrupt handle
static intr_handle_t gpio_intr_handle = NULL;

// LED state
static volatile uint32_t led_state = 0;


// STEP 7: Interrupt Service Routine
void IRAM_ATTR button_ISR(void *arg)
{
    // Check whether GPIO 4 caused the interrupt
    uint32_t status = REG_READ(GPIO_STATUS_REG);

    if (status & BUTTON_MASK)
    {
        // Clear interrupt status
        REG_WRITE(GPIO_STATUS_W1TC_REG, BUTTON_MASK);

        // Toggle LED state
        led_state ^= 1U;

        // Update LED output using hardware registers
        if (led_state)
        {
            REG_WRITE(GPIO_OUT_W1TS_REG, LED_MASK);
        }
        else
        {
            REG_WRITE(GPIO_OUT_W1TC_REG, LED_MASK);
        }
    }
}


// MAIN APPLICATION
void app_main(void)
{
    uint32_t reg;


    // STEP 1: Configure GPIO 2 as OUTPUT

    reg = REG_READ(IO_MUX_GPIO2_REG);

    // Select GPIO function
    reg = (reg & ~(0x7U << 12)) | (1U << 12);

    REG_WRITE(IO_MUX_GPIO2_REG, reg);

    // Enable GPIO 2 output
    REG_WRITE(GPIO_ENABLE_W1TS_REG, LED_MASK);

    // Initially LED OFF
    REG_WRITE(GPIO_OUT_W1TC_REG, LED_MASK);


    // STEP 2: Configure GPIO 4 as INPUT

    reg = REG_READ(IO_MUX_GPIO4_REG);

    // Select GPIO function
    reg = (reg & ~(0x7U << 12)) | (1U << 12);

    // Enable input
    reg |= (1U << 9);

    REG_WRITE(IO_MUX_GPIO4_REG, reg);

    // Make sure GPIO 4 output is disabled
    REG_WRITE(GPIO_ENABLE_W1TC_REG, BUTTON_MASK);


    // STEP 3: Enable internal pull-up

    reg = REG_READ(IO_MUX_GPIO4_REG);

    // Enable pull-up
    reg |= (1U << 8);

    // Disable pull-down
    reg &= ~(1U << 7);

    REG_WRITE(IO_MUX_GPIO4_REG, reg);


    // STEP 4: Configure FALLING EDGE detection

    reg = REG_READ(BUTTON_PIN_REG);

    // Clear INT_TYPE bits [7:5]
    reg &= ~(0x7U << 5);

    // INT_TYPE = 2 means falling edge
    reg |= (2U << 5);

    REG_WRITE(BUTTON_PIN_REG, reg);


    // STEP 5: Clear any pending interrupt

    REG_WRITE(GPIO_STATUS_W1TC_REG, BUTTON_MASK);


    // STEP 6: Register ISR and connect GPIO interrupt to CPU

    ESP_ERROR_CHECK(
        esp_intr_alloc(
            ETS_GPIO_INTR_SOURCE,
            ESP_INTR_FLAG_IRAM,
            button_ISR,
            NULL,
            &gpio_intr_handle
        )
    );


    // Main application continues running
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
...................,.................,.................................................
