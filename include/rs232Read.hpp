int setupRS232(uint8_t baudRate)
{
    Serial.begin(baudRate, SERIAL_8N1, RXD, TXD);

    return 0;
}

byte communicationRS232()
{
  return Serial.read();
}
