#include "GetUsbIdentification.h"

#include <Windows.h>
#include <WinIoCtl.h>
#include <InitGuid.h>
#include <SetupAPI.h>
#include <stdio.h>
#pragma comment(lib,"setupapi.lib")

#define MAX_DEVICE 256
#define SERIAL_SIZE 33

// IOCTL控制码
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

DEFINE_GUID(UsbClassGuid, 0xa5dcbf10L, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed);
#define INTERFACE_DETAIL_SIZE 1024

// 查询存储设备属性的类型
/*typedef enum _STORAGE_QUERY_TYPE {
    PropertyStandardQuery = 0,          // 读取描述
    PropertyExistsQuery,                // 测试是否支持
    PropertyMaskQuery,                  // 读取指定的描述
    PropertyQueryMaxDefined             // 验证数据
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;
*/

// 查询存储设备还是适配器属性
//typedef enum _STORAGE_PROPERTY_ID {
//    StorageDeviceProperty = 0,          // 查询设备属性
//    StorageAdapterProperty              // 查询适配器属性
//} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;
// 
// 查询属性输入的数据结构
//typedef struct _STORAGE_PROPERTY_QUERY {
//    STORAGE_PROPERTY_ID PropertyId;     // 设备/适配器
//    STORAGE_QUERY_TYPE QueryType;       // 查询类型 
//    UCHAR AdditionalParameters[1];      // 额外的数据(仅定义了象征性的1个字节)
//} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;
// 
//// 查询属性输出的数据结构
//typedef struct _STORAGE_DEVICE_DESCRIPTOR {
//    ULONG Version;                    // 版本
//    ULONG Size;                       // 结构大小
//    UCHAR DeviceType;                 // 设备类型
//    UCHAR DeviceTypeModifier;         // SCSI-2额外的设备类型
//    BOOLEAN RemovableMedia;           // 是否可移动
//    BOOLEAN CommandQueueing;          // 是否支持命令队列
//    ULONG VendorIdOffset;             // 厂家设定值的偏移
//    ULONG ProductIdOffset;            // 产品ID的偏移
//    ULONG ProductRevisionOffset;      // 产品版本的偏移
//    ULONG SerialNumberOffset;         // 序列号的偏移
//    STORAGE_BUS_TYPE BusType;         // 总线类型
//    ULONG RawPropertiesLength;        // 额外的属性数据长度
//    UCHAR RawDeviceProperties[1];     // 额外的属性数据(仅定义了象征性的1个字节)
//} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

int GetDevicePath(LPGUID lpGuid, LPTSTR* pszDevicePath)
{
	HDEVINFO hDevInfoSet;    //设备信息集句柄；
	SP_DEVICE_INTERFACE_DATA ifdata;
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDetail;
	int nCount;
	BOOL bResult;

	// 取得一个该GUID相关的设备信息集句柄
	hDevInfoSet = ::SetupDiGetClassDevs((LPGUID)&UsbClassGuid,     // class GUID 
		NULL,                    // 无关键字
		NULL,                    // 不指定父窗口句柄
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);    // 目前存在的设备

	// 失败...
	if (hDevInfoSet == INVALID_HANDLE_VALUE)
	{
		//printf("Error!\n");
		//   break;
		return -1;
	}
	// 申请设备接口数据空间
	pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT, INTERFACE_DETAIL_SIZE);

	pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	nCount = 0;
	bResult = TRUE;

	// 设备序号=0,1,2... 逐一测试设备接口，到失败为止
	while (bResult)
	{
		ifdata.cbSize = sizeof(ifdata);

		// 枚举符合该GUID的设备接口
		bResult = ::SetupDiEnumDeviceInterfaces(
			hDevInfoSet,     // 设备信息集句柄
			NULL,            // 不需额外的设备描述
			lpGuid,          // GUID
			(ULONG)nCount,   // 设备信息集里的设备序号
			&ifdata);        // 设备接口信息

		if (bResult)
		{
			// 取得该设备接口的细节(设备路径)
			bResult = SetupDiGetInterfaceDeviceDetail(
				hDevInfoSet,    // 设备信息集句柄
				&ifdata,        // 设备接口信息
				pDetail,        // 设备接口细节(设备路径)
				INTERFACE_DETAIL_SIZE,   // 输出缓冲区大小
				NULL,           // 不需计算输出缓冲区大小(直接用设定值)
				NULL);          // 不需额外的设备描述
			if (bResult)
			{
				// 复制设备路径到输出缓冲区
				wcscpy_s((wchar_t *)pszDevicePath[nCount],wcslen((wchar_t *)pDetail->DevicePath)+1,(wchar_t *)pDetail->DevicePath);
				// 调整计数值
				nCount++;
			}
		}
	}
	// printf("%d\n", nCount);
	// printf("%s\n",pszDevicePath[1]);
	// 释放设备接口数据空间
	::GlobalFree(pDetail);
	// 关闭设备信息集句柄
	::SetupDiDestroyDeviceInfoList(hDevInfoSet);
	return nCount;
}

HANDLE OpenDevice(char* DevicePath)
// 返回句柄 hDevice
{
	HANDLE hDevice;
	hDevice = CreateFile(DevicePath,
		GENERIC_READ && GENERIC_WRITE,
		FILE_SHARE_READ && FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	return hDevice;
}

int GetUsbIdentification(char *USBSerial)
{
	int i, nDevice; // nDevice 表示有多少个USB设备
	int ndevice = 0; // 表示多少个U盘设备(可进行读写的设备)
	char* szDevicePath[MAX_DEVICE];        // 设备路径

	// 定义一个 PSTORAGE_DEVICE_DESCRIPTOR 变量，存放设备属性
	PSTORAGE_DEVICE_DESCRIPTOR DeviceDesc;
	// 变量初始化
	DeviceDesc=(PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
	DeviceDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

	// 分配需要的空间
	for (i = 0; i < MAX_DEVICE; i++)
	{
		szDevicePath[i] = new char[256];
	}

	// 取设备路径
	nDevice = ::GetDevicePath((LPGUID)&UsbClassGuid,(LPTSTR *)szDevicePath);
#ifdef _DEBUG_
	for(i = 0; i< nDevice; i++)
	   printf("Movable Device %d:\n%s\n",i+1,szDevicePath[i]);
#endif
	if (nDevice < 0) return -1;

	// 逐一获取设备信息
	for (i = 0; i < nDevice; i++)
	{
		int j;
		for(j = 0; j < 4; j++)
		{
			USBSerial[ndevice*SERIAL_SIZE+j] = szDevicePath[i][j+12]; 
		}
		for(; j < 8; j++)
		{
			USBSerial[ndevice*SERIAL_SIZE+j] = szDevicePath[i][j+17]; 
		}
		for(; j < 32; j++)
		{
			USBSerial[ndevice*SERIAL_SIZE+j] = szDevicePath[i][j+18]; 
		}
		USBSerial[ndevice*SERIAL_SIZE+j]=0;
#ifdef _DEBUG_
		printf("Serial Number：");
		printf("%s\n", &USBSerial[ndevice*SERIAL_SIZE]);
#endif
		ndevice++;
	} 

	// 释放空间
	for (i = 0; i < MAX_DEVICE; i++)
	{
		delete []szDevicePath[i];
	}
	return nDevice;
}