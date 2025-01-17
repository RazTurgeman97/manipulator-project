#include <Servo.h>

Servo motor;

void setup() {
  // put your setup code here, to run once:
  motor.attach(8); // the number of the port the motor is connected to on the arduino board.
  motor.write(90); // initial posiotion in degrees

  Serial.begin(115200);
  Serial.setTimeout(1);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){

    int angle = Serial.readString().toInt();
    motor.write(angle);
  }

  delay(1);
}
