#ifndef TCP_COMMUNICATION_H
#define TCP_COMMUNICATION_H

#include "communications.h"

class TCPCommunications :public Communications
{
public:
	TCPCommunications();
	virtual ~TCPCommunications();

// 接口函数
public:
	virtual BOOL InitCom();											// 初始化相关函数
	virtual BOOL Send(BYTE abyDestIp[IP_BYTE_LENGTH], UINT uiDesPort,
		byte *pbyBuffer, size_t uiDataLength, size_t uiFrameNum);	// 完成发送代码
	virtual BOOL Receive(byte* &pbyBuffer,
		size_t& uiDataLength, size_t& uiFrameNum);					// 完成接收代码
	virtual BOOL SetSender(BYTE abyLocalIp[IP_BYTE_LENGTH],
		UINT uiLocalPort);											// 设置本地的IP地址
	virtual BOOL AddToAccepter(BYTE abyServerIP[IP_BYTE_LENGTH],
		UINT uiServerPort);											// 设置接收端的代码

protected:
	virtual BOOL ReleaseCom();										// 完成释放代码

// 该类自己的特性
private:
	BOOL AddServerObj(BYTE abyServerIp[IP_BYTE_LENGTH], UINT uiServerPort);

private:
	SOCKET m_sktTCPSender;			// 本地发送的socket
	SOCKET m_sktTCPReceiver;		// 本地接受的socket

};


#endif