#define DEV_ADD_TYPE 0xFF
#define DEV_EUI_TYPE 0xFE
#define APP_EUI_TYPE 0xFD

#include "digiedge_frame_generator.h"

void setup() {
  Serial.begin(9600);   
  // Creat a Header
  Header header;
  byte dev_add[] = {0xA2, 0xB2, 0xC2, 0xD2};
  byte dev_eui[] = {0xA2, 0xB3, 0xC4, 0xD5, 0xE6, 0xF7, 0xA8, 0xB9};
  byte app_eui[] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};

  //Initiate header with the following fields DevADD, DevEUI and AppEUI
  Header_init(&header, dev_add, dev_eui, app_eui);

   //we will append for exemple RSSI data to header
   // 1. creat an array holds the data
   byte rssi_value[] = {0x63, 0x66, 0x75}; //here we just have one byte
   // 2. then a varuable contain the type
   byte rssi_type = RSSI_TYPE;
   AppendInHeader(&header,rssi_type, rssi_value, sizeof(rssi_value));
  
   // the type can be costumized
   // 1. creat an array holds the data
   byte rand_value[] = {0x63, 0x75}; //here we just have one byte
   // 2. then a varuable contain the type
   AppendInHeader(&header,0x09, rand_value, sizeof(rand_value));

   //we will append for exemple Battery data to header
   // 1. creat an array holds the data
   byte battery_value[] = {0x60, 0x62}; //here we just have two bytes
   // 2. then a varuable contain the type
   byte battery_type = BATTERY_TYPE;
   AppendInHeader(&header, battery_type, battery_value, sizeof(battery_value));

  //then Send the Payload 
   float data = 12.10f;
   uint16_t n  = frame_size(data, header); 
   byte * f = GenerateFrame(data, header);
   for(int i = 0; i<n;i++){
      Serial.print(f[i], HEX);Serial.print(" ");
   }

}
void loop() {
}
