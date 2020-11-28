#include "serialport.h"

#define RETURN_SUCCESS 1
#define RETURN_FAIL 0


class CSerialComm
{
public:
	CSerialComm();
	~CSerialComm();

	CSerialPort  serial;
	int    connect(const char* _portNum);
	int    sendCommand(BYTE pos);
	BYTE recvCommand(BYTE &resp);
	BYTE* recvCommand(BYTE* resp, UINT size);
	void   disconnect();
};