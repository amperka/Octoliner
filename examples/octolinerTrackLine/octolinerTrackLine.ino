// библиотека для работы cо сборкой датчиков линии
#include <Octoliner.h>

// создаём объект для работы с датчиками линии
Octoliner octoliner(42);

void setup() {
  // открываем Serial-порт
  Serial.begin(115200);;
  // начало работы с датчиками линии
  octoliner.begin();
  // выставляем чувствительность фотоприёмников в диапазоне от 0 до 255
  octoliner.setSensitivity(208);
}

void loop() {
  // массив для хранения показателей с датчиков линии
  int16_t dataSensors[8];
  // считываем значение с датчиков линии
  for (uint8_t i = 0; i < 8; i++) {
    // выводим значения в Serial-порт
    dataSensors[i] = octoliner.analogRead(i);
    Serial.print(dataSensors[i]);
    Serial.print("\t");
  }
  Serial.println(octoliner.trackLine(dataSensors));
  Serial.println();
  // ждём пол секунды
  delay(500);
}
