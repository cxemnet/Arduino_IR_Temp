#include <i2cmaster.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

void setup(){
  //Serial.begin(9600);
	
  i2c_init();                     // initialise the i2c bus
  PORTC = (1 << PORTC4) | (1 << PORTC5);  //enable pullups
  
  display.begin();
  display.setContrast(40);        // set contrast (установка контрастности дисплея)
  delay(1000);
  display.clearDisplay();         // clears the screen and buffer (очистка экрана и буфера)
  display.setTextSize(1);         // set text size (установка размера текста)
  display.setTextColor(BLACK);
  delay(1000);
}

void loop(){
    int dev = 0x5A<<1;
    int data_low = 0;
    int data_high = 0;
    int pec = 0;
    
    i2c_start_wait(dev+I2C_WRITE);
    i2c_write(0x07);
    
    // read
    i2c_rep_start(dev+I2C_READ);
    data_low = i2c_readAck();     // read 1 byte and then send ack
    data_high = i2c_readAck();    // read 1 byte and then send ack
    pec = i2c_readNak();
    i2c_stop();
    
    //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
    double tempFactor = 0.02;     // 0.02 degrees per LSB (measurement resolution of the MLX90614)
    double tempData = 0x0000;     // zero out the data
    int frac; // data past the decimal point
    
    // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
    tempData = (double)(((data_high & 0x007F) << 8) + data_low);
    tempData = (tempData * tempFactor)-0.01;
    
    float celcius = tempData - 273.15;
    float fahrenheit = (celcius*1.8) + 32;

    //Serial.print("Celcius: ");
    //Serial.println(celcius);
    
    display.clearDisplay();              // clears the screen and buffer (очистка экрана и буфера)
    display.setCursor(0,0);     
    
    display.print("Temp (C)=");          // print text temp C (вывод температуры в градусах Цельсия)
    display.println(celcius);            // вывод значения температуры с переводом строки
    display.println();                   // news line (перевод строки)
    
    display.print("Temp (F)=");          // print text temp F (вывод температуры в градусах Фаренгейта)
    display.println(fahrenheit);         // вывод значения температуры с переводом строки
    display.println();                   // news line (перевод строки)

    display.display();                   // show on display (вывод данных на дисплей)
    
    delay(1000);
}
