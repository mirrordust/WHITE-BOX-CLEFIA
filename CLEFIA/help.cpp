#include "stdafx.h"


void BytePut(const unsigned char *data, int bytelen)
{
	while(bytelen-- > 0){
		printf("%02x", *data++);
	}
	printf("\n");
}