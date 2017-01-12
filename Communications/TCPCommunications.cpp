#include "TCPCommunications.h"
#include <atlconv.h>

static const int MAX_LISTEN_NUM = 5;

TCPCommunications::TCPCommunications()
{

}

TCPCommunications::~TCPCommunications()
{
	ReleaseCom();
}

BOOL TCPCommunications::InitCom()
{
	int iInitialError;								// ��������ֵ
	WSADATA data;       							// ��ʼ���׽���
	iInitialError = WSAStartup(MAKEWORD(1, 1), &data);	// 2.2�汾�׽���

	//  initiates use of WS2_32.DLL
	if (0 != iInitialError)
		return FALSE;

	// �жϼ��ص��׽��ְ汾�Ƿ���ȷ
	if (LOBYTE(data.wVersion) != 1 || HIBYTE(data.wVersion) != 1)
	{
		// release use of WS2_32.DLL
		WSACleanup();
		return FALSE;
	}

	return TRUE;
}

BOOL TCPCommunications::Send(BYTE abyDestIp[IP_BYTE_LENGTH],
	UINT uiDesPort, byte *pbyBuffer, size_t uiDataLength, size_t uiFrameNum)
{

	USES_CONVERSION;

	char pcDesIp[IP_CHAR_LENGTH];
	ByteIpToStringIp(abyDestIp, pcDesIp);

	sockaddr_in sdServeraddr;                    //Ŀ�ķ������˵�ַ��
	sdServeraddr.sin_family = AF_INET;
	sdServeraddr.sin_addr.S_un.S_addr = inet_addr(pcDesIp);
	sdServeraddr.sin_port = htons(uiDesPort);    //�˿ں�,�����ֽ���ת��Ϊ�����ֽ��򣬵ȴ�����
	
	if (m_sktTCPSender != INVALID_SOCKET)
		closesocket(m_sktTCPSender);

	// ����UDP�����׽���
	m_sktTCPSender = socket(AF_INET,      // Э���ַ���壬windows��һ��ΪAF_INET
		SOCK_STREAM,						  // Э���׽������ͣ��˴�UDP�������ݱ���
		IPPROTO_TCP);								  // Э�飬�˴�����TCPЭ��

	// �жϷ����׽����Ƿ񴴽��ɹ�
	if (INVALID_SOCKET == m_sktTCPSender)
	{
		// �õ��������
		int iSocketErr = WSAGetLastError();

		// �ж��Ƿ����׽��ְ汾����ȷ
		if (iSocketErr == WSANOTINITIALISED)// �����׽���ʧ�ܣ����ز����õ��׽���
			return WSA_CREATE_SOCKET_ERROR;
	}

	// ��������
	int iErrMsg = connect(m_sktTCPSender, (sockaddr*)&sdServeraddr, sizeof(sdServeraddr));
	if (iErrMsg < 0)
		return FALSE;

	//���ٷ��ͻ�������СΪMAXLENGTH�ֽ�
	PST_PACKAGE pstSendPacket = new ST_PACKAGE;

	//��ʼ�����ͻ���
	ZeroMemory(pstSendPacket->abyData, DATA_BUF_LENGTH);	// ���ݻ���
	pstSendPacket->uiDataLength = -1;						// ���ݰ������ݳ���
	pstSendPacket->uiFrameNum = uiFrameNum;			        // ���ݰ�֡��
	pstSendPacket->uiPackageNum = -1;						// ���ݰ�����
	pstSendPacket->uiTotalDataLength = uiDataLength;		// ���������ݵ��ܳ��ȣ�����UDP��������ƣ��ڷ��͹����н��ᷢ���а���
	memset(pstSendPacket->abySourceIp, 0, IP_BYTE_LENGTH);
	pstSendPacket->uiSourcePort = 0;

	//�������δ�����꣬��ѭ����������
	while (uiDataLength != 0)
	{
		size_t uipacknum = 0;                       //���ڼ������ݰ��ĸ���������䷢�͵����ݰ�����

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
			int reti = send(m_sktTCPSender, (const char*)pstSendPacket, sizeof(ST_PACKAGE), 0); //�������ݰ�

			// �ж��Ƿ��ͳɹ�
			if (SOCKET_ERROR == reti) // CCsccLog::DebugPrint("���ݷ��ͳ���\n");
			{
				shutdown(m_sktTCPSender, SD_SEND);
				return FALSE;
			}

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
			int reti = send(m_sktTCPSender, (const char*)pstSendPacket, sizeof(ST_PACKAGE), 0);

			//�ж��Ƿ��ͳɹ�
			if (SOCKET_ERROR == reti)
			{
				shutdown(m_sktTCPSender, SD_SEND);
				return FALSE;
			}

			uiDataLength = 0; //��ʾ�����Ѿ��������
			uipacknum++;     //���ݰ����ż�һ
		}
	}

	shutdown(m_sktTCPSender, SD_SEND);
	closesocket(m_sktTCPSender);
	delete pstSendPacket;   //ɾ�����ݻ���ָ��
	pstSendPacket = NULL;   //�ÿ����ݻ���ָ��
	return TRUE;
}

