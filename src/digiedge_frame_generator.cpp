#include "digiedge_frame_generator.h"

Base* AddElementToArray(Base* array, byte size, Base element) {
    Base* newArray = new Base[size + 1];  // Allocate memory for the expanded array

    if(size == 0){
        newArray[0] = element;
        return newArray;
    }
    else {
        // Copy existing elements to the new array
        for (byte i = 0; i < size; i++) {
            newArray[i] = array[i];
        }

        newArray[size] = element;  // Add the new element at the end

        delete[] array;  // Free memory allocated for the old array

        return newArray;
    }
}

void Header_init(Header *header, byte* devAddVal, byte* devEuiVal, byte* appEuiVal, byte devAddType, byte devEuiType, byte appEuiType) {
    // DevADD Field    
    header->DevADD.type = devAddType;
    header->DevADD.size = 0x04;
    header->DevADD.value = new byte[header->DevADD.size];
    memcpy(header->DevADD.value, devAddVal, header->DevADD.size);

    // DevEUI Field
    header->DevEUI.type = devEuiType;
    header->DevEUI.size = 0x08;
    header->DevEUI.value = new byte[header->DevEUI.size];
    memcpy(header->DevEUI.value, devEuiVal, header->DevEUI.size);

    // AppEUI Field
    header->AppEUI.type = appEuiType;
    header->AppEUI.size = 0x08;
    header->AppEUI.value = new byte[header->AppEUI.size];
    memcpy(header->AppEUI.value, appEuiVal, header->AppEUI.size);
    
    header->Info = nullptr;
    header->size = 6 + header->DevADD.size + header->DevEUI.size + header->AppEUI.size;
}

byte CalculateSize(const byte* data) {
    byte size = 0;
    while (data[size] != 0) {
        size++;
    }
    return size;
}


byte counter = 0;

void AppendInHeader(Header* header, byte type, byte* value, byte size){
    Base temp;
    temp.type = type;
    temp.size = size;
    temp.value = new byte[temp.size];
    memcpy(temp.value, value, temp.size);
    header->Info = AddElementToArray(header->Info, counter, temp);
    counter++;
    header->size += 2 + temp.size;
}

byte* CaclulateHeaderSize(const Header header){
    byte* size = new byte[2];
    size[0] = header.size & 0xFF;        // LSB
    size[1] = (header.size >> 8) & 0xFF; // MSB
    return size;
}

byte* CalculatePayloadSize(const byte* payload , uint16_t size_){
    byte* size = new byte[2];
    size[0] = size_ & 0xFF;         // Extract the least significant byte
    size[1] = (size_ >> 8) & 0xFF; // Extract the most significant byte
    return size;
}

