#include "fontlib.h"
#include "drv_lcd.h"
#include "rtdevice.h"
#include "drv_gpio.h"

#define SOFTWARE_SPI_ENABLE    1

/*
    GND        GND
    3.3V       VCC
    PB13       SCL
    PB15       SDA

    PA5       CS

    PA0       ERS
    PA1       DC
    PA4       BL
    */
#if SOFTWARE_SPI_ENABLE

#define LCD_SCLK_LOW()  gpio_bits_reset(GPIOA,GPIO_PINS_5)//SCL=SCLK
#define LCD_SCLK_HIGH() gpio_bits_set(GPIOA,GPIO_PINS_5)

#define LCD_MOSI_LOW()  gpio_bits_reset(GPIOA,GPIO_PINS_7)//SDA=MOSI
#define LCD_MOSI_HIGH() gpio_bits_set(GPIOA,GPIO_PINS_7)

#endif

#define LCD_CS_LOW()    gpio_bits_reset(GPIOA,GPIO_PINS_4)//CS
#define LCD_CS_HIGH()   gpio_bits_set(GPIOA,GPIO_PINS_4)

#define LCD_RES_LOW()   gpio_bits_reset(GPIOB,GPIO_PINS_0)//RES  rst
#define LCD_RES_HIGH()  gpio_bits_set(GPIOB,GPIO_PINS_0)

#define LCD_DC_LOW()    gpio_bits_reset(GPIOB,GPIO_PINS_1)//DC  data command select
#define LCD_DC_HIGH()   gpio_bits_set(GPIOB,GPIO_PINS_1)

#define LCD_BLK_LOW()   gpio_bits_reset(GPIOB,GPIO_PINS_2)//BLK
#define LCD_BLK_HIGH()  gpio_bits_set(GPIOB,GPIO_PINS_2)

#define USE_HORIZONTAL 2

#if USE_HORIZONTAL==0 || USE_HORIZONTAL==1
    #define LCD_W 128  //0x80
    #define LCD_H 160  //0xA0
#else
    #define LCD_W 160  //0xA0
    #define LCD_H 128  //0x80
#endif



void LCD_Writ_Bus(u8 dat)
{
    LCD_CS_LOW();

#if SOFTWARE_SPI_ENABLE

    u8 i;

    for(i=0;i<8;i++)
    {
        LCD_SCLK_LOW();
        if(dat&0x80)
        {
           LCD_MOSI_HIGH();
        }
        else
        {
           LCD_MOSI_LOW();
        }
        LCD_SCLK_HIGH();
        dat<<=1;
    }
#else

    spi_i2s_data_transmit(SPI1, dat ); /

     while(spi_i2s_flag_get(SPI1, SPI_I2S_TDBE_FLAG) == RESET) {};  //等待发送完成，我感觉以后要加超时判断，要不会出问题会阻塞在这里

#endif
    LCD_CS_HIGH();
}


void LCD_WR_DATA8(u8 dat)
{
    LCD_Writ_Bus(dat);
}


void LCD_WR_DATA(u16 dat)
{
    LCD_Writ_Bus(dat>>8);
    LCD_Writ_Bus(dat);
}


void LCD_WR_REG(u8 dat)
{
    LCD_DC_LOW();
    LCD_Writ_Bus(dat);
    LCD_DC_HIGH();
}

void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
    if(USE_HORIZONTAL==0)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1+2);
        LCD_WR_DATA(x2+2);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1+1);
        LCD_WR_DATA(y2+1);
        LCD_WR_REG(0x2c);
    }
    else if(USE_HORIZONTAL==1)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1+2);
        LCD_WR_DATA(x2+2);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1+1);
        LCD_WR_DATA(y2+1);
        LCD_WR_REG(0x2c);
    }
    else if(USE_HORIZONTAL==2)
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1+1);
        LCD_WR_DATA(x2+1);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1+2);
        LCD_WR_DATA(y2+2);
        LCD_WR_REG(0x2c);
    }
    else
    {
        LCD_WR_REG(0x2a);
        LCD_WR_DATA(x1+1);
        LCD_WR_DATA(x2+1);
        LCD_WR_REG(0x2b);
        LCD_WR_DATA(y1+2);
        LCD_WR_DATA(y2+2);
        LCD_WR_REG(0x2c);
    }
}


struct LCD_PIN {
    uint8_t LCD_RST;
    uint8_t LCD_DC;
    uint8_t LCD_CS;
    uint8_t LCD_CLK;
    uint8_t LCD_SDA;
    uint8_t LCD_LED;
};

static struct  LCD_PIN st7735;

void Gpio_Init(void){
    st7735.LCD_CLK = rt_pin_get("PA.5");
    st7735.LCD_CS = rt_pin_get("PA.4");
    st7735.LCD_DC = rt_pin_get("PB.1");
    st7735.LCD_RST = rt_pin_get("PB.0");
    st7735.LCD_SDA = rt_pin_get("PA.7");
    st7735.LCD_LED = rt_pin_get("PD.13");
    // 设置引脚输出方式
    rt_pin_mode(st7735.LCD_CLK, PIN_MODE_OUTPUT);
    rt_pin_mode(st7735.LCD_CS, PIN_MODE_OUTPUT);
    rt_pin_mode(st7735.LCD_DC , PIN_MODE_OUTPUT);
    rt_pin_mode(st7735.LCD_RST, PIN_MODE_OUTPUT);
    rt_pin_mode(st7735.LCD_SDA, PIN_MODE_OUTPUT);
    rt_pin_mode(st7735.LCD_LED, PIN_MODE_OUTPUT);
    rt_pin_write(st7735.LCD_SDA, PIN_LOW);
    rt_pin_write(st7735.LCD_CS, PIN_HIGH);
    rt_pin_write(st7735.LCD_DC, PIN_LOW);
    rt_pin_write(st7735.LCD_RST, PIN_LOW);
    rt_pin_write(st7735.LCD_SDA, PIN_LOW);
    rt_pin_write(st7735.LCD_LED, PIN_HIGH);
}


