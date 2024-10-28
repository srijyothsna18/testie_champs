#include "temp.h"

//We will assume that, temp value will be updated in this value.
volatile int temp_value = 35;


int temp_read( void )
{
       return temp_value;
}
