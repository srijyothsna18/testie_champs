#include "bit_manipulation.h"
#include "temp.h"

uint8_t a = 0x00;
uint8_t b = 0xFF;
uint8_t c = 0x00;



int8_t check_bit (int8_t pos)
{
    if( ( pos < 0 ) || ( pos > 7 ) ) 
    {
        //pos should be 0 to 7. Because we are going to modify 8 bit value.
        return -1;
    }
    
    
    int res = temp_read();
    if(res >= 30)
    {
	    printf("temperature is high\n");
    }
    else
    {
	    printf("temperature is low\n");
    }
    
    return 0;
}
