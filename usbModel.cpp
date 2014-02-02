#include "usbModel.h"
#include "GetUsbIdentification.h"

#include <stdio.h>
#include <string.h>
#include "Byte2HexTable.h"

usbModel::usbModel(char device)
{
	sector_Key=sector_CheckSum=0;
	char deviceName[20];
	sprintf(deviceName,"\\\\.\\%c:",device);
	UsbIO.OpenDevice(deviceName);
}

int usbModel::getUsbId(char *Serial )
{
	return 0;
}

bool usbModel::is_UKey(DWORD *pKey/* =NULL */, DWORD *pCheckSum/* =NULL */ )
{
	BYTE pData[6];				//������
	LARGE_INTEGER li ={0};

	//��ȡ�ڶ�������5�ֽ�
	if ( 5 == UsbIO.readFileForUsb(FILE_BEGIN,li.LowPart+perSectorSize*2-5,&li.HighPart,pData,5))
		//��֤UKey���
		if (!strncmp((char*)&pData[2],"\xAA\x55\xAA",3))
		{
			if (pKey) *pKey = (int)pData[0];
			sector_Key=(int)pData[0];
			if (pCheckSum) *pCheckSum = (int)pData[1];
			sector_CheckSum=(int)pData[1];
			return true;
		}
	return false;
}

int usbModel::createUKey(char *Key, char *CheckSum )
{
	BYTE pData[513];		//������
	LARGE_INTEGER li ={0};

	//��֤UKey���
	if (is_UKey())	return ALREADY_EXIST;

	//��֤���̱��
	if ( 2 != UsbIO.readFileForUsb(FILE_BEGIN,li.LowPart+perSectorSize-2,&li.HighPart,pData,2))
		return UNABLE_TO_READ;
	if (strncmp((char*)pData,"\x55\xAA",2)) return INVALID_DISK;

	//д����Կ��Ϣ
	if (!findNullSector(sector_Key,sector_CheckSum))	return SPACE_IS_NOT_ENOUGH;
	if ( KeySize != UsbIO.writeFileForUsb(sector_Key,0,(BYTE*)Key,KeySize))	return UNABLE_TO_WRITE;
	if ( CheckSumSize != UsbIO.writeFileForUsb(sector_CheckSum,0,(BYTE*)CheckSum,CheckSumSize)) return	UNABLE_TO_WRITE;

	//д��UKey���
	pData[0] = (BYTE)sector_Key;
	pData[1] = (BYTE)sector_CheckSum;
	pData[2] = 0xAA; pData[3] = 0x55; pData[4] = 0xAA;
	if ( 5 != UsbIO.writeFileForUsb(FILE_BEGIN,li.LowPart+perSectorSize*2-5,&li.HighPart,pData,5))
		return UNABLE_TO_WRITE;

	//��֤UKey���
	if ( !is_UKey() ) return UNABLE_TO_WRITE;

	return OPERATION_SUCCESS;
}

int usbModel::deleteUkey()
{
	BYTE pData[513];	//������
	LARGE_INTEGER li ={0};

	//��֤UKey
	if ( !is_UKey() ) return INVALID_DISK;

	//���Key��Ϣ
	memset(pData,0,512);
	if ( perSectorSize != UsbIO.writeFileForUsb(sector_Key,0,pData,perSectorSize))
		return UNABLE_TO_WRITE;
	if ( perSectorSize != UsbIO.writeFileForUsb(sector_CheckSum,0,pData,perSectorSize))
	{
		if ( 1 != UsbIO.writeFileForUsb(FILE_BEGIN,li.LowPart+perSectorSize*2-3,&li.HighPart,0x00,1))
			return DANGOURS_ERROR;//Σ�մ���
		return UNABLE_TO_WRITE;
	}

	//���UKey���
	if ( 5 != UsbIO.writeFileForUsb(FILE_BEGIN,li.LowPart+perSectorSize*2-5,&li.HighPart,pData,5))
		return DANGOURS_ERROR;//Σ�մ���

	return OPERATION_SUCCESS;
}

bool usbModel::getKey(char *Key )
{
	//��ȡKey�洢����
	if (!sector_Key) 
		if ( !is_UKey())
			return false;

	//��ȡKey
	if ( KeySize != UsbIO.readFileForUsb(sector_Key,0,(BYTE*)Key,KeySize)) return false;

	return true;
}

bool usbModel::getCheckSum(char *CheckSum )
{
	//��ȡCheckSum�洢����
	if (!sector_CheckSum) 
		if ( !is_UKey())
			return false;

	//��ȡKey
	if ( CheckSumSize != UsbIO.readFileForUsb(sector_CheckSum,0,(BYTE*)CheckSum,CheckSumSize)) return false;

	return true;
}

bool usbModel::findNullSector(DWORD &pKey, DWORD &pCheckSum)
{
	BYTE pData[513];
	pKey=pCheckSum=0;
	int sector,i=0;
	for (sector=2; sector < 256 ; sector++,i=0)
	{
		if ( perSectorSize == UsbIO.readFileForUsb(sector,0,pData,perSectorSize))
		{
			while(!pData[i++] && i<512){};
			if (i==512)
			{
				if(!pKey)
					pKey=sector;
				else
				{
					pCheckSum=sector;
					return true;
				}
			}
		}
	}
	return false;
}