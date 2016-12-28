#include "LogHandler.h"
#include <time.h> 

std::ofstream* LogHandler::m_logFile = NULL;
LogHandler* LogHandler::m_logHandler = NULL;
bool LogHandler::m_logStart = true;
bool LogHandler::m_winLogStart = true;
char* LogHandler::m_infoBuffer = new char[BUFFER_SIZE];
char* LogHandler::m_msgBuffer = new char[BUFFER_SIZE];
const char* FILE_PATH = "LogFile.log";
const char* FILE_TITLE = "/*************************Welcome to use the LogHandler****************************/";

/***************************************************************************
* �������ƣ�   LogHandler
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* �Ρ�������   [in]  char * fileName
* �Ρ�������   [in]  char * filePath
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
LogHandler::LogHandler(const char* fileName /* = NULL */, const char* filePath /* = NULL */)
{
	Init(fileName, filePath);
}

/***************************************************************************
* �������ƣ�   ~LogHandler
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
LogHandler::~LogHandler()
{
	CloseFile();
	if (NULL != m_infoBuffer)
		delete m_infoBuffer;
	m_infoBuffer = NULL;

	if (NULL != m_msgBuffer)
		delete m_msgBuffer;
	m_msgBuffer = NULL;
}

// use this function create or delete object, if filePath != null, we use it instead of fileName
/***************************************************************************
* �������ƣ�   CreateLogHandler
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* �Ρ�������   [in]  char * fileName
* �Ρ�������   [in]  char * filePath
* ����ֵ����   LogHandler*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
LogHandler* LogHandler::CreateLogHandler(const char* fileName, const char* filePath)
{
	if (NULL == m_logHandler)
		m_logHandler = new LogHandler(fileName, filePath);

	return m_logHandler;
}

/***************************************************************************
* �������ƣ�   DeleteLogHandler
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
void LogHandler::DeleteLogHandler()
{
	if (NULL != m_logHandler)
		delete m_logHandler;

	m_logHandler = NULL;
}

/***************************************************************************
* �������ƣ�   Warning
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* �Ρ�������   [in]  char * info
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
void LogHandler::Warning(const char* msg,...)
{
	if (!m_logStart&&!m_winLogStart)
		return;

	ResetBuffer();

	va_list arg_ptr;       va_start(arg_ptr, msg);
	DecodeMSG(msg, arg_ptr, m_msgBuffer);
	va_end(arg_ptr);

	GenerateInfo(MSGType::WarningType, m_infoBuffer, m_msgBuffer);

	if (m_logStart)
		WriteFile(m_infoBuffer);

	if (m_winLogStart)
		ShowInfo(m_infoBuffer);
}

/***************************************************************************
* �������ƣ�   Error
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* �Ρ�������   [in]  char * info
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
void LogHandler::Error(const char* msg, ...)
{
	if (!m_logStart&&!m_winLogStart)
		return;

	ResetBuffer();

	va_list arg_ptr;
	va_start(arg_ptr, msg);
	DecodeMSG(msg, arg_ptr, m_msgBuffer);
	va_end(arg_ptr);

	GenerateInfo(MSGType::ErrorType, m_infoBuffer, m_msgBuffer);

	if (m_logStart)
		WriteFile(m_infoBuffer);

	if (m_winLogStart)
		ShowInfo(m_infoBuffer);
}

/***************************************************************************
* �������ƣ�   Debug
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* �Ρ�������   [in]  char * info
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
void LogHandler::Debug(const char* msg, ...)
{
	if (!m_logStart&&!m_winLogStart)
		return;

	ResetBuffer();

	va_list arg_ptr;
	va_start(arg_ptr, msg);
	DecodeMSG(msg, arg_ptr, m_msgBuffer);
	va_end(arg_ptr);

	GenerateInfo(MSGType::DebugType, m_infoBuffer, m_msgBuffer);

	if (m_logStart)
		WriteFile(m_infoBuffer);

	if (m_winLogStart)
		ShowInfo(m_infoBuffer);
}

/***************************************************************************
* �������ƣ�   Info
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* �Ρ�������   [in]  char * info
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
void LogHandler::Info(const char* msg, ...)
{
	if (!m_logStart&&!m_winLogStart)
		return;

	ResetBuffer();

	va_list arg_ptr;
	va_start(arg_ptr, msg);
	DecodeMSG(msg, arg_ptr, m_msgBuffer);
	va_end(arg_ptr);

	GenerateInfo(MSGType::InfoType, m_infoBuffer, m_msgBuffer);

	if (m_logStart)
		WriteFile(m_infoBuffer);

	if (m_winLogStart)
		ShowInfo(m_infoBuffer);
}

/***************************************************************************
* �������ƣ�   Init
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  char * fileName
* �Ρ�������   [in]  char * filePath
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
void LogHandler::Init(const char* fileName /* = NULL */, const char* filePath /* = NULL */)
{
	OpenFile(fileName, filePath);
}

