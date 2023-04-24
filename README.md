# Firmware for scale hardware
This firmware makes and auto detection to the UART baudrate and then
sends the data over BLE (Bluethooth Low Energy)
 
## Esta es la manera de hacer para cortar un mensaje a la vez sin conocer ni su tamaño ni sus caracteres comunes.
Para usar este método supondremos que existe un numero alpha tal que para toda balanza el tiempo de transmisión entre el final del último mensaje y el inicio del nuevo es este número alpha, así tendremos que para un alpha = 5, el mensaje iniciará cuando se habilite la UART y el mensaje terminará cuando se alcance el alpha timeout (antes de que empiece el siguiente mensaje):

```
int sendTime = 100;
int INCOME_BUFFER = 64;
int currentTimeSendMessage = 0;
String incomingString;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(5);
}

void loop()
{
  if (Serial.available()) {
    incomingString = Serial.readString();
  }
  if (millis() - currentTimeSendMessage >= sendTime)
  {
    Serial.println(incomingString);
    Serial.println("--------------");
    currentTimeSendMessage = millis();
   }
}
```