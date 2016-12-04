#include "communications.h"

#include <stdlib.h>
#include <IPHlpApi.h>  // ����������صĺ�����ͷ�ļ�
#include "UDPCommunications.h"
#include <string>

#pragma comment(lib,"Iphlpapi.lib")


/***************************************************************************
* �������ƣ�   CreateCommunications
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* �Ρ�������   [in]  COMMUNICATIONS_TYPE type
* ����ֵ����   Communications*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/11/18      ���ǲ�        ���
***************************************************************************/
Communications* Communications::CreateCommunications(COMMUNICATIONS_TYPE type)
{
	switch (type)
	{
	case UDP:
		return new UDPCommunications();
		break;
	case TCP:
		break;
	case MULTICAST:
		break;
	case BROADCAST:
		break;
	default:
		return NULL;
		break;
	}

	return NULL;
}

Communications::Communications()
{

}

Communications::~Communications()
{
}


/***************************************************************************
* �������ƣ�   GetIpInfo
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  PST_IP_INFO pstIpInfo
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/11/18      ���ǲ�        ���
***************************************************************************/
void Communications::GetIpInfo(PST_IP_INFO pstIpInfo)
{
	int iAdapterIdx = 0;			        // �����ϸ���������������
	PIP_ADAPTER_INFO pAdapterInfo;	    	// ������Ϣ�ṹ��
	PIP_ADAPTER_INFO pAdapter = NULL;
	ULONG ulOutBufLen;					    // ����������Ϣ�������Ĵ�С
	pAdapterInfo = new IP_ADAPTER_INFO;     // Ϊ������Ϣ�ṹ������ڴ�
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	DWORD dwRetVal = 0; // ����ֵ

	// ��һ�ε���GetAdapterInfo��ȡulOutBufLen��С����ʱ���û����
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		delete pAdapterInfo;

		// ������ջ����С
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}

	// �ڶ��ε���GetAdapterInfo��ȡ������Ϣ������ɹ������ͨ���ú�����һ����������������ֵ��ȡ�����Ϣ
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;

		// ���λ�ȡ����������Ϣ
		while (pAdapter)
		{
			// ������MAC��ַ��Ϣ
			memcpy(pstIpInfo[iAdapterIdx].abyMacAddr, pAdapter->Address, sizeof(BYTE)*MAC_BYTE_LENGTH);
//  			pstIpInfo[iAdapterIdx].abyMacAddr[0] = pAdapter->Address[0];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[1] = pAdapter->Address[1];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[2] = pAdapter->Address[2];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[3] = pAdapter->Address[3];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[4] = pAdapter->Address[4];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[5] = pAdapter->Address[5];

			// IP��ַ
			IPStringtoByteArray(pAdapter->IpAddressList.IpAddress.String, pstIpInfo[iAdapterIdx].abyIPAddr);

			// ��ȡ����������Ϣ,������ṹ��
			IPStringtoByteArray(pAdapter->IpAddressList.IpMask.String, pstIpInfo[iAdapterIdx].abySubNetMask);

			// ��ȡIP��ַ��Ϣ,������ṹ��
			IPStringtoByteArray(pAdapter->GatewayList.IpAddress.String, pstIpInfo[iAdapterIdx].abyGateWay);

			// ����㲥��ַ����λ����BroadCastAddr=IPAddr+ !(��������)��á�����+��ʾ��!��ʾ�ǣ�����ȡ����
			for (int i = 0; i < IP_BYTE_LENGTH; i++)
			{
				pstIpInfo[iAdapterIdx].abyBroadCastAddr[i] =
					(pstIpInfo[iAdapterIdx].abyIPAddr[i]) | ~(pstIpInfo[iAdapterIdx].abySubNetMask[i]);
			}

			// ��ȡ��һ������
			pAdapter = pAdapter->Next;

			// ����������һ
			iAdapterIdx++;
		}
	}
	delete pAdapterInfo;
	pAdapterInfo = NULL;
}

