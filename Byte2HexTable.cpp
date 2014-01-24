#include "Byte2HexTable.h"
#include <stdio.h>

void Byte2HexTable(unsigned char *str, size_t len)
{
	int i,j,l = len / 16;
	printf_s("Offset  0  1  2  3  4  5  6  7   8  9  A  B  C  D  E  F\n");
	for (i=0; i<l; i++)
	{
		printf_s("0x%03d0  ",i);
		for (j=0; j<8; j++)
			printf_s("%02d ",*str++);
		printf_s(" ");
		for (j=0; j<8; j++)
			printf_s("%02d ",*str++);
		printf_s("\n");
	}
	printf_s("0x%03d0  ",i);
	i*=16;
	while (i < len)
	{
		printf_s("%02d ",*str++);
		i++;
		if (i % 8 == 0) printf_s(" ");
	}
	printf_s("\n");
}

void Byte2HexTable(char *str, size_t len)
{
	Byte2HexTable((unsigned char *)str,len);
}