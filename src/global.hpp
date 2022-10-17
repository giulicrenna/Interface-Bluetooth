#define RE 26 //receiver enable
#define DE 33 //driver enable
#define RXD_232 34
#define TXD_232 35
#define RXD_485 27
#define TXD_485 28
#define MAX485_RE_DE 33
#define RX_BUFF_SIZE 30
//#define BLUEMAN_OUTLINE

const char *deviceID = std::to_string(ESP.getEfuseMac()).c_str();

uint64_t currentTimeBluethoothMessage = 0;
uint64_t bluethoothMessageTime = 1000;

