#ifndef _GetUsbIdentification_H_
#define _GetUsbIdentification_H_

#define _DEBUG_    //�Ƿ����������Ϣ

//��ȡU�����к�,PID+VID+������ʶ��ʽ(32λ).
//�ɹ������ƶ��豸����,USBSerialΪ�� 0x00 �ָ�Ķ�Ӧ����豸���к�
//ʧ�� < 0
//-1   ��ȡ�豸·������
//-2   ��ȡUKEY��ʶ��ʧ��
int GetUsbIdentification(char *USBSerial);

#endif