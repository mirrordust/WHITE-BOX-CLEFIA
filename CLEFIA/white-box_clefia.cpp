#include"stdafx.h"

void ByteCpy(unsigned char *dst, const unsigned char *src, int bytelen)
{
	while(bytelen-- > 0){
		*dst++ = *src++; //以Byte为单位 进行Copy
	}
}

void ByteXor(unsigned char *dst, const unsigned char *a, const unsigned char *b, int bytelen)
{
	while(bytelen-- > 0){
		*dst++ = *a++ ^ *b++; //以Byte为单位 异或运算
	}
}

unsigned char ClefiaMul2(unsigned char x)
{
	/* multiplication over GF(2^8) (p(x) = '11d') */
	if(x & 0x80U){ //0x80 ==> 10000000
		x ^= 0x0eU; //0x0e ==> 00001110
	}
	return ((x << 1) | (x >> 7));
}

#define ClefiaMul4(_x) (ClefiaMul2(ClefiaMul2((_x))))
#define ClefiaMul6(_x) (ClefiaMul2((_x)) ^ ClefiaMul4((_x)))
#define ClefiaMul8(_x) (ClefiaMul2(ClefiaMul4((_x))))
#define ClefiaMulA(_x) (ClefiaMul2((_x)) ^ ClefiaMul8((_x)))


//产生随机数
void RandomNumber(unsigned char * R)
{
	const int max_rn_count = (26 - 2) * 2;
	unsigned char _R[max_rn_count * 4];
	srand(time(NULL));
	int t = 0;
	unsigned char * r;
	for (int i = 0; i < max_rn_count * 4 / 2; i++){
		t = rand();
		r = (unsigned char *)&t;
		ByteCpy(_R + 2 * i, r, 2);
	}
	ByteCpy(R, _R, max_rn_count * 4);
}

void WBF0Table(unsigned char ** tables, unsigned char * rk, unsigned char * r1, unsigned char * r2, unsigned char * r3)
{
	unsigned char t_tables[16][256];
	unsigned char WK1[4],WK2[4],WK3[4],WK4[4],temp[4],x[4],y[4];
	srand(time(NULL));
	//WK1
	for (int i = 0; i < 4; i++)
	{
		WK1[i] = (unsigned char)rand();
	}
	//WK2
	for (int i = 0; i < 4; i++)
	{
		WK2[i] = (unsigned char)rand();
	}
	//WK3
	for (int i = 0; i < 4; i++)
	{
		WK3[i] = (unsigned char)rand();
	}
	//WK4
	ByteXor(temp,r2,r3,4);
	ByteXor(temp,temp,WK1,4);
	ByteXor(temp,temp,WK2,4);
	ByteXor(WK4,temp,WK3,4);

	//x[0]
	unsigned char src = 0x00U;
	for (int i = 0; i < 256; i++)
	{
		unsigned char t;
		ByteXor(&t,&src,rk + 0,1);
		ByteXor(&t,&t,r1 + 0,1);
		x[0] = clefia_s0[t];

		y[0] = x[0];
		y[1] = ClefiaMul2(x[0]);
		y[2] = ClefiaMul4(x[0]);
		y[3] = ClefiaMul6(x[0]);

		for (int j = 0; j < 4; j++)
		{
			ByteXor(&t_tables[0 + 4*j][src], y + j, WK1 + j, 1);
		}
		//src加1
		src += 0x01U;
	}
	//x[1]
	src = 0x00U;
	for (int i = 0; i < 256; i++)
	{
		unsigned char t;
		ByteXor(&t,&src,rk + 1,1);
		ByteXor(&t,&t,r1 + 1,1);
		x[1] = clefia_s1[t];

		y[0] = ClefiaMul2(x[1]);
		y[1] = x[1];
		y[2] = ClefiaMul6(x[1]);
		y[3] = ClefiaMul4(x[1]);

		for (int j = 0; j < 4; j++)
		{
			ByteXor(&t_tables[1 + 4*j][src], y + j, WK2 + j, 1);
		}
		//src加1
		src += 0x01U;
	}
	//x[2]
	src = 0x00U;
	for (int i = 0; i < 256; i++)
	{
		unsigned char t;
		ByteXor(&t,&src,rk + 2,1);
		ByteXor(&t,&t,r1 + 2,1);
		x[2] = clefia_s0[t];

		y[0] = ClefiaMul4(x[2]);
		y[1] = ClefiaMul6(x[2]);
		y[2] = x[2];
		y[3] = ClefiaMul2(x[2]);

		for (int j = 0; j < 4; j++)
		{
			ByteXor(&t_tables[2 + 4*j][src], y + j, WK3 + j, 1);
		}
		//src加1
		src += 0x01U;
	}
	//x[3]
	src = 0x00U;
	for (int i = 0; i < 256; i++)
	{
		unsigned char t;
		ByteXor(&t,&src,rk + 3,1);
		ByteXor(&t,&t,r1 + 3,1);
		x[3] = clefia_s1[t];

		y[0] = ClefiaMul6(x[3]);
		y[1] = ClefiaMul4(x[3]);
		y[2] = ClefiaMul2(x[3]);
		y[3] = x[3];

		for (int j = 0; j < 4; j++)
		{
			ByteXor(&t_tables[3 + 4*j][src], y + j, WK4 + j, 1);
		}
		//src加1
		src += 0x01U;
	}
	for (int m = 0; m < 16; m++){
		ByteCpy(*(tables+m),t_tables[m],256);
	}
}

