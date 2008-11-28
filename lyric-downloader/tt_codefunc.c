#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//static short length;	//存放Artist和Title一块内存的长度
//static char song[20];	//

static void show_bytes(unsigned char * start, int len)
{
	return;
	int i;
	for(i=len-1;i>=0;i--)
		printf("%.2x",start[i]);
	printf("\n");
}

int tt_CodeFunc(unsigned int Id, char* song)
{
	int tmp1;
	int tmp2=0;
	int tmp3=0;
	int i;
	
	short length = strlen(song);
	//0x00015F18
	tmp1 = (Id & 0x0000FF00) >> 8;		//右移8位后为0x0000015F
										//tmp1 0x0000005F
	if ( (Id & 0x00FF0000) == 0 ) {
		tmp3 = 0x000000FF & ~tmp1;		//CL 0x000000E7
	} else {
		tmp3 = 0x000000FF & ((Id & 0x00FF0000) >> 16);	//右移16位后为0x00000001
	}
	tmp3 = tmp3 | ((0x000000FF & Id) << 8);				//tmp3 0x00001801
	tmp3 = tmp3 << 8;									//tmp3 0x00180100
	tmp3 = tmp3 | (0x000000FF & tmp1);					//tmp3 0x0018015F
	tmp3 = tmp3 << 8;									//tmp3 0x18015F00
	if ( (Id & 0xFF000000) == 0 ) {
		tmp3 = tmp3 | (0x000000FF & (~Id));				//tmp3 0x18015FE7
	} else {
		tmp3 = tmp3 | (0x000000FF & (Id >> 24));		//右移24位后为0x00000000
	}
	
	//tmp3	18015FE7
	
	i=length-1;
	while(i >= 0){
		//printf("%d\n",*(song + i));
		tmp2 = (*(song + i)) + tmp2 + (tmp2 << (i%2 + 4));
		//show_bytes((unsigned char*)&tmp2,4);
		i--;
	}
	//tmp2 88203cc2
	i=0;
	tmp1=0;
	while(i<=length-1){
		tmp1 = (*(song+i)) + tmp1 + (tmp1 << (i%2 + 3));
		i++;
	}
	//EBX 5CC0B3BA
	
	//EDX = EBX | Id;
	//EBX = EBX | tmp3;
	//show_bytes((unsigned char*)&tmp1,4);
	//show_bytes((unsigned char*)&tmp2,4);
	//show_bytes((unsigned char*)&tmp3,4);
	return  ((tmp2 ^ tmp3) + (tmp1 | Id)) * (tmp1 | tmp3) * (tmp2 ^ Id);
}

