#ifndef  _LCD12864_H_
#define  _LCD12864_H_

#define com 0
#define dat 1
#define LINE_0 0X80
#define LINE_1 0X90
#define LINE_2 0X88
#define LINE_3 0X98


void softspi_init(void);

void lcd_byte(unsigned char byte);

void lcd_write(unsigned char dat_com,unsigned char byt);

unsigned char lcd_read(void);

void lcd_busy(void);

void lcd_init(void);

void Show(char address,char *hz);

void LcdDrawClear(void);

void lcd_dadian(unsigned char X,unsigned char Y);

void v_Lcd12864DrawLineX_f(unsigned char X0, unsigned char X1, unsigned char Y);

void v_Lcd12864DrawLineY_f( unsigned char X, unsigned char Y0, unsigned char Y1 );

void lcd_huaxian(unsigned char start_x,unsigned char start_y,unsigned char end_x,unsigned char end_y);

void DrawPicture(const unsigned char *pic);

void IMG_Clear();

void PutDot(unsigned char X,unsigned char Y,unsigned char dot);

#endif  //  _LCD12864_H_
