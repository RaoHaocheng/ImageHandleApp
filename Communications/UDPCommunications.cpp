#include "UDPCommunications.h"

#include <atlconv.h>


UDPCommunications::UDPCommunications()
{
}

UDPCommunications::~UDPCommunications()
{
}

BOOL UDPCommunications::InitCom()                    // ��ʼ����غ���
{
	int iInitialError;								// ��������ֵ
	WSADATA data;       							// ��ʼ���׽���
	iInitialError = WSAStartup(MAKEWORD(2, 2), &data);	// 2.2�汾�׽���

	//  initiates use of WS2_32.DLL
	if (0 != iInitialError)
		return false;

	// �жϼ��ص��׽��ְ汾�Ƿ���ȷ
	if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2)
	{
		// release use of WS2_32.DLL
		WSACleanup();
		return false;
	}

	return false;
}

BOOL UDPCommunications::Send(BYTE abyDestIp[IP_BYTE_LENGTH], UINT uiDesPort,
	byte *pbyBuffer, UINT uiDataLength, UINT uiFrameNum)                    // ��ɷ��ʹ���
{
	USES_CONVERSION;

	char pcDesIp[IP_CHAR_LENGTH];
	ByteIpToStringIp(abyDestIp, pcDesIp);

	sockaddr_in sdServeraddr;                    //Ŀ�ķ������˵�ַ��
	sdServeraddr.sin_family = AF_INET;
	sdServeraddr.sin_addr.S_un.S_addr = inet_addr(pcDesIp);
	sdServeraddr.sin_port = htons(uiDesPort);    //�˿ں�,�����ֽ���ת��Ϊ�����ֽ��򣬵ȴ�����

	//���ٷ��ͻ�������СΪMAXLENGTH�ֽ�
	PST_PACKAGE pstSendPacket = new ST_PACKAGE;

	//��ʼ�����ͻ���
	ZeroMemory(pstSendPacket->abyData, DATA_BUF_LENGTH);	// ���ݻ���
	pstSendPacket->uiDataLength = -1;						// ���ݰ������ݳ���
	pstSendPacket->uiFrameNum = uiFrameNum;					// ���ݰ�֡��
	pstSendPacket->uiPackageNum = -1;						// ���ݰ�����
	pstSendPacket->uiTotalDataLength = uiDataLength;		// ���������ݵ��ܳ��ȣ�����UDP��������ƣ��ڷ��͹����н��ᷢ���а���
	memset(pstSendPacket->abySourceIp, 0, IP_BYTE_LENGTH);
	pstSendPacket->uiSourcePort = 0;

	//�������δ�����꣬��ѭ����������
	while (uiDataLength != 0)
	{
		UINT uipacknum = 0;                       //���ڼ������ݰ��ĸ���������䷢�͵����ݰ�����

		//���ݰ��ܳ��ȴ���һ�����ݰ������ݵ�����
		while (uiDataLength > DATA_BUF_LENGTH)
		{

			// ������ݰ�����
			pstSendPacket->uiPackageNum = uipacknum;
			// ��ǰ���ݰ������ݳ���
			pstSendPacket->uiDataLength = DATA_BUF_LENGTH;
			// UDP������
			memcpy(pstSendPacket->abyData, pbyBuffer + DATA_BUF_LENGTH * uipacknum, DATA_BUF_LENGTH); //ѭ�����ƻ��������ݵ����ݰ��ṹ��

			// ÿ�η���DATA_BUF���ֽ�
			int reti = sendto(m_sktUDPSender, (const char*)pstSendPacket, sizeof(ST_PACKAGE),
				0, (SOCKADDR*)&sdServeraddr, sizeof(SOCKADDR)); //�������ݰ�

			// �ж��Ƿ��ͳɹ�
			if (SOCKET_ERROR == reti) // CCsccLog::DebugPrint("���ݷ��ͳ���\n");
				return FALSE;

			// һ֡����ʣ���������
			uiDataLength -= DATA_BUF_LENGTH;
			// ���ݰ����ż�һ
			uipacknum++;
		}

		if (uiDataLength > 0)//ʣ���ֽڷ���
		{
			//���һ�����ݰ����DATA_BUFС������Ҫ���ڴ�ȫ����ʽ��Ϊ0
			ZeroMemory(pstSendPacket->abyData, DATA_BUF_LENGTH); //��ջ���,ʹδ����������Ҳ��ʼ��Ϊ0
			//������ݰ�����
			pstSendPacket->uiPackageNum = uipacknum;

			// ��ǰ���ݰ������ݳ���
			pstSendPacket->uiDataLength = uiDataLength;

			//�����������������ݰ�
			memcpy(pstSendPacket->abyData, pbyBuffer + uipacknum * DATA_BUF_LENGTH, uiDataLength); //��ʣ�����ݿ��������ݰ���׼������

			//�������һ�����ݰ������ݰ��е����ݳ���Ϊʣ�������ֽڣ��Ҳ�����DATA_BUF
			int reti = sendto(m_sktUDPSender, (const char*)pstSendPacket, sizeof(ST_PACKAGE),
				0, (sockaddr*)&sdServeraddr, sizeof(sockaddr));

			//�ж��Ƿ��ͳɹ�
			if (SOCKET_ERROR == reti)
				return FALSE;

			uiDataLength = 0; //��ʾ�����Ѿ��������
			uipacknum++;     //���ݰ����ż�һ
		}
	}

	delete pstSendPacket;   //ɾ�����ݻ���ָ��
	pstSendPacket = NULL;   //�ÿ����ݻ���ָ��

	return TRUE;
}

