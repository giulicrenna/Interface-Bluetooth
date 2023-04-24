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

    const char *cmd_8 = "NEW_PASSWORD";
    const char *cmd_8_exp = "\n[PASSWORD:STRING:STRING] \nSENDS THE PASSWORD TO THE BOARD; IF THE PASSWORD IS CORRECT IT WILL CHANGE THE DEFAULT PASSWORD.\n THE FIRST PARAM. IS THE OLD PASSWORD.\nTHE SECOND PARAM IS THE NEW PASSWORD.";

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

    const char *cmd_13 = "UART_AUTO_CONFIG";
    const char *cmd_13_param_1 = "TRUE"; 
    const char *cmd_13_param_2 = "FALSE"; 
    const char *cmd_13_exp = "\n[UART_AUTO_CONFIG:BOOLEAN] \nSETS IF THE DETECTION OF THE UART CONFIG IS AUTOMATICALLY (EXPERIMENTAL).";

    const char *cmd_14 = "STATUS";
    const char *cmd_14_exp = "\n[STATUS] \nSENDS THE CURRENT CONFIG.";

    const char *help = "HELP";

}commandList;

void lexator()
{
    if (SerialBT.available() > 0)
    {
        String command = SerialBT.readString();
        command.trim();
        if (command == commandList.cmd_1)
        {
            sendToDevice = false;
            SerialBT.println(debugging.sta_1);
            // break;
        }
        else if (command == commandList.cmd_2)
        {
            sendToDevice = true;
            config.putBool("send", true);
            SerialBT.println(debugging.sta_2);
        }
        else if (command == commandList.cmd_3)
        {
            petition = true;
        }
        else if (command == commandList.cmd_4)
        {
            config.clear();
        }
        else if(command == commandList.help){
            Blue_send("\n----COMMANDS HELP----\n");
            Blue_send(commandList.cmd_1_exp);
            Blue_send(commandList.cmd_2_exp);
            Blue_send(commandList.cmd_3_exp);
            Blue_send(commandList.cmd_4_exp);
            Blue_send(commandList.cmd_5_exp);
            Blue_send(commandList.cmd_6_exp);
            Blue_send(commandList.cmd_7_exp);
            Blue_send(commandList.cmd_8_exp);
            Blue_send(commandList.cmd_9_exp);
            Blue_send(commandList.cmd_10_exp);
            Blue_send(commandList.cmd_11_exp);
            Blue_send(commandList.cmd_12_exp);
            Blue_send(commandList.cmd_13_exp);
            Blue_send(commandList.cmd_14_exp);
            Blue_send("\n-------------------\n");
        }
        else
        {
            String cmd[5];
            std::vector<std::string> cmd_v = mstd::strip(command.c_str(), ':');
            for (int k = 0; k < cmd_v.size(); k++)
            {
                cmd[k] = String(cmd_v.at(k).c_str());
            }
            if (cmd[0] == commandList.cmd_5)
            {
                try
                {
                    int temp_time = std::stoi(cmd[1].c_str());
                    sendTime = temp_time;
                    config.putInt("time", sendTime);
                    SerialBT.println(debugging.sta_3);
                }
                catch (const std::exception &e)
                {
                    SerialBT.println(debugging.err_4);
                    // break;
                }
            }
            else if (cmd[0] == commandList.cmd_6)
            {
                try
                {
                    int temp_buffer = std::stoi(cmd[1].c_str());
                    INCOME_BUFFER = temp_buffer;
                    config.putInt("buffer", INCOME_BUFFER);
                    SerialBT.println(debugging.sta_4);
                }
                catch (const std::exception &e)
                {
                    SerialBT.println(debugging.err_4);
                    // break;
                }
            }
            else if (cmd[0] == commandList.cmd_7)
            {
                try
                {
                    int timeout = std::stoi(cmd[1].c_str());
                    config.putInt("rx_timeout", timeout);
                    UARTparam.timeout = timeout;
                    SerialBT.println(debugging.sta_4);
                    currentState = INIT_UART;
                }
                catch (const std::exception &e)
                {
                    SerialBT.println(debugging.err_4);
                    // break;
                }
            }
            else if (cmd[0] == commandList.cmd_8)
            {
                try
                {
                    if (cmd[1] == pinc)
                    {
                        pinc = cmd[2].c_str();
                        config.putString("pinc", cmd[2]);
                        SerialBT.println(debugging.sta_6);
                        // break;
                    }
                    else
                    {
                        SerialBT.println(debugging.err_5);
                        // break;
                    }
                }
                catch (const std::exception &e)
                {
                    SerialBT.println(debugging.err_6);
                }
            }
            else if (cmd[0] == commandList.cmd_9)
            {
                config.putString("pinc", master);
                Blue_send(debugging.sta_11);
            }
            else if (cmd[0] == commandList.cmd_10)
            {
                ESP.restart();
            }
            else if (cmd[0] == commandList.cmd_14)
            {
                String Status = "\n--------------STATUS---------------\n";
                Status += "BAUDRATE: " + String(UARTparam.baud) + "\n";
                Status += "PARIDAD: " + UARTparam.parity_str + "\n";
                if (UARTparam.isRS232)
                {
                    Status += "PROTOCOLO: RS232\n";
                }
                else
                {
                    Status += "PROTOCOLO: RS485\n";
                }
                if (UARTparam.isAuto)
                {
                    Status += "DETECCION DE BAUDIOS: ACTIVADA\n";
                }
                else
                {
                    Status += "DETECCION DE BAUDIOS: DESACTIVADA\n";
                }
                if (sendToDevice)
                {
                    Status += "ESTADO DE TRANSMISION: ENVIANDO\n";
                }
                else
                {
                    Status += "ESTADO DE TRANSMISION: PAUSADA\n";
                }
                if (petitionMode)
                {
                    Status += "MODO DE TRABAJO: POR PETICION\n";
                }
                else
                {
                    Status += "MODO DE TRABAJO: ENVIO AUTOMATICO\n";
                }

                Status += "UUID: " + String(std::to_string(ESP.getEfuseMac()).c_str()) + "\n";
                Status += "TAMAÑO DEL BUFFER: " + String(std::to_string(INCOME_BUFFER).c_str()) + "\n";
                Status += "TIEMPO DE ENVIO: " + String(std::to_string(sendTime).c_str()) + "\n";
                Status += "RX TIMEOUT: " + String(std::to_string(UARTparam.timeout).c_str()) + "\n";
                Status += "-----------------------------------\n";
                Blue_send(Status);
            }
            else if (cmd[0] == commandList.cmd_11)
            {
                if (cmd[1] == commandList.cmd_11_param_1)
                {
                    UARTparam.isAuto = true;
                    UARTparam.parity = SERIAL_8N1;
                    config.putBool("isAuto", UARTparam.isAuto);
                    SerialBT.println(debugging.sta_8);
                    currentState = DETERMINATE_BAUD_232_NI;
                    return;
                }
                else
                {
                    UARTparam.baud = std::stoi(cmd[1].c_str());
                    UARTparam.isAuto = false;
                    config.putInt("baud", UARTparam.baud);
                    config.putBool("isAuto", UARTparam.isAuto);
                    try
                    {
                        if (cmd[2] == "RS232")
                        {
                            UARTparam.isRS232 = true;
                        }
                        else if (cmd[2] == "RS485")
                        {
                            UARTparam.isRS232 = false;
                        }
                        config.putBool("isRS232", UARTparam.isRS232);
                    }
                    catch (const std::exception &e)
                    {
                        SerialBT.println(debugging.sta_9);
                    }
                    determinateParity(cmd[3]);
                    config.putInt("parity", UARTparam.parity);
                    SerialBT.println(debugging.sta_9 + cmd[1]);
                }
                currentState = MANAGE_DEVICE_CONFIGS;
            }
            else if (cmd[0] == commandList.cmd_12)
            {
                if (cmd[1] == commandList.cmd_12_param_1)
                {
                    petitionMode = false;
                    config.putBool("petitionMode", petitionMode);
                    currentState = SEND_BY_TIME;
                    SerialBT.println(debugging.sta_12);
                }
                else if (cmd[1] == commandList.cmd_12_param_2)
                {
                    petitionMode = true;
                    config.putBool("petitionMode", petitionMode);
                    currentState = SEND_BY_PETITION;
                    SerialBT.println(debugging.sta_12);
                }
                else
                {
                    SerialBT.println(debugging.err_7);
                }
            }
            else if (cmd[0] == commandList.cmd_13)
            {
                if (cmd[1] == commandList.cmd_13_param_1)
                {
                    couldDetectUartConfig = true;
                    config.putBool("couldDetectUartConfig", couldDetectUartConfig);
                    Blue_send(debugging.sta_15);
                }
                else if (cmd[1] == commandList.cmd_13_param_2)
                {
                    couldDetectUartConfig = false;
                    config.putBool("couldDetectUartConfig", couldDetectUartConfig);
                    Blue_send(debugging.sta_15);
                }
                else
                {
                    SerialBT.println(debugging.err_7);
                }
            }
            else
            {
                SerialBT.println(debugging.err_7);
            }

            // break;
        }
    }
}
