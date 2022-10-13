byte BufferValueRS232[8];

int setupRS232(uint8_t baudRate)
{
    Serial.begin(baudRate, SERIAL_8N1, RXD, TXD);

    return 0;
}

byte communicationRS232()
{
    for(byte i = 4; i > 0; i--){
        BufferValueRS232[i] = Serial.read();
    }

    return BufferValueRS232[8];
}
