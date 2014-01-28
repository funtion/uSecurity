#include "UsbIo.h"
#include <WINIOCTL.H>
#include <STDIO.H>
#include <IOSTREAM>
using namespace std;



UsbIo::UsbIo()
{
	deviceName = new char[128];
	deviceHandle = NULL;
}



UsbIo::~UsbIo()
{
	delete[] deviceName;
	if(deviceHandle)
        ::CloseHandle(deviceHandle);
}

void UsbIo::getRemovableDevice()
{
	DWORD drive = ::GetLogicalDrives();
	
	
	char result = 'A';
	
	int i= 0;
	int j=0;
	while((1<<i)<drive)
	{
		if (drive& ((DWORD)(1<<i)) )
		{
			char drivestring[1024];
			sprintf(drivestring,"%c:\\",char(result+i));
			if (GetDriveType(drivestring)==DRIVE_REMOVABLE)
			{
				deviceName[j++] = char(result+i);
			}
		}
		i++;
  }
	deviceName[j] = '\0';
}

DWORD UsbIo::readFileForUsb(DWORD dwMoveMethod,LONG DistanceToMoveLow,PLONG DistanceToMoveHigh,BYTE* pData,DWORD nNumberOfBytesToRead)
{
   DWORD byteRet = 0; 
   if(!::DeviceIoControl(deviceHandle,FSCTL_LOCK_VOLUME,NULL,0,NULL,0,&byteRet,NULL))
   {
	   cout <<" send the dwIoControlCode FSCTL_LOCK_VOLUME failed!";
	   return ReadFailed;
   }
   
   byteRet = 0;
  BYTE* pTempData;
  DWORD number = (nNumberOfBytesToRead + DistanceToMoveLow % perSectorSize) / perSectorSize + 1;  
  try
  {
	  	  	  
	     ::SetFilePointer(deviceHandle,(DistanceToMoveLow/perSectorSize)*perSectorSize,DistanceToMoveHigh,dwMoveMethod);
	  
	  try
	  {
         pTempData = new BYTE[number *perSectorSize];
	  }
	  catch(...)
	  {
         cout <<"alloc memory failed!"<<endl;
	  }
	  ::ReadFile(deviceHandle,pTempData,number * perSectorSize,&byteRet,NULL);
	  DWORD err = GetLastError();
  }
  catch(...)
  {   
	  cout <<"the lastError is: "<<GetLastError()<<endl;
	  return ReadFailed;
  }

  try{
	 int i=0;
     for (; i<nNumberOfBytesToRead; i++)
     {
		 pData[i] = pTempData[i + DistanceToMoveLow%perSectorSize];
		 byteRet = i;
     }
	 pData[i] = '\0';
  }
  catch(...)
  {
      cout <<"copy pTempData to pData failed when call the readFileForUsb(HANDLE hFile,DWORD dwMoveMethod,LONG DistanceToMoveLow,PLONG DistanceToMoveHigh,BYTE* pData,DWORD nNumberOfBytesToRead,bool isResetFilePoint) "<<endl;
	  return ReadFailed;
  }
  DWORD temp = 0;
  if (!::DeviceIoControl(deviceHandle,FSCTL_UNLOCK_VOLUME,NULL,0,NULL,0,&temp,NULL))
  {
	  cout <<" send the dwIoControlCode FSCTL_UNLOCK_VOLUME failed!";
	  return ReadFailed;
  }
  return byteRet+1;
}

DWORD UsbIo::readFileForUsb(DWORD sectorNumber, DWORD offest, BYTE* pData, DWORD nNumberOfBytesToRead)
{
    DWORD byteRet = 0; 

	if(!::DeviceIoControl(deviceHandle,FSCTL_LOCK_VOLUME,NULL,0,NULL,0,&byteRet,NULL))
	{
		cout <<" send the dwIoControlCode FSCTL_LOCK_VOLUME failed!";
		return ReadFailed;
	}
	
	byteRet = 0;
	DWORD number = (nNumberOfBytesToRead+offest) / perSectorSize + 1;
	BYTE* pTempData= NULL;
	try
	{
	
		   ::SetFilePointer(deviceHandle,sectorNumber * perSectorSize ,NULL,FILE_BEGIN);

				
		try
		{
			pTempData = new BYTE[perSectorSize*number];
		}
		catch (...)
		{
           cout <<"alloc memory failed!";
		   return ReadFailed;
		}
		
		::ReadFile(deviceHandle,pTempData,number * perSectorSize ,&byteRet,NULL);


	}
	catch(...)
	{   
		cout <<"the LastError is: "<<GetLastError()<<endl;
		return ReadFailed;
	}
   
	try
	{
		int i=0;
	    for (; i< nNumberOfBytesToRead; i++)
		{
	     	pData[i] = pTempData[i+offest];
		    byteRet = i;
		}
		pData[i] = '\0';
	}
	catch(...)
	{
		cout <<"copy pTempData to pData failed!"<<endl;
		return ReadFailed;
	}

   if (pTempData)
   {
     delete[] pTempData;
   }

    DWORD temp = 0;
	if (!::DeviceIoControl(deviceHandle,FSCTL_UNLOCK_VOLUME,NULL,0,NULL,0,&temp,NULL))
	{
		cout <<" send the dwIoControlCode FSCTL_UNLOCK_VOLUME failed!";
		return ReadFailed;
	}


  return byteRet+1;


}

