#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include "define.h"

// 通信的类型
enum COMMUNICATIONS_TYPE
{
	UDP = 1,
	TCP = 2,
	MULTICAST = 3,
	BROADCAST = 4
};

class DLLS_PORT Communications
{

public:
	static Communications* CreateCommunications(COMMUNICATIONS_TYPE type);
	virtual ~Communications();

// 构造函数
protected:
	Communications();

// 以下部分为接口部分
public:
	virtual BOOL InitCom() = 0;                                             // 初始化相关函数
	virtual BOOL Send(BYTE abyDestIp[IP_BYTE_LENGTH], UINT uiDesPort,
		byte *pbyBuffer, size_t uiDataLength, size_t uiFrameNum) = 0;       // 完成发送代码
	virtual BOOL SetSender(BYTE abyLocalIp[IP_BYTE_LENGTH], 
		UINT uiLocalPort) = 0;                                              // 设置本地的IP地址
	virtual BOOL Receive(byte* &pbyBuffer, 
		size_t &uiDataLength, size_t &uiFrameNum) = 0;                        // 完成接收代码
	virtual BOOL AddToAccepter(BYTE abyServerIP[IP_BYTE_LENGTH],
		UINT uiServerPort) = 0;                                             // 设置接收端的代码

protected:
	virtual BOOL ReleaseCom() = 0;						                    // 完成释放代码

// 以下是基类需要提供的相关服务，以便让子类调用
public:
	void GetIpInfo(PST_IP_INFO pstIpInfo);    	             // 取网卡信息
	void GetIpAddr(PST_IP_INFO pstIpInfo, UINT &uiIpNum);    // 获取主机IP地址
	void IPStringtoByteArray(char szIp[IP_CHAR_LENGTH], 
		BYTE abyByteArray[IP_BYTE_LENGTH]);
	void ByteIpToStringIp(byte abyByteArray[IP_BYTE_LENGTH],
		char* pcStringIp);							    	 // 将字符串型IP地址分割成为四个字段，并将它们存入字节数组	

private:

};
#endif