/*
 * ili9341.c


 *
 *  Created on: 12 янв. 2015 г.
 *      Author: Sem
 */

#include "ili9341.h"

static void transmitCmdData(uint8_t cmd, uint8_t * data, uint8_t numDataByte)
{
    TFT_CS_ACTIVE;
   	TFT_DC_COMMAND;
   	hspi_TxRx(&cmd, 1);
    if (numDataByte > 0)
    {
    	TFT_DC_DATA;
        hspi_TxRx(data, numDataByte);
    }
    TFT_CS_DEACTIVE;
}

static void transmitData(uint8_t *data, uint8_t numByte, uint32_t numRepeat)
{
	TFT_DC_DATA;
    TFT_CS_ACTIVE;
    hspi_Tx(data, numByte, numRepeat);
    TFT_CS_DEACTIVE;
}

static uint8_t readRegister(uint8_t cmd, uint8_t numParameter)
{
    uint8_t data = 0;
    data = 0x10 + numParameter;
    transmitCmdData(0xD9, &data, 1); // secret command and 0x11 is the first Parameter
    transmitCmdData(cmd, &data, 1);
    return data;
}

static void swap(uint16_t *a, uint16_t *b)
{
    (*a) = (*a) ^ (*b);
    (*b) = (*a) ^ (*b);
    (*a) = (*a) ^ (*b);
}

static void constrain(uint16_t *value, uint16_t min, uint16_t max)
{
	if (*value < min)
		*value = min;
	else if (*value > max)
		*value = max;
}

static void setCol(uint16_t start, uint16_t end)
{
	uint8_t data[4] = {start >> 8, start & 0xFF, end >> 8, end & 0xFF};
	transmitCmdData(0x2A, (uint8_t *)data, 4);	//Column Command address
}

static void setPage(uint16_t start, uint16_t end)
{
	uint8_t data[4] = {start >> 8, start & 0xFF, end >> 8, end & 0xFF};
	transmitCmdData(0x2B, (uint8_t *)data, 4);	//Column Command address
}

uint32_t tft_readId(void)
{
    uint8_t i = 0;
	uint32_t_bytes id = {0};

    id.bytes.b2 = readRegister(0xd3, 1);
    id.bytes.b1 = readRegister(0xd3, 2);
    id.bytes.b0 = readRegister(0xd3, 3);

    return  id.all;
}

void tft_configRegister(void)
{
	uint8_t data[15] = {0};

	data[0] = 0x39;
	data[1] = 0x2C;
	data[2] = 0x00;
	data[3] = 0x34;
	data[4] = 0x02;
	transmitCmdData(0xCB, data, 5);

	data[0] = 0x00;
	data[1] = 0XC1;
	data[2] = 0X30;
	transmitCmdData(0xCF, data, 3);

	data[0] = 0x85;
	data[1] = 0x00;
	data[2] = 0x78;
	transmitCmdData(0xE8, data, 3);

	data[0] = 0x00;
	data[1] = 0x00;
	transmitCmdData(0xEA, data, 2);

	data[0] = 0x64;
	data[1] = 0x03;
	data[2] = 0X12;
	data[3] = 0X81;
	transmitCmdData(0xED, data, 4);

	data[0] = 0x20;
	transmitCmdData(0xF7, data, 1);

	data[0] = 0x23;   	//VRH[5:0]
	transmitCmdData(0xC0, data, 1);    	//Power control

	data[0] = 0x10;   	//SAP[2:0];BT[3:0]
	transmitCmdData(0xC1, data, 1);    	//Power control

	data[0] = 0x3e;   	//Contrast
	data[1] = 0x28;
	transmitCmdData(0xC5, data, 2);    	//VCM control

	data[0] = 0x86;  	 //--
	transmitCmdData(0xC7, data, 1);    	//VCM control2

	data[0] = 0x48;  	//C8	   //48 68绔栧睆//28 E8 妯睆
	transmitCmdData(0x36, data, 1);    	// Memory Access Control

	data[0] = 0x55;
	transmitCmdData(0x3A, data, 1);

	data[0] = 0x00;
	data[1] = 0x18;
	transmitCmdData(0xB1, data, 2);

	data[0] = 0x08;
	data[1] = 0x82;
	data[2] = 0x27;
	transmitCmdData(0xB6, data, 3);    	// Display Function Control

	data[0] = 0x00;
	transmitCmdData(0xF2, data, 1);    	// 3Gamma Function Disable

	data[0] = 0x01;
	transmitCmdData(0x26, data, 1);    	//Gamma curve selected

	data[0] = 0x0F;
	data[1] = 0x31;
	data[2] = 0x2B;
	data[3] = 0x0C;
	data[4] = 0x0E;
	data[5] = 0x08;
	data[6] = 0x4E;
	data[7] = 0xF1;
	data[8] = 0x37;
	data[9] = 0x07;
	data[10] = 0x10;
	data[11] = 0x03;
	data[12] = 0x0E;
	data[13] = 0x09;
	data[14] = 0x00;
	transmitCmdData(0xE0, data, 15);    	//Set Gamma

	data[0] = 0x00;
	data[1] = 0x0E;
	data[2] = 0x14;
	data[3] = 0x03;
	data[4] = 0x11;
	data[5] = 0x07;
	data[6] = 0x31;
	data[7] = 0xC1;
	data[8] = 0x48;
	data[9] = 0x08;
	data[10] = 0x0F;
	data[11] = 0x0C;
	data[12] = 0x31;
	data[13] = 0x36;
	data[14] = 0x0F;
	transmitCmdData(0xE1, data, 15);    	//Set Gamma

	transmitCmdData(0x11, 0, 0);    	//Exit Sleep
	os_delay_us(120000);

	transmitCmdData(0x29, 0, 0);    //Display on
	transmitCmdData(0x2c, 0, 0);
}