BOOL UDPCommunications::SetSender(BYTE abyLocalIp[IP_BYTE_LENGTH], UINT uiLocalPort)
{

	USES_CONVERSION;

	// ����UDP�����׽���
	m_sktUDPSender = socket(AF_INET,      // Э���ַ���壬windows��һ��ΪAF_INET
		SOCK_DGRAM,   // Э���׽������ͣ��˴�UDP�������ݱ���
		IPPROTO_UDP); // Э�飬�˴�����UDPЭ��

	// �жϷ����׽����Ƿ񴴽��ɹ�
	if (INVALID_SOCKET == m_sktUDPSender)
	{
		// �õ��������
		int iSocketErr = WSAGetLastError();

		// �ж��Ƿ����׽��ְ汾����ȷ
		if (iSocketErr == WSANOTINITIALISED)// �����׽���ʧ�ܣ����ز����õ��׽���
			return WSA_CREATE_SOCKET_ERROR;
	}

	int iErrCode;						// ����������
	UINT uiBuflen;						// �����õ��׽��ֻ�������С  
	UINT uiOptlen = sizeof(uiBuflen);	// ��������С

	// ��ȡ���ͻ���������
	iErrCode = getsockopt(m_sktUDPSender,			// �׽���
		SOL_SOCKET,			// �׽��ּ��𣬴˴�ѡ���׽������ڵ�Ӧ�ò�
		SO_SNDBUF,			// ��Ҫ���õ��׽���ѡ�����ƣ��˴�Ϊ��������С
		(char *)&uiBuflen,	// ���õ��׽���ѡ��ֵ
		(int*)&uiOptlen);		// ��������С

	// �жϻ�ȡ�����������Ƿ��쳣
	if (SOCKET_ERROR == iErrCode)
		return WSA_GET_SOCKET_OPT_ERROR;

	// ��ԭ�ȵĻ�������С����ʮ����ϵͳĬ��Ϊ8192byte
	uiBuflen *= 10;

	//�趨��������С                                                           
	iErrCode = setsockopt(m_sktUDPSender, SOL_SOCKET, SO_RCVBUF, (char*)&uiBuflen, uiOptlen);

	//�ж����û�������С�Ƿ��쳣
	if (SOCKET_ERROR == iErrCode)
		return WSA_SET_SOCKET_OPT_ERROR;

	//��黺������С�Ƿ��޸ĳɹ�
	unsigned int uiNewRcvBuf;
	iErrCode = getsockopt(m_sktUDPSender, SOL_SOCKET, SO_SNDBUF, (char*)&uiNewRcvBuf, (int*)&uiOptlen);
	if (SOCKET_ERROR == iErrCode || uiNewRcvBuf == uiBuflen)
		//CCsccLog::DebugPrint("�޸�ϵͳ�������ݻ�����ʧ�ܣ�\n");
		return WSA_GET_SOCKET_OPT_ERROR;

	//���÷��Ͷ˶˿�����
	BOOL bIsReusePort;
	int iReusePortLen = sizeof(bIsReusePort);
	iErrCode = setsockopt(m_sktUDPSender, SOL_SOCKET, SO_REUSEADDR, (char*)&bIsReusePort, iReusePortLen);
	if (SOCKET_ERROR == iErrCode)//CCsccLog::DebugPrint("���ö˿�����ʧ�ܣ�\n");
		return WSA_SET_SOCKET_OPT_ERROR;

	// ����IP
	char pcLocalIp[IP_CHAR_LENGTH];
	ByteIpToStringIp(abyLocalIp, pcLocalIp);

	//�󶨱���IP��ַ
	sockaddr_in sdClientAddr;

	//���ͱ���IP
	sdClientAddr.sin_addr.S_un.S_addr = inet_addr(pcLocalIp);	//TODO:�˴��Ƿ���Ҫ��DLL�ڲ��Զ���ȡ������IP��ַ
	sdClientAddr.sin_family = AF_INET;							//���ص�ַ��
	sdClientAddr.sin_port = htons(uiLocalPort);					//�˿ں�,�����ֽ���ת��Ϊ�����ֽ��򣬵ȴ�����

	//���׽���d
	int iBindErr = bind(m_sktUDPSender, (LPSOCKADDR)&sdClientAddr, sizeof(sdClientAddr));
	if (iBindErr == SOCKET_ERROR)
		return WSA_BIND_ERROR;

	return TRUE;
}

