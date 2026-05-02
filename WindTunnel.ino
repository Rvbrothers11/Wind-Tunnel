#include <SPI.h> 
#include <Encoder.h>
#include <Servo.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>

#define TFT_CS      10
#define TFT_DC      8
#define TFT_RST     9

#define FAN_PWN     1
#define FAN_TACHO   2

#define SERVO_PIN   3

#define ENC1_A      4
#define ENC1_B      5
#define ENC1_BTN    6
#define ENC2_A      7
#define ENC2_B      14
#define ENC_2BTN    15

Adafruit_ILI9341_t3 tft = Adafruit_ILI9341_t3(TFT_CS, TFT_DC, TFT_RST);
Encoder angleEncoder(ENC1_A, ENC1_B);
Encoder speedEncoder(ENC2_A, ENC2_B);
Servo airfoilServo;

int currentAngle = 90;
int currentSpeed = 0;
int oldAngle = -1;
int oldSpeed = -1;

volatile unsigned long tachoPulses = 0;
unsigned long lastRpmTime = 0;
int current RPM = 0;
int old RPM = -1;

void countPulses() {
  tachoPulses++;
}

void setup() {
  Serial.begin(115200);
  pinMode(FAN_PWM, OUTPUT);
  analogWriteFrequency(FAN_PWN, 25000);
  analogWrite(FAN_PWM, 0);

  pinMode(FAN_TACHO, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FAN_TACHO), countPulses, FALLING);

  pinMode(ENC1_BTN, INPUT_PULLUP);
  pinMode(ENC2_BTN, INPUT_PULLUP);

  airfoilServo.attach(SERVO_PIN);
  airfoilServo.write(currentAngle);

  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);

  tft.setTextColor(ILI9341_CYAN);
  tft.setTextSize(3);
  tft.setCursor(10, 10);
  tft.print("WIND TUNNEL");

  tft.drawFastHLine(0, 45, 320, ILI9341_WHITE);

  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(10, 70);
  tft.print("Airfoil Angle:");

  tft.setCursor(10, 130);
  tft.print("Fan Speed (%):");

  tft.setCursor(10, 190);
  tft.print("Actual RPM   :");

  angleEncoder.write(currentAngle * 4);
  speedEncoder.write(currentSpeed * 4);
}

void loop() {
  long newAngle = angleEncoder.read() / 4;
  long newSpeed = speedEncoder.read() / 4;
  if (newAngle > 180) { newAngle = 180; angleEncoder.write(180 * 4); }
  if (newAngle < 0) { newAngle = 0; angleEncoder.write(0); }

  if (newSpeed > 100) { newSpeed = 100; speedEncoder.write(100 * 4); }
  if (newSpeed < 0) { newSpeed = 0; speedEncoder.write(0); }

  currentAngle = newAngle;
  currentSpeed = newSpeed;

  if (digitalRead(ENC1_BTN) == LOW) {
    currentAngle = 90;
    angleEncoder.write(90 * 4);
    delay(200);
  }

  if (digitalRead(ENC2_BTN) == LOW) {
    currentSpeed = 0;
    speedEncoder.write (0);
    delay(200);
  }

  if (currentAngle != oldAngle) {
    airfoilServo.write(currentAngle);
  }

  if (currentSpeed != oldSpeed) {
    int pwmValue = map(currentSpeed, 0, 100, 0, 255);
    analogWrite(FAN_PWM, pwmValue);
  }

  if (millis() - lastRpmTime >= 1000) {
    noInterrupts();
    unsigned long pulses = tachoPulses;
    tachopulses = 0;
    interrupts();

    currentRPM = (pulses / 2) * 60;
    lastRpmTime = millis();
  }

  tft.setTextSize(3);

  if (currentAngle != oldAngle) {
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setCursor(200, 65);

    tft.print(currentAngle); tft.print("   ");
    oldAngle = currentAngle:
  }

  if (currentSpeed != oldSpeed) {
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.setCursor(200, 125);
    tft.print(currentSpeed); tft.print("   ");
    oldSpeed = currentSpeed;
  }

  if (current RPM != oldRPM) {
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.setCursor(200, 185);
    tft.print(currentRPM); tft.print("    ");
    oldRPM = currentRPM;
  }
}