#include "CException.h"
#include "build/test/mocks/mock_temp.h"
#include "src/bit.h"
#include "/var/lib/gems/2.7.0/gems/ceedling-0.31.1/vendor/unity/src/unity.h"








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





void test_bit(void)

{

 int8_t result;



 temp_read_CMockExpectAndReturn(30,10);

 result = check_bit( 2 );

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-1)), (UNITY_INT)(UNITY_INT8 )((result)), (

  ((void *)0)

  ), (UNITY_UINT)(32), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0x00)), (UNITY_INT)(UNITY_INT8 )((a)), (

  ((void *)0)

  ), (UNITY_UINT)(33), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0xFF)), (UNITY_INT)(UNITY_INT8 )((b)), (

  ((void *)0)

  ), (UNITY_UINT)(34), UNITY_DISPLAY_STYLE_INT8);

 UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0x00)), (UNITY_INT)(UNITY_INT8 )((c)), (

((void *)0)

), (UNITY_UINT)(35), UNITY_DISPLAY_STYLE_INT8);



}







void test_bit_1(void)

{

   int8_t result;



 temp_read_CMockExpectAndReturn(45,35);

 result = check_bit( 5 );

  UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((-1)), (UNITY_INT)(UNITY_INT8 )((result)), (

 ((void *)0)

 ), (UNITY_UINT)(47), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0x00)), (UNITY_INT)(UNITY_INT8 )((a)), (

  ((void *)0)

  ), (UNITY_UINT)(48), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0xFF)), (UNITY_INT)(UNITY_INT8 )((b)), (

  ((void *)0)

  ), (UNITY_UINT)(49), UNITY_DISPLAY_STYLE_INT8);

   UnityAssertEqualNumber((UNITY_INT)(UNITY_INT8 )((0x00)), (UNITY_INT)(UNITY_INT8 )((c)), (

  ((void *)0)

  ), (UNITY_UINT)(50), UNITY_DISPLAY_STYLE_INT8);

}
