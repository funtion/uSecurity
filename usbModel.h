/*
           �ࣺ                  usbModel
           ���ܣ�                UKEY U�̲�����װ
           ��Ҫ������            get
								 
*/



#ifndef _USECURITY_H_
#define _USECURITY_H_

#include "UsbIo.h"

//���ߴ�512
#define KeySize			512
#define CheckSumSize	32

//UKey ����ֵ
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

	//����UKey
	int createUKey(
		__in	char *Key,					//д�����Կ
		__in	char *CheckSum				//д���У���
	);

	//ɾ��UKey
	int deleteUkey();

	//�жϵ�ǰ�豸�Ƿ���UKey
	bool is_UKey(
		__out	DWORD *pKey=NULL,			//Key�洢����
		__out	DWORD *pCheckSum=NULL		//CheckSum�洢����
	);

	//��ȡ��ǰ�豸��ʶ��
	//ʧ�� < 0
	//-1   ��ȡ�豸·������
	//-2   ��ȡUKEY��ʶ��ʧ��
	int getUsbId(
		__out	char *Serial				//�豸��ʶ��
	);

	//��ȡ�����Key
	bool getKey(
		__out char *Key						//��Կ
	);

	bool getCheckSum(
		__out char *CheckSum		//У���
	);

private:
	UsbIo UsbIO;				//U�̶�д����
	DWORD sector_Key,sector_CheckSum;

	bool findNullSector(DWORD &pKey, DWORD &pCheckSum);
};

#endif