void LcdInit(void)
{
    Gpio_Init();

    LCD_CS_LOW();
    LCD_RES_LOW();
    rt_thread_mdelay(100);
    LCD_RES_HIGH();
    rt_thread_mdelay(100);

    LCD_BLK_HIGH();
    rt_thread_mdelay(100);

    LCD_WR_REG(0x11); //Sleep out
    rt_thread_mdelay(120);              //Delay 120ms



    LCD_WR_REG(0x3A);
    LCD_WR_DATA8(0x05);
    LCD_WR_REG(0x36);

    if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
    else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
    else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
    else LCD_WR_DATA8(0xA0);

    LCD_WR_DATA8(0xA8);
    LCD_WR_REG(0x29); //Display on
}


void LcdFill(u16 xsta,u16 ysta,u16 xend,u16 yend,u16 color)
{
    u16 i,j;

    LCD_Address_Set(xsta,ysta,xend-1,yend-1);

    for(i=ysta;i<yend;i++)
    {
        for(j=xsta;j<xend;j++)
        {
            LCD_WR_DATA(color);
        }
    }
}


void LcdDrawPoint(u16 x,u16 y,u16 color)
{
    LCD_Address_Set(x,y,x,y);
    LCD_WR_DATA(color);
}


void LcdDrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
    u16 t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;

    delta_x=x2-x1;
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;
    if(delta_x>0)incx=1;
    else if (delta_x==0)incx=0;
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;
    else {incy=-1;delta_y=-delta_y;}
    if(delta_x>delta_y)distance=delta_x;
    else distance=delta_y;

    for(t=0;t<distance+1;t++)
    {
        LcdDrawPoint(uRow,uCol,color);
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}


void LcdDrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
    LcdDrawLine(x1,y1,x2,y1,color);
    LcdDrawLine(x1,y1,x1,y2,color);
    LcdDrawLine(x1,y2,x2,y2,color);
    LcdDrawLine(x2,y1,x2,y2,color);
}


void LcdDrawCircle(u16 x0,u16 y0,u8 r,u16 color)
{
    int a,b;

    a=0;b=r;
    while(a<=b)
    {
        LcdDrawPoint(x0-b,y0-a,color);             //3
        LcdDrawPoint(x0+b,y0-a,color);             //0
        LcdDrawPoint(x0-a,y0+b,color);             //1
        LcdDrawPoint(x0-a,y0-b,color);             //2
        LcdDrawPoint(x0+b,y0+a,color);             //4
        LcdDrawPoint(x0+a,y0-b,color);             //5
        LcdDrawPoint(x0+a,y0+b,color);             //6
        LcdDrawPoint(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))
        {
            b--;
        }
    }
}


void LcdShowChar(u16 x,u16 y,u8 ch,u16 fc,u16 bc,u8 csize)
{
    const unsigned char  *temp;
    u8 i,j,k;
    u16 x0=x,y0=y;
    u8 row=0,column=0;

    row=(csize/8+((csize%8)?1:0));
    column=csize/2;
    ch=ch-' ';
    switch(csize)
    {
        case 12:temp=Ascii_6x12[ch]; break;
        case 16:temp=Ascii_8x16[ch]; break;
        case 24:temp=Ascii_12x24[ch]; break;
        case 32:temp=Ascii_16x32[ch]; break;
        default: temp=Ascii_6x12[ch]; break;
    }

    for(i=0;i<row;i++)
    {
        for(j=0;j<column;j++)
        {
            for(k=0;k<8;k++)
            {
                if((*temp)&(0x01<<k))
                {
                    LcdDrawPoint(x,y,fc);
                }
                else
                {
                    LcdDrawPoint(x,y,bc);
                }
                y++;
            }
            y=y0;
            x++;
            temp++;
        }
        x=x0;
        y0+=8;
        y=y0;
    }
}

void LcdShowString(uint16_t x,uint16_t y,const char *p,uint16_t fc,uint16_t bc,uint8_t csize)
{
    while(*p!='\0')
    {
        LcdShowChar(x,y,*p,fc,bc,csize);
        x+=csize/2;
        p++;
    }
}


void LcdShow16x16Hz(u32 x, u32 y, u8 index, u16 fc, u16 bc)
{
    const char *temp=Hzk[index];
    u8 y0=y,x0=x;
    u8 i,j,k;
    u8 row=0,column=0;

    row=16/8;
    column=16;

    for(i=0;i<row;i++)
    {
        for(j=0;j<column;j++)
        {
            for( k=0;k<8;k++)
            {
                if(*temp&(0x01<<k))
                {
                    LcdDrawPoint(x,y,fc);
                }
                else
                {
                    LcdDrawPoint(x,y,bc);
                }
                y++;
            }
            temp++;
            x++;
            y=y0;
        }
        x=x0;
        y0+=8;
        y=y0;
    }
}


void LcdShowImage(u16 x, u16 y, u16 width, u16 height, const u8 *image)
{
    u16 colorData=0;
    u32 cnt=0;

    for(u16 i=0; i<height; i++)
    {
        LCD_Address_Set(x, y+i, x+width, y+height);
        for(u16 j=0; j<width; j++)
        {
            colorData=(image[cnt*2+1]<<8) | image[cnt*2];
            LCD_WR_DATA(colorData);
            cnt++;
        }
    }
}