BOOL TCPCommunications::SetSender(BYTE abyLocalIp[IP_BYTE_LENGTH], UINT uiLocalPort)
{
	USES_CONVERSION;

	if (m_sktTCPSender != INVALID_SOCKET)
		closesocket(m_sktTCPSender);

	// ����UDP�����׽���
	m_sktTCPSender = socket(AF_INET,      // Э���ַ���壬windows��һ��ΪAF_INET
		SOCK_STREAM,						  // Э���׽������ͣ��˴�UDP�������ݱ���
		IPPROTO_TCP);								  // Э�飬�˴�����TCPЭ��

	// �жϷ����׽����Ƿ񴴽��ɹ�
	if (INVALID_SOCKET == m_sktTCPSender)
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
	iErrCode = getsockopt(m_sktTCPSender,			// �׽���
		SOL_SOCKET,			// �׽��ּ��𣬴˴�ѡ���׽������ڵ�Ӧ�ò�
		SO_SNDBUF,			// ��Ҫ���õ��׽���ѡ�����ƣ��˴�Ϊ��������С
		(char *)&uiBuflen,	// ���õ��׽���ѡ��ֵ
		(int*)&uiOptlen);		// ��������С

	// �жϻ�ȡ�����������Ƿ��쳣
	if (SOCKET_ERROR == iErrCode)
		return WSA_GET_SOCKET_OPT_ERROR;

	// ��ԭ�ȵĻ�������С����ʮ����ϵͳĬ��Ϊ8192byte
	uiBuflen *= BUFFER_SIZE_TIMES;

	//�趨��������С                                                           
	iErrCode = setsockopt(m_sktTCPSender, SOL_SOCKET, SO_RCVBUF, (char*)&uiBuflen, uiOptlen);

	//�ж����û�������С�Ƿ��쳣
	if (SOCKET_ERROR == iErrCode)
		return WSA_SET_SOCKET_OPT_ERROR;

	//��黺������С�Ƿ��޸ĳɹ�
	unsigned int uiNewRcvBuf;
	iErrCode = getsockopt(m_sktTCPSender, SOL_SOCKET, SO_SNDBUF, (char*)&uiNewRcvBuf, (int*)&uiOptlen);
	if (SOCKET_ERROR == iErrCode || uiNewRcvBuf == uiBuflen)
		//CCsccLog::DebugPrint("�޸�ϵͳ�������ݻ�����ʧ�ܣ�\n");
		return WSA_GET_SOCKET_OPT_ERROR;

	//���÷��Ͷ˶˿�����
	BOOL bIsReusePort;
	int iReusePortLen = sizeof(bIsReusePort);
	iErrCode = setsockopt(m_sktTCPSender, SOL_SOCKET, SO_REUSEADDR, (char*)&bIsReusePort, iReusePortLen);
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
	int iBindErr = bind(m_sktTCPSender, (LPSOCKADDR)&sdClientAddr, sizeof(sdClientAddr));
	if (iBindErr == SOCKET_ERROR)
		return WSA_BIND_ERROR;

	return TRUE;
}

