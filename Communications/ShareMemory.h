#ifndef SHARE_MEMORY_H
#define SHARE_MEMORY_H

#include "define.h"

// 用于描述该共享内存的信息，后语有内容可以继续添加
typedef struct ST_SHARE_MEMORY_INFO
{
	int pocessNum;   // 用于判断当前的共享内存有多少个进程在使用
}SHARE_MEMORY_INFO;


class DLLS_PORT ShareMemory
{
public:
	ShareMemory(const char* name = NULL);
	~ShareMemory();

public:
	bool OpenShareMemory(const char* name = NULL);
	void CloseShareMemory();
	bool WriteMem(BYTE* date, size_t len);
	bool ReadMem(BYTE* date, size_t len);
	inline LPVOID MemoryAddress() const { return m_buf; }

protected:

private:
	HANDLE m_handle;
	BYTE* m_buf;
	BYTE* m_file;
	SHARE_MEMORY_INFO m_shareMemInfo;
};
#endif // !SHARE_MEMORY_H