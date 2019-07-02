#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
char* payload ="015d1a13494fd751c57675e444030002f0011f1f1866031d0000";
uint8_t getByte(char* msg);
float get_float(unsigned char* bytestring);
void float_to_hex(float f,unsigned char* hexstring);
float IEEE754_Convert(uint32_t sign, uint32_t exp, uint32_t mant);
int main()
{
    /* Isolate ID */
    uint8_t id = getByte(payload);
    payload+=2;
    /* Epoch Time */
    uint32_t time = 0;
    for (int i = 0; i < 4; ++i)
    {
       uint8_t byte= getByte(payload);
       time |= byte<<8*(3-i);
       payload+=2;
    }
    /* Coordinates*/
    unsigned char byte[4];
    float coord[2];
    for (int k = 0; k < 2; ++k)
    {
        for (int j = 0; j < 4; ++j)
        {
            byte[j] = getByte(payload);
            payload+=2;
        }
        coord[k] = get_float(byte);
    }
    /* Dilation of Precision*/
    float pdop[3];
    for (int l = 0; l < 3; ++l)
    {
        pdop[l] = getByte(payload);
        payload+=2;
        pdop[l] += ((float)getByte(payload))/1000;
        payload+=2;
    }
    uint8_t signal = getByte(payload);
    payload+=2;
    uint8_t num_sats = (signal&0b11110000)>>2;
    uint8_t signal_type = (signal&0b11);
    float v_bat = (float)getByte(payload);
    payload+=2;
    v_bat += (float)getByte(payload)/1000;
    payload+=2;
    float temp = (float)getByte(payload); //TODO: figure out how temp is converted and stored
    temp+=(float)getByte(payload)/1000;
    printf("Packet ID: %d\n",id);
    printf("Epoch Time: %d\n",time);
    printf("Coordinates: Lat = %0.8f Long = %0.8f\n",coord[0],coord[1]);
    printf("DIAGNOSTICS:\nHDOP = %0.2f PDOP = %0.2f VDOP = %0.2f\n",pdop[0],pdop[1],pdop[2]);
    printf("Number of Satelites = %d Signal type = %d\n",num_sats,signal_type);
    printf("Ambient Temperature: %0.2f C\n",v_bat);
    printf("Battery Voltage: %0.2f V\n",temp);

    return 0;
}

uint8_t getByte(char* msg)
{
    uint8_t byte = 0;
    for (int i = 0; i < 2; ++i)
    {
        if((msg[i]>= '0') &&(msg[i] <= '9'))
        {
            byte+= (msg[i]-'0');
        }
        if((msg[i]>='a') &&(msg[i] <= 'f'))
        {
            byte+= (msg[i]-87);
        }
        byte = byte<<4*(1-i);
    }
    return byte;
}

float get_float(unsigned char* bytestring)
{
    uint32_t num = 0;
    for (int i = 0; i < 4; ++i)
    {
        num = (num<<8) | bytestring[3-i];
    }
    // sign
    uint32_t sign = (num&(0b1<<31));
    num-= sign;
    sign = sign>>31;
    uint32_t exp = num&(0xFF<<23);
    uint32_t mant = (num-exp);
    exp = exp>>23;

    float temp = IEEE754_Convert(sign,exp,mant);
    return temp;
}

void float_to_hex(float f,unsigned char* hexstring)
{
    union {
         unsigned char * byte[4];
        float f;
    }floatasval;
    floatasval.f =f;
   memcpy(hexstring,floatasval.byte, sizeof(float));
}

float IEEE754_Convert(uint32_t sign, uint32_t exp, uint32_t mant)
{
    float temp = 1;
    for (int i = 0; i < 23; ++i)
    {
       //get bit
       uint32_t mask = 1<<(22-i);
       uint8_t bit = (mant & mask)>>(22-i);
       temp += ((float)bit)*(powf(2,-(i+1)));
    }
    float s = 1-2*((float) sign);
    float e = powf(2,exp-127);
    return s*e*temp;
}