BOOL TCPCommunications::Receive(byte* &pbyBuffer, size_t& uiDataLength, size_t& uiFrameNum)
{
	// init the data
	PST_PACKAGE pstReceivePacket = new ST_PACKAGE;
	sockaddr stReceiveAddr;
	int nSize = sizeof(stReceiveAddr);
	size_t packageNum = 0;
	memset(pbyBuffer, 0, uiDataLength);

	SOCKET fd_client;

	fd_client = accept(m_sktTCPReceiver, (sockaddr *)&stReceiveAddr, &nSize);

	std::cout << "i get info" << std::endl;

	// get the data
	while (true)
	{
		int retVal = recv(fd_client, (char*)pstReceivePacket, sizeof(ST_PACKAGE), 0);
		if (retVal == SOCKET_ERROR)
		{
			closesocket(fd_client);
			WSACleanup();
			delete pstReceivePacket;
			return FALSE;
		}

		std::cout << "i get info2" << std::endl;
		// ˵���ǵ�һ����
		if (0 == pstReceivePacket->uiPackageNum)
			packageNum = 0;

		std::cout << "i get info3" << std::endl;
		// �鿴����Ŀռ��Ƿ��㹻
		if (uiDataLength < pstReceivePacket->uiTotalDataLength && 0 == pstReceivePacket->uiPackageNum)
		{
			std::cout << "i get info10" << std::endl;
			if (NULL != pbyBuffer)
				delete[] pbyBuffer;
			std::cout << "i get info11" << std::endl;
			pbyBuffer = new byte[pstReceivePacket->uiTotalDataLength];
			uiDataLength = pstReceivePacket->uiTotalDataLength;
			memset(pbyBuffer, 0, uiDataLength);
			uiFrameNum = pstReceivePacket->uiFrameNum;
		}
		std::cout << "i get info4" << std::endl;
		// �������Ƿ���ȷ
		if (packageNum != pstReceivePacket->uiPackageNum)
		{
			std::cout << "error:" << packageNum << "," << pstReceivePacket->uiPackageNum << std::endl;
			if (NULL != pbyBuffer)
				delete[] pbyBuffer;
			pbyBuffer = NULL;
			uiDataLength = 0;
			delete pstReceivePacket;
			return FALSE;
		}
		std::cout << "i get info5" << std::endl;
		// ����ʣ��İ�
		memcpy(pbyBuffer + packageNum*DATA_BUF_LENGTH,
			pstReceivePacket->abyData, pstReceivePacket->uiDataLength);

		std::cout << "i get info6" << std::endl;
		// �ж��Ƿ��Ѿ���ȫ������
		if (DATA_BUF_LENGTH != pstReceivePacket->uiDataLength)
			break;
		std::cout << "i get info7" << std::endl;
		packageNum++;
	}
	std::cout << "i get info8" << std::endl;
	closesocket(fd_client);

	delete pstReceivePacket;
	return TRUE;
}

BOOL TCPCommunications::AddToAccepter(BYTE abyServerIP[IP_BYTE_LENGTH], UINT uiServerPort)
{
	if (AddServerObj(abyServerIP, uiServerPort))
		return TRUE;
	else
		return FALSE;
}

