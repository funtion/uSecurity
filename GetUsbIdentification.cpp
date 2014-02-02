#include "GetUsbIdentification.h"

#include <Windows.h>
#include <winioctl.h>
#include <initguid.h>
#include <setupapi.h>
#include <stdio.h>
#include "usbiodef.h"

#ifndef __BORLANDC__
#pragma comment(lib, "setupapi.lib")
#endif

bool GetUsbIdentification(char cDiskID, char *lpPID)
{
	char szDrv[4];
	sprintf(szDrv, "%c:\\", cDiskID);
	if(GetDriveType(szDrv) != DRIVE_REMOVABLE)
	{
		return false;
	}

	char lpRegPath[512] = { 0 };
	char lpRegValue[256] = { 0 };
	sprintf(lpRegPath, "SYSTEM\\MountedDevices");
	sprintf(lpRegValue, "\\DosDevices\\%c:", cDiskID);
	//
	DWORD dwDataSize(0);
	DWORD dwRegType(REG_BINARY);
	LPBYTE lpRegBinData(NULL);
	LPSTR lpUSBKeyData(NULL);

	// 查询注册表中映射驱动器的设备信息
	HKEY hKey;
	long lRet = ::RegOpenKeyEx(
		HKEY_LOCAL_MACHINE, // root key
		lpRegPath, // 要访问的键的位置
		0,         //
		KEY_READ,  // 以查询的方式访问注册表
		&hKey);    // hKEY保存此函数所打开的键的句柄
	if(lRet != ERROR_SUCCESS)
		return false;
	else
	{
		lRet = ::RegQueryValueEx(hKey, // 所打开的键的句柄
			lpRegValue,    // 要查询的键值名
			NULL,
			&dwRegType,    // 查询数据的类型
			lpRegBinData,  // 保存所查询的数据
			&dwDataSize);  // 预设置的数据长度
		if(lRet != ERROR_SUCCESS)
		{
			::RegCloseKey(hKey);
			return false;
		}
		else
		{
			lpRegBinData = new BYTE[dwDataSize];
			lpUSBKeyData = new char[dwDataSize];
			memset(lpUSBKeyData, 0, dwDataSize);
			lRet = ::RegQueryValueEx(hKey,
				lpRegValue,
				NULL,
				&dwRegType,
				lpRegBinData,
				&dwDataSize);
			if(lRet != ERROR_SUCCESS)
			{
				delete []lpRegBinData;
				delete []lpUSBKeyData;
				::RegCloseKey(hKey);
				return false;
			}
		}
	}
	::RegCloseKey(hKey);
	// 过滤二进制串中的无用信息(将0x0字符去除)
	int j = 0;
	for(DWORD i=0; i<dwDataSize; i++)
	{
		if(lpRegBinData[i] != 0x0)
		{
			lpUSBKeyData[j] = lpRegBinData[i];
			j++;
		}
	}
	delete []lpRegBinData;

	// 截取lpUSBKeyData中的有用信息, 例: 7&100a16f&0
	LPSTR lpPos = NULL;
	lpPos = strtok(lpUSBKeyData,"#");
	lpPos = strtok(NULL,"#");
	lpPos = strtok(NULL,"#");
	lpPos = strtok(lpPos,"&");
	strcpy(lpUSBKeyData, strupr(lpPos));


	// (USB设备)的GUID
	GUID guidUSB=GUID_DEVINTERFACE_USB_DEVICE;

	//
	HDEVINFO hUSB = SetupDiGetClassDevs(
		&guidUSB, NULL, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if(hUSB == INVALID_HANDLE_VALUE)
	{
		delete []lpUSBKeyData;
		return false;
	}

	//
	int nDevIndex = 0;
	bool bSuccess;
	SP_DEVINFO_DATA DevData;
	SP_DEVICE_INTERFACE_DATA DevIntData;
	PSP_DEVICE_INTERFACE_DETAIL_DATA lpDevIntDetailData;
	DWORD dwBytesReturned;
	LPSTR lpPos1=NULL,lpPos2=NULL;

	// 遍历设备
	do
	{
		DevIntData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		bSuccess = SetupDiEnumDeviceInterfaces(hUSB, NULL, &guidUSB,
			nDevIndex, &DevIntData) > 0;

		if(bSuccess)
		{
			// 获取接口详细信息
			DevData.cbSize = sizeof(SP_DEVINFO_DATA);
			dwBytesReturned = 0;
			SetupDiGetDeviceInterfaceDetailA(hUSB, &DevIntData,
				NULL, 0, &dwBytesReturned, &DevData);
			if(dwBytesReturned != 0 && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				lpDevIntDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)
					GlobalAlloc(GMEM_FIXED, dwBytesReturned);
				lpDevIntDetailData->cbSize =
					sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
				if(SetupDiGetDeviceInterfaceDetailA(hUSB, &DevIntData,
					lpDevIntDetailData, dwBytesReturned, &dwBytesReturned, &DevData))
				{
					// 取得设备接口详细信息并根据转化后的路径在注册表中查询
					LPSTR lpPathTemp = new char[strlen(lpDevIntDetailData->DevicePath) + 1];
					strcpy(lpPathTemp, lpDevIntDetailData->DevicePath);
					lpPos1 = strtok(lpPathTemp,"#");
					lpPos1 = strtok(NULL,"#");
					lpPos2 = strtok(NULL,"#");
					
					if ( !strcmp(strupr(lpPos2),lpPos) )
					{
						strcpy(lpPID,lpPos1);
						strcat(lpPID,"&");
						strcat(lpPID,lpPos2);
						delete []lpPathTemp;
						GlobalFree(lpDevIntDetailData);
						break;
					}
					delete []lpPathTemp;
					nDevIndex++;
				}
				GlobalFree(lpDevIntDetailData);
			}
		}
	}while(bSuccess);

	SetupDiDestroyDeviceInfoList(hUSB);

	delete []lpUSBKeyData;

	return true;
}