Preferences temporalData;

void setupFileSystem()
{
    temporalData.begin("baud", false);
}

bool loadData(bool inverted = false, int rx = 0, int tx = 0, int baud = 0)
{
    int tempBaud = temporalData.getInt("baud", 0); 
    if(tempBaud == 0)
    {
        temporalData.putInt("baud", baud);
        temporalData.putInt("rx", rx);
        temporalData.putInt("tx", tx);
        temporalData.putBool("inverted", inverted);
        ESP.restart();
    }else{
        UARTparam.baud = tempBaud;
        UARTparam.rxd = temporalData.getInt("rx", 0);
        UARTparam.txd = temporalData.getInt("tx", 0);
        UARTparam.inverted = temporalData.getBool("inverted", false);
        temporalData.clear();
    }
    return true;
}

/*
if(temporalData.getInt("baud", 0) == 0){
            temporalData.putInt("baud", detectedBaudRate);
        }
*/