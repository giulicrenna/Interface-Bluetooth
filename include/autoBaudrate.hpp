#include <cstdlib>

#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_intr_alloc.h"

int UART_CONFIGS[] = {
    SERIAL_7O2,
    SERIAL_7O1,
    SERIAL_8O2,
    SERIAL_8O1,
    SERIAL_8E2,
    SERIAL_8E1,
    SERIAL_7E2,
    SERIAL_7E1,
    SERIAL_7N2,
    SERIAL_7N1,
    SERIAL_8N2,
    SERIAL_8N1,
    };

void onRxInterrups(hardwareSerial_error_t interrupt);

int checkBetweenBauds(int b)
{
    int bauds[21] = {110, 300, 600, 1200, 4800, 9600, 115200, 57600, 2400, 4800, 14400, 19200, 28800, 38400, 76800, 128000, 230400, 256000, 460800, 576000, 921600};
    for (int baud : bauds)
    {
        if (std::abs(baud - b) < 150)
        {
            return baud;
        }
    }
    return 0;
}

#ifdef _EXTERN_
extern "C"
{
    int returnBaud()
    {
        UART_MUTEX_LOCK();

        uint32_t freq_clock = uart_ll_get_sclk_freq(UART0);
        typeof(UART0->clk_div) div_register = UART0->clk_div;
        int baudrate = ((freq_clock << 4)) / ((div_register << 4) | div_register.div_frag);
        return checkBetweenBauds(baudrate);
        UART_MUTEX_UNLOCK();
    }
}
#endif

/**
 * @brief This function detects interruption on the RX of the UART and manage them.
 * 
 * @param interrupt 
 */

void onRxInterrups(hardwareSerial_error_t interrupt)
{
    switch (interrupt)
    {
    
    case UART_BREAK_ERROR:
    {
        break;
    }
    case UART_BUFFER_FULL_ERROR:
    {
        break;
    }
    case UART_FIFO_OVF_ERROR:
    {
        break;
    }
    
    case UART_FRAME_ERROR:
    {   
        Blue_send(debugging.err_10);
        currentState = DETECT_NON_ASCII;
        break;
    }
    case UART_PARITY_ERROR:
    {
        Blue_send(debugging.err_11);
        currentState = DETECT_NON_ASCII;
        break;
    }
    default:
        Blue_send(debugging.err_9);
    }
}

/**
 * @brief This function detect the baudrate and returns 0 if could not detect anything.
 *
 * @param inverted
 * @param rxd
 * @param txd
 * @param timeout_
 * @return int
 */
int optimalBaudrateDetection(bool inverted, int rxd, int txd, int timeout_ = 3000)
{   
    Serial.begin(0, UARTparam.parity, rxd, txd, inverted, timeout_); // Passing 0 for baudrate to detect it, the last parameter is a timeout in ms
    unsigned long detectedBaudRate = Serial.baudRate();
    if (detectedBaudRate)
    {
        detectedBaudRate = checkBetweenBauds(detectedBaudRate);
        return detectedBaudRate;
    }
    else
    {
        return 0;
    }

    return 0;
}