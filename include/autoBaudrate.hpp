#include <HardwareSerial.h>
#include <cstdlib>
// #include <esp32-hal-cpu.h>
// #include <hal/uart_ll.h>


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
 * @brief This function detect the baudrate and returns 0 if could not detect anything.
 * 
 * @param inverted 
 * @param rxd 
 * @param txd 
 * @param timeout_ 
 * @return int 
 */
int optimalBaudrateDetection(bool inverted, int rxd, int txd, int timeout_ = 1000)
{
    Serial.begin(0, SERIAL_8N1, rxd, txd, inverted, timeout_); // Passing 0 for baudrate to detect it, the last parameter is a timeout in ms
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