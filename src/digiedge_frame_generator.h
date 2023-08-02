#ifndef DIGIEDGE_FRAME_GENERATOR_H
#define DIGIEDGE_FRAME_GENERATOR_H
#include <Arduino.h>

using namespace std;

#define SFD 0x01

#define EOF_MSB 0x0D
#define EOF_LSB 0x0A

#define RSSI_TYPE 0x04
#define BATTERY_TYPE 0x05

#ifndef DEV_ADD_TYPE
#define DEV_ADD_TYPE 0x01
#endif

#ifndef DEV_EUI_TYPE
#define DEV_EUI_TYPE 0x02
#endif

#ifndef APP_EUI_TYPE
#define APP_EUI_TYPE 0x03
#endif


struct Base
{
    byte type;
    byte size;
    byte* value;
};
typedef struct Base Base;

struct Header
{
    uint16_t size;
    Base DevADD;
    Base DevEUI;
    Base AppEUI;
    Base* Info;
};



typedef struct Header Header;

Base* AddElementToArray(Base* array, uint16_t size, Base element);
void Header_init(Header *header, byte* devAddVal, byte* devEuiVal,byte* appEuiVal,byte devAddType = DEV_ADD_TYPE, byte devEuiType = DEV_EUI_TYPE,
                 byte appEuiType = APP_EUI_TYPE);
byte* SendPayload(float payload_, const Header& header) ;
byte* CaclulateHeaderSize(const Header header);
byte* CalculatePayloadSize(const byte* payload, uint16_t size_);
void AppendInHeader(Header* header, byte type, byte* value, byte size);
byte calculateCRC8(const byte *data, uint16_t size);
byte CalculateSize(const byte* data);
uint16_t frame_size(float payload_, Header header);
byte* floatToByteArray(float number);
uint16_t byteArrayToUint16(const byte* byteArray);
#endif /*DIGIEDGE_FRAME_GENERATOR_H*/