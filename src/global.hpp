#define __ARDUINO__
#define DEBUG
#define RE GPIO_NUM_4 // receiver enable
#define PIN_RED GPIO_NUM_12
#define PIN_GREEN GPIO_NUM_26
#define PIN_BLUE GPIO_NUM_25

typedef enum
{
    BLUE_PAIRING,
    BLUE_ASK_KEY,
    DETERMINATE_BAUD_232_NI,
    DETERMINATE_BAUD_232_I,
    DETERMINATE_BAUD_485_NI,
    DETERMINATE_BAUD_485_I,
    INIT_UART,
    SEND_MSG,
    READ_DATA,
    SEND_FAIL,
    SEND_TEST
} States;

struct lastParams
{
    int baud = 0;
    bool inverted;
    int rxd;
    int txd;
} UARTparam;

String msg = "";

int TXD_485 = GPIO_NUM_17;
int RXD_485 = GPIO_NUM_16;
int TXD_232 = GPIO_NUM_1; // PIN 35
int RXD_232 = GPIO_NUM_3; // PIN 34

const char *deviceID = std::to_string(ESP.getEfuseMac()).c_str();
const char *deviceName = "Darkflow-Balanza-1";
bool couldDetect;

const char *pinc = "12345";
int pin = 12345;
uint64_t currentTimeBluetoothMessage = 0;
uint64_t currentTimeSendMessage = 0;
uint64_t currentTimeBluetoothTestMessage = 0;
uint64_t bluetoothMessageTime = 1000;
