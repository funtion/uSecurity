#include "GetUsbIdentification.h"

#include <Windows.h>
#include <WinIoCtl.h>
#include <InitGuid.h>
#include <SetupAPI.h>
#include <stdio.h>
#pragma comment(lib,"setupapi.lib")

#define MAX_DEVICE 256
#define SERIAL_SIZE 33

// IOCTL������
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)

DEFINE_GUID(UsbClassGuid, 0xa5dcbf10L, 0x6530, 0x11d2, 0x90, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x51, 0xed);
#define INTERFACE_DETAIL_SIZE 1024

// ��ѯ�洢�豸���Ե�����
/*typedef enum _STORAGE_QUERY_TYPE {
    PropertyStandardQuery = 0,          // ��ȡ����
    PropertyExistsQuery,                // �����Ƿ�֧��
    PropertyMaskQuery,                  // ��ȡָ��������
    PropertyQueryMaxDefined             // ��֤����
} STORAGE_QUERY_TYPE, *PSTORAGE_QUERY_TYPE;
*/

// ��ѯ�洢�豸��������������
//typedef enum _STORAGE_PROPERTY_ID {
//    StorageDeviceProperty = 0,          // ��ѯ�豸����
//    StorageAdapterProperty              // ��ѯ����������
//} STORAGE_PROPERTY_ID, *PSTORAGE_PROPERTY_ID;
// 
// ��ѯ������������ݽṹ
//typedef struct _STORAGE_PROPERTY_QUERY {
//    STORAGE_PROPERTY_ID PropertyId;     // �豸/������
//    STORAGE_QUERY_TYPE QueryType;       // ��ѯ���� 
//    UCHAR AdditionalParameters[1];      // ���������(�������������Ե�1���ֽ�)
//} STORAGE_PROPERTY_QUERY, *PSTORAGE_PROPERTY_QUERY;
// 
//// ��ѯ������������ݽṹ
//typedef struct _STORAGE_DEVICE_DESCRIPTOR {
//    ULONG Version;                    // �汾
//    ULONG Size;                       // �ṹ��С
//    UCHAR DeviceType;                 // �豸����
//    UCHAR DeviceTypeModifier;         // SCSI-2������豸����
//    BOOLEAN RemovableMedia;           // �Ƿ���ƶ�
//    BOOLEAN CommandQueueing;          // �Ƿ�֧���������
//    ULONG VendorIdOffset;             // �����趨ֵ��ƫ��
//    ULONG ProductIdOffset;            // ��ƷID��ƫ��
//    ULONG ProductRevisionOffset;      // ��Ʒ�汾��ƫ��
//    ULONG SerialNumberOffset;         // ���кŵ�ƫ��
//    STORAGE_BUS_TYPE BusType;         // ��������
//    ULONG RawPropertiesLength;        // ������������ݳ���
//    UCHAR RawDeviceProperties[1];     // �������������(�������������Ե�1���ֽ�)
//} STORAGE_DEVICE_DESCRIPTOR, *PSTORAGE_DEVICE_DESCRIPTOR;

int GetDevicePath(LPGUID lpGuid, LPTSTR* pszDevicePath)
{
	HDEVINFO hDevInfoSet;    //�豸��Ϣ�������
	SP_DEVICE_INTERFACE_DATA ifdata;
	PSP_DEVICE_INTERFACE_DETAIL_DATA pDetail;
	int nCount;
	BOOL bResult;

	// ȡ��һ����GUID��ص��豸��Ϣ�����
	hDevInfoSet = ::SetupDiGetClassDevs((LPGUID)&UsbClassGuid,     // class GUID 
		NULL,                    // �޹ؼ���
		NULL,                    // ��ָ�������ھ��
		DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);    // Ŀǰ���ڵ��豸

	// ʧ��...
	if (hDevInfoSet == INVALID_HANDLE_VALUE)
	{
		//printf("Error!\n");
		//   break;
		return -1;
	}
	// �����豸�ӿ����ݿռ�
	pDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::GlobalAlloc(LMEM_ZEROINIT, INTERFACE_DETAIL_SIZE);

	pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	nCount = 0;
	bResult = TRUE;

	// �豸���=0,1,2... ��һ�����豸�ӿڣ���ʧ��Ϊֹ
	while (bResult)
	{
		ifdata.cbSize = sizeof(ifdata);

		// ö�ٷ��ϸ�GUID���豸�ӿ�
		bResult = ::SetupDiEnumDeviceInterfaces(
			hDevInfoSet,     // �豸��Ϣ�����
			NULL,            // ���������豸����
			lpGuid,          // GUID
			(ULONG)nCount,   // �豸��Ϣ������豸���
			&ifdata);        // �豸�ӿ���Ϣ

		if (bResult)
		{
			// ȡ�ø��豸�ӿڵ�ϸ��(�豸·��)
			bResult = SetupDiGetInterfaceDeviceDetail(
				hDevInfoSet,    // �豸��Ϣ�����
				&ifdata,        // �豸�ӿ���Ϣ
				pDetail,        // �豸�ӿ�ϸ��(�豸·��)
				INTERFACE_DETAIL_SIZE,   // �����������С
				NULL,           // ������������������С(ֱ�����趨ֵ)
				NULL);          // ���������豸����
			if (bResult)
			{
				// �����豸·�������������
				wcscpy_s((wchar_t *)pszDevicePath[nCount],wcslen((wchar_t *)pDetail->DevicePath)+1,(wchar_t *)pDetail->DevicePath);
				// ��������ֵ
				nCount++;
			}
		}
	}
	// printf("%d\n", nCount);
	// printf("%s\n",pszDevicePath[1]);
	// �ͷ��豸�ӿ����ݿռ�
	::GlobalFree(pDetail);
	// �ر��豸��Ϣ�����
	::SetupDiDestroyDeviceInfoList(hDevInfoSet);
	return nCount;
}

HANDLE OpenDevice(char* DevicePath)
// ���ؾ�� hDevice
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
	int i, nDevice; // nDevice ��ʾ�ж��ٸ�USB�豸
	int ndevice = 0; // ��ʾ���ٸ�U���豸(�ɽ��ж�д���豸)
	char* szDevicePath[MAX_DEVICE];        // �豸·��

	// ����һ�� PSTORAGE_DEVICE_DESCRIPTOR ����������豸����
	PSTORAGE_DEVICE_DESCRIPTOR DeviceDesc;
	// ������ʼ��
	DeviceDesc=(PSTORAGE_DEVICE_DESCRIPTOR)new BYTE[sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1];
	DeviceDesc->Size = sizeof(STORAGE_DEVICE_DESCRIPTOR) + 512 - 1;

	// ������Ҫ�Ŀռ�
	for (i = 0; i < MAX_DEVICE; i++)
	{
		szDevicePath[i] = new char[256];
	}

	// ȡ�豸·��
	nDevice = ::GetDevicePath((LPGUID)&UsbClassGuid,(LPTSTR *)szDevicePath);
#ifdef _DEBUG_
	for(i = 0; i< nDevice; i++)
	   printf("Movable Device %d:\n%s\n",i+1,szDevicePath[i]);
#endif
	if (nDevice < 0) return -1;

	// ��һ��ȡ�豸��Ϣ
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
		printf("Serial Number��");
		printf("%s\n", &USBSerial[ndevice*SERIAL_SIZE]);
#endif
		ndevice++;
	} 

	// �ͷſռ�
	for (i = 0; i < MAX_DEVICE; i++)
	{
		delete []szDevicePath[i];
	}
	return nDevice;
}