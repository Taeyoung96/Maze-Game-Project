/////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: Comm.cpp
// Version	: 1.0.1
// Date		: 2019.08.21
// Writer	: Lee, Seungmin (CDSL)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//include//
#include "Comm.h"

#include <chrono>
#include <thread>
#include <mutex>

//Windows Platform
#if defined(_WIN32) || defined(_WIN64)
#define __WINDOWS_PLATFORM_
#include <Windows.h>

//Linux Platform
#elif defined(__linux__)
#define __LINUX_PLATFORM_
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/serial.h>
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Class Name	: CComm::_CPortHandler
// Summury		: Private Class for Port Handler
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CComm::_CPortHandler {

// Define ////////////////////////////////////////////////////////
public:

protected:

private:


// Method ////////////////////////////////////////////////////////
public:

	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: _CPortHandler
	// Input	: None
	// Output	: None
	// Summury	: Standard constructor
	////////////////////////////////////////////////////////////////////////////////////////////
	_CPortHandler() :
		_timeout(0.001), _bufSize(4096), _isOpen(false){

		memset(_portName, 0, 100);
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: ~_CPortHandler
	// Input	: None
	// Output	: None
	// Summury	: Standard destructor
	////////////////////////////////////////////////////////////////////////////////////////////
	~_CPortHandler() {

	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: Open
	// Input	: port name(const char), baudrate(int)
	// Output	: Result(bool)
	// Summury	: Open port handler.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool Open(const char* port, int baudrate) {

		bool result = false;

		sprintf_s(_portName, sizeof(_portName), "\\\\.\\%s", port);
		_baudrate = baudrate;

		if (!_open(_baudrate)) {
			result = false;
			_isOpen = false;
			return result;
		}

		result = true;
		_isOpen = true;
		return result;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: Close
	// Input	: None
	// Output	: Result(bool)
	// Summury	: Close port handler.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool Close() {

		bool result = false;

		if (!_isOpen)
			return result;

		_close();

		result = true;
		_isOpen = false;
		return result;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: isOpen
	// Input	: None
	// Output	: isopen(bool)
	// Summury	: The port is open or not
	////////////////////////////////////////////////////////////////////////////////////////////
	bool isOpen() {

		return _isOpen;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: Write
	// Input	: data buffer(const char*), data length(int)
	// Output	: length of sended data(int)
	// Summury	: Write data to port.
	////////////////////////////////////////////////////////////////////////////////////////////
	int Write(const char* buf, int length) {

		DWORD dwWrite = 0;

		if (WriteFile(_handle, buf, (DWORD)length, &dwWrite, NULL) == FALSE)
			return -1;

		return (int)dwWrite;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: Read
	// Input	: data buffer(char*), data length(int)
	// Output	: length of recved data(int)
	// Summury	: Read data from recv buffer.
	////////////////////////////////////////////////////////////////////////////////////////////
	int Read(char* buf, int length) {

		DWORD dwRead = 0;

		if (ReadFile(_handle, buf, (DWORD)length, &dwRead, NULL) == FALSE)
			return -1;

		return dwRead;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: SetBufSize
	// Input	: buffer size(int)
	// Output	: None
	// Summury	: Set buffer size.
	////////////////////////////////////////////////////////////////////////////////////////////
	void SetBufSize(int size) {

		_bufSize = size;
	}



	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: SetTimeOut
	// Input	: time(double)
	// Output	: None
	// Summury	: Set Time Out.
	////////////////////////////////////////////////////////////////////////////////////////////
	void SetTimeOut(double t) {

		_timeout = t;
	}

protected:

private:
	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: _open
	// Input	: baudrate(int)
	// Output	: Result(bool)
	// Summury	: Open port handler.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool _open(int baudrate) {

		DCB dcb;
		COMMTIMEOUTS timeouts;
		DWORD dwError;

		_close();
		
		//Create Port Handle.
		_handle = CreateFileA(_portName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (_handle == INVALID_HANDLE_VALUE)
		{
			printf("[PortHandlerWindows::SetupPort] Error opening serial port!\n");
			return false;
		}

		dcb.DCBlength = sizeof(DCB);
		if (GetCommState(_handle, &dcb) == FALSE) {
			_close();
			return false;
		}

		// Set baudrate
		dcb.BaudRate = (DWORD)baudrate;
		dcb.ByteSize = 8;					// Data bit = 8bit
		dcb.Parity = NOPARITY;				// No parity
		dcb.StopBits = ONESTOPBIT;          // Stop bit = 1
		dcb.fParity = NOPARITY;             // No Parity check
		dcb.fBinary = 1;                    // Binary mode
		dcb.fNull = 0;						// Get Null byte
		dcb.fAbortOnError = 0;
		dcb.fErrorChar = 0;

		// Not using XOn/XOff
		dcb.fOutX = 0;
		dcb.fInX = 0;

		// Not using H/W flow control
		dcb.fDtrControl = DTR_CONTROL_DISABLE;
		dcb.fRtsControl = RTS_CONTROL_DISABLE;
		dcb.fDsrSensitivity = 0;
		dcb.fOutxDsrFlow = 0;
		dcb.fOutxCtsFlow = 0;

		if (SetCommState(_handle, &dcb) == FALSE) {
			_close();
			return false;
		}


		if (SetCommMask(_handle, 0) == FALSE ||
			SetupComm(_handle, 4096, 4096) == FALSE ||
			PurgeComm(_handle, PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_RXCLEAR) == FALSE||
			ClearCommError(_handle, &dwError, NULL) == FALSE ||
			GetCommTimeouts(_handle, &timeouts) == FALSE
			) {

			_close();
			return false;
		}

		timeouts.ReadIntervalTimeout = 0;
		timeouts.ReadTotalTimeoutMultiplier = 0;
		timeouts.ReadTotalTimeoutConstant = 1; // must not be zero.
		timeouts.WriteTotalTimeoutMultiplier = 0;
		timeouts.WriteTotalTimeoutConstant = 0;
		if (SetCommTimeouts(_handle, &timeouts) == FALSE) {
			_close();
			return false;
		}

		return true;
	}


	
	////////////////////////////////////////////////////////////////////////////////////////////
	// Method	: _close
	// Input	: None
	// Output	: Result(bool)
	// Summury	: Close port handler.
	////////////////////////////////////////////////////////////////////////////////////////////
	bool _close() {
		
		if (_handle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(_handle);
			_handle = INVALID_HANDLE_VALUE;

			return true;
		}

		return false;
	}


// Member ////////////////////////////////////////////////////////
public:

protected:

private:
	double _timeout;
	int _bufSize;
	bool _isOpen;

	HANDLE _handle;
	char _portName[100];
	int _baudrate;
};




CComm::CComm()
{
	_poHandler = new _CPortHandler;
}



CComm::~CComm()
{
	delete _poHandler;
}



bool CComm::Open(const char* port, int baudrate) {

	return _poHandler->Open(port, baudrate);
}



bool CComm::Close() {

	return _poHandler->Close();
}



bool CComm::isOpen() {

	return _poHandler->isOpen();
}



int CComm::Write(const char* buf, int length) {

	return _poHandler->Write(buf, length);
}



int CComm::Read(char* buf, int length) {

	return _poHandler->Read(buf, length);
}



void CComm::SetBufSize(int size) {

	return _poHandler->SetBufSize(size);
}



void CComm::SetTimeOut(double t) {

	return _poHandler->SetTimeOut(t);
}
