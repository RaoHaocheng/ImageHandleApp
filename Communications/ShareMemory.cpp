#include "ShareMemory.h"

const char* SHARE_MEMORY_DEFAULT_NAME = "Default_Share_Mem";

/***************************************************************************
* 函数名称：   ShareMemory
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const char * name
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/03      饶智博        添加
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
* 函数名称：   ~ShareMemory
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/03      饶智博        添加
***************************************************************************/
ShareMemory::~ShareMemory()
{
	CloseShareMemory();
}

/***************************************************************************
* 函数名称：   OpenShareMemory
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const char * name
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/03      饶智博        添加
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
	{    // 打开失败，创建之  
		m_handle = ::CreateFileMapping(INVALID_HANDLE_VALUE,
			NULL,
			PAGE_READWRITE,
			0,
			SHARE_MEM_BUF_SIZE,
			shareMemName);
		// 映射对象的一个视图，得到指向共享内存的指针，设置里面的数据  
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
	{    // 打开成功，映射对象的一个视图，得到指向共享内存的指针，显示出里面的数据  
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
* 函数名称：   ReadMem
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  BYTE * data
* 参　　数：   [in]  size_t len
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/03      饶智博        添加
***************************************************************************/
bool ShareMemory::ReadMem(BYTE* data, size_t len)
{
	if (NULL == m_buf)
		return false;

	memcpy(data, m_buf, len);

	return true;
}

/***************************************************************************
* 函数名称：   WriteMem
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  BYTE * data
* 参　　数：   [in]  size_t len
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/03      饶智博        添加
***************************************************************************/
bool ShareMemory::WriteMem(BYTE* data, size_t len)
{
	if (NULL == m_buf)
		return false;

	memcpy(m_buf, data, len);

	return true;
}

/***************************************************************************
* 函数名称：   CloseShareMemory
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/03      饶智博        添加
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