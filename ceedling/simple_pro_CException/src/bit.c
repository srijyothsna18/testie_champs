#include <stdint.h>   // Include for uint8_t and int8_t
#include <stdio.h>    // Include for printf

#include "CExceptionConfig.h"
#include "CException.h"
#include "temp.h"


uint8_t a;  // Assuming this variable is defined elsewhere
uint8_t b;
uint8_t c;

int8_t check_bit(int position) {
    volatile CEXCEPTION_T e;

    StartTryHook();
    
    Try {
        if (position < 0 || position >= 8) {
            Throw(ERR_BIT_ERROR);  // Throw if bit position is invalid
        }

        int temperature = temp_read();  // Assuming temp_read() is defined elsewhere

        if (temperature < 15) {
            Throw(ERR_TEMP_LOW);  // Throw if temperature is too low
        } else if (temperature > 30) {
            Throw(ERR_TEMP_HIGH); // Throw if temperature is too high
        }

        return (a & (1 << position)) ? 1 : 0;  // Return the bit value
	HappyTryHook();

    } Catch(e) {
	StartCatchHook();
        switch (e) {
            case ERR_TEMP_LOW:
                printf("Error: Temperature too low!\n");
                return -1;
            case ERR_TEMP_HIGH:
                printf("Error: Temperature too high!\n");
                return -1;
            case ERR_BIT_ERROR:
                printf("Error: Invalid bit position!\n");
		return -1;
            default:
                printf("Error: Unknown exception caught!\n");
                return -1;
        }
    } 
    AfterTryHook();
   
}

