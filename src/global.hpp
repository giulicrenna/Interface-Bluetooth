#define __ARDUINO__
//#define _EXTERN_
#define DEBUG
#define RE GPIO_NUM_4 // receiver enable
#define PIN_RED GPIO_NUM_12
#define PIN_GREEN GPIO_NUM_26
#define PIN_BLUE GPIO_NUM_25

Preferences config;

typedef enum
{
    BLUE_PAIRING,
    BLUE_ASK_KEY,
    CHANGE_UART_CONFIG,
    DETECT_NON_ASCII,
    DETERMINATE_BAUD_232_NI,
    DETERMINATE_BAUD_232_I,
    DETERMINATE_BAUD_485_NI,
    DETERMINATE_BAUD_485_I,
    MANAGE_DEVICE_CONFIGS,
    INIT_UART,
    SEND_BY_TIME,
    SEND_BY_PETITION,
    SEND_FAIL,
    SEND_TEST
} States;

States currentState = BLUE_PAIRING;

String msg;
String master = "19622439C17D7D3479263C2BDC6BD";
String pinc;

int TXD_485 = GPIO_NUM_17;
int RXD_485 = GPIO_NUM_16;
int TXD_232 = GPIO_NUM_1; // PIN 35
int RXD_232 = GPIO_NUM_3; // PIN 34
int sendTime = 1000;
int INCOME_BUFFER = 32;
int pin;
int lastUartConfigIndex = 0;

uint64_t currentTimeBluetoothMessage = 0;
uint64_t currentCheckAscii = 0;
uint64_t currentTimeSendMessage = 0;
uint64_t currentTimeBluetoothTestMessage = 0;
uint64_t bluetoothMessageTime = 1000;

const char *deviceID = std::to_string(ESP.getEfuseMac()).c_str();
const char *deviceName = "Darkflow-Balanza-2";

bool couldDetect;
bool couldDetectUartConfig;
bool petitionMode;
bool sendToDevice = true;
bool petition = false;

struct lastParams
{
    int baud;
    int parity = SERIAL_8N1;
    int rxd;
    int txd;
    int timeout;
    bool inverted;
    bool isRS232 = true;
    bool isAuto = true;
    char EOL = '\n';
    String parity_str = "SERIAL_8N1";
} UARTparam;

struct debugMessages
{
    const char *err_0 = "\n[err 0] No se pudo detectar la configuración en RS232";
    const char *err_1 = "\n[err 1] No se pudo detectar la configuración en RS485";
    const char *err_2 = "\n[err 2] Contraseña incorrecta";
    const char *err_3 = "\n[err 3] No se pudo obtener la configuracion";
    const char *err_4 = "\n[err 4] Parametros incorrectos";
    const char *err_5 = "\n[err 5] Contraseña incorrecta";
    const char *err_6 = "\n[err 6] Contraseña incorrecta";
    const char *err_7 = "\n[err 7] Comando invalido";
    const char *err_8 = "\n[err 8] Configuracion erronea\nCambiando la configuracion de la UART";
    const char *err_9 = "\n[err 9] Error desconocido";
    const char *err_10 = "\n[err 10] Error de framing";
    const char *err_11 = "\n[err 11] Error en la paridad";

    const char *sta_0 = "\n[est 0] Contraseña correcta";
    const char *sta_1 = "\n[est 1] Configuración actualizada correctamente";
    const char *sta_2 = "\n[est 2] Configuración actualizada correctamente";
    const char *sta_3 = "\n[est 3] Configuración actualizada correctamente";
    const char *sta_4 = "\n[est 4] Configuración actualizada correctamente";
    const char *sta_5 = "\n[est 5] Baudrate detectado: ";
    const char *sta_6 = "\n[est 6] La configuración se ha actualizada correctamente";
    const char *sta_7 = "\n[est 7] Se ha configurado la paridad: ";
    const char *sta_8 = "\n[est 8] El baudrate se establecerá automáticamente";
    const char *sta_9 = "\n[est 9] Protocolo serie por defecto ";
    const char *sta_10 = "\n[est 10] Baudrate establecido correctamente: ";
    const char *sta_11 = "\n[est 11] La contraseña se restableció exitosamente";
    const char *sta_12 = "\n[est 12] El modo cambió exitosamente";
    const char *sta_13 = "\n[est 13] Cambiando la configuracion de la UART";
    const char *sta_14 = "\n[est 14] Inicializando UART";
} debugging;


