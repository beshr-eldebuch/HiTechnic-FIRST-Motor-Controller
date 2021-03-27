/// Author: Beshr Eldebuch

#include <Arduino.h>
#include <Wire.h>
#include "motor_controller.h"
#include <stdint.h>

   //I2C device address -- this could be found by running "sudo i2cdetect" in the terminal 
   MotorController::MotorController(uint8_t address)
    {
        this->BASE_ADDRESS = address;
        // reset();
    }

    void MotorController::reset ()
    {
        Wire.beginTransmission(this->BASE_ADDRESS);
        Wire.write ( MODE_FIRST_MOTOR );
        byte data [4] = {3, 0, 0, 3};
        Wire.write(data, 4); //array, length
        Wire.endTransmission();
        delay(250);
    }

    ///returns "HITECHNIC "
    char* MotorController::get_manufacturer()
    {
        Wire.beginTransmission(this->BASE_ADDRESS);
        Wire.write(MANUFACTURER);
        Wire.endTransmission();
        
        char manufacturer[8];
        Wire.requestFrom(this->BASE_ADDRESS, 8); 
        
        uint8_t i =0;
        while(Wire.available() )
        {
            manufacturer[i] = Wire.read();
            i++;
        }
         
        return manufacturer;
    }

    void MotorController::move_constant_speed( int8_t power1, int8_t power2)
    {
        Wire.beginTransmission(this->BASE_ADDRESS);
        Wire.write(0x44);
        power1 = constrain(power1, -100, 100);
        power2 = constrain(power2, -100, 100);
        //NTO bit = 16 just add it  to 1
        byte data [4] = {1,  power1, power2, 1};
        Wire.write(data, 4); //array, length
        Wire.endTransmission();
        delay(10);
    }
    
    /// move to "target" (degrees) at speed "power"
    void MotorController::move_run_to_position(uint8_t power1, int32_t target1, uint8_t power2, int32_t target2)
    {
        power1 = abs(power1);
        power2 = abs(power2);
        // since every 1440 encoder value is 360 degree
        target1 *= 4;
        target2 *= 4;
        byte data[12]={};

        for (int i=0; i<4 ;i++)
        {
            data[3-i]  = target1 >> i*8;
            data[11-i] = target2 >> i*8;
        }
        data[4] = 2; //run to pos MODE_FIRST_MOTOR
        data[5] = power1;
        data[6] = power2;
        data[7] = 2;

        Wire.beginTransmission(this->BASE_ADDRESS);
        Wire.write(TARGET_FIRST_MOTOR);
        
        ///Only one motor !!!!!
        Wire.write(data, 12);
        Wire.endTransmission();
        delay(10);
    }
    void MotorController::move_run_to_position_first_motor(uint8_t power, int32_t target)
    {
        power = abs(power);
        // since every 1440 encoder value is 360 degree
        target *= 4;
        byte data[6];

        for (int i=0; i<4 ;i++)
        {
            data[3-i]  = target >> i*8;
        }
        data[4] = 2; //run to pos MODE_FIRST_MOTOR
        data[5] = power;

        Wire.beginTransmission(this->BASE_ADDRESS);
        Wire.write(TARGET_FIRST_MOTOR);        
        Wire.write(data, 6);
        Wire.endTransmission();
        delay(10);
    }
        void MotorController::move_run_to_position_second_motor(uint8_t power, int32_t target)
    {
        power = abs(power);
        // since every 1440 encoder value is 360 degree
        target *= 4;
        byte data[6]={};

        for (int i=0; i<4 ;i++)
        {
            data[5-i] = target >> i*8;
        }
        data[0] = power;
        data[1] = 2;

        Wire.beginTransmission(this->BASE_ADDRESS);
        Wire.write(TARGET_SECOND_MOTOR);
        
        Wire.write(data, 6);
        Wire.endTransmission();
        delay(10);
    }

    ///power mode only (no encoders are considered here)
    void MotorController::move_power(int8_t power1, int8_t power2)
    {
        uint8_t data [4] = { 0, power1, power2, 0};
        
        Wire.beginTransmission(this->BASE_ADDRESS);
        Wire.write(MODE_FIRST_MOTOR);
        Wire.write(data, 4);  //array, length
        Wire.endTransmission();
        delay(10);
    }   

    ///returns true if the motor is doing something
    bool MotorController::is_busy ( uint8_t motor_number)
    {
        //channel_MODE_FIRST_MOTOR = []
        uint8_t channel_MODE_FIRST_MOTOR;    
        if(motor_number == 1)
        {
            Wire.beginTransmission(this->BASE_ADDRESS);
            Wire.write(MODE_FIRST_MOTOR);
            channel_MODE_FIRST_MOTOR = Wire.read(); //array, length
            Wire.endTransmission();
        }
        else if  (motor_number == 2)
        {
            Wire.beginTransmission(this->BASE_ADDRESS);
            Wire.write(0x47);
            channel_MODE_FIRST_MOTOR = Wire.read(); //array, length
            Wire.endTransmission();
        }
        else
        {
        //   raise ("Should be 1 or 2");
        }
        delay(0.01);
        
        bool busy = channel_MODE_FIRST_MOTOR >> 7;
        return busy;
    }
    
    /// this will stop the controller untill "busy" bit is false
    void MotorController::wait_until_busy()
    {
        while(is_busy(1) || is_busy(2))
        {
            //DoNothing -- Just wait
        }
    }

    ///returns the encoder value for a given motor number (1 or 2)
    int32_t MotorController::get_current_encoder_value(uint8_t motor_number)
    {
        // returns a list of 4 bytes
        uint8_t current_first_motor_encoder [4];
        if (motor_number == 1)
        {
            Wire.beginTransmission(this->BASE_ADDRESS);
            Wire.write(CURRENT_FIRST_MOTOR);
            for (int i=0;i<3;i++)
                current_first_motor_encoder[i] = Wire.read(); //array, length
            
            Wire.endTransmission();
        }
        else if (motor_number == 2)
        {
            Wire.beginTransmission(this->BASE_ADDRESS);
            Wire.write(0X50);
            for (int i=0;i<3;i++)
                current_first_motor_encoder[i] = Wire.read(); //array, length
            Wire.endTransmission();
           
        }
        // else
            // raise UserWarning("Motor number should be 1 or 2")

        uint32_t x = 0 ;
        for (int8_t i=0, j=0; i<4; i++)
        {
            x |= current_first_motor_encoder[i];
            j++;
            if (j<4)
                x = x << 8 ;
        }
        
        return int (x/4);
    }
