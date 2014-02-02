#include "GetUsbIdentification.h"
#include "usbModel.h"
#include <stdio.h>
#include <iostream>
using namespace std;
int main()
{
	char Ser[256];
	usbModel t('H');


/*
	char Key[513],CheckSum[33];
	char tKey[513],tCheck[33];
	memset(Key,0x55,512);
	memset(CheckSum,0x11,32);
	Key[512]=CheckSum[32]=0;
	cout<<Key<<endl<<CheckSum<<endl;
	cout<<t.createUKey(Key,CheckSum)<<endl;
	cout<<t.getKey(tKey)<<endl;
	cout<<t.getCheckSum(tCheck)<<endl;
	tKey[512]=tCheck[32]=0;
	cout<<tKey<<endl<<tCheck<<endl;*/
	cout<<t.getUsbId(Ser)<<endl;
	cout<<Ser<<endl;
	//cout<<t.deleteUkey()<<endl;


	getchar();
	return 1;
}