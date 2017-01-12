#ifndef COMMUNICATIONS_DEFINE_H
#define COMMUNICATIONS_DEFINE_H
// ���������ݽṹ������

// ������׼���е�����
#include <vector>    // ����
#include <set>
#include <list>
#include <iostream>  // ��׼�������

// windows������
#include <windows.h>
//#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

// ������뻷��
#ifdef  _DLL
#define  DLLS_PORT   __declspec(dllexport)      // ��̬���ӿ⵼��
#else
#define  DLLS_PORT
#endif

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02
#define MAX_FILE_PATH	    256
#define MAX_LOG_LENGTH	    512
#define SERVER_WAIT_TIME    100            // �������ȴ�ʱ��
#define RECV_BUF            2048           // ���ջ����С
#define WSA_IS_MEMBER                2     // �������
#define WSA_CREATE_SOCKET_ERROR      3     // �����׽��ִ�����
#define WSA_CONNECT_ERROR            4     // �ͻ������ӷ�����������
#define WSA_BIND_ERROR               5     // ���׽��ִ�����
#define WSA_SET_SOCKET_OPT_ERROR     6
#define WSA_GET_SOCKET_OPT_ERROR     7
#define WSA_RCV_DATA_ERROR           8

// �����������Լ���Ҫʹ��ʹ�õ�����
static const int NIC_NUM = 1;              // ��������������
static const int HOST_NAME_LENGTH = 256;   // �������ֳ���
static const int MAC_BYTE_LENGTH = 6;      // MAC��ַ�ֽ���
static const int IP_BYTE_LENGTH = 4;       // IP��ַ�ֽ���
static const int IP_CHAR_LENGTH = 16;      // IP��Char���͵ĳ���
static const int IP_BIT_LENGTH = 3;        // IP��Char���͵ĳ���
static const int IP_FIRST_NUM = 0;         // ip�еĵ�һλ
static const int IP_SCE_NUM = 1;           // ip�еĵڶ�λ
static const int IP_THIRD_NUM = 2;         // ip�еĵ���λ
static const int IP_FOURTH_NUM = 3;        // ip�еĵ���λ
static const int DATA_BUF_LENGTH = 1024;   // ���ݰ���ŵ����ݴ�С
static const int BUFFER_SIZE_TIMES = 10;   // ���汶��

// ShareMemory Data struct
static const int SHARE_MEM_BUF_SIZE = 1024 * 1024 * 5;	//5M
static const int CHARS_BUFFER_SIZE = 255;

typedef struct STRUCT_IP_INFO//IP��ַ��Ϣ
{
	BYTE abyIPAddr[IP_BYTE_LENGTH];		   // IP��ַ
	BYTE abySubNetMask[IP_BYTE_LENGTH];    // ��������
	BYTE abyGateWay[IP_BYTE_LENGTH];       // ����
	BYTE abyMacAddr[MAC_BYTE_LENGTH];      // MAC��ַ
	BYTE abyBroadCastAddr[IP_BYTE_LENGTH]; // �㲥��ַ
}ST_IP_INFO, *PST_IP_INFO;


// ���ݰ���ṹ��
typedef struct STRUCT_PACKAGE
{
	BYTE  abySourceIp[IP_BYTE_LENGTH];	// ����Դ��ַ
	UINT  uiSourcePort;					// ����Դ�˿�
	size_t uiFrameNum;					// ֡��
	size_t uiPackageNum;				// ����
	byte abyData[DATA_BUF_LENGTH];		// ���ݻ���
	size_t uiDataLength;				// �������ݳ���
	size_t uiTotalDataLength;			// ���������ݵ��ܳ���
}ST_PACKAGE, *PST_PACKAGE;
#endif // !DEFINE_H