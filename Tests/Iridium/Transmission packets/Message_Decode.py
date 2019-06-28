import ctypes

def convert(num_string):
    temp = int(num_string, 16)
    sign = (temp&(1<<31))>>31
    exp  = (temp&0b11111111 << 23) >> 23
    mant = ""+str(temp&0b11111111111111111111111)

    vale = pow(2,exp- 127)
    valmant = 0.0
    for i in range (0,len(mant)):
        valmant += int(mant[i:i+1])*pow(2,-i)



def decode_message(message):
    # Byte 0: ID
    msgId = int(message[0:2], 16)
    print(msgId)
    # bytes 1-4: EpochTime
    eTime = int(message[2:10], 16)
    print(eTime)
    # bytes 5-9 latitude
    convert(message[10:18])




f = open("messages-1561712428712.csv", 'r')
f.readline()
line = f.readline().split(',')
message = line[3]
decode_message(message)
