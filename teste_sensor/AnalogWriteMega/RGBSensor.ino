#define RED 0
#define GREEN 1
#define BLUE 2

class RGBSensor {
  int leds[3];
  int sensor;
  
  public:
    RGBSensor(int sensor, int red_led, int green_led, int blue_led) {
      leds[RED] = red_led;
      leds[GREEN] = green_led;
      leds[BLUE] = blue_led;
      this->sensor = sensor;
      for(int i=0; i<3; i++) {
        pinMode(leds[i], OUTPUT);  
      }
    };
    void turn_led_on(int color) {
      for(int i=0; i<3; i++) {
        digitalWrite(leds[i], HIGH);
      }
      digitalWrite(leds[color], LOW);
    }
};

