#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <memory.h>
typedef struct
{
    uint8_t ID;
    uint32_t Etime;
    float coord[2];
    float dop[3];
    uint8_t signal_type;
    uint8_t num_sats;
    char temp[10];

}Packet;
char* data;
char* dirpath = "C:\\Users\\user\\Documents\\GitHub\\BouyDev\\Tests\\Transmission Tests\\Data\\";
char data_buffer[1024];
uint8_t getByte(char* msg);
void decode_Message(char* payload);
float get_float(unsigned char* bytestring);
void float_to_hex(float f,unsigned char* hexstring);
float IEEE754_Convert(uint32_t sign, uint32_t exp, uint32_t mant);
Packet packet;
int main()
{
        FILE* filepointer;
        FILE* writefilepointer;
        //open file in read mode
        char* filename = "Log_1.csv"; //file to read from
        char* write_file = "Data.csv"; //file to write to
        int flen = strlen(filename);
        int dirlen = strlen(dirpath);
        int wflen = strlen(write_file);
        char path[dirlen+flen+1];
        char writepath[dirlen+wflen+1];
        sprintf(path,"%s%s\0",dirpath,filename);
        sprintf(writepath,"%s%s\0",dirpath,write_file);

        //check if write file exists
        writefilepointer = fopen(writepath,"r");
        if(writefilepointer == NULL)
        {
            printf("Write file does not exist, creating file...\n");
            writefilepointer = fopen(writepath,"w");
            char headings[1000];
            sprintf(headings,"Packet Type,Packet Number,Time,Latitude,Longitude,HDOP,VDOP,PDOP,Signal Type,Num Satelites,Temperature\n");
            fputs((char*)headings,writefilepointer);

        }else
        {
            fclose(writefilepointer);
            printf("File Exists. Appending To file");
            writefilepointer = fopen(writepath,"a");
        }
        filepointer = fopen((char*)path,"rt");
        if(filepointer == NULL)
        {
            printf("\nError opening File: please check if file exists or is not currently in use.");
            return 0;
        }
        printf("Successfully opened file: %s\n",filename);
        //read untill end of file
       char line[241];
       fgets(line,113,filepointer);
       while(fgets(line,241,filepointer) != NULL)
       {
         char* data = strtok(line,",");
         for(int i = 0; i < 3;i++)
         {
             data = strtok(NULL,",");
         }
         //split into bytes of 25
         char payload[51]={0};
         for(int i = 0;i <4;i++)
         {
            strncpy(payload,&data[50*i],50);
            payload[51] = 0;
            decode_Message(payload);
            printf("%s",(char*)data_buffer);
            //save to file
            fputs(data_buffer,writefilepointer);
         }
            //chop off the rest

       }
       fclose(writefilepointer);
       fclose(filepointer);
    return 0;
}

void decode_Message(char* payload)
{
    /* Isolate ID */
    packet.ID = getByte(payload);
    payload+=2;
    /* Epoch Time */
    uint32_t time = 0;
    for (int i = 0; i < 4; ++i)
    {
       uint8_t byte= getByte(payload);
       time |= byte<<8*(3-i);
       payload+=2;
    }
    packet.Etime = time;
    /* Coordinates*/
    unsigned char byte[4];
    for (int k = 0; k < 2; ++k)
    {
        for (int j = 0; j < 4; ++j)
        {
            byte[j] = getByte(payload);
            payload+=2;
        }
        packet.coord[k] = get_float(byte);
    }
    /* Dilation of Precision*/

    for (int l = 0; l < 3; ++l)
    {
        packet.dop[l] = getByte(payload);
        payload+=2;
        packet.dop[l] += ((float)getByte(payload))/1000;
        payload+=2;
    }
    uint8_t signal = getByte(payload);
    payload+=2;
    packet.num_sats = (signal&0b11110000)>>2;
    packet.signal_type = (signal&0b11);
    int8_t t = (float)getByte(payload); //TODO: figure out how temp is converted and stored
    payload+=2;
    uint8_t TH = getByte(payload);
    payload+=2;
    uint8_t TL = getByte(payload);
    uint16_t T_temp = (TH<<8)|TL;
    sprintf(packet.temp,"%d.%d",t,T_temp);
    sprintf(data_buffer,"GPS,%d,%d,%f,%f,%0.2f,%0.2f,%0.2f,%d,%d,%s\n",packet.ID,packet.Etime,packet.coord[0],packet.coord[1],packet.dop[0],packet.dop[1],packet.dop[2],packet.signal_type,packet.num_sats,packet.temp);
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