void WBF1Table(unsigned char ** tables, unsigned char * rk, unsigned char * r1, unsigned char * r2, unsigned char * r3)
{
	unsigned char t_tables[16][256];
	unsigned char WK1[4],WK2[4],WK3[4],WK4[4],temp[4],x[4],y[4];
	srand(time(NULL));
	//WK1
	for (int i = 0; i < 4; i++)
	{
		WK1[i] = (unsigned char)rand();
	}
	//WK2
	for (int i = 0; i < 4; i++)
	{
		WK2[i] = (unsigned char)rand();
	}
	//WK3
	for (int i = 0; i < 4; i++)
	{
		WK3[i] = (unsigned char)rand();
	}
	//WK4
	ByteXor(temp,r2,r3,4);
	ByteXor(temp,temp,WK1,4);
	ByteXor(temp,temp,WK2,4);
	ByteXor(WK4,temp,WK3,4);

	//x[0]
	unsigned char src = 0x00U;
	for (int i = 0; i < 256; i++)
	{
		unsigned char t;
		ByteXor(&t,&src,rk + 0,1);
		ByteXor(&t,&t,r1 + 0,1);
		x[0] = clefia_s1[t];

		y[0] = x[0];
		y[1] = ClefiaMul8(x[0]);
		y[2] = ClefiaMul2(x[0]);
		y[3] = ClefiaMulA(x[0]);

		for (int j = 0; j < 4; j++)
		{
			ByteXor(&t_tables[0 + 4*j][src], y + j, WK1 + j, 1);
		}
		//src加1
		src += 0x01U;
	}
	//x[1]
	src = 0x00U;
	for (int i = 0; i < 256; i++)
	{
		unsigned char t;
		ByteXor(&t,&src,rk + 1,1);
		ByteXor(&t,&t,r1 + 1,1);
		x[1] = clefia_s0[t];

		y[0] = ClefiaMul8(x[1]);
		y[1] = x[1];
		y[2] = ClefiaMulA(x[1]);
		y[3] = ClefiaMul2(x[1]);

		for (int j = 0; j < 4; j++)
		{
			ByteXor(&t_tables[1 + 4*j][src], y + j, WK2 + j, 1);
		}
		//src加1
		src += 0x01U;
	}
	//x[2]
	src = 0x00U;
	for (int i = 0; i < 256; i++)
	{
		unsigned char t;
		ByteXor(&t,&src,rk + 2,1);
		ByteXor(&t,&t,r1 + 2,1);
		x[2] = clefia_s1[t];

		y[0] = ClefiaMul2(x[2]);
		y[1] = ClefiaMulA(x[2]);
		y[2] = x[2];
		y[3] = ClefiaMul8(x[2]);

		for (int j = 0; j < 4; j++)
		{
			ByteXor(&t_tables[2 + 4*j][src], y + j, WK3 + j, 1);
		}
		//src加1
		src += 0x01U;
	}
	//x[3]
	src = 0x00U;
	for (int i = 0; i < 256; i++)
	{
		unsigned char t;
		ByteXor(&t,&src,rk + 3,1);
		ByteXor(&t,&t,r1 + 3,1);
		x[3] = clefia_s0[t];

		y[0] = ClefiaMulA(x[3]);
		y[1] = ClefiaMul2(x[3]);
		y[2] = ClefiaMul8(x[3]);
		y[3] = x[3];

		for (int j = 0; j < 4; j++)
		{
			ByteXor(&t_tables[3 + 4*j][src], y + j, WK4 + j, 1);
		}
		//src加1
		src += 0x01U;
	}
	for (int m = 0; m < 16; m++){
		ByteCpy(*(tables+m),t_tables[m],256);
	}
}

void WBTableSet128(unsigned char ** tables, unsigned char * rk, unsigned char * R, unsigned char * wk)
{
	rk += 8;
	unsigned char zero[4] = {0,0,0,0};
	for (int i = 0; i < 18; i++)
	{
		if (0 == i){
			WBF0Table(tables, rk, zero, R, wk);
			WBF1Table(tables + 16, rk + 4, zero, R + 4, wk + 4);
			R += 8;
		}else if (1 == i){
			WBF0Table(tables, rk, R - 8, zero, R);
			WBF1Table(tables + 16, rk + 4, R - 4, zero, R + 4);
			R += 8;
		}else if (16 == i){
			WBF0Table(tables, rk, R - 8, R - 12, zero);
			WBF1Table(tables + 16, rk + 4, R - 4, R - 16, zero);
		}else if (17 == i){
			WBF0Table(tables, rk, zero, R - 4, wk + 8);
			WBF1Table(tables + 16, rk + 4, zero, R - 8, wk + 12);
			break;
		}else{
			WBF0Table(tables, rk, R - 8, R - 12, R);
			WBF1Table(tables + 16, rk + 4, R - 4, R - 16, R + 4);
			R += 8;
		}
		tables += 32;
		rk += 8;
	}
}

