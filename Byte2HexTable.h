#ifndef _BYTE2HEXTABLE_H_
#define _BYTE2HEXTABLE_H_

//将字符以16进制表的形式输出
//Example:
/*
Offset  0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F
0x0000  80 8F 0D 83 07 DC D4 C8  00 40 20 00 00 40 CD 00
0x0010  10
*/
void Byte2HexTable(unsigned char *str, size_t len);
void Byte2HexTable(char *str, size_t len);

#endif