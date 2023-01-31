#ifndef RGBLED_H
#define RGBLED_H
/*
RED
BLUE
GREEN
PINK
BLACK
*/
int ledState = LOW;
int previousMillis = 0;

class myLeds
{
public:
    int color[5][3] = {{255, 0, 0},
                       {0, 0, 255},
                       {0, 255, 0},
                       {255, 0, 255},
                       {0, 0, 0}};
    int PIN_RED, PIN_GREEN, PIN_BLUE;
    myLeds(int pr_, int pg_, int pb_)
    {
        PIN_RED = pr_;
        PIN_GREEN = pg_;
        PIN_BLUE = pb_;
    }

    void setupLed()
    {
        pinMode(PIN_RED, OUTPUT);
        pinMode(PIN_GREEN, OUTPUT);
        pinMode(PIN_BLUE, OUTPUT);
    }
    void changeColour(int n)
    {
        setColor(color[n][0], color[n][1], color[n][2]);
    }

    void blink(int k, bool isRS232)
    {
        if (isRS232)
        {
            if (k % 2 == 0)
            {
                changeColour(0);
            }
            else
            {
                changeColour(4);
            }
        }
        else
        {
            if (k % 2 == 0)
            {
                changeColour(1);
            }
            else
            {
                changeColour(4);
            }
        }
    }

    void setColor(int R, int G, int B)
    {
        R = 255 - R;
        G = 255 - G;
        B = 255 - B;
        analogWrite(PIN_RED, R);
        analogWrite(PIN_GREEN, G);
        analogWrite(PIN_BLUE, B);
    }
};

void blink()
{
    if (millis() - previousMillis >= 500)
    {
        // if the LED is off turn it on and vice-versa:
        ledState = (ledState == LOW) ? HIGH : LOW;

        // set the LED with the ledState of the variable:
        digitalWrite(GPIO_NUM_27, ledState);

        // save the last time you blinked the LED
        previousMillis = millis();
    }
}

#endif
