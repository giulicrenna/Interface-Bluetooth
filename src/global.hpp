<<<<<<< HEAD
//#define OLDBT

=======
>>>>>>> parent of 926e4d3 (Old BT support)
#define TR GPIO_NUM_4 //receiver enable
#define TXD_485 GPIO_NUM_17
#define RXD_485 GPIO_NUM_16
#define TXD_232 GPIO_NUM_1 // PIN 35
#define RXD_232 GPIO_NUM_3 // PIN 34
#define PIN_RED    GPIO_NUM_27
#define PIN_GREEN  GPIO_NUM_26
#define PIN_BLUE   GPIO_NUM_25

const char *deviceID = std::to_string(ESP.getEfuseMac()).c_str();
const char *deviceName = "Darkflow-Device-XX";
bool couldDetect;

int pin = 741963;
uint64_t currentTimeBluetoothMessage = 0;
uint64_t bluetoothMessageTime = 1000;

