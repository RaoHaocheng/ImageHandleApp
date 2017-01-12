#ifndef SHARE_MEMORY_H
#define SHARE_MEMORY_H

#include "define.h"

// ���������ù����ڴ����Ϣ�����������ݿ��Լ������
typedef struct ST_SHARE_MEMORY_INFO
{
	int pocessNum;   // �����жϵ�ǰ�Ĺ����ڴ��ж��ٸ�������ʹ��
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