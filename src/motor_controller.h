/// Author: Beshr Eldebuch

#ifndef MotorController_H_ 
#define MotorController_H_
#include <stdint.h>
class MotorController
{
private:
    #define MANUFACTURER  0x08
    #define MODE_FIRST_MOTOR  0X44 
    #define POWER_FIRST_MOTOR  0X45
    #define CURRENT_FIRST_MOTOR  0X4C
    #define TARGET_FIRST_MOTOR  0X40
    #define TARGET_SECOND_MOTOR  0X46

    #define MODE_SECOND_MOTOR  0X47
        
    uint8_t BASE_ADDRESS;
public:
    MotorController (uint8_t address);
    void reset ();
    char* get_manufacturer();
    void move_constant_speed( int8_t power1, int8_t power2);
    void move_run_to_position( uint8_t power1, int32_t target1, uint8_t power2,int32_t  target2 );
    void move_run_to_position_first_motor( uint8_t power, int32_t target);
    void move_run_to_position_second_motor( uint8_t power, int32_t target);
    void move_power( int8_t, int8_t);
    bool is_busy ( uint8_t);
    void wait_until_busy();
    int32_t get_current_encoder_value(uint8_t);
};

#endif