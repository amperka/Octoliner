// библиотека для работы cо сборкой датчиков линии
#include <Octoliner.h>

// пин подключение потенциометра регулировки чувствительности приёмника
#define PIN_SENSITIVITY   A0

// создаём объект для работы с датчиками линии
Octoliner octoliner(42);

void setup() {
  // открываем Serial-порт
  Serial.begin(115200);
  // начало работы с датчиками линии
  octoliner.begin();
}

void loop() {
  // pсчитываем значение с потенциометров
  int16_t sensitivity = map(analogRead(PIN_SENSITIVITY), 0, 1024, 0, 255);

  // выставляем чувствительность фотоприёмников
  octoliner.setSensitivity(sensitivity);
  
  // считываем значение с датчиков линии
  for (uint8_t i = 0; i < 8; i++) {
    // преобразуем диапазон 0…4096 -> 0…32
    int16_t sharpCount = octoliner.analogRead(i) >> 7;
    // выводим положение линии в графическом виде
    for (int16_t y = 0; y <= sharpCount; y++) {
      Serial.print('#');
    }
    Serial.println();
  }
  // вставляем пустые строки для форматирвоания вывода
  Serial.println("\r\n\r\n");
  delay(100);
}