////生成Lookup Table
//void LookupTable(unsigned char * table, int Mp, int Sp, unsigned char * rk, unsigned char * rn, unsigned char * wk)
//{
//	unsigned char * src = (unsigned char *)malloc(sizeof(unsigned char));
//	unsigned char t_table[256];
//	*src = 0x00U;
//	for (*src; *src <= 0xFFU; (*src)++)
//	{
//		unsigned char temp;
//		ByteXor(&temp,src,rk,1);
//		ByteXor(&temp,&temp,rn,1);
//		if (Sp == 0){
//			temp = clefia_s0[temp];
//		}
//		else if (Sp == 1){
//			temp = clefia_s1[temp];
//		}
//		else{
//			printf("%s","error");
//		}
//		switch (Mp)
//		{
//		case 0 :
//		case 11 :
//		case 22 :
//		case 33 :
//		case 100 :
//		case 111 :
//		case 122 :
//		case 133 : {
//			temp = temp;
//			break;
//				   }
//		case 1 :
//		case 10 :
//		case 23 :
//		case 32 :
//		case 102 :
//		case 113 : 
//		case 120 :
//		case 131 : {
//			temp = ClefiaMul2(temp);
//			break;
//				   }
//		case 2 :
//		case 13 :
//		case 20 :
//		case 31 :{
//			temp = ClefiaMul4(temp);
//			break;
//				 }
//		case 3 :
//		case 12 :
//		case 21 :
//		case 30 :{
//			temp = ClefiaMul6(temp);
//			break;
//				 }
//		case 101 :
//		case 110 :
//		case 123 :
//		case 132 :{
//			temp = ClefiaMul8(temp);
//			break;
//				  }
//		case 103 :
//		case 112 :
//		case 121 :
//		case 130 :{
//			temp = ClefiaMulA(temp);
//			break;
//				  }
//		default : {
//			printf("%s","error");
//			break;
//				  }
//		}
//		ByteXor(&temp,&temp,wk,1);
//		t_table[*src] = temp;
//		if (*src == 0xFFU)
//			break;
//	}
//	ByteCpy(table,t_table,256);
//}

