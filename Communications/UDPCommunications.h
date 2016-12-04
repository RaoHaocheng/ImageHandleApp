#ifndef UDP_COMMUNICATIONS_H
#define UDP_COMMUNICATIONS_H

#include "communications.h"

class UDPCommunications:public Communications
{
public:
	UDPCommunications();
	virtual ~UDPCommunications();

public:
	virtual BOOL InitCom();                 // 初始化相关函数
	virtual BOOL Send(BYTE abyDestIp[IP_BYTE_LENGTH], UINT uiDesPort,
		byte *pbyBuffer, UINT uiDataLength, UINT uiFrameNum);                           // 完成发送代码
	virtual BOOL Receive();                 // 完成接收代码
	virtual BOOL SetSender(BYTE abyLocalIp[IP_BYTE_LENGTH], UINT uiLocalPort);		    // 设置本地的IP地址
	virtual BOOL AddToAcceptList(BYTE abyServerIP[IP_BYTE_LENGTH], UINT uiServerPort);  // 设置接收端的代码

protected:
	virtual BOOL ReleaseCom();              // 完成释放代码
	BOOL AddServerObj(BYTE abyServerIp[IP_BYTE_LENGTH], UINT uiServerPort);

private:
	SOCKET m_sktUDPSender;                  // 本地发送的代码
	SOCKET m_sktReceiveSender;                  // 本地发送的代码
};

#endif