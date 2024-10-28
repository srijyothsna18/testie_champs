#ifdef TEST

#include "unity.h"

#include "bit_manipulation.h"
#include "temp.h"
#include "mock_temp.h"

extern uint8_t a; 
extern uint8_t b; 
extern uint8_t c;

void setUp(void)
{
	a=0x00;
	b=0xFF;
	c=0x00;
}

void tearDown(void)
{
}

//Test Case 0
void test_bit(void)
{
	int8_t result;

	temp_read_CMockExpectAndReturn(__LINE__,10);
	result = check_bit( 5 );
  	TEST_ASSERT_EQUAL_INT8( -1, result );
  	TEST_ASSERT_EQUAL_INT8( 0x00, a );
  	TEST_ASSERT_EQUAL_INT8( 0xFF, b );
	TEST_ASSERT_EQUAL_INT8( 0x00, c );
    
}


//Test Case 1 
void test_bit_1(void) 
{
  	int8_t result;
  	
	temp_read_CMockExpectAndReturn(__LINE__,35);
	result = check_bit( 5 );
 	TEST_ASSERT_EQUAL_INT8( -1, result );
  	TEST_ASSERT_EQUAL_INT8( 0x00, a );
  	TEST_ASSERT_EQUAL_INT8( 0xFF, b );
  	TEST_ASSERT_EQUAL_INT8( 0x00, c );
}

/*
//Test Case 2 
void test_bit_2(void) 
{
  	int8_t result;
  	int8_t position = -5;
  	result = check_bit( position );
  	TEST_ASSERT_EQUAL_INT8( 0, result );
  	TEST_ASSERT_BIT_HIGH( position, a );
  	TEST_ASSERT_BIT_LOW( position, b );
  	TEST_ASSERT_BIT_HIGH( position, c );
}
*/

#endif // TEST