/***************************************************************************
* �������ƣ�   SetFilePath
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* �Ρ�������   [in]  char * fileName
* �Ρ�������   [in]  char * filePath
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
void LogHandler::SetFilePath(const char* fileName /* = NULL */, const char* filePath /* = NULL */)
{
	if (NULL == m_logHandler)
	{
		CreateLogHandler(fileName, filePath);
		return;
	}

	OpenFile(fileName, filePath);
}

/***************************************************************************
* �������ƣ�   WriteFile
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  char * info
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/25      ���ǲ�        ���
***************************************************************************/
void LogHandler::WriteFile(const char* info)
{
	if (NULL == m_logHandler)
		SetFilePath();

	// error in open file;
	if (!m_logFile->is_open())
		SetFilePath();

	(*m_logFile) << info;
	if (!CheckInfoEnd(info, "\n"))
		(*m_logFile) << "\n";
}

/***************************************************************************
* �������ƣ�   SetLogHandlerState
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public static 
* �Ρ�������   [in]  bool ok
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
void LogHandler::SetLogHandlerState(bool ok)
{
	m_logStart = ok; 
}

void LogHandler::SetWinLogHandlerState(bool ok)
{
	m_winLogStart = ok;
}

/***************************************************************************
* �������ƣ�   OpenFile
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  const char * fileName
* �Ρ�������   [in]  const char * filePath
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
void LogHandler::OpenFile(const char* fileName /* = NULL */, const char* filePath /* = NULL */)
{
	if (NULL != m_logFile){
		m_logFile->close();
		m_logFile->clear();
		delete m_logFile;
		m_logFile = NULL;
	}

	m_logFile = new std::ofstream();

	char fileFullPath[BUFFER_SIZE];
	memset(fileFullPath, 0, BUFFER_SIZE);

	int filePathLen = 0;

	if (filePath)
	{
		filePathLen = (int)strlen(filePath);
		memcpy(fileFullPath, filePath, filePathLen);
	}

	if (fileName)
		memcpy(fileFullPath + filePathLen, fileName, strlen(fileName));
	else
		memcpy(fileFullPath + filePathLen, FILE_PATH, strlen(FILE_PATH));

	// check the file exist
	std::ifstream logFile(fileFullPath);
	bool newFile = false;
	if (!logFile)
		newFile = true;
	else
		logFile.close();

	// create a write object
	m_logFile->open(fileFullPath, std::ofstream::out | std::ofstream::app);

	if (newFile)
		WriteFile(FILE_TITLE);
}