//生成所有Lookup Tables
//void AllLookupTables(unsigned char ** tables, const unsigned char * rk, unsigned char * R, unsigned char * _WK, const int key_bitlen)
//{
//	int round_number = 0, Mp = 0, Sp = 0;
//	unsigned char WK[32];
//	ByteCpy(WK,_WK,32);
//	unsigned char zero[4];
//	for (int k = 0; k < 4; k++){
//		zero[k] = 0x00U;
//	}
//	if (key_bitlen == 128){
//		round_number = 18;
//		unsigned char t_tables[576][256],wk[4][4];
//		unsigned char t_rk[4],r1[4],r2[4],r3[4],temp[4];
//		//round = 1		16 * 2 = 32 lookup tables
//		//left
//		ByteCpy(t_rk,rk,4); rk += 4;
//		ByteCpy(r1,zero,4);
//		//ByteCpy(r1,R,4);
//		//R += 4;
//		ByteCpy(r2,R,4); R += 4;
//		ByteCpy(r3,&WK[0],4);
//		//ByteCpy(r3,R,4);
//		//R += 4;
//		ByteCpy(wk[0],R,4); R += 4;
//		ByteCpy(wk[1],R,4); R += 4;
//		ByteCpy(wk[2],R,4); R += 4;
//
//		ByteXor(temp,r2,r3,4);
//		ByteXor(temp,temp,wk[0],4);
//		ByteXor(temp,temp,wk[1],4);
//		ByteXor(wk[3],temp,wk[2],4);
//
//		for (int i = 0; i < 4; i++)
//		{
//			for (int j = 0; j < 4; j++)
//			{
//				Mp = i * 10 + j;
//				if (j % 2 == 0)
//					Sp = 0;
//				else
//					Sp = 1;
//				LookupTable(t_tables[4 * i + j],Mp,Sp,&t_rk[j],&r1[j],&wk[j][i]);
//			}
//		}
//		//right
//		ByteCpy(t_rk,rk,4); rk += 4;
//		ByteCpy(r1,zero,4);
//		ByteCpy(r2,R,4); R += 4;
//		ByteCpy(r3,&WK[1],4);
//		ByteCpy(wk[0],R,4); R += 4;
//		ByteCpy(wk[1],R,4); R += 4;
//		ByteCpy(wk[2],R,4); R += 4;
//
//		ByteXor(temp,r2,r3,4);
//		ByteXor(temp,temp,wk[0],4);
//		ByteXor(temp,temp,wk[1],4);
//		ByteXor(wk[3],temp,wk[2],4);
//
//		for (int i = 0; i < 4; i++)
//		{
//			for (int j = 0; j < 4; j++)
//			{
//				Mp = 100 + i * 10 + j;
//				if (j % 2 == 1)
//					Sp = 0;
//				else
//					Sp = 1;
//				LookupTable(t_tables[16 + 4 * i + j],Mp,Sp,&t_rk[j],&r1[j],&wk[j][i]);
//			}
//		}
//		//round = 2,...,r-1
//		for (int l = 1; l < round_number - 1; l++)
//		{
//			//left
//			ByteCpy(t_rk,rk,4); rk += 4;
//			ByteCpy(r1,R-8*4,4);
//			if (l == 1){
//				ByteCpy(r2,zero,4);
//			}else{
//				ByteCpy(r2,R-12*4,4);
//			}
//			if (l == round_number - 2){
//				ByteCpy(r3,zero,4);
//			}else{
//				ByteCpy(r3,R,4); R += 4;
//			}
//			ByteCpy(wk[0],R,4); R += 4;
//			ByteCpy(wk[1],R,4); R += 4;
//			ByteCpy(wk[2],R,4); R += 4;
//
//			ByteXor(temp,r2,r3,4);
//			ByteXor(temp,temp,wk[0],4);
//			ByteXor(temp,temp,wk[1],4);
//			ByteXor(wk[3],temp,wk[2],4);
//
//			for (int i = 0; i < 4; i++)
//			{
//				for (int j = 0; j < 4; j++)
//				{
//					Mp = i * 10 + j;
//					if (j % 2 == 0)
//						Sp = 0;
//					else
//						Sp = 1;
//					LookupTable(t_tables[32 * l + 4 * i + j],Mp,Sp,&t_rk[j],&r1[j],&wk[j][i]);
//				}
//			}
//			//right
//			ByteCpy(t_rk,rk,4); rk += 4;
//			if (l == round_number - 2){
//				ByteCpy(r1,R-7*4,4);
//			}else{
//				ByteCpy(r1,R-8*4,4);
//			}
//			if (l == 1){
//				ByteCpy(r2,zero,4);
//			}else if (l == round_number - 2){
//				ByteCpy(r2,R-19*4,4);
//			}{
//				ByteCpy(r2,R-20*4,4);
//			}
//			if (l == round_number - 2){
//				ByteCpy(r3,zero,4);
//			}else{
//				ByteCpy(r3,R,4); R += 4;
//			}
//			ByteCpy(wk[0],R,4); R += 4;
//			ByteCpy(wk[1],R,4); R += 4;
//			ByteCpy(wk[2],R,4); R += 4;
//
//			ByteXor(temp,r2,r3,4);
//			ByteXor(temp,temp,wk[0],4);
//			ByteXor(temp,temp,wk[1],4);
//			ByteXor(wk[3],temp,wk[2],4);
//
//			for (int i = 0; i < 4; i++)
//			{
//				for (int j = 0; j < 4; j++)
//				{
//					Mp = 100 + i * 10 + j;
//					if (j % 2 == 1)
//						Sp = 0;
//					else
//						Sp = 1;
//					LookupTable(t_tables[32 * l + 16 + 4 * i + j],Mp,Sp,&t_rk[j],&r1[j],&wk[j][i]);
//				}
//			}
//		}
//		//round = r		16 * 2 = 32 lookup tables
//		//left
//		ByteCpy(t_rk,rk,4); rk += 4;
//		ByteCpy(r1,zero,4);
//		ByteCpy(r2,R-10*4,4);
//		ByteCpy(r3,&WK[2],4);
//		ByteCpy(wk[0],R,4); R += 4;
//		ByteCpy(wk[1],R,4); R += 4;
//		ByteCpy(wk[2],R,4); R += 4;
//
//		ByteXor(temp,r2,r3,4);
//		ByteXor(temp,temp,wk[0],4);
//		ByteXor(temp,temp,wk[1],4);
//		ByteXor(wk[3],temp,wk[2],4);
//
//		for (int i = 0; i < 4; i++)
//		{
//			for (int j = 0; j < 4; j++)
//			{
//				Mp = i * 10 + j;
//				if (j % 2 == 0)
//					Sp = 0;
//				else
//					Sp = 1;
//				LookupTable(t_tables[32 * (round_number - 1) + 4 * i + j],Mp,Sp,&t_rk[j],&r1[j],&wk[j][i]);
//			}
//		}
//		//right
//		ByteCpy(t_rk,rk,4); rk += 4;
//		ByteCpy(r1,zero,4);
//		ByteCpy(r2,R-17*4,4);
//		ByteCpy(r3,&WK[3],4);
//		ByteCpy(wk[0],R,4); R += 4;
//		ByteCpy(wk[1],R,4); R += 4;
//		ByteCpy(wk[2],R,4); R += 4;
//
//		ByteXor(temp,r2,r3,4);
//		ByteXor(temp,temp,wk[0],4);
//		ByteXor(temp,temp,wk[1],4);
//		ByteXor(wk[3],temp,wk[2],4);
//
//		for (int i = 0; i < 4; i++)
//		{
//			for (int j = 0; j < 4; j++)
//			{
//				Mp = i * 10 + j;
//				if (j % 2 == 1)
//					Sp = 0;
//				else
//					Sp = 1;
//				LookupTable(t_tables[32 * (round_number - 1) + 16 + 4 * i + j],Mp,Sp,&t_rk[j],&r1[j],&wk[j][i]);
//			}
//		}
//		//for (int i = 0; i < 576; i++)///////////////////////////////////////////////
//		//{
//		//	for (int j = 0; j < 256; j++)
//		//	{
//		//		printf("%02x  ",t_tables[i][j]);
//		//	}
//		//	printf("\n-------------------------------------\n");
//		//}/////////////////////////////////////////////////////////////////////
//		for (int i = 0; i < 576; i++)
//		{
//			ByteCpy(*(tables + i),t_tables[i],256);
//		}
//		//for (int i = 0; i < 576; i++)/////////////////////////////////////////
//		//{
//		//	for (int j = 0; j < 256; j++)
//		//	{
//		//		printf("%02x  ",*(*(tables+i)+j));
//		//	}
//		//	printf("\n-------------------------------------\n");
//		//}//////////////////////////////////////////////////////
//	}
//}

