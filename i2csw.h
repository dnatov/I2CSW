//
// Created by dnatov on 9/8/2021.
//

#ifndef I2CSW_I2CSW_H
#define I2CSW_I2CSW_H

#include "stdint.h"
#include "stdbool.h"

/// Gets the defined SDA pin
/// \param pin the pin to set or clear
typedef bool (*I2C_BitBang_GetSdaMethod)();

/// Sets or Clears the defined pin
/// \param pin the pin to set or clear
/// \param set true = set, false = clear
typedef void (*I2C_BitBang_ClearSetSCLPinMethod)(bool set);

/// Sets or Clears the defined pin
/// \param pin the pin to set or clear
/// \param set true = set, false = clear
typedef void (*I2C_BitBang_ClearSetSDAPinMethod)(bool set);

/// Gets the defined SCL pin
/// \param pin the pin to set or clear
typedef bool (*I2C_BitBang_GetSclMethod)(uint16_t pin);

/// Simple delay method
typedef void (*I2C_BitBang_DelayMethod)(void);

struct I2C_BitBang_Interface_t
{
    I2C_BitBang_ClearSetSCLPinMethod ClearSetSCLPinMethodPtr;
    I2C_BitBang_ClearSetSDAPinMethod ClearSetSDAPinMethodPtr;
    I2C_BitBang_GetSdaMethod GetSdaMethodPtr;
    I2C_BitBang_DelayMethod DelayMethodPtr;
};

bool i2c_send_byte(struct I2C_BitBang_Interface_t* instance, uint8_t address, uint8_t data);
uint8_t i2c_receive_byte(struct I2C_BitBang_Interface_t* instance, uint8_t address);
uint8_t i2c_receive_byte_data( struct I2C_BitBang_Interface_t* instance, uint8_t address, uint8_t reg );
bool i2c_send_byte_data( struct I2C_BitBang_Interface_t* instance, uint8_t address, uint8_t reg, uint8_t data );

#endif //I2CSW_I2CSW_H
