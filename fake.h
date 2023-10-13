#define interrupt 
#define cregister 
#define __cregister 
#define __interrupt 

#define __asm (void)

// TMU 内联函数声明
float __div2pif32(float src);
float __mpy2pif32(float src);
float __sinpuf32(float src);
float __cospuf32(float src);

// 其他硬件相关函数
#define __byte_peripheral_32 *
#define __byte *