void WBGfn4(unsigned char ** tables, unsigned char * y, const unsigned char * x, int r)
{
	unsigned char fin[16], fout[16];
	unsigned char temp[4], t[4], t_XOR[4];
	unsigned char t_tables[576][256], C[4][4];
	for (int i = 0; i < 576; i++)
	{
		ByteCpy(t_tables[i],*(tables+i),256);
	}
	ByteCpy(fin,x,16);
	for (int k = 0; k < r; k++){
		//left
		ByteCpy(fout,fin,4); //C[r,0]
		ByteCpy(temp,fin,4); //C[r,1]
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++)
			{
				ByteCpy(&C[i][j],&t_tables[32*k + 4*i + j][temp[j]],1);
			}
			//ByteCpy(&t_XOR[i],&t[0],1);
			//ByteXor(&t_XOR[i],&t_XOR[i],&t[1],1);
			//ByteXor(&t_XOR[i],&t_XOR[i],&t[2],1);
			//ByteXor(&t_XOR[i],&t_XOR[i],&t[3],1);
		}
		for (int i = 0; i < 4; i++)
		{
			ByteCpy(&t_XOR[i],&C[i][0],1);
			ByteXor(&t_XOR[i],&t_XOR[i],&C[i][1],1);
			ByteXor(&t_XOR[i],&t_XOR[i],&C[i][2],1);
			ByteXor(&t_XOR[i],&t_XOR[i],&C[i][3],1);
		}
		ByteXor(t_XOR,t_XOR,temp,4);
		ByteCpy(fout + 4,t_XOR,4);
		//right
		ByteCpy(fout + 8,fin + 8,4); //C[r,2]
		ByteCpy(temp,fin + 8,4); //C[r,3]
		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++)
			{
				ByteCpy(&C[i][j],&t_tables[32*k + 16 + 4*i + j][temp[j]],1);
			}
		}
		for (int i = 0; i < 4; i++)
		{
			ByteCpy(&t_XOR[i],&C[i][0],1);
			ByteXor(&t_XOR[i],&t_XOR[i],&C[i][1],1);
			ByteXor(&t_XOR[i],&t_XOR[i],&C[i][2],1);
			ByteXor(&t_XOR[i],&t_XOR[i],&C[i][3],1);
		}
		ByteXor(t_XOR,t_XOR,temp,4);
		ByteCpy(fout + 12,t_XOR,4);
		//<<<<
		if (k < r-1){
			ByteCpy(fin + 0,  fout + 4, 12);
			ByteCpy(fin + 12, fout + 0, 4);
		}
	}
	ByteCpy(y,fout,16);
}

void ClefiaF0Xor(unsigned char *dst, const unsigned char *src, const unsigned char *rk)
{
	unsigned char x[4], y[4], z[4];

	/* F0 */
	/* Key addition */
	ByteXor(x, src, rk, 4);
	/* Substitution layer */
	z[0] = clefia_s0[x[0]];
	z[1] = clefia_s1[x[1]];
	z[2] = clefia_s0[x[2]];
	z[3] = clefia_s1[x[3]];
	/* Diffusion layer (M0) */
	y[0] =            z[0]  ^ ClefiaMul2(z[1]) ^ ClefiaMul4(z[2]) ^ ClefiaMul6(z[3]);
	y[1] = ClefiaMul2(z[0]) ^            z[1]  ^ ClefiaMul6(z[2]) ^ ClefiaMul4(z[3]);
	y[2] = ClefiaMul4(z[0]) ^ ClefiaMul6(z[1]) ^            z[2]  ^ ClefiaMul2(z[3]);
	y[3] = ClefiaMul6(z[0]) ^ ClefiaMul4(z[1]) ^ ClefiaMul2(z[2]) ^            z[3] ;

	/* Xoring after F0 */
	ByteCpy(dst + 0, src + 0, 4);
	ByteXor(dst + 4, src + 4, y, 4);
}

