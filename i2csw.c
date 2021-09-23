// Portable I2C bit bang library
// Inspired by and adapted from: https://calcium3000.wordpress.com/2016/08/19/i2c-bit-banging-tutorial-part-i/

#include "stdint.h"
#include "i2csw.h"
#include "stdbool.h"

#define I2C_SET_SCL instance->ClearSetSCLPinMethodPtr(false);
#define I2C_CLR_SCL instance->ClearSetSCLPinMethodPtr(true);
#define I2C_SET_SDA instance->ClearSetSDAPinMethodPtr(false);
#define I2C_CLR_SDA instance->ClearSetSDAPinMethodPtr(true);
#define I2C_GET_SDA instance->GetSdaMethodPtr()
#define I2C_DELAY instance->DelayMethodPtr();

/**
 * @brief I2C master write 8-bit data bit-bang
 * @param unsigned char b - data to transmit
 * @retval unsigned char ack – acknowledgement received
 */
void I2C_master_write_bit (struct I2C_BitBang_Interface_t* instance, unsigned char b)
{
    if( b > 0 ) I2C_SET_SDA
    else I2C_CLR_SDA;

    I2C_DELAY
    I2C_SET_SCL
    I2C_DELAY
    I2C_CLR_SCL
}

/**
 * @brief I2C master read 8-bit bit-bang
 * @param unsigned char ack – acknowledgement control
 * @retval unsigned char b – data received
 */
unsigned char I2C_master_read_bit (struct I2C_BitBang_Interface_t* instance)
{
    uint8_t b;

    I2C_SET_SDA
    I2C_DELAY
    I2C_SET_SCL
    I2C_DELAY

    if( I2C_GET_SDA ) b = 1;
    else b = 0;

    I2C_CLR_SCL

    return b;
}

/**
 * @brief I2C start
 * @param none
 * @retval none
 */
void I2C_Start_condition(struct I2C_BitBang_Interface_t* instance)
{
    I2C_SET_SCL
    I2C_SET_SDA
    I2C_DELAY
    I2C_CLR_SDA
    I2C_DELAY
    I2C_CLR_SCL
    I2C_DELAY
}

/**
 * @brief I2C stop
 * @param instance
 * @retval none
 */
void I2C_Stop_condition(struct I2C_BitBang_Interface_t* instance)
{
    I2C_CLR_SDA
    I2C_DELAY
    I2C_SET_SCL
    I2C_DELAY
    I2C_SET_SDA
    I2C_DELAY
}

// Writing a byte with I2C:
bool i2c_write_byte(struct I2C_BitBang_Interface_t* instance, uint8_t B, bool start, bool stop)
{
    uint8_t ack = 0;

    if( start ) I2C_Start_condition(instance);

    uint8_t i;
    for( i = 0; i < 8; i++ )
    {
        I2C_master_write_bit(instance, B & 0x80 );   // write the most-significant bit
        B <<= 1;
    }

    ack = !I2C_master_read_bit(instance);

    if( stop ) I2C_Stop_condition(instance);

    return ack;
}

// Reading a byte with I2C:
uint8_t i2c_read_byte(struct I2C_BitBang_Interface_t* instance, bool ack, bool stop)
{
    uint8_t B = 0;

    uint8_t i;
    for( i = 0; i < 8; i++ )
    {
        B <<= 1;
        B = I2C_master_read_bit(instance) ? (B | 0x01) : (B & 0x7E);
    }

    if( ack ) I2C_master_write_bit(instance, 0);
    else I2C_master_write_bit(instance, 1);

    if( stop ) I2C_Stop_condition(instance);

    return B;
}
// Sending a byte with I2C:
bool i2c_send_byte(struct I2C_BitBang_Interface_t* instance, uint8_t address, uint8_t data)
{
    if( i2c_write_byte( instance, address << 1, true, false ) )   // start, send address, write
    {
        // send data, stop
        if( i2c_write_byte( instance, data, false, true ) )  return true;
    }

    I2C_Stop_condition(instance);   // make sure to impose a stop if NAK'd
    return false;
}

// Receiving a byte with a I2C:
uint8_t i2c_receive_byte(struct I2C_BitBang_Interface_t* instance, uint8_t address)
{
    if( i2c_write_byte( instance, ( address << 1 ) | 0x01, true, false ) )   // start, send address, read
    {
        return i2c_read_byte( instance, false, true );
    }

    return 0;   // return zero if NAK'd
}

// Receiving a byte of data with I2C:
uint8_t i2c_receive_byte_data( struct I2C_BitBang_Interface_t* instance, uint8_t address, uint8_t reg )
{
    if( i2c_write_byte( instance, address << 1, true, false ) )   // start, send address, write
    {
        if( i2c_write_byte( instance, reg, false, false ) )   // send desired register
        {
            if( i2c_write_byte( instance, ( address << 1) | 0x01, true, false ) )   // start again, send address, read
            {
                return i2c_read_byte( instance, false, true );   // read data
            }
        }
    }

    I2C_Stop_condition(instance);
    return 0;   // return zero if NACKed
}

// Sending a byte of data with I2C:
bool i2c_send_byte_data( struct I2C_BitBang_Interface_t* instance, uint8_t address, uint8_t reg, uint8_t data )
{
    if( i2c_write_byte( instance, address << 1, true, false ) )   // start, send address, write
    {
        if( i2c_write_byte( instance, reg, false, false ) )   // send desired register
        {
            if( i2c_write_byte( instance, data, false, true ) ) return true;   // send data, stop
        }
    }

    I2C_Stop_condition(instance);
    return false;
}