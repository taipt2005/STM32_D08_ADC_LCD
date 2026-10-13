#include "lcd_i2c.h"

#include "d08_config.h"

static I2C_HandleTypeDef *port;

static HAL_StatusTypeDef nibble(uint8_t n) {

    uint8_t b[3]={(uint8_t)(n|8),(uint8_t)(n|12),(uint8_t)(n|8)};

    return HAL_I2C_Master_Transmit(port,LCD_ADDR_7BIT<<1,b,3,3);

}

static HAL_StatusTypeDef byte(uint8_t v,uint8_t rs) {

    uint8_t hi=(v&0xf0)|rs|8,lo=((v<<4)&0xf0)|rs|8;

    uint8_t b[6]={hi,(uint8_t)(hi|4),hi,lo,(uint8_t)(lo|4),lo};

    return HAL_I2C_Master_Transmit(port,LCD_ADDR_7BIT<<1,b,6,3);

}

HAL_StatusTypeDef lcd_init(I2C_HandleTypeDef *bus) {

    port=bus;

    HAL_Delay(50);

    if(HAL_I2C_IsDeviceReady(port,LCD_ADDR_7BIT<<1,2,10)!=HAL_OK)return HAL_ERROR;

    if(nibble(0x30)!=HAL_OK)return HAL_ERROR;

    HAL_Delay(5);

    if(nibble(0x30)!=HAL_OK)return HAL_ERROR;

    HAL_Delay(1);

    if(nibble(0x30)!=HAL_OK)return HAL_ERROR;

    HAL_Delay(1);

    if(nibble(0x20)!=HAL_OK)return HAL_ERROR;

    if(byte(0x28,0)!=HAL_OK)return HAL_ERROR;

    if(byte(0x08,0)!=HAL_OK)return HAL_ERROR;

    if(byte(0x01,0)!=HAL_OK)return HAL_ERROR;

    HAL_Delay(2);

    if(byte(0x06,0)!=HAL_OK)return HAL_ERROR;

    return byte(0x0c,0);

}

HAL_StatusTypeDef lcd_rows(const char *a,const char *b) {

    const char *rows[2]={a,b};

    for(unsigned r=0;r<2;++r) {

        if(byte(r?0xc0:0x80,0)!=HAL_OK)return HAL_ERROR;

        const char *s=rows[r];

        for(unsigned c=0;c<16;++c) {

            uint8_t v=' ';

            if(*s)v=(uint8_t)*s++;

            if(byte(v,1)!=HAL_OK)return HAL_ERROR;

        }

    }

    return HAL_OK;

}