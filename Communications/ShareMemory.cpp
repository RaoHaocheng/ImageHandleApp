#include "ShareMemory.h"

const char* SHARE_MEMORY_DEFAULT_NAME = "Default_Share_Mem";

/***************************************************************************
* �������ƣ�   ShareMemory
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const char * name
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/03      ���ǲ�        ���
***************************************************************************/
ShareMemory::ShareMemory(const char* name /* = NULL */)
	:m_buf(NULL),
	m_handle(NULL),
	m_file(NULL)
{
	memset(&m_shareMemInfo,0,sizeof(m_shareMemInfo));
	OpenShareMemory(name);
}

/***************************************************************************
* �������ƣ�   ~ShareMemory
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/03      ���ǲ�        ���
***************************************************************************/
ShareMemory::~ShareMemory()
{
	CloseShareMemory();
}

/***************************************************************************
* �������ƣ�   OpenShareMemory
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const char * name
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/03      ���ǲ�        ���
***************************************************************************/
bool ShareMemory::OpenShareMemory(const char* name /* = NULL */)
{
	char* shareMemName = new char[CHARS_BUFFER_SIZE];
	if (NULL == name)
		memcpy(shareMemName, SHARE_MEMORY_DEFAULT_NAME, sizeof(SHARE_MEMORY_DEFAULT_NAME));
	else
		memcpy(shareMemName, name, sizeof(name));

	CloseShareMemory();
	m_handle = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, shareMemName);

	bool ok = false;
	if (NULL == m_handle)
	{    // ��ʧ�ܣ�����֮  
		m_handle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			SHARE_MEM_BUF_SIZE,
			shareMemName);
		// ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬�������������  
		m_file = (BYTE*)::MapViewOfFile(m_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		// set the file info
		memset(&m_shareMemInfo, 0, sizeof(m_shareMemInfo));
		m_shareMemInfo.pocessNum++;
		memcpy(m_file, &m_shareMemInfo, sizeof(m_shareMemInfo));

		// set  the address of buffer
		m_buf = m_file + sizeof(m_shareMemInfo);

		ok = false;
	}
	else
	{    // �򿪳ɹ���ӳ������һ����ͼ���õ�ָ�����ڴ��ָ�룬��ʾ�����������  
		m_file = (BYTE*)::MapViewOfFile(m_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		// read the file info
		memcpy(&m_shareMemInfo, m_file, sizeof(m_shareMemInfo));
		m_shareMemInfo.pocessNum++;
		memcpy(m_file, &m_shareMemInfo, sizeof(m_shareMemInfo));

		// set the address of the buffer
		m_buf = m_file + sizeof(m_shareMemInfo);

		ok = true;
	}

	delete[] shareMemName;
	return ok;
}

/***************************************************************************
* �������ƣ�   ReadMem
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  BYTE * data
* �Ρ�������   [in]  size_t len
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/03      ���ǲ�        ���
***************************************************************************/
bool ShareMemory::ReadMem(BYTE* data, size_t len)
{
	if (NULL == m_buf)
		return false;

	memcpy(data, m_buf, len);

	return true;
}

/***************************************************************************
* �������ƣ�   WriteMem
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  BYTE * data
* �Ρ�������   [in]  size_t len
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/03      ���ǲ�        ���
***************************************************************************/
bool ShareMemory::WriteMem(BYTE* data, size_t len)
{
	if (NULL == m_buf)
		return false;

	memcpy(m_buf, data, len);

	return true;
}

/***************************************************************************
* �������ƣ�   CloseShareMemory
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/03      ���ǲ�        ���
***************************************************************************/
void ShareMemory::CloseShareMemory()
{
	if (NULL != m_handle)
	{
		memcpy(&m_shareMemInfo, m_file, sizeof(m_shareMemInfo));

		// check the the pocess number,if no one use the sharememory, delete it
		if (!(--m_shareMemInfo.pocessNum))
			::UnmapViewOfFile(m_buf);
		else
			memcpy(m_file, &m_shareMemInfo, sizeof(m_shareMemInfo));

		::CloseHandle(m_handle);
		m_handle = NULL;
	}

	m_buf = NULL;
	m_file = NULL;
}