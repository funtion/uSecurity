#ifndef _GetUsbIdentification_H_
#define _GetUsbIdentification_H_

//---------------------------------------------------------------------------
// 获取指定盘符的序列号(针对U盘有效)
// char cDiskID: 指定盘符，如 'L'
// LPSTR lpPID : 序列号存放的缓冲，至少需要256字节才能保证不溢出
//---------------------------------------------------------------------------
bool GetUsbIdentification(char cDiskID, char *lpPID);

#endif