DWORD UsbIo::writeFileForUsb( DWORD dwMoveMethod, LONG DistanceToMoveLow,  PLONG DistanceToMoveHigh, BYTE* pData, DWORD nNumberOfBytesToWrite)
{
	DWORD err;
	DWORD byteRet = 0; 
	BYTE* pTempData;
	DWORD number = (DistanceToMoveLow % perSectorSize + nNumberOfBytesToWrite) / perSectorSize + 1;
	try
	{
		pTempData = new BYTE[number*perSectorSize];
	}
	catch(...)
	{
		cout <<"alloc memory failed when write!"<<endl;
		return WriteFailed;
	}
	try
	{
		::SetFilePointer(deviceHandle,(DistanceToMoveLow/perSectorSize)*perSectorSize,NULL,FILE_BEGIN);
		DWORD byteRet=0;
		::ReadFile(deviceHandle,pTempData,number*perSectorSize,&byteRet,NULL);
	}
	catch(...)
	{
        cout << "call readFileForUsb() failed when writing  process!"<<endl;
		return WriteFailed;
	}

	try
	{
		int i=0;
		for(; i<nNumberOfBytesToWrite; i++)
			pTempData[DistanceToMoveLow%perSectorSize + i] = pData[i];
		byteRet = i;
	}
	catch(...)
	{
		cout <<"copy the pData to the pTempData failed when writing process!"<<endl;
	}

	
	try
	{
		::SetFilePointer(deviceHandle,(DistanceToMoveLow/perSectorSize) * perSectorSize,DistanceToMoveHigh,dwMoveMethod);
		err = GetLastError();
		DWORD temp;
		::WriteFile(deviceHandle,pTempData,number*perSectorSize,&temp,NULL);
		err = GetLastError();
	}
	catch(...)
	{
		cout <<"the lastError is: "<<GetLastError()<<endl;
		return ReadFailed;
	}
    DWORD byteRet1;
	::DeviceIoControl(deviceHandle,FSCTL_UNLOCK_VOLUME,NULL,0,NULL,0,&byteRet1,NULL);
	return byteRet;
}

DWORD UsbIo::writeFileForUsb(DWORD sectorNumber, DWORD offest, BYTE* pData, DWORD nNumberOfBytesToWrite)
{
    DWORD byteRet = 0 ; 
    BYTE* pTempData;
	DWORD number = (nNumberOfBytesToWrite + offest) / perSectorSize + 1;
	try
	{
		pTempData = new BYTE[number*perSectorSize];
	}
	catch(...)
	{
		cout <<"alloc memory failed when write!"<<endl;
		return WriteFailed;
	}

	try
	{   
		::SetFilePointer(deviceHandle,sectorNumber*perSectorSize,NULL,FILE_BEGIN);
		DWORD byteRet=0;
		::ReadFile(deviceHandle,pTempData,number*perSectorSize,&byteRet,NULL);
	}
	catch(...)
	{
        cout << "call readFileForUsb() failed when writing  process!"<<endl;
		return WriteFailed;
	}
    
	try
	{
		int i=0;
          for(; i<nNumberOfBytesToWrite; i++)
                 pTempData[offest + i] = pData[i];
		  byteRet = i;
	}
	catch(...)
	{
		cout <<"copy the pData to the pTempData failed when writing process!"<<endl;
	}
    
    try
	{
		DWORD temp = 0;
		::SetFilePointer(deviceHandle,sectorNumber*perSectorSize,NULL,FILE_BEGIN);
		::WriteFile(deviceHandle,pTempData,number*perSectorSize,&temp,NULL);
		DWORD err = GetLastError();
	}
	catch(...)
	{
		cout <<"call the WriteFile() failed when writing process!"<<endl;
		return WriteFailed;
	}

	DWORD temp = 0;
	::DeviceIoControl(deviceHandle,FSCTL_UNLOCK_VOLUME,NULL,0,NULL,0,&temp,NULL);

	return byteRet;
}



HANDLE UsbIo::OpenDevice(LPCTSTR filename)
{
   
   deviceHandle = CreateFile(filename,
	                    GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,
						NULL,
						NULL);


   if( deviceHandle == INVALID_HANDLE_VALUE )
	   cout <<"failed to open the device %s!"<<filename<<endl;
   return deviceHandle;
}
HANDLE UsbIo::getDeviceHandle()
{
	return deviceHandle;
}