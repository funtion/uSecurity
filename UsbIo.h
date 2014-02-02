/*
           �ࣺ                  UsbIo
           ���ܣ�                ��д����
           ��Ҫ������            getRemovableDevice()  ��ȡ�ƶ��豸�̷�
                                 OpenDevice(...)          ���豸��������д֮ǰ������ô˺���
								 writeFileForUsb(...)     д��������  
                                 readFileForUsb(...)      ����������  
								 
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

	char* deviceName;  //���ڴ洢�豸�̷�

	UsbIo();

	~UsbIo();

	void getRemovableDevice(); //��ȡ�ƶ��豸�̷�

	//����д����
    //����ֵΪд�볤��
	DWORD writeFileForUsb(
//	__in	HANDLE hFile,                    //�豸���   
	__in	DWORD  dwMoveMethod,             //��ȡ��ʼ��,ΪFILE_BEGIN,FILE_END(����setFilePoint����)
	__in	LONG   DistanceToMoveLow,        //��λƫ����
	__in    PLONG  DistanceToMoveHigh,       //��λƫ����(ͨ������һΪ��ı�������Long temp=0,&temp����,�˴��ο�setFilePoint()��������)
	__in	BYTE*  pData,                    //Ҫд���ַ���
	__in	DWORD  nNumberOfBytesToWrite     //д��ĳ���
	);   

	DWORD writeFileForUsb(
//	__in 	HANDLE hFile,                      //�豸���  
	__in 	DWORD  sectorNumber,               //�������
	__in	DWORD  offest,                     //ƫ����
	__in 	BYTE*  pData,                      //Ҫд���ַ���
	__in 	DWORD  nNumberOfBytesToWrite       //д��ĳ���
	);

    
   //������ȡ����	
   //����ֵΪ��ȡ����
	DWORD readFileForUsb( 
//	__in	HANDLE hFile,                     //�豸���
	__in 	DWORD  sectorNumber,              //�������
	__in	DWORD  offest,                    //ƫ����
	__in 	BYTE*  pData,                     //��Ŷ�ȡ���ַ���                       
	__in 	DWORD  nNumberOfBytesToRead       //��ȡ����
	);


    

	DWORD readFileForUsb( 
//	__in    HANDLE hFile,                     //�豸���
    __in	DWORD  dwMoveMethod,              //��ȡ��ʼ��,ΪFILE_BEGIN,FILE_END(����setFilePoint����)
	__in 	LONG   DistanceToMoveLow,         //��λƫ����
	__in 	PLONG  DistanceToMoveHigh,        //��λƫ����(ͨ������һΪ��ı�������Long temp=0,&temp����,�˴��ο�setFilePoint()��������)
	__in 	BYTE*  pData,                     //��Ŷ�ȡ���ַ���
	__in 	DWORD  nNumberOfBytesToRead       //��ȡ����
	);



	HANDLE OpenDevice(LPCTSTR filename); //���豸��������װCraetFile()
    
	HANDLE getDeviceHandle();
	

private:
	HANDLE deviceHandle;   //�豸������

	
};