#ifndef _GetUsbIdentification_H_
#define _GetUsbIdentification_H_

//---------------------------------------------------------------------------
// ��ȡָ���̷������к�(���U����Ч)
// char cDiskID: ָ���̷����� 'L'
// LPSTR lpPID : ���кŴ�ŵĻ��壬������Ҫ256�ֽڲ��ܱ�֤�����
//---------------------------------------------------------------------------
bool GetUsbIdentification(char cDiskID, char *lpPID);

#endif