BOOL TCPCommunications::AddServerObj(BYTE abyServerIp[IP_BYTE_LENGTH], UINT uiServerPort)
{
	USES_CONVERSION;

	char pcServerIp[IP_CHAR_LENGTH];
	ByteIpToStringIp(abyServerIp, pcServerIp);

 	if (m_sktTCPReceiver != INVALID_SOCKET)
 		closesocket(m_sktTCPReceiver);

	//����UDPͨ���׽���
	m_sktTCPReceiver = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_sktTCPReceiver)   // CCsccLog::DebugPrint("������������UDPͨ���׽��ִ���\n");
		return FALSE;

	//����������
	int iErrCode;

	//�����õ��׽��ֻ�������С
	UINT uiBuflen;
	//��������С
	UINT uiOptlen = sizeof(uiBuflen);

	//��ȡ���ͻ���������
	iErrCode = getsockopt(m_sktTCPReceiver,		//�׽���
		SOL_SOCKET,			//�׽��ּ��𣬴˴�ѡ���׽������ڵ�Ӧ�ò�
		SO_SNDBUF,			//��Ҫ���õ��׽���ѡ�����ƣ��˴�Ϊ��������С
		(char *)&uiBuflen,	//���õ��׽���ѡ��ֵ
		(int*)&uiOptlen);	//��������С

	//�жϻ�ȡ�����������Ƿ��쳣
	if (SOCKET_ERROR == iErrCode)
		return FALSE;

	//��ԭ�ȵĻ�������С����ʮ����ϵͳĬ��Ϊ8192byte(8k)
	uiBuflen *= BUFFER_SIZE_TIMES;

	//�趨��������С                                                           
	iErrCode = setsockopt(m_sktTCPReceiver, SOL_SOCKET, SO_RCVBUF, (char *)&uiBuflen, uiOptlen);

	//�ж����û�������С�Ƿ��쳣
	if (SOCKET_ERROR == iErrCode)
		return FALSE;

	// ��黺������С�Ƿ��޸ĳɹ�
	unsigned int uiNewRcvBuf;
	iErrCode = getsockopt(m_sktTCPReceiver, SOL_SOCKET, SO_SNDBUF, (char*)&uiNewRcvBuf, (int*)&uiOptlen);
	if (SOCKET_ERROR == iErrCode || uiNewRcvBuf == uiBuflen) // CCsccLog::DebugPrint("�޸�ϵͳ�������ݻ�����ʧ�ܣ�\n");
		return FALSE;

	// �󶨷�����IP��ַ
	sockaddr_in sdServerAddr;									// �������˵�ַ��
	sdServerAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);	    // ���ͱ���IP
	sdServerAddr.sin_family = AF_INET;
	sdServerAddr.sin_port = htons(uiServerPort);				// �˿ں�,�����ֽ�����ת��Ϊ�����ֽ���

	//���׽���
	int iBindErr = bind(m_sktTCPReceiver, (LPSOCKADDR)&sdServerAddr, sizeof(sdServerAddr));
	if (iBindErr == SOCKET_ERROR)   // CCsccLog::DebugPrint("�󶨽����׽��ֳ���\n");
		return FALSE;

	listen(m_sktTCPReceiver, MAX_LISTEN_NUM);
	//SetServerObj(m_sktReceiveSender, abyServerIp, uiServerPort);			// ��UDPͨ���׽��ּ��뵽�������˶�����
	return TRUE;
}

BOOL TCPCommunications::ReleaseCom()
{
	if (m_sktTCPSender != INVALID_SOCKET)
		closesocket(m_sktTCPSender);

	if (m_sktTCPReceiver != INVALID_SOCKET)
		closesocket(m_sktTCPReceiver);

	int iUnInitialErr = 0;					// ��������ֵ
	// �ͷ��׽��ֿ�
	iUnInitialErr = WSACleanup();

	// �ж��Ƿ�ɹ��ͷ�
	if (0 != iUnInitialErr)
	{
		// �жϴ�������Ƿ���WSANOTINITIALISED
		if (WSANOTINITIALISED == WSAGetLastError())
			return FALSE;

		return TRUE;
	}
	else
		return FALSE;

}