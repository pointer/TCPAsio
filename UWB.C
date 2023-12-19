include "board.h"
#include "fsl_debug_console.h"
#include "fsl_uart.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#define UWB_MODE_ACTIVE 0x01

int main(void) {
    uart_config_t uartConfig;
    gpio_pin_config_t gpioConfig;
    uint8_t buf[255];

    // Initialize the board
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    // Initialize the UART
    UART_GetDefaultConfig(&uartConfig);
    uartConfig.baudRate_Bps = 115200;
    UART_Init(UART0, &uartConfig, CLOCK_GetFreq(kCLOCK_CoreSysClk));
    UART_EnableTx(UART0, true);
    UART_EnableRx(UART0, true);

    // Initialize the GPIO
    PORT_SetPinMux(PORTB, 2U, kPORT_MuxAsGpio);
    gpioConfig.pinDirection = kGPIO_DigitalOutput;
    gpioConfig.outputLogic = 1U;
    GPIO_PinInit(GPIOB, 2U, &gpioConfig);

    // Set the UWB mode to active
    buf[0] = UWB_MODE_ACTIVE;
    UART_WriteBlocking(UART0, buf, 1);

    // Get the distance between two devices
    UART_ReadBlocking(UART0, buf, 255);

    // Print the distance
    PRINTF("Distance: %s meters\n", buf);

    while (1) {}

    return 0;
}