void ClefiaF1Xor(unsigned char *dst, const unsigned char *src, const unsigned char *rk)
{
	unsigned char x[4], y[4], z[4];

	/* F1 */
	/* Key addition */
	ByteXor(x, src, rk, 4);
	/* Substitution layer */
	z[0] = clefia_s1[x[0]];
	z[1] = clefia_s0[x[1]];
	z[2] = clefia_s1[x[2]];
	z[3] = clefia_s0[x[3]];
	/* Diffusion layer (M1) */
	y[0] =            z[0]  ^ ClefiaMul8(z[1]) ^ ClefiaMul2(z[2]) ^ ClefiaMulA(z[3]);
	y[1] = ClefiaMul8(z[0]) ^            z[1]  ^ ClefiaMulA(z[2]) ^ ClefiaMul2(z[3]);
	y[2] = ClefiaMul2(z[0]) ^ ClefiaMulA(z[1]) ^            z[2]  ^ ClefiaMul8(z[3]);
	y[3] = ClefiaMulA(z[0]) ^ ClefiaMul2(z[1]) ^ ClefiaMul8(z[2]) ^            z[3] ;

	/* Xoring after F1 */
	ByteCpy(dst + 0, src + 0, 4);
	ByteXor(dst + 4, src + 4, y, 4);
}

void ClefiaGfn4(unsigned char *y, const unsigned char *x, const unsigned char *rk, int r)
{
	unsigned char fin[16], fout[16]; // 16 * 8 = 128
	// [0,3]|[4,7]|[8,11]|[12,15]
	//   P0    P1    P2      P3
	ByteCpy(fin, x, 16);
	while(r-- > 0){
		ClefiaF0Xor(fout + 0, fin + 0, rk + 0); // fin + 0 ==> P0
		ClefiaF1Xor(fout + 8, fin + 8, rk + 4); // fin + 8 ==> p2
		rk += 8; // rk point to next round
		if(r){ /* swapping for encryption */
			ByteCpy(fin + 0,  fout + 4, 12);
			ByteCpy(fin + 12, fout + 0, 4);
		}
	}
	ByteCpy(y, fout, 16);
}

void ClefiaGfn8(unsigned char *y, const unsigned char *x, const unsigned char *rk, int r)
{
	unsigned char fin[32], fout[32];

	ByteCpy(fin, x, 32);
	while(r-- > 0){
		ClefiaF0Xor(fout + 0,  fin + 0,  rk + 0);
		ClefiaF1Xor(fout + 8,  fin + 8,  rk + 4);
		ClefiaF0Xor(fout + 16, fin + 16, rk + 8);
		ClefiaF1Xor(fout + 24, fin + 24, rk + 12);
		rk += 16;
		if(r){ /* swapping for encryption */
			ByteCpy(fin + 0,  fout + 4, 28);
			ByteCpy(fin + 28, fout + 0, 4);
		}
	}
	ByteCpy(y, fout, 32);
}

void ClefiaGfn4Inv(unsigned char *y, const unsigned char *x, const unsigned char *rk, int r)
{
	unsigned char fin[16], fout[16];

	rk += (r - 1) * 8;
	ByteCpy(fin, x, 16);
	while(r-- > 0){
		ClefiaF0Xor(fout + 0, fin + 0, rk + 0);
		ClefiaF1Xor(fout + 8, fin + 8, rk + 4);
		rk -= 8;
		if(r){ /* swapping for decryption */
			ByteCpy(fin + 0, fout + 12, 4);
			ByteCpy(fin + 4, fout + 0,  12);
		}
	}
	ByteCpy(y, fout, 16);
}

void ClefiaDoubleSwap(unsigned char *lk) /*
										 X(128-bit) ==> Y(128-bit)
										 Y = X[7-63] | X[121-127] | X[0-6] | X[64-120]

										 X(16-Byte) ==> Y(16-Byte)
										 */
{
	unsigned char t[16];

	t[0]  = (lk[0] << 7) | (lk[1]  >> 1);
	t[1]  = (lk[1] << 7) | (lk[2]  >> 1);
	t[2]  = (lk[2] << 7) | (lk[3]  >> 1);
	t[3]  = (lk[3] << 7) | (lk[4]  >> 1);
	t[4]  = (lk[4] << 7) | (lk[5]  >> 1);
	t[5]  = (lk[5] << 7) | (lk[6]  >> 1);
	t[6]  = (lk[6] << 7) | (lk[7]  >> 1);
	t[7]  = (lk[7] << 7) | (lk[15] & 0x7fU);

	t[8]  = (lk[8]  >> 7) | (lk[0]  & 0xfeU);
	t[9]  = (lk[9]  >> 7) | (lk[8]  << 1);
	t[10] = (lk[10] >> 7) | (lk[9]  << 1);
	t[11] = (lk[11] >> 7) | (lk[10] << 1);
	t[12] = (lk[12] >> 7) | (lk[11] << 1);
	t[13] = (lk[13] >> 7) | (lk[12] << 1);
	t[14] = (lk[14] >> 7) | (lk[13] << 1);
	t[15] = (lk[15] >> 7) | (lk[14] << 1);

	ByteCpy(lk, t, 16);
}

