#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

/*
  Mega analogWrite() test

  This sketch fades LEDs up and down one at a time on digital pins 2 through 13.
  This sketch was written for the Arduino Mega, and will not work on other boards.

  The circuit:
  - LEDs attached from pins 2 through 13 to ground.

  created 8 Feb 2009
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogWriteMega
*/

// These constants won't change. They're used to give names to the pins used:
const int red_led = 23;
const int green_led = 25;
const int blue_led = 27;
const int sensor = 12;

void setup() {
  // set pins 2 through 13 as outputs:
  pinMode(red_led, OUTPUT);
  pinMode(green_led, OUTPUT);
  pinMode(blue_led, OUTPUT);
  digitalWrite(red_led, LOW);
  digitalWrite(red_led, HIGH);
  digitalWrite(green_led, LOW);
  digitalWrite(green_led, HIGH);
  digitalWrite(blue_led, LOW);
  digitalWrite(blue_led, HIGH);
  lcd.begin(16,2);
}

void read_sensor() {
  int n_reads = 10;
  int sum_reads = 0;
  for(int i = 0; i < n_reads; i++) {
    sum_reads += analogRead(sensor);
    lcd.setCursor(0,1);
    lcd.print("              ");
    lcd.setCursor(0,1);
    lcd.print(sum_reads / i);
    delay(100);
  }
  delay(2000);
}

void loop() {
  int delay_time = 0;
  
  digitalWrite(red_led, LOW);
  lcd.setCursor(0,0);
  lcd.print("           ");
  lcd.setCursor(0,0);
  lcd.print("Vermelho");
  delay(delay_time);
  read_sensor();
  digitalWrite(red_led, HIGH);

  digitalWrite(green_led, LOW);
  lcd.setCursor(0,0);
  lcd.print("           ");
  lcd.setCursor(0,0);
  lcd.print("Verde");
  delay(delay_time);
  read_sensor();
  digitalWrite(green_led, HIGH);

  digitalWrite(blue_led, LOW);
  lcd.setCursor(0,0);
  lcd.print("           ");
  lcd.setCursor(0,0);
  lcd.print("Azul");
  delay(delay_time);
  read_sensor();
  digitalWrite(blue_led, HIGH);

  digitalWrite(red_led, HIGH);
  digitalWrite(blue_led, HIGH);
  digitalWrite(green_led, HIGH);
  lcd.setCursor(0,0);
  lcd.print("           ");
  lcd.setCursor(0,0);
  lcd.print("Apagado");
  delay(delay_time);
  read_sensor();
}
