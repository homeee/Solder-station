/*
 ПАЯЛЬНАЯ СТАНЦИЯ ver. 0.5

 Дата создания 2015
 Автор Олег Андреев
 d-serviss@inbox.lv
 http://www.d-serviss.lv

 */

#include <LiquidCrystal.h>
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);  // Выходы для дисплея 1602

int pinSolderOut = 5;  // Выход для паяльника
int pinSolderIn = A4;  // Потенциометр паяльника
int pinSolderTCouple = A3;  // Термопара паяльника
int pinSolderButton = 2;  // Кнопка вкл. и выкл. паяльника

int pinHotAirOut = 6;  // Выход для фена
int pinHotAirIn = A2;  // Потенциометр фена
int pinHotAirTCouple = A1;  // Термопара фена
int pinHotAirCoolerOut = 3;  // Выход для вентилятора фена ( PWM )
int pinHotAirCoolerIn = A0;  // Потенциометр вентилятора фена
int pinHotAirButton = 4;  // Кнопка вкл.и выкл. фена

uint8_t char_cel[8] = {
  B00111, B00101, B00111, B00000, B00000, B00000, B00000
};

void setup()
{
  TCCR2B = TCCR2B & 0b11111000 | 0x01; // Частота ШИМ 11 и 3
  pinMode(pinSolderOut, OUTPUT);
  pinMode(pinSolderButton, INPUT);
  pinMode(pinHotAirOut, OUTPUT);
  pinMode(pinHotAirButton, INPUT);
  lcd.begin(16, 2);
  lcd.createChar(1, char_cel);

  // Вывод приветствия
  lcd.setCursor(0, 0);
  lcd.print("SOLDER STATION");
  lcd.setCursor(0, 1);
  lcd.print("ver. 0.5");
  // задержка
  delay (3000);
  lcd.clear();
}

void loop()
{
  // Преобразовываем значения
  int setSolderTemp = map(analogRead(pinSolderIn), 0, 1023, 0, 480);
  int solderTCouple = map(analogRead(pinSolderTCouple), 0, 750, 0, 480);
  int setHotAirTemp = map(analogRead(pinHotAirIn), 0, 1023, 0, 480);
  int hotAirTCouple = map(analogRead(pinHotAirTCouple), 0, 750, 0, 480);
  int setHotAirCooler = map(analogRead(pinHotAirCoolerIn), 0, 1023, 130, 255);
  int displayHotAirCooler = map(analogRead(pinHotAirCoolerIn), 0, 1023, 0, 99);

  // Защита, если не работает термопара
  if (solderTCouple > 480) {
    setSolderTemp = 0;
  }
  if (hotAirTCouple > 480) {
    setHotAirTemp = 0;
  }

  // Поддержка установленной температуры паяльника
  if (setSolderTemp >= solderTCouple && digitalRead(pinSolderButton) == HIGH)
  {
    digitalWrite(pinSolderOut, LOW);
    // delay(100);
    digitalWrite(pinSolderOut, HIGH);
  }
  else {
    digitalWrite(pinSolderOut, LOW);
  }

  // Поддержка установленной температуры фена
  if (setHotAirTemp >= hotAirTCouple && digitalRead(pinHotAirButton) == HIGH)
  {
    digitalWrite(pinHotAirOut, HIGH);
    delay(100);
    digitalWrite(pinHotAirOut, LOW);
  }
  else {
    digitalWrite(pinHotAirOut, LOW);
  }

  // Установка оборотов вентилятора фена
  if (hotAirTCouple < 85 && digitalRead(pinHotAirButton) == LOW) {
    analogWrite(pinHotAirCoolerOut, 0);
  }
  else {
    analogWrite(pinHotAirCoolerOut, setHotAirCooler);
  }

  // Убираем прыганье цифр
  if ((setSolderTemp + 10) > solderTCouple && (setSolderTemp - 10) < solderTCouple) {
    solderTCouple = setSolderTemp;
  }
  if ((setHotAirTemp + 10) > hotAirTCouple && (setHotAirTemp - 10) < hotAirTCouple) {
    hotAirTCouple = setHotAirTemp;
  }

  // Данные паяльника на дисплей
  lcd.clear();
  lcd.print("Solder:");
  lcd.setCursor(7, 0);
  if (digitalRead(pinSolderButton) == HIGH) {
    if (solderTCouple < 480) {
      lcd.print(setSolderTemp);
      lcd.print("\1");
      lcd.setCursor(12, 0);
      lcd.print(solderTCouple);
      lcd.print("\1");
    }
    else {
      lcd.print(" Error");
    }
  }
  else {
    lcd.print(" Off");
  }
  // Данные фена на дисплей
  lcd.setCursor(0, 1);
  lcd.print("Smd:");
  lcd.setCursor(4, 1);
  if (digitalRead(pinHotAirButton) == HIGH) {
    if (hotAirTCouple < 480) {
      lcd.print(setHotAirTemp);
      lcd.print("\1");
      lcd.setCursor(9, 1);
      lcd.print(hotAirTCouple);
      lcd.print("\1");
      // Вентилятор фена на дисплей
      lcd.setCursor(14, 1);
      lcd.print(displayHotAirCooler);
    }
    else {
      lcd.print(" Error");
    }
  }
  else {
    lcd.print(" Off");
  }
  delay(100);
}

