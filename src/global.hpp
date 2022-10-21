#define RE 26 //receiver enable
#define DE 33 //driver enable
#define RXD_232 GPIO_NUM_16
#define TXD_232 GPIO_NUM_17
#define RXD_485 27
#define TXD_485 28
#define MAX485_RE_DE 33
#define RX_BUFF_SIZE 30
#define PIN_RED    20
#define PIN_GREEN  21
#define PIN_BLUE   22

const char *deviceID = std::to_string(ESP.getEfuseMac()).c_str();

int pin = 741963;
uint64_t currentTimeBluethoothMessage = 0;
uint64_t bluethoothMessageTime = 1000;