void tft_init(void)
{
	hspi_init();

	TFT_CS_INIT;
	TFT_DC_INIT;
	TFT_RST_INIT;

	TFT_RST_ACTIVE;
	os_delay_us(10000);
	TFT_RST_DEACTIVE;
	os_delay_us(1000);

	tft_readId();
	tft_configRegister();
	tft_fillRectangle(MIN_TFT_X, MAX_TFT_X, MIN_TFT_Y, MAX_TFT_Y, 0);
}

void tft_fillRectangle(uint16_t xLeft, uint16_t xRight, uint16_t yUp, uint16_t yDown, uint16_t color)
{
    uint32_t numRepeat = 0;
    uint32_t i = 0;
    uint8_t data[2] = {0};

    if (xLeft > xRight) swap(&xLeft, &xRight);
    if (yUp > yDown) swap(&yUp, &yDown);

    constrain(&xLeft, MIN_TFT_X, MAX_TFT_X);
    constrain(&xRight, MIN_TFT_X, MAX_TFT_X);
    constrain(&yUp, MIN_TFT_Y, MAX_TFT_Y);
    constrain(&yDown, MIN_TFT_Y, MAX_TFT_Y);

    numRepeat = (xRight - xLeft + 1) * (yDown - yUp + 1);

    setCol(xLeft, xRight);
    setPage(yUp, yDown);
    transmitCmdData(0x2C, 0, 0);//  start to write to display RAM

    data[0] = color >> 8;
    data[1] = color & 0xff;

   	transmitData(data, 2, numRepeat);
}

void tft_setPixel(uint16_t poX, uint16_t poY, uint16_t color)
{
	uint8_t data[2] = {0};

    setCol(poX, poX);
    setPage(poY, poY);
    transmitCmdData(0x2C, 0, 0);//  start to write to display RAM

    data[0] = color >> 8;
    data[1] = color & 0xff;
   	transmitData(data, 2, 1);
}

void tft_drawLine( uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    int16_t dx = abs(x1 - x0);
    int16_t dy = -abs(y1 - y0);
    int8_t sx = (x0 < x1) ? 1 : -1;
    int8_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy;
    int16_t e2 = 0;
    for (;;)
    {
//    	wdt_feed();
    	tft_setPixel(x0, y0, color);
        e2 = 2*err;
        if (e2 >= dy)
        {
            if (x0 == x1) break;
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx)
        {
            if (y0 == y1) break;
            err += dx;
            y0 += sy;
        }
    }
}

void tft_drawChar(int16_t ascii, uint16_t posX, uint16_t posY, uint16_t size, uint16_t color)
{
	uint8_t i = 0;
	uint8_t j = 0;

	ascii = ( (ascii < 32) || (ascii > 127) ) ? 0x20 : ascii - 0x20;


    for (i = 0; i < FONT_X; ++i )
    {
    	for(j = 0; j < FONT_Y; ++j)
    	{
    			uint16_t finalColor = ((simpleFont[ascii][i] >> j) & 0x01) ? color : 0;

    			tft_fillRectangle(posX + i * size, posX + (i+1) * size,  posY + j * size, posY + (j+1) * size, finalColor);
    	}
    }
}

#define STREAM_STRING_SIZE_FONT		2
#define STREAM_STRING_COLOR_FONT	0xFFFF

static void nextString(uint16_t *x, uint16_t *y)
{
    *x = 0;
    *y += FONT_Y * STREAM_STRING_SIZE_FONT;

    if( (*y + FONT_Y * STREAM_STRING_SIZE_FONT) > (MAX_TFT_Y + 1) )
    {
    	*x = 0;
    	*y = 0;
    }

    tft_fillRectangle(0, MAX_TFT_X, *y, *y + FONT_Y * STREAM_STRING_SIZE_FONT, 0x0000);
}

void tft_drawStreamString(char * str)
{
	static uint16_t posX = 0;
	static uint16_t posY = 0;
    while(*str)
    {
        tft_drawChar(*str, posX, posY, STREAM_STRING_SIZE_FONT, STREAM_STRING_COLOR_FONT);
        str++;

        posX += FONT_X * STREAM_STRING_SIZE_FONT; // Move cursor right

        if ( (posX + FONT_X * STREAM_STRING_SIZE_FONT) > (MAX_TFT_X + 1) )
        	nextString(&posX, &posY);

        if (*str == 0x0A)
        {
        	str++;
        	nextString(&posX, &posY);
        }
    }
}