/***************************************************************************
* �������ƣ�   CloseFile
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
void LogHandler::CloseFile()
{
	if (NULL != m_logFile){
		m_logFile->close();
		m_logFile->clear();
		delete m_logFile;
		m_logFile = NULL;
	}
}

/***************************************************************************
* �������ƣ�   GetLocationTime
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  char * currentTime
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
void LogHandler::GetLocationTime(char* currentTime)
{
	time_t nowtime;
	nowtime = time(NULL);
	tm* t = new tm;
	localtime_s(t,&nowtime);
	const int STANDARD_YEAR = 1900;
	const int STANDARD_MON = 1;

	sprintf_s(currentTime, BUFFER_SIZE, "Date:%d-%02d-%02d Time:%02d:%02d:%02d",
			  t->tm_year + STANDARD_YEAR,
			  t->tm_mon + STANDARD_MON,
			  t->tm_mday,
		      t->tm_hour,
		      t->tm_min,
		      t->tm_sec);
	delete t;
}

/***************************************************************************
* �������ƣ�   GenerateInfo
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  MSGType type
* �Ρ�������   [in]  char * msg
* �Ρ�������   [in]  const char * info
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
void LogHandler::GenerateInfo(MSGType type, char* info, const char* msg)
{
	int len = 0;
	info[len++] = '<';
	GetLocationTime(&(info[len]));
	len = (int)strlen(info);

	char msgBuffer[BUFFER_SIZE];
	memset(msgBuffer, 0, BUFFER_SIZE);

	// generate the Info
	switch (type)
	{
	case LogHandler::MSGType::WarningType:
		sprintf_s(msgBuffer, " Warning> %s", msg);
		break;
	case LogHandler::MSGType::ErrorType:
		sprintf_s(msgBuffer, " Error> %s", msg);
		break;
	case LogHandler::MSGType::InfoType:
		sprintf_s(msgBuffer, " Info> %s", msg);
		break;
	case LogHandler::MSGType::DebugType:
		sprintf_s(msgBuffer, " Debug> %s", msg);
		break;
	case LogHandler::MSGType::UnkownType:
		sprintf_s(msgBuffer, " Unkown> %s", msg);
		break;
	default:
		sprintf_s(msgBuffer, " MsgType Error> %s", msg);
		break;
	}

	memcpy(&(info[len]), msgBuffer, strlen(msgBuffer)+1);
}

/***************************************************************************
* �������ƣ�   DecodeMSG
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  const char * pfmt
* �Ρ�������   [in]  va_list arg_ptr
* �Ρ�������   [in]  char * msg
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
void LogHandler::DecodeMSG(const char* pfmt, va_list arg_ptr, char* msg)
{
	double vargflt = 0;
	int  vargint = 0;
	char* vargpch = NULL;
	char vargch = 0;
	size_t len = 0;

	while (*pfmt)
	{
		if (*pfmt == '%')
		{
			switch (*(++pfmt))
			{

			case 'c':
				vargch = va_arg(arg_ptr, char);
				/*    va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
				mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
				sprintf_s(msg + len, BUFFER_SIZE, "%c", vargch);
				len = strlen(msg);
				break;
			case 'd':
			case 'i':
				vargint = va_arg(arg_ptr, int);
				sprintf_s(msg + len, BUFFER_SIZE, "%d", vargint);
				len = strlen(msg);
				break;
			case 'f':
				vargflt = va_arg(arg_ptr, double);
				/*    va_arg(ap, type), if type is narrow type (char, short, float) an error is given in strict ANSI
				mode, or a warning otherwise.In non-strict ANSI mode, 'type' is allowed to be any expression. */
				sprintf_s(msg + len, BUFFER_SIZE, "%f", vargflt);
				len = strlen(msg);
				break;
			case 's':
				vargpch = va_arg(arg_ptr, char*);
				sprintf_s(msg + len, BUFFER_SIZE, "%s", vargpch);
				len = strlen(msg);
				break;
			case 'o':
			case 'O':
				vargint = va_arg(arg_ptr, int);
				sprintf_s(msg + len, BUFFER_SIZE, "%o", vargint);
				len = strlen(msg);
				break;
			case 'x':
			case 'X':
				vargint = va_arg(arg_ptr, int);
				sprintf_s(msg + len, BUFFER_SIZE, "%x", vargint);
				len = strlen(msg);
				break;
			case '%':
				sprintf_s(msg + len, BUFFER_SIZE, "%c", '%');
				len = strlen(msg);
				break;
			default:
				break;
			}
			pfmt++;
		}
		else
		{
			sprintf_s(msg + len, BUFFER_SIZE, "%c", *pfmt++);
			len = strlen(msg);
		}
	}
}

/***************************************************************************
* �������ƣ�   ResetBuffer
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
void LogHandler::ResetBuffer()
{
	if (NULL == m_infoBuffer)
		m_infoBuffer = new char[BUFFER_SIZE];
	memset(m_infoBuffer, 0, BUFFER_SIZE);

	if (NULL == m_msgBuffer)
		m_msgBuffer = new char[BUFFER_SIZE];
	memset(m_msgBuffer, 0, BUFFER_SIZE);
}

/***************************************************************************
* �������ƣ�   ShowInfo
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  const char * info
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
void LogHandler::ShowInfo(const char* info)
{
	OutputDebugString(m_infoBuffer);
	if (!CheckInfoEnd(info,"\n"))
		OutputDebugString("\n");
}

/***************************************************************************
* �������ƣ�   CheckInfoEnd
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private static 
* �Ρ�������   [in]  const char * info
* �Ρ�������   [in]  const char * end
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/26      ���ǲ�        ���
***************************************************************************/
bool LogHandler::CheckInfoEnd(const char* info, const char* end)
{
	const size_t infoLen = strlen(info);
	const size_t endLen = strlen(end);

	if (endLen > infoLen)
		return false;

	char* infoEnd = new char[infoLen];
	memset(infoEnd, 0, infoLen);
	memcpy(infoEnd, info + infoLen - endLen, endLen);

	if (0 == strcmp(infoEnd, end))
		return true;
	else
		return false;
}