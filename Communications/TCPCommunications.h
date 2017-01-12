#ifndef TCP_COMMUNICATION_H
#define TCP_COMMUNICATION_H

#include "communications.h"

class TCPCommunications :public Communications
{
public:
	TCPCommunications();
	virtual ~TCPCommunications();

// �ӿں���
public:
	virtual BOOL InitCom();											// ��ʼ����غ���
	virtual BOOL Send(BYTE abyDestIp[IP_BYTE_LENGTH], UINT uiDesPort,
		byte *pbyBuffer, size_t uiDataLength, size_t uiFrameNum);	// ��ɷ��ʹ���
	virtual BOOL Receive(byte* &pbyBuffer,
		size_t& uiDataLength, size_t& uiFrameNum);					// ��ɽ��մ���
	virtual BOOL SetSender(BYTE abyLocalIp[IP_BYTE_LENGTH],
		UINT uiLocalPort);											// ���ñ��ص�IP��ַ
	virtual BOOL AddToAccepter(BYTE abyServerIP[IP_BYTE_LENGTH],
		UINT uiServerPort);											// ���ý��ն˵Ĵ���

protected:
	virtual BOOL ReleaseCom();										// ����ͷŴ���

// �����Լ�������
private:
	BOOL AddServerObj(BYTE abyServerIp[IP_BYTE_LENGTH], UINT uiServerPort);

private:
	SOCKET m_sktTCPSender;			// ���ط��͵�socket
	SOCKET m_sktTCPReceiver;		// ���ؽ��ܵ�socket

};


#endif