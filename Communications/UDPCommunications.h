#ifndef UDP_COMMUNICATIONS_H
#define UDP_COMMUNICATIONS_H

#include "communications.h"

class UDPCommunications:public Communications
{
public:
	UDPCommunications();
	virtual ~UDPCommunications();

public:
	virtual BOOL InitCom();                 // ��ʼ����غ���
	virtual BOOL Send(BYTE abyDestIp[IP_BYTE_LENGTH], UINT uiDesPort,
		byte *pbyBuffer, UINT uiDataLength, UINT uiFrameNum);                           // ��ɷ��ʹ���
	virtual BOOL Receive();                 // ��ɽ��մ���
	virtual BOOL SetSender(BYTE abyLocalIp[IP_BYTE_LENGTH], UINT uiLocalPort);		    // ���ñ��ص�IP��ַ
	virtual BOOL AddToAcceptList(BYTE abyServerIP[IP_BYTE_LENGTH], UINT uiServerPort);  // ���ý��ն˵Ĵ���

protected:
	virtual BOOL ReleaseCom();              // ����ͷŴ���
	BOOL AddServerObj(BYTE abyServerIp[IP_BYTE_LENGTH], UINT uiServerPort);

private:
	SOCKET m_sktUDPSender;                  // ���ط��͵Ĵ���
	SOCKET m_sktReceiveSender;                  // ���ط��͵Ĵ���
};

#endif