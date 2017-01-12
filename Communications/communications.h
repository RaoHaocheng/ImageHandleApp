#ifndef COMMUNICATIONS_H
#define COMMUNICATIONS_H

#include "define.h"

// ͨ�ŵ�����
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

// ���캯��
protected:
	Communications();

// ���²���Ϊ�ӿڲ���
public:
	virtual BOOL InitCom() = 0;                                             // ��ʼ����غ���
	virtual BOOL Send(BYTE abyDestIp[IP_BYTE_LENGTH], UINT uiDesPort,
		byte *pbyBuffer, size_t uiDataLength, size_t uiFrameNum) = 0;       // ��ɷ��ʹ���
	virtual BOOL SetSender(BYTE abyLocalIp[IP_BYTE_LENGTH], 
		UINT uiLocalPort) = 0;                                              // ���ñ��ص�IP��ַ
	virtual BOOL Receive(byte* &pbyBuffer, 
		size_t &uiDataLength, size_t &uiFrameNum) = 0;                        // ��ɽ��մ���
	virtual BOOL AddToAccepter(BYTE abyServerIP[IP_BYTE_LENGTH],
		UINT uiServerPort) = 0;                                             // ���ý��ն˵Ĵ���

protected:
	virtual BOOL ReleaseCom() = 0;						                    // ����ͷŴ���

// �����ǻ�����Ҫ�ṩ����ط����Ա����������
public:
	void GetIpInfo(PST_IP_INFO pstIpInfo);    	             // ȡ������Ϣ
	void GetIpAddr(PST_IP_INFO pstIpInfo, UINT &uiIpNum);    // ��ȡ����IP��ַ
	void IPStringtoByteArray(char szIp[IP_CHAR_LENGTH], 
		BYTE abyByteArray[IP_BYTE_LENGTH]);
	void ByteIpToStringIp(byte abyByteArray[IP_BYTE_LENGTH],
		char* pcStringIp);							    	 // ���ַ�����IP��ַ�ָ��Ϊ�ĸ��ֶΣ��������Ǵ����ֽ�����	

private:

};
#endif