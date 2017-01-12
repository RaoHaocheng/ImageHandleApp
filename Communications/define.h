#ifndef COMMUNICATIONS_DEFINE_H
#define COMMUNICATIONS_DEFINE_H
// 基本的数据结构和类型

// 包含标准库中的内容
#include <vector>    // 容器
#include <set>
#include <list>
#include <iostream>  // 标准输入输出

// windows基本库
#include <windows.h>
//#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

// 定义编译环境
#ifdef  _DLL
#define  DLLS_PORT   __declspec(dllexport)      // 动态链接库导出
#else
#define  DLLS_PORT
#endif

#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02
#define MAX_FILE_PATH	    256
#define MAX_LOG_LENGTH	    512
#define SERVER_WAIT_TIME    100            // 服务器等待时间
#define RECV_BUF            2048           // 接收缓存大小
#define WSA_IS_MEMBER                2     // 错误代码
#define WSA_CREATE_SOCKET_ERROR      3     // 创建套接字错误码
#define WSA_CONNECT_ERROR            4     // 客户端连接服务器错误码
#define WSA_BIND_ERROR               5     // 绑定套接字错误码
#define WSA_SET_SOCKET_OPT_ERROR     6
#define WSA_GET_SOCKET_OPT_ERROR     7
#define WSA_RCV_DATA_ERROR           8

// 以下是我们自己需要使用使用的数据
static const int NIC_NUM = 1;              // 机器的网卡个数
static const int HOST_NAME_LENGTH = 256;   // 主机名字长度
static const int MAC_BYTE_LENGTH = 6;      // MAC地址字节数
static const int IP_BYTE_LENGTH = 4;       // IP地址字节数
static const int IP_CHAR_LENGTH = 16;      // IP的Char类型的长度
static const int IP_BIT_LENGTH = 3;        // IP的Char类型的长度
static const int IP_FIRST_NUM = 0;         // ip中的第一位
static const int IP_SCE_NUM = 1;           // ip中的第二位
static const int IP_THIRD_NUM = 2;         // ip中的第三位
static const int IP_FOURTH_NUM = 3;        // ip中的第四位
static const int DATA_BUF_LENGTH = 1024;   // 数据包存放的数据大小
static const int BUFFER_SIZE_TIMES = 10;   // 缓存倍数

// ShareMemory Data struct
static const int SHARE_MEM_BUF_SIZE = 1024 * 1024 * 5;	//5M
static const int CHARS_BUFFER_SIZE = 255;

typedef struct STRUCT_IP_INFO//IP地址信息
{
	BYTE abyIPAddr[IP_BYTE_LENGTH];		   // IP地址
	BYTE abySubNetMask[IP_BYTE_LENGTH];    // 子网掩码
	BYTE abyGateWay[IP_BYTE_LENGTH];       // 网关
	BYTE abyMacAddr[MAC_BYTE_LENGTH];      // MAC地址
	BYTE abyBroadCastAddr[IP_BYTE_LENGTH]; // 广播地址
}ST_IP_INFO, *PST_IP_INFO;


// 数据包体结构体
typedef struct STRUCT_PACKAGE
{
	BYTE  abySourceIp[IP_BYTE_LENGTH];	// 数据源地址
	UINT  uiSourcePort;					// 数据源端口
	size_t uiFrameNum;					// 帧号
	size_t uiPackageNum;				// 包号
	byte abyData[DATA_BUF_LENGTH];		// 数据缓存
	size_t uiDataLength;				// 包中数据长度
	size_t uiTotalDataLength;			// 待发送数据的总长度
}ST_PACKAGE, *PST_PACKAGE;
#endif // !DEFINE_H