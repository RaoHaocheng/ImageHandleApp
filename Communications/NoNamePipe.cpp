#include "NoNamePipe.h"


/***************************************************************************
* �������ƣ�   NoNamePipe
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  ProcessType type
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/21      ���ǲ�        ���
***************************************************************************/
NoNamePipe::NoNamePipe(ProcessType type):
	m_hPipeRead(NULL),
	m_hPipeWrite(NULL),
	m_type(type)
{
}

/***************************************************************************
* �������ƣ�   ~NoNamePipe
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/21      ���ǲ�        ���
***************************************************************************/
NoNamePipe::~NoNamePipe()
{
	CloseHandle(m_hPipeWrite);
	CloseHandle(m_hPipeRead);
}

/***************************************************************************
* �������ƣ�   StartProcess
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  char * szCmdLine
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/21      ���ǲ�        ���
***************************************************************************/
bool NoNamePipe::StartProcess(char* szCmdLine)
{
	if (ParentProcess != m_type)
		return false;

	ZeroMemory(&m_piProcInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&m_siStartInfo, sizeof(STARTUPINFO));
	m_siStartInfo.cb = sizeof(STARTUPINFO);
	m_siStartInfo.hStdError = m_hPipeWrite;
	m_siStartInfo.hStdOutput = m_hPipeWrite;
	m_siStartInfo.hStdInput = m_hPipeRead;
	m_siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	bool bSuccess = CreateProcess(NULL,
		szCmdLine,	   // command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&m_siStartInfo,// STARTUPINFO pointer 
		&m_piProcInfo);// receives PROCESS_INFORMATION 

	if (!bSuccess)
		return false;
	else
	{
		CloseHandle(m_piProcInfo.hProcess);
		CloseHandle(m_piProcInfo.hThread);
		return true;
	}
}

/***************************************************************************
* �������ƣ�   WriteToPipe
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const char * buf
* �Ρ�������   [in]  size_t & len
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/21      ���ǲ�        ���
***************************************************************************/
bool NoNamePipe::WriteToPipe(const char* buf, size_t& len)
{
	DWORD dwWritten;
	bool bSuccess = WriteFile(m_hPipeWrite, buf, len, &dwWritten, NULL);
	len = dwWritten;
	return bSuccess;
}

/***************************************************************************
* �������ƣ�   InitNoNamePipe
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/21      ���ǲ�        ���
***************************************************************************/
bool NoNamePipe::InitNoNamePipe()
{
	bool bRet;
	switch (m_type)
	{
	case ParentProcess:
		bRet = CreateNoNamePipe();
		break;
	case ChildProcess:
		bRet = GetNoNamePipe();
		break;
	default:
		break;
	}

	return bRet;
}

/***************************************************************************
* �������ƣ�   CreateNoNamePipe
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/21      ���ǲ�        ���
***************************************************************************/
bool NoNamePipe::CreateNoNamePipe()
{
	m_saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	m_saAttr.bInheritHandle = true;
	m_saAttr.lpSecurityDescriptor = NULL;

	// ���������̹ܵ�
	if (!CreatePipe(&m_hPipeRead, &m_hPipeWrite, &m_saAttr, 0))
		return false;

	if (!SetHandleInformation(m_hPipeRead, HANDLE_FLAG_INHERIT, 0)
		&& !SetHandleInformation(m_hPipeWrite, HANDLE_FLAG_INHERIT, 0))
		return false;
}

/***************************************************************************
* �������ƣ�   GetNoNamePipe
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/21      ���ǲ�        ���
***************************************************************************/
bool NoNamePipe::GetNoNamePipe()
{
	m_hPipeRead = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hPipeWrite = GetStdHandle(STD_INPUT_HANDLE);
	if ((m_hPipeRead == INVALID_HANDLE_VALUE) ||
		(m_hPipeWrite == INVALID_HANDLE_VALUE))
		return false;
	else
		return true;
}

/***************************************************************************
* �������ƣ�   ReadFromPipe
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  char * buf
* �Ρ�������   [in]  size_t & len
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/21      ���ǲ�        ���
***************************************************************************/
bool NoNamePipe::ReadFromPipe(char* buf, size_t& len)
{
	DWORD dwRead;
	bool bSuccess = ReadFile(m_hPipeRead, buf, len, &dwRead, NULL);
	len = dwRead;
	return bSuccess;
}