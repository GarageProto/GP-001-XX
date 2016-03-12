
#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <Adafruit_MLX90614.h>
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

float DS18B20_temp;
OneWire  ds(2);  //

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  mlx.begin();
  

}

// ####################################################################################################################################
void loop() {
  // put your main code here, to run repeatedly:

//getMLX90614Temp();
calculate_DS18B20();
Serial.println("IRSTART");
Serial.println("MLX90614ESF-BAC");
//Serial.println("19.86,");
Serial.println(mlx.readObjectTempC());
Serial.print("TA=");
Serial.print(mlx.readAmbientTempC());
Serial.print(",CPIX=NA,PTAT=NA,EMISSIVITY=1.00,V_TH=NA,K_T1=NA,K_T2=NA,");
Serial.print("MY_TEMP=");
Serial.print(DS18B20_temp);
Serial.println(",IREND");

delay(100);

// DATA FORMAT MUST BE IN THIS FORMAT FOR THE GarageProto MELEXIS SENSOR APP TO PARSE THE DATA
//
// IRSTART
// MLX90614ESF-BAC
// 20.23
// TA=20.97,CPIX=NA,PTAT=NA,EMISSIVITY=1.00,V_TH=NA,K_T1=NA,K_T2=NA,MY_TEMP=22.81,END

}

// ####################################################################################################################################
void getMLX90614Temp() {
  //Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
  //Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
  //Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
  //Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");

  //Serial.println();
  //delay(100);
}

// ####################################################################################################################################
void calculate_DS18B20(){

  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  //byte data[9];
  byte addr[8];
  float celsius, fahrenheit;
  
  if ( !ds.search(addr)) {
    ///Serial1111.println("No more addresses.");
    ///Serial1111.println();
    ds.reset_search();
    delay(1);
    return;
  }
  for( i = 0; i < 8; i++) {
      }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      ///Serial1111.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(10);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    
      }
  
  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  DS18B20_temp = celsius;
  //Serial1111.print("  Temperature = ");
  //Serial1111.print(celsius);
  //Serial1111.print(" Celsius, ");
  //Serial1111.print(fahrenheit);
  //Serial1111.println(" Fahrenheit");
  
}

// ####################################################################################################################################
