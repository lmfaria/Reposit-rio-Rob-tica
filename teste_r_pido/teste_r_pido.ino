#include <Adafruit_MotorShield.h>


// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *motor1 = AFMS.getMotor(1);

Adafruit_DCMotor *motor2 = AFMS.getMotor(2);

void setup() {
AFMS.begin(); // create with the default frequency 1.6KHz
  

}

void loop() {
    motor1->setSpeed(255);
    motor1->run(FORWARD);
    delay(2000);    
    motor1->run(BACKWARD);
    delay(2000);

    

}
