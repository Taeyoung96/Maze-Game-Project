#include "serialcomm.h"


CSerialComm::CSerialComm() {}
CSerialComm::~CSerialComm() {}


int CSerialComm::connect(const char* _portNum)
{
	if (!serial.OpenPort("COM3"))
		return RETURN_FAIL;

	serial.ConfigurePort(CBR_115200, 8, FALSE, NOPARITY, ONESTOPBIT);
	serial.SetCommunicationTimeouts(0, 0, 0, 0, 0);

	return RETURN_SUCCESS;
}


int CSerialComm::sendCommand(unsigned char pos)
{
	if (serial.WriteByte(pos))
		return RETURN_SUCCESS;
	else
		return RETURN_FAIL;
}

BYTE CSerialComm::recvCommand(BYTE &resp) {
	//����
	if (serial.ReadByte(resp))
		return resp;
	else
		return -1;
}

BYTE* CSerialComm::recvCommand(BYTE* resp, UINT size) {
	//����
	if (serial.ReadByte(resp, size))
		return resp;
	else
		return nullptr;
}

void CSerialComm::disconnect()
{
	serial.ClosePort();
}