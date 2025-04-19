#include <Arduino.h>
#include <unity.h>
#include "../src/pHControl.h"
#ifdef UNIT_TEST

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

/*class int32_pe
{
public:
    int32_t val;
    void Init(int32_t v)
    {
        val = v;
    }
    void SetVal(int32_t v)
    {
        val = v;
    }
    int32_t GetVal(void)
    {
        return val;
    }
};
*/
 EEPROMClass param::eep;
 int32_pe pumpOffTime(1, 100); // minut
 int32_pe pHDownDose(1, 100);  // mililitru/m3 na 1pH
 int32_pe pHUPDose(1, 100);    // mililitru/m3 na 1pH

 int32_pe poolVol(1, 100); // metry krychlove

 int32_pe phSetPoint(1, 140);
 int32_pe phAutoCtrlD(0,1);
 int32_pe phAutoCtrlU(0,1);

void test_pHControl_testInit(void)
{
    pHControl::Init();
    TEST_ASSERT_EQUAL(30, pumpOffTime.GetVal());
TEST_ASSERT_EQUAL(30, pHDownDose.GetVal());
TEST_ASSERT_EQUAL(30, pHUPDose.GetVal());
TEST_ASSERT_EQUAL(70, phSetPoint.GetVal());
TEST_ASSERT_EQUAL(0, phAutoCtrlD.GetVal());
TEST_ASSERT_EQUAL(0, phAutoCtrlU.GetVal());
TEST_ASSERT_EQUAL(10, poolVol.GetVal());
}

void setup()
{
    // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);

    UNITY_BEGIN(); // IMPORTANT LINE!
}

uint8_t i = 0;
uint8_t max_blinks = 5;

void loop()
{
    RUN_TEST(test_pHControl_testInit);
    UNITY_END(); // stop unit testing
}
#endif