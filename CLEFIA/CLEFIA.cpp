// CLEFIA.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	const unsigned char skey[32] = {
		0xffU,0xeeU,0xddU,0xccU,0xbbU,0xaaU,0x99U,0x88U,
		0x77U,0x66U,0x55U,0x44U,0x33U,0x22U,0x11U,0x00U,
		0xf0U,0xe0U,0xd0U,0xc0U,0xb0U,0xa0U,0x90U,0x80U,
		0x70U,0x60U,0x50U,0x40U,0x30U,0x20U,0x10U,0x00U
	};
	const unsigned char pt[16] = {
		0x00U,0x01U,0x02U,0x03U,0x04U,0x05U,0x06U,0x07U,
		0x08U,0x09U,0x0aU,0x0bU,0x0cU,0x0dU,0x0eU,0x0fU
	};
	unsigned char ct[16];
	unsigned char dst[16],dst2[16];
	unsigned char rk[8 * 26 + 16]; /* 8 bytes x 26 rounds(max) + whitening keys */
	int r;

	//随机数 (128-bit key)
	unsigned char R[(18 - 2) * 4];
	RandomNumber_32bit(R, (18 - 2));
	
	unsigned char C_out[4 * 4];
	RandomNumber_32bit(C_out, 4);

	printf("--- Test ---\n");
	printf("plaintext:  "); BytePut(pt, 16);
	printf("secretkey:  "); BytePut(skey, 32);

	/* for 128-bit key */
	printf("--- CLEFIA-128 ---\n");
	/* encryption */
	r = ClefiaKeySet(rk, skey, 128);
	ClefiaEncrypt(dst, pt, rk, r);
	printf("ciphertext: "); BytePut(dst, 16);


	/* for White-Box CLEFIA */
	printf("--- White-box CLEFIA ---\n");
	//whitening keys
	unsigned char wk[16];
	//ByteCpy(wk, skey, 16);
	ByteCpy(wk,rk,8);
	ByteCpy(wk+8,rk+8*18+16-8,8);

	//tables
	unsigned char * tables[576];
	for (int i = 0; i < 576; i++){
		tables[i] = (unsigned char *)calloc(256,sizeof(unsigned char));
	}
	WBTableSet128(tables, rk, R, wk, C_out);

	//加密
	printf("ciphertext: ");

	unsigned char pt_WB[16];
	ByteCpy(pt_WB, pt, 16);
	ByteXor(pt_WB + 0, pt_WB + 0, C_out + 0, 4);
	ByteXor(pt_WB + 8, pt_WB + 8, C_out + 4, 4);

	WBInterEnc128(dst2,pt_WB,tables);

	ByteXor(dst2 + 0, dst2 + 0, C_out + 8, 4);
	ByteXor(dst2 + 8, dst2 + 8, C_out + 12, 4);

	BytePut(dst2,16);

	printf("=====================================================\n");

	/* decryption */
	ByteCpy(ct, dst2, 16);
	r = ClefiaKeySet(rk, skey, 128);
	ClefiaDecrypt(dst2, ct, rk, r);
	printf("plaintext : "); BytePut(dst2, 16);

	/*for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			for (int k = 0; k < 16; k++)
			{
				printf("%02x, ",*(*(tables+i)+j*16+k));
			}
			printf("\n");
		}
		printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>.\n");
	}*/

	//* for 192-bit key */
	//printf("--- CLEFIA-192 ---\n");
	//* encryption */
	//r = ClefiaKeySet(rk, skey, 192);
	//ClefiaEncrypt(dst, pt, rk, r);
	//printf("ciphertext: "); BytePut(dst, 16);
	//* decryption */
	//ByteCpy(ct, dst, 16);
	//r = ClefiaKeySet(rk, skey, 192);
	//ClefiaDecrypt(dst, ct, rk, r);
	//printf("plaintext : "); BytePut(dst, 16);

	//* for 256-bit key */
	//printf("--- CLEFIA-256 ---\n");
	//* encryption */
	//r = ClefiaKeySet(rk, skey, 256);
	//ClefiaEncrypt(dst, pt, rk, r);
	//printf("ciphertext: "); BytePut(dst, 16);
	//* decryption */
	//ByteCpy(ct, dst, 16);
	//r = ClefiaKeySet(rk, skey, 256);
	//ClefiaDecrypt(dst, ct, rk, r);
	//printf("plaintext : "); BytePut(dst, 16);

	return 0;
}