//std::map<const char*, std::list<std::map<const char*, const char*>>> commandList;

struct command_types{
    const char *cmd_1 = "PAUSE";
    const char *cmd_1_exp = "\n[PAUSE] \nSTOP THE TRANSMISION WHEN THE SENDING MODE IS BY TIME.";

    const char *cmd_2 = "CONTINUE";
    const char *cmd_2_exp = "\n[CONTINUE] \nCONTINUE THE TRANSMISION WHEN THE SENDING MODE IS BY TIME.";

    const char *cmd_3 = "SEND_BUFFER";
    const char *cmd_3_exp = "\n[SEND_BUFFER] \nSENDS A SINGLE TRANSMISION BUFFER WHEN THE BUFFER IS FULL AND THE SENDING MODE YS BY PETITION.";

    const char *cmd_4 = "RESET_CONFIGS";
    const char *cmd_4_exp = "\n[RESET_CONFIGS] \nRESETS ALL THE STORED CONFIGS.";

    const char *cmd_5 = "TIME";
    const char *cmd_5_exp = "\n[TIME:INT] \nSETS THE TIME BETWEEN EACH TRANSMISION.\nTHE FIRST PARAM. IS THE TIME IN MILLISECONDS.";

    const char *cmd_6 = "BUFFER";
    const char *cmd_6_exp = "\n[BUFFER:INT] \nSETS THE BUFFER OF THE TRANSMISION LENGHT.\nTHE FIRST PARAM. IS THE BUFFER SIZE IN BYTES.";

    const char *cmd_7 = "RX_TIMEOUT";
    const char *cmd_7_exp = "\n[RX_TIMEOUT:INT] \nSETS THE TIMEOUT OF THE RX PINS.\nTHE FIRST PARAM. IS THE TIME OF THE TIMEOUT IN MILLISECONDS.";

    const char *cmd_8 = "PASSWORD";
    const char *cmd_8_exp = "\n[PASSWORD:STRING] \nSENDS THE PASSWORD TO THE BOARD; IF THE PASSWORD IS CORRECT IT WILL START SENDING DATA.\n THE FIRST PARAM. IS THE PASSWORD.";

    const char *cmd_9 = "RESET_PASSWORD";
    const char *cmd_9_exp = "\n[RESET_PASSWORD] \nRESET THE PASSWORD TO THE DEFAULT TOKEN.";

    const char *cmd_10 = "RESTART";
    const char *cmd_10_exp = "\n[RESTART] \nREBOOT THE DEVICE.";

    const char *cmd_11 = "UART";
    const char *cmd_11_param_1 = "AUTO"; 
    const char *cmd_11_exp = "\n[UART:STRING/INT:STRING] \nCONFIGURES THE UART.\nIF THE FIRST PARAM IS AUTO, THE BAUDRATE WILL BE AUTOMATICALLY DETECTED. THE SECOND PARAMETER IS THE UART PARTITY AND STOP BIT.";

    const char *cmd_12 = "MODE";
    const char *cmd_12_param_1 = "TIME"; 
    const char *cmd_12_param_2 = "PETITION"; 
    const char *cmd_12_exp = "\n[MODE:TIME/PETITION] \nCONFIGURES DEVICE MODE.";

    const char *cmd_13 = "AUTO_UART_CONFIG";
    const char *cmd_13_param_1 = "TRUE"; 
    const char *cmd_13_param_2 = "FALSE"; 
    const char *cmd_13_exp = "\n[AUTO_UART_CONFIG:BOOLEAN] \nSETS IF THE DETECTION OF THE UART CONFIG IS AUTOMATICALLY (EXPERIMENTAL).";

    const char *cmd_14 = "STATUS";
    const char *cmd_14_exp = "\n[STATUS] \nSENDS THE CURRENT CONFIG.";

    const char *help = "HELP";

}commandList;