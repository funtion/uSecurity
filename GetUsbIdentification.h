#ifndef _GetUsbIdentification_H_
#define _GetUsbIdentification_H_

#define _DEBUG_    //是否输出调试信息

//获取U盘序列号,PID+VID+分区标识形式(32位).
//成功返回移动设备个数,USBSerial为用 0x00 分割的对应多个设备序列号
//失败 < 0
//-1   获取设备路径出错
//-2   获取UKEY标识符失败
int GetUsbIdentification(char *USBSerial);

#endif