BOOL UDPCommunications::Receive()                 // ��ɽ��մ�
{

}


BOOL UDPCommunications::ReleaseCom()        // ����ͷŴ���
{
	closesocket(m_sktUDPSender);            // �ر��׽���

	int iUnInitialErr = 0;					// ��������ֵ
	// �ͷ��׽��ֿ�
	iUnInitialErr = WSACleanup();

	// �ж��Ƿ�ɹ��ͷ�
	if (0 != iUnInitialErr)
	{
		// �жϴ�������Ƿ���WSANOTINITIALISED
		if (WSANOTINITIALISED == WSAGetLastError())
			return false;
	}
	else
		return false;
}


BOOL UDPCommunications::AddToAcceptList(BYTE abyServerIP[IP_BYTE_LENGTH], UINT uiServerPort)
{
	if (AddServerObj(abyServerIP, uiServerPort))
		return TRUE;
	else
		return FALSE;
}


BOOL UDPCommunications::AddServerObj(BYTE abyServerIp[IP_BYTE_LENGTH], UINT uiServerPort)
{
	USES_CONVERSION;

//	CString strServerIp = _T("");
//	strServerIp.Format(_T("%d.%d.%d.%d"), abyServerIp[0], abyServerIp[1], abyServerIp[2], abyServerIp[3]);
//	char* pcServerIp = "";
//	pcServerIp = W2A(strServerIp);
	char pcServerIp[IP_CHAR_LENGTH];
	ByteIpToStringIp(abyServerIp, pcServerIp);

	//����UDPͨ���׽���
	m_sktReceiveSender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_sktReceiveSender)   // CCsccLog::DebugPrint("������������UDPͨ���׽��ִ���\n");
		return FALSE;

	//����������
	int iErrCode;

	//�����õ��׽��ֻ�������С
	UINT uiBuflen;
	//��������С
	UINT uiOptlen = sizeof(uiBuflen);

	//��ȡ���ͻ���������
	iErrCode = getsockopt(m_sktReceiveSender,		//�׽���
		SOL_SOCKET,			//�׽��ּ��𣬴˴�ѡ���׽������ڵ�Ӧ�ò�
		SO_SNDBUF,			//��Ҫ���õ��׽���ѡ�����ƣ��˴�Ϊ��������С
		(char *)&uiBuflen,	//���õ��׽���ѡ��ֵ
		(int*)&uiOptlen);	//��������С

	//�жϻ�ȡ�����������Ƿ��쳣
	if (SOCKET_ERROR == iErrCode)
		return INVALID_SOCKET;

	//��ԭ�ȵĻ�������С����ʮ����ϵͳĬ��Ϊ8192byte(8k)
	uiBuflen *= 10;

	//�趨��������С                                                           
	iErrCode = setsockopt(m_sktReceiveSender, SOL_SOCKET, SO_RCVBUF, (char *)&uiBuflen, uiOptlen);

	//�ж����û�������С�Ƿ��쳣
	if (SOCKET_ERROR == iErrCode)
		return INVALID_SOCKET;

	// ��黺������С�Ƿ��޸ĳɹ�
	unsigned int uiNewRcvBuf;
	iErrCode = getsockopt(m_sktReceiveSender, SOL_SOCKET, SO_SNDBUF, (char*)&uiNewRcvBuf, (int*)&uiOptlen);
	if (SOCKET_ERROR == iErrCode || uiNewRcvBuf == uiBuflen) // CCsccLog::DebugPrint("�޸�ϵͳ�������ݻ�����ʧ�ܣ�\n");
		return INVALID_SOCKET;

	// �󶨷�����IP��ַ
	sockaddr_in sdServerAddr;									// �������˵�ַ��
	sdServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	    // ���ͱ���IP
	sdServerAddr.sin_family = AF_INET;
	sdServerAddr.sin_port = htons(uiServerPort);				// �˿ں�,�����ֽ�����ת��Ϊ�����ֽ���

	//���׽���
	int iBindErr = bind(m_sktReceiveSender, (LPSOCKADDR)&sdServerAddr, sizeof(sdServerAddr));
	if (iBindErr == SOCKET_ERROR)   // CCsccLog::DebugPrint("�󶨽����׽��ֳ���\n");
		return FALSE;

	//SetServerObj(m_sktReceiveSender, abyServerIp, uiServerPort);			// ��UDPͨ���׽��ּ��뵽�������˶�����

	return TRUE;

}
