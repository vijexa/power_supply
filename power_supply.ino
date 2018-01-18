#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x26
#define En_pin 2
#define Rw_pin 1
#define Rs_pin 0
#define D4_pin 4
#define D5_pin 5
#define D6_pin 6
#define D7_pin 7
#define BACKLIGHT 3

#define bat_main_resistor 100.0
#define bat_secondary_resistor 200.0
#define bat_calibration 1.0

#define volt_main_resistor 100.0
#define volt_secondary_resistor 300.0
#define volt_calibration 1.025

#define shunt 2.5
#define max_amp 2.0

#define buzzer_freq 500

#define critical_temp 60
#define normal_temp 100

#define battery A0
#define voltmeter A1
#define ampermeter A2
#define thermometer A3

#define relay 6
#define buzzer 9

volatile LiquidCrystal_I2C lcd(I2C_ADDR, En_pin, Rw_pin, Rs_pin, D4_pin, D5_pin, D6_pin, D7_pin);

void setup() {
  lcd.begin (16, 2);
  lcd.setBacklightPin(BACKLIGHT, POSITIVE);
  lcd.setBacklight(HIGH);
  pinMode(relay, OUTPUT);
  pinMode(9, OUTPUT);
  tone(buzzer, buzzer_freq);
  digitalWrite(relay, HIGH);
  Serial.begin(9600);

  lcd.clear();
  delay(1000);
  lcd.home();
  noTone(buzzer);
}

float buf;
float bat;
float volt;
float amp;
int temp;
bool short_flag = false;
bool temp_flag = false;
unsigned long last_time = 0;
void loop() {
  lcd.home();

  buf = analogRead(battery);
  buf = analogRead(battery);
  buf = (buf * 5.0) / 1024.0;
  bat = buf + (buf * (bat_secondary_resistor / bat_main_resistor));
  bat *= bat_calibration;

  buf = analogRead(voltmeter);
  buf = analogRead(voltmeter);
  buf = (buf * 5.0) / 1024.0;
  volt = buf + (buf * (volt_secondary_resistor / volt_main_resistor));
  volt *= volt_calibration;

  amp = analogRead(ampermeter);
  amp = analogRead(ampermeter);
  amp = ((amp * 5.0) / 1024.0) / shunt;
  if (amp >= max_amp-0.02) {
    tone(buzzer, buzzer_freq);
    if (short_flag == false) {
      digitalWrite(relay, LOW);
      lcd.clear();
      lcd.print("SHORT CIRCUIT");
      short_flag = true;
    }
  } else {
    if (short_flag == true) {
      noTone(buzzer);
      digitalWrite(relay, HIGH);
      short_flag = false;
    }
  }

  temp = analogRead(thermometer);
  temp = analogRead(thermometer);
  Serial.println(temp);
  if ((temp < critical_temp) && !temp_flag) {
    temp_flag = true;
    digitalWrite(relay, LOW);
    lcd.clear();
    lcd.home();
    lcd.print("OVERHEATING");
  }
  if ((temp > normal_temp) && temp_flag) {
    digitalWrite(relay, HIGH);
    temp_flag = false;
  }

  //drawing
  if (!short_flag && !temp_flag) {
    if (millis() - last_time > 1000) {
      lcd.home();

      lcd.print("B:");
      lcd.print(bat, 3);
      lcd.print("   ");

      lcd.setCursor(9, 0);
      lcd.print("V:");
      lcd.print(volt, 3);

      lcd.setCursor(0, 1);
      lcd.print("T:");
      lcd.print(temp);
      lcd.print(" ");

      lcd.setCursor(9, 1);
      lcd.print("A:");
      lcd.print(amp, 3);

      last_time = millis();
    }
  }
}

