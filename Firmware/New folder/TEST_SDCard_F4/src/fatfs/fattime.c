/* Martin Thomas 4/2009 */

#include "stm32f4xx.h"
#include "fattime.h"

DWORD get_fattime(void) {
  int32_t year, month, day, hour, minute, second;
  year = 2018;
  month = 3;
  day = 1;
  hour = 00;
  minute = 00;
  second = 00;
  return (((year - 1980) & 0x7F) << 25) + ((month & 0x0F) << 21) + ((day & 0x1F) << 16) + ((hour & 0x1F) << 11) + ((minute & 0x3F) << 5)
      + ((second & 0x1F) >> 1);
}