/***************************************************************************
* �������ƣ�   GetIpAddr
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  PST_IP_INFO pstIpInfo
* �Ρ�������   [in]  UINT & uiIpNum
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/11/18      ���ǲ�        ���
***************************************************************************/
void Communications::GetIpAddr(PST_IP_INFO pstIpInfo, UINT &uiIpNum)
{
	char szHostName[HOST_NAME_LENGTH]; // ��������

	// ��ȡ����������ַ
	if (gethostname(szHostName, HOST_NAME_LENGTH) == 0)
	{
		// ����������ַ��Ϣ
		struct hostent* pstHostAddrInfo;

		// �����������ֻ�ȡ���ַ��Ϣ
		pstHostAddrInfo = gethostbyname(szHostName);

		// ����ж�����������������λ�ȡ���ĸ���IP��ַ
		for (UINT i = 0; pstHostAddrInfo != NULL && pstHostAddrInfo->h_addr_list[i] != NULL; i++)
		{
			BYTE szTempIpBuf[IP_BYTE_LENGTH];		// ���IP�Ļ���

			memset(szTempIpBuf, 0, IP_BYTE_LENGTH);	// ��ʼ��IP����

			// ���IP���ĸ��ֶ�
			for (int j = 0; j < pstHostAddrInfo->h_length; j++)
			{
				szTempIpBuf[j] = (BYTE)((unsigned char*)pstHostAddrInfo->h_addr_list[i])[j];
			}

			// ������ЧIP
			if (szTempIpBuf[0] != 127 && szTempIpBuf[0] != 0 && szTempIpBuf[0] < 224)
			{
				memcpy(pstIpInfo[uiIpNum].abyIPAddr, szTempIpBuf, IP_BYTE_LENGTH);
				// IP������һ
				uiIpNum++;
			}
		}
	}

	return;
}

/***************************************************************************
* �������ƣ�   IPStringtoByteArray
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  char szIp[IP_CHAR_LENGTH]
* �Ρ�������   [in]  BYTE abyByteArray[IP_BYTE_LENGTH]
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/11/18      ���ǲ�        ���
***************************************************************************/
void Communications::IPStringtoByteArray(char szIp[IP_CHAR_LENGTH], BYTE abyByteArray[IP_BYTE_LENGTH])
{
	byte byIp;
	int iIpByteNum = 0;               // IP�ֶκ�
	int iByteTotalNum = 0;            // ��ǰIP������
	int iByteBeginNum = 0;            // ����һ���ֽ���

	for (int i = 0; i < IP_CHAR_LENGTH; i++)
	{
		if (szIp[i] == '.' || szIp[i] == '\0')
		{
			char cIpByte[IP_BYTE_LENGTH] = { '\0' };
			memcpy(cIpByte, &szIp[iByteBeginNum], iByteTotalNum);

			// ��IP��һ���ֶ���CStingת����Ϊ����
			byIp = atoi(cIpByte);

			// ����IP��ǰ�ֶ�
			abyByteArray[iIpByteNum] = byIp;

			// ��Ӧ�ı�������
			++(iByteBeginNum = i);
			iIpByteNum++;
			iByteTotalNum = 0;

			// ����Ƿ��ǽ�����
			if (iIpByteNum == IP_BYTE_LENGTH)
				return;
		}
		else
			iByteTotalNum++;
	}
}

/***************************************************************************
* �������ƣ�   ByteIpToStringIp
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  byte abyByteArray[IP_BYTE_LENGTH]
* �Ρ�������   [in]  char * pcStringIp
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/11/18      ���ǲ�        ���
***************************************************************************/
void Communications::ByteIpToStringIp(byte abyByteArray[IP_BYTE_LENGTH], char* pcStringIp)
{
	sprintf_s(pcStringIp, IP_CHAR_LENGTH, "%d,%d,%d,%d", 
		abyByteArray[IP_FIRST_NUM], abyByteArray[IP_SCE_NUM], 
		abyByteArray[IP_THIRD_NUM], abyByteArray[IP_FOURTH_NUM]);
}