byte calculateCRC8(const byte *payload, uint16_t size) {
    uint16_t dataSize = size;
    if (payload == nullptr)
    {
        // handle null input
        return 0;
    }

    const byte POLYNOM_VALUE = 0x07;
    byte crc = 0;

    for (byte i = 0; i < dataSize; i++)
    {
        crc ^= payload[i];
        for (byte j = 0; j < 8; j++)
        {
            if (crc & 0x80)
            {
                crc = (crc << 1) ^ POLYNOM_VALUE;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}

uint16_t byteArrayToUint16(const byte* byteArray) {
    uint16_t result = 0;
    result |= static_cast<uint16_t>(byteArray[0]);
    result |= static_cast<uint16_t>(byteArray[1]) << 8;
    return result;
}

uint16_t frame_size(float payload_, Header header) {
    byte* payload = floatToByteArray(payload_);
    byte* PayloadSize = CalculatePayloadSize(payload, 4);
    uint16_t PSize = byteArrayToUint16(PayloadSize);
    return 5 + header.size + PSize + 1 + 2;
}

byte* floatToByteArray(float number)
{
    uint32_t floatAsInt;
    memcpy(&floatAsInt, &number, sizeof(floatAsInt));

    byte* byteArray = new byte[sizeof(floatAsInt)];
    memcpy(byteArray, &floatAsInt, sizeof(floatAsInt));
    // Swap the byte order
    for (size_t i = 0, j =  sizeof(floatAsInt) - 1; i < j; ++i, --j)
    {
        byte temp = byteArray[i];
        byteArray[i] = byteArray[j];
        byteArray[j] = temp;
    }

    return byteArray;
}

byte* GenerateFrame(float payload_, const Header& header) {
    byte* payload = floatToByteArray(payload_);
    uint16_t FrameSize = frame_size(payload_, header);
    byte* frame = new byte[FrameSize];
    byte StartFrameDelimiter = SFD;
    byte* HeaderSize = CaclulateHeaderSize(header);
    byte HS_MSB = HeaderSize[1];
    byte HS_LSB = HeaderSize[0];
    byte* PayloadSize = CalculatePayloadSize(payload, 4);
    byte DS_MSB = PayloadSize[1];
    byte DS_LSB = PayloadSize[0];
    byte DevAdd_type = header.DevADD.type;
    byte DevAdd_size = header.DevADD.size;
    byte* DevAdd_val = new byte[DevAdd_size];
    for (byte i = 0; i < DevAdd_size; i++) {
        DevAdd_val[i] = header.DevADD.value[i];
    }
    byte DevEUI_type = header.DevEUI.type;
    byte DevEUI_size = header.DevEUI.size;
    byte* DevEUI_val = new byte[DevEUI_size];
    for (byte i = 0; i < DevEUI_size; i++) {
        DevEUI_val[i] = header.DevEUI.value[i];
    }
    byte AppEUI_type = header.AppEUI.type;
    byte AppEUI_size = header.AppEUI.size;
    byte* AppEUI_val = new byte[AppEUI_size];
    for (byte i = 0; i < AppEUI_size; i++) {
        AppEUI_val[i] = header.AppEUI.value[i];
    }

    byte values_taille = header.size - 26;
    byte* Extra_info = new byte[values_taille]; // 256 - 31 - 1 - 2 = 222
    byte acc = 0;
    for (byte i = 0; i < counter; i++) {
        Extra_info[i*acc] = header.Info[i].type;
        Extra_info[i*acc +1] = header.Info[i].size;
        acc+=2;
        for (byte j = 0; j < header.Info[i].size; j++) {
            Extra_info[acc + j] = header.Info[i].value[j];
        }
        acc+= header.Info[i].size;
    }
    byte CRC = calculateCRC8(payload, 4);
    byte EndOfFrame_MSB = EOF_MSB;
    byte EndOfFrame_LSB = EOF_LSB;

    // Populate the frame array with the bytes
    frame[0] = StartFrameDelimiter;
    frame[1] = HS_MSB;
    frame[2] = HS_LSB;
    frame[3] = DS_MSB;
    frame[4] = DS_LSB;
    frame[5] = DevAdd_type;
    frame[6] = DevAdd_size;
    for (byte i = 0; i < DevAdd_size; i++) {
        frame[7 + i] = DevAdd_val[i];
    }
    frame[7 + DevAdd_size] = DevEUI_type;
    frame[8 + DevAdd_size] = DevEUI_size;
    for (byte i = 0; i < DevEUI_size; i++) {
        frame[9 + DevAdd_size + i] = DevEUI_val[i];
    }
    frame[9 + DevAdd_size + DevEUI_size] = AppEUI_type;
    frame[10 + DevAdd_size + DevEUI_size] = AppEUI_size;
    for (byte i = 0; i < AppEUI_size; i++) {
        frame[11 + DevAdd_size + DevEUI_size + i] = AppEUI_val[i];
    }
    byte acc_1 = 0;
    // Add Extra_info bytes
    for (byte i = 0; i < counter; i++) {
        frame[11 + DevAdd_size + DevEUI_size + AppEUI_size +acc_1] = Extra_info[acc_1*i];
        frame[11 + DevAdd_size + DevEUI_size + AppEUI_size + acc_1 + 1] = Extra_info[acc_1*i  + 1];
        acc_1+= 2;
        for (byte j = 0; j < header.Info[i].size; j++) {
            frame[11 + DevAdd_size + DevEUI_size + AppEUI_size + acc_1 + j] = Extra_info[acc_1 + j];
        }
        acc_1+= header.Info[i].size;
    }
    // Add the payload
    uint16_t PL_sz = byteArrayToUint16(PayloadSize);
    for(uint16_t i = 0; i<PL_sz;i++){
        frame[11 + DevAdd_size + DevEUI_size + AppEUI_size + acc_1  +i] = payload[i];
    }
    
    // Add remaining bytes
    frame[11 + DevAdd_size + DevEUI_size + AppEUI_size + acc_1  + PL_sz] = CRC;
    frame[12 + DevAdd_size + DevEUI_size + AppEUI_size + acc_1  + PL_sz] = EndOfFrame_MSB;
    frame[13 + DevAdd_size + DevEUI_size + AppEUI_size + acc_1  + PL_sz] = EndOfFrame_LSB;

    return frame;
}
