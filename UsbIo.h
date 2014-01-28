/*
           类：                  UsbIo
           功能：                读写扇区
           主要函数：            getRemovableDevice()  获取移动设备盘符
                                 OpenDevice(...)          打开设备函数，读写之前必须调用此函数
								 writeFileForUsb(...)     写扇区函数  
                                 readFileForUsb(...)      读扇区函数  
								 
*/
#include <windows.h>
#define ReadFailed -1
#define WriteFailed -2
#define ReadSucceed 1
#define WriteSucceed 2
#define __in                  
#define __out
#define perSectorSize 512

class UsbIo
{
public:

	char* deviceName;  //用于存储设备盘符

	UsbIo();

	~UsbIo();

	void getRemovableDevice(); //获取移动设备盘符

	//扇区写函数
    //返回值为写入长度
	DWORD writeFileForUsb(
//	__in	HANDLE hFile,                    //设备句柄   
	__in	DWORD  dwMoveMethod,             //读取起始点,为FILE_BEGIN,FILE_END(参照setFilePoint函数)
	__in	LONG   DistanceToMoveLow,        //低位偏移量
	__in    PLONG  DistanceToMoveHigh,       //高位偏移量(通常传入一为零的变量，如Long temp=0,&temp即可,此处参考setFilePoint()函数参数)
	__in	BYTE*  pData,                    //要写的字符串
	__in	DWORD  nNumberOfBytesToWrite     //写入的长度
	);   

	DWORD writeFileForUsb(
//	__in 	HANDLE hFile,                      //设备句柄  
	__in 	DWORD  sectorNumber,               //扇区序号
	__in	DWORD  offest,                     //偏移量
	__in 	BYTE*  pData,                      //要写的字符串
	__in 	DWORD  nNumberOfBytesToWrite       //写入的长度
	);

    
   //扇区读取函数	
   //返回值为读取长度
	DWORD readFileForUsb( 
//	__in	HANDLE hFile,                     //设备句柄
	__in 	DWORD  sectorNumber,              //扇区序号
	__in	DWORD  offest,                    //偏移量
	__in 	BYTE*  pData,                     //存放读取的字符串                       
	__in 	DWORD  nNumberOfBytesToRead       //读取长度
	);


    

	DWORD readFileForUsb( 
//	__in    HANDLE hFile,                     //设备句柄
    __in	DWORD  dwMoveMethod,              //读取起始点,为FILE_BEGIN,FILE_END(参照setFilePoint函数)
	__in 	LONG   DistanceToMoveLow,         //低位偏移量
	__in 	PLONG  DistanceToMoveHigh,        //高位偏移量(通常传入一为零的变量，如Long temp=0,&temp即可,此处参考setFilePoint()函数参数)
	__in 	BYTE*  pData,                     //存放读取的字符串
	__in 	DWORD  nNumberOfBytesToRead       //读取长度
	);



	HANDLE OpenDevice(LPCTSTR filename); //打开设备函数，封装CraetFile()
    
	HANDLE getDeviceHandle();
	

private:
	HANDLE deviceHandle;   //设备句柄句柄

	
};