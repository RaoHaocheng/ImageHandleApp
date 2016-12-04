#include "communications.h"

#include <stdlib.h>
#include <IPHlpApi.h>  // 操作网卡相关的函数库头文件
#include "UDPCommunications.h"
#include <string>

#pragma comment(lib,"Iphlpapi.lib")


/***************************************************************************
* 函数名称：   CreateCommunications
* 摘　　要：   
* 全局影响：   public static 
* 参　　数：   [in]  COMMUNICATIONS_TYPE type
* 返回值　：   Communications*
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/18      饶智博        添加
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
* 函数名称：   GetIpInfo
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  PST_IP_INFO pstIpInfo
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/18      饶智博        添加
***************************************************************************/
void Communications::GetIpInfo(PST_IP_INFO pstIpInfo)
{
	int iAdapterIdx = 0;			        // 本机上各个网卡的索引号
	PIP_ADAPTER_INFO pAdapterInfo;	    	// 网卡信息结构体
	PIP_ADAPTER_INFO pAdapter = NULL;
	ULONG ulOutBufLen;					    // 接收网卡信息缓冲区的大小
	pAdapterInfo = new IP_ADAPTER_INFO;     // 为网卡信息结构体分配内存
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	DWORD dwRetVal = 0; // 返回值

	// 第一次调用GetAdapterInfo获取ulOutBufLen大小，此时调用会出错
	if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
	{
		delete pAdapterInfo;

		// 分配接收缓存大小
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}

	// 第二次调用GetAdapterInfo获取网卡信息，如果成功则可以通过该函数第一个参数所带出来的值获取相关信息
	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;

		// 依次获取各个网卡信息
		while (pAdapter)
		{
			// 网卡的MAC地址信息
			memcpy(pstIpInfo[iAdapterIdx].abyMacAddr, pAdapter->Address, sizeof(BYTE)*MAC_BYTE_LENGTH);
//  			pstIpInfo[iAdapterIdx].abyMacAddr[0] = pAdapter->Address[0];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[1] = pAdapter->Address[1];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[2] = pAdapter->Address[2];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[3] = pAdapter->Address[3];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[4] = pAdapter->Address[4];
//  			pstIpInfo[iAdapterIdx].abyMacAddr[5] = pAdapter->Address[5];

			// IP地址
			IPStringtoByteArray(pAdapter->IpAddressList.IpAddress.String, pstIpInfo[iAdapterIdx].abyIPAddr);

			// 获取子网掩码信息,并存入结构体
			IPStringtoByteArray(pAdapter->IpAddressList.IpMask.String, pstIpInfo[iAdapterIdx].abySubNetMask);

			// 获取IP地址信息,并存入结构体
			IPStringtoByteArray(pAdapter->GatewayList.IpAddress.String, pstIpInfo[iAdapterIdx].abyGateWay);

			// 计算广播地址，由位运算BroadCastAddr=IPAddr+ !(子网掩码)求得。其中+表示或，!表示非（即，取反）
			for (int i = 0; i < IP_BYTE_LENGTH; i++)
			{
				pstIpInfo[iAdapterIdx].abyBroadCastAddr[i] =
					(pstIpInfo[iAdapterIdx].abyIPAddr[i]) | ~(pstIpInfo[iAdapterIdx].abySubNetMask[i]);
			}

			// 获取下一块网卡
			pAdapter = pAdapter->Next;

			// 网卡个数加一
			iAdapterIdx++;
		}
	}
	delete pAdapterInfo;
	pAdapterInfo = NULL;
}

/***************************************************************************
* 函数名称：   GetIpAddr
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  PST_IP_INFO pstIpInfo
* 参　　数：   [in]  UINT & uiIpNum
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/18      饶智博        添加
***************************************************************************/
void Communications::GetIpAddr(PST_IP_INFO pstIpInfo, UINT &uiIpNum)
{
	char szHostName[HOST_NAME_LENGTH]; // 主机名字

	// 获取本地主机地址
	if (gethostname(szHostName, HOST_NAME_LENGTH) == 0)
	{
		// 接收主机地址信息
		struct hostent* pstHostAddrInfo;

		// 依据主机名字获取其地址信息
		pstHostAddrInfo = gethostbyname(szHostName);

		// 针对有多个网卡的主机，依次获取它的各个IP地址
		for (UINT i = 0; pstHostAddrInfo != NULL && pstHostAddrInfo->h_addr_list[i] != NULL; i++)
		{
			BYTE szTempIpBuf[IP_BYTE_LENGTH];		// 存放IP的缓存

			memset(szTempIpBuf, 0, IP_BYTE_LENGTH);	// 初始化IP缓存

			// 存放IP的四个字段
			for (int j = 0; j < pstHostAddrInfo->h_length; j++)
			{
				szTempIpBuf[j] = (BYTE)((unsigned char*)pstHostAddrInfo->h_addr_list[i])[j];
			}

			// 保存有效IP
			if (szTempIpBuf[0] != 127 && szTempIpBuf[0] != 0 && szTempIpBuf[0] < 224)
			{
				memcpy(pstIpInfo[uiIpNum].abyIPAddr, szTempIpBuf, IP_BYTE_LENGTH);
				// IP个数加一
				uiIpNum++;
			}
		}
	}

	return;
}

/***************************************************************************
* 函数名称：   IPStringtoByteArray
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  char szIp[IP_CHAR_LENGTH]
* 参　　数：   [in]  BYTE abyByteArray[IP_BYTE_LENGTH]
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/18      饶智博        添加
***************************************************************************/
void Communications::IPStringtoByteArray(char szIp[IP_CHAR_LENGTH], BYTE abyByteArray[IP_BYTE_LENGTH])
{
	byte byIp;
	int iIpByteNum = 0;               // IP字段号
	int iByteTotalNum = 0;            // 当前IP的总数
	int iByteBeginNum = 0;            // 从哪一个字节起

	for (int i = 0; i < IP_CHAR_LENGTH; i++)
	{
		if (szIp[i] == '.' || szIp[i] == '\0')
		{
			char cIpByte[IP_BYTE_LENGTH] = { '\0' };
			memcpy(cIpByte, &szIp[iByteBeginNum], iByteTotalNum);

			// 将IP的一个字段有CSting转化成为整型
			byIp = atoi(cIpByte);

			// 保存IP当前字段
			abyByteArray[iIpByteNum] = byIp;

			// 相应的变量增加
			++(iByteBeginNum = i);
			iIpByteNum++;
			iByteTotalNum = 0;

			// 检查是否是结束了
			if (iIpByteNum == IP_BYTE_LENGTH)
				return;
		}
		else
			iByteTotalNum++;
	}
}

/***************************************************************************
* 函数名称：   ByteIpToStringIp
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  byte abyByteArray[IP_BYTE_LENGTH]
* 参　　数：   [in]  char * pcStringIp
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/18      饶智博        添加
***************************************************************************/
void Communications::ByteIpToStringIp(byte abyByteArray[IP_BYTE_LENGTH], char* pcStringIp)
{
	sprintf_s(pcStringIp, IP_CHAR_LENGTH, "%d,%d,%d,%d", 
		abyByteArray[IP_FIRST_NUM], abyByteArray[IP_SCE_NUM], 
		abyByteArray[IP_THIRD_NUM], abyByteArray[IP_FOURTH_NUM]);
}