/*
           类：                  usbModel
           功能：                UKEY U盘操作封装
           主要函数：            get
								 
*/



#ifndef _USECURITY_H_
#define _USECURITY_H_

#include "UsbIo.h"

//最大尺寸512
#define KeySize			512
#define CheckSumSize	32

//UKey 返回值
#define OPERATION_SUCCESS		1
#define INVALID_DISK			0
#define	UNABLE_TO_READ			-1
#define UNABLE_TO_WRITE			-2
#define SPACE_IS_NOT_ENOUGH		-3
#define ALREADY_EXIST			-4
#define DANGOURS_ERROR			-100

class usbModel{
public:
	usbModel(char device);
	~usbModel(){};

	//制作UKey
	int createUKey(
		__in	char *Key,					//写入的密钥
		__in	char *CheckSum				//写入的校验和
	);

	//删除UKey
	int deleteUkey();

	//判断当前设备是否是UKey
	bool is_UKey(
		__out	DWORD *pKey=NULL,			//Key存储扇区
		__out	DWORD *pCheckSum=NULL		//CheckSum存储扇区
	);

	//获取当前设备标识符
	//失败 < 0
	//-1   获取设备路径出错
	//-2   获取UKEY标识符失败
	int getUsbId(
		__out	char *Serial				//设备标识符
	);

	//获取储存的Key
	bool getKey(
		__out char *Key						//密钥
	);

	bool getCheckSum(
		__out char *CheckSum		//校验和
	);

private:
	UsbIo UsbIO;				//U盘读写操作
	DWORD sector_Key,sector_CheckSum;

	bool findNullSector(DWORD &pKey, DWORD &pCheckSum);
};

#endif