void ClefiaConSet(unsigned char *con, const unsigned char *iv, int lk)
{
	unsigned char t[2];
	unsigned char tmp;

	ByteCpy(t, iv, 2);
	while(lk-- > 0){
		con[0] = t[0] ^ 0xb7U; /* P_16 = 0xb7e1 (natural logarithm) */
		con[1] = t[1] ^ 0xe1U;
		con[2] = ~((t[0] << 1) | (t[1] >> 7));
		con[3] = ~((t[1] << 1) | (t[0] >> 7));
		con[4] = ~t[0] ^ 0x24U; /* Q_16 = 0x243f (circle ratio) */
		con[5] = ~t[1] ^ 0x3fU;
		con[6] = t[1];
		con[7] = t[0];
		con += 8;

		/* updating T */
		if(t[1] & 0x01U){
			t[0] ^= 0xa8U;
			t[1] ^= 0x30U;
		}
		tmp = t[0] << 7;
		t[0] = (t[0] >> 1) | (t[1] << 7);
		t[1] = (t[1] >> 1) | tmp;
	}    
}

void ClefiaKeySet128(unsigned char *rk, const unsigned char *skey)
{
	const unsigned char iv[2] = {0x42U, 0x8aU}; /* cubic root of 2 */
	unsigned char lk[16];
	unsigned char con128[4 * 60];
	int i;

	/* generating CONi^(128) (0 <= i < 60, lk = 30) */
	ClefiaConSet(con128, iv, 30);
	/* GFN_{4,12} (generating L from K) */
	ClefiaGfn4(lk, skey, con128, 12);

	ByteCpy(rk, skey, 8); /* initial whitening key (WK0, WK1) */
	rk += 8;
	for(i = 0; i < 9; i++){ /* round key (RKi (0 <= i < 36)) */
		ByteXor(rk, lk, con128 + i * 16 + (4 * 24), 16);
		if(i % 2){
			ByteXor(rk, rk, skey, 16); /* Xoring K */
		}
		ClefiaDoubleSwap(lk); /* Updating L (DoubleSwap function) */
		rk += 16;
	}
	ByteCpy(rk, skey + 8, 8); /* final whitening key (WK2, WK3) */
}

void ClefiaKeySet192(unsigned char *rk, const unsigned char *skey)
{
	const unsigned char iv[2] = {0x71U, 0x37U}; /* cubic root of 3 */
	unsigned char skey256[32];
	unsigned char lk[32];
	unsigned char con192[4 * 84];
	int i;

	ByteCpy(skey256, skey, 24);
	for(i = 0; i < 8; i++){
		skey256[i + 24] = ~skey[i];
	}

	/* generating CONi^(192) (0 <= i < 84, lk = 42) */
	ClefiaConSet(con192, iv, 42);
	/* GFN_{8,10} (generating L from K) */
	ClefiaGfn8(lk, skey256, con192, 10);

	ByteXor(rk, skey256, skey256 + 16, 8); /* initial whitening key (WK0, WK1) */
	rk += 8;
	for(i = 0; i < 11; i++){ /* round key (RKi (0 <= i < 44)) */
		if((i / 2) % 2){
			ByteXor(rk, lk + 16, con192 + i * 16 + (4 * 40), 16); /* LR */
			if(i % 2){
				ByteXor(rk, rk, skey256 + 0,  16); /* Xoring KL */
			}
			ClefiaDoubleSwap(lk + 16); /* updating LR */
		}else{
			ByteXor(rk, lk + 0,  con192 + i * 16 + (4 * 40), 16); /* LL */
			if(i % 2){
				ByteXor(rk, rk, skey256 + 16, 16); /* Xoring KR */
			}
			ClefiaDoubleSwap(lk + 0);  /* updating LL */
		}
		rk += 16;
	}
	ByteXor(rk, skey256 + 8, skey256 + 24, 8); /* final whitening key (WK2, WK3) */
}

void ClefiaKeySet256(unsigned char *rk, const unsigned char *skey)
{
	const unsigned char iv[2] = {0xb5, 0xc0U}; /* cubic root of 5 */
	unsigned char lk[32];
	unsigned char con256[4 * 92];
	int i;

	/* generating CONi^(256) (0 <= i < 92, lk = 46) */
	ClefiaConSet(con256, iv, 46);
	/* GFN_{8,10} (generating L from K) */
	ClefiaGfn8(lk, skey, con256, 10);

	ByteXor(rk, skey, skey + 16, 8); /* initial whitening key (WK0, WK1) */
	rk += 8;
	for(i = 0; i < 13; i++){ /* round key (RKi (0 <= i < 52)) */
		if((i / 2) % 2){
			ByteXor(rk, lk + 16, con256 + i * 16 + (4 * 40), 16); /* LR */
			if(i % 2){
				ByteXor(rk, rk, skey + 0,  16); /* Xoring KL */
			}
			ClefiaDoubleSwap(lk + 16); /* updating LR */
		}else{
			ByteXor(rk, lk + 0,  con256 + i * 16 + (4 * 40), 16); /* LL */
			if(i % 2){
				ByteXor(rk, rk, skey + 16, 16); /* Xoring KR */
			}
			ClefiaDoubleSwap(lk + 0);  /* updating LL */
		}
		rk += 16;
	}
	ByteXor(rk, skey + 8, skey + 24, 8); /* final whitening key (WK2, WK3) */
}

int ClefiaKeySet(unsigned char *rk, const unsigned char *skey, const int key_bitlen)
{
	if(128 == key_bitlen){
		ClefiaKeySet128(rk, skey);
		return 18;
	}else if(192 == key_bitlen){
		ClefiaKeySet192(rk, skey);
		return 22;
	}else if(256 == key_bitlen){
		ClefiaKeySet256(rk, skey);
		return 26;
	}

	return 0; /* invalid key_bitlen */
}

void test(unsigned char * ct, const unsigned char * pt, unsigned char ** tables)
{
	unsigned char C1[4][4],C2[4][4];
	unsigned char fin[16], fout[16], temp[4];
	ByteCpy(fin,pt,16);
	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				C1[j][k] = tables[32*i + 4*j + k][fin[k]];
				C2[j][k] = tables[32*i + 16 + 4*j + k][fin[8 + k]];
			}
		}
		ByteCpy(fout,fin,4);

		for (int l = 0; l < 4; l++)
		{
			ByteXor(&temp[l],&C1[l][0],&C1[l][1],1);
			ByteXor(&temp[l],&temp[l],&C1[l][2],1);
			ByteXor(&temp[l],&temp[l],&C1[l][3],1);
		}
		ByteXor(fout+4,temp,fin+4,4);

		ByteCpy(fout+8,fin+8,4);

		for (int l = 0; l < 4; l++)
		{
			ByteXor(&temp[l],&C2[l][0],&C2[l][1],1);
			ByteXor(&temp[l],&temp[l],&C2[l][2],1);
			ByteXor(&temp[l],&temp[l],&C2[l][3],1);
		}
		ByteXor(fout+12,temp,fin+12,4);

		if(i!=17){
			ByteCpy(fin,fout+4,12);
			ByteCpy(fin+12,fout,4);
		}
	}
	ByteCpy(ct,fout,16);
}

void WBInterEnc128(unsigned char * ct, const unsigned char * pt, unsigned char ** tables)
{
	unsigned char items[32], fin[16], fout[16], temp[4];
	ByteCpy(fin,pt,16);
	for (int i = 0; i < 18; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				items[j*4 + k] = tables[i*32 + j + k*4][fin[j]];
				items[j*4 + k +16] = tables[i*32 + 16 + j +k*4][fin[8+j]];
			}
		}
		ByteCpy(fout,fin,4);

		ByteXor(temp,items,items+4,4);
		ByteXor(temp,temp,items+8,4);
		ByteXor(temp,temp,items+12,4);
		ByteXor(fout+4,temp,fin+4,4);

		ByteCpy(fout+8,fin+8,4);

		ByteXor(temp,items+16,items+20,4);
		ByteXor(temp,temp,items+24,4);
		ByteXor(temp,temp,items+28,4);
		ByteXor(fout+12,temp,fin+12,4);
		if (i != 17){
			ByteCpy(fin,fout+4,12);
			ByteCpy(fin+12,fout,4);
		}
	}
	ByteCpy(ct,fout,16);
}

void WBEncrypt(unsigned char ** table, unsigned char * Cout,unsigned char * ct, const unsigned char * pt, const int r)
{
	unsigned char rin[16], rout[16];

	ByteCpy(rin, pt, 16);

	unsigned char aa[4];
	ByteCpy(aa,rin+4,4);
	//BytePut(aa,4);//////
	ByteXor(rin + 0, rin + 0, Cout + 0, 4);
	ByteCpy(aa,rin+4,4);
	//BytePut(aa,4);////////
	ByteXor(rin + 8, rin + 8, Cout + 4, 4);

	WBGfn4(table, rout, rin, r);

	ByteCpy(ct, rout, 16);
	ByteXor(ct + 4, ct + 4, Cout + 8, 4);
	ByteXor(ct + 12, ct + 12, Cout + 12, 4);
}

void ClefiaEncrypt(unsigned char *ct, const unsigned char *pt, const unsigned char *rk, const int r)
{
	unsigned char rin[16], rout[16];

	ByteCpy(rin,  pt,  16);

	ByteXor(rin + 4,  rin + 4,  rk + 0, 4); /* initial key whitening */
	ByteXor(rin + 12, rin + 12, rk + 4, 4);
	rk += 8;

	ClefiaGfn4(rout, rin, rk, r); /* GFN_{4,r} */

	ByteCpy(ct, rout, 16);
	ByteXor(ct + 4,  ct + 4,  rk + r * 8 + 0, 4); /* final key whitening */
	ByteXor(ct + 12, ct + 12, rk + r * 8 + 4, 4);
}

void ClefiaDecrypt(unsigned char *pt, const unsigned char *ct, const unsigned char *rk, const int r)
{
	unsigned char rin[16], rout[16];

	ByteCpy(rin, ct, 16);

	ByteXor(rin + 4,  rin + 4,  rk + r * 8 + 8,  4); /* initial key whitening */
	ByteXor(rin + 12, rin + 12, rk + r * 8 + 12, 4);
	rk += 8;

	ClefiaGfn4Inv(rout, rin, rk, r); /* GFN^{-1}_{4,r} */

	ByteCpy(pt, rout, 16);
	ByteXor(pt + 4,  pt + 4,  rk - 8, 4); /* final key whitening */
	ByteXor(pt + 12, pt + 12, rk - 4, 4);
}