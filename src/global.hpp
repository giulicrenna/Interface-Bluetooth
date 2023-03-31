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
    MANAGE_UART,
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

String msg = "";
String pinc;

int TXD_485 = GPIO_NUM_17;
int RXD_485 = GPIO_NUM_16;
int TXD_232 = GPIO_NUM_1; // PIN 35
int RXD_232 = GPIO_NUM_3; // PIN 34
int sendTime = 1000;
int INCOME_BUFFER = 32;
int pin; 

uint64_t currentTimeBluetoothMessage = 0;
uint64_t currentTimeSendMessage = 0;
uint64_t currentTimeBluetoothTestMessage = 0;
uint64_t bluetoothMessageTime = 1000;

const char *deviceID = std::to_string(ESP.getEfuseMac()).c_str();
const char *deviceName = "Darkflow-Balanza-2";

bool couldDetect;
bool sendToDevice = true;

struct lastParams
{
    int baud = 0;
    int parity = SERIAL_8N1;
    int rxd;
    int txd;
    bool inverted;
    bool isRS232 = true;
    bool isAuto = true;
} UARTparam;

struct debugMessages
{
    const char *err_0 = "[err 0] No se pudo detectar la configuración en RS232";
    const char *err_1 = "[err 1] No se pudo detectar la configuración en RS485";
    const char *err_2 = "[err 2] Contraseña incorrecta";
    const char *err_3 = "[err 3] No se pudo obtener la configuracion";
    const char *err_4 = "[err 4] Parametros incorrectos";
    const char *err_5 = "[err 5] Contraseña incorrecta";
    const char *err_6 = "[err 6] Contraseña incorrecta";
    const char *err_7 = "[err 7] Comando invalido";

    const char *sta_0 = "[est 0] Contraseña correcta";
    const char *sta_1 = "[est 1] Configuración actualizada correctamente";
    const char *sta_2 = "[est 2] Configuración actualizada correctamente";
    const char *sta_3 = "[est 3] Configuración actualizada correctamente";
    const char *sta_4 = "[est 4] Configuración actualizada correctamente";
    const char *sta_5 = "[est 5] Baudrate detectado: ";
    const char *sta_6 = "[est 6] La configuración se ha actualizada correctamente";
    const char *sta_7 = "[est 7] Se ha configurado la paridad: ";
    const char *sta_8 = "[est 8] El baudrate se establecerá automáticamente";
    const char *sta_9 = "[est 9] Protocolo serie por defecto";
    const char *sta_10 = "[est 10] Baudrate stablecido correctamente: ";
} debugging;