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
* 函数名称：   LogHandler
* 摘　　要：   
* 全局影响：   private 
* 参　　数：   [in]  char * fileName
* 参　　数：   [in]  char * filePath
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
***************************************************************************/
LogHandler::LogHandler(const char* fileName /* = NULL */, const char* filePath /* = NULL */)
{
	Init(fileName, filePath);
}

/***************************************************************************
* 函数名称：   ~LogHandler
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
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
* 函数名称：   CreateLogHandler
* 摘　　要：   
* 全局影响：   public static 
* 参　　数：   [in]  char * fileName
* 参　　数：   [in]  char * filePath
* 返回值　：   LogHandler*
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
***************************************************************************/
LogHandler* LogHandler::CreateLogHandler(const char* fileName, const char* filePath)
{
	if (NULL == m_logHandler)
		m_logHandler = new LogHandler(fileName, filePath);

	return m_logHandler;
}

/***************************************************************************
* 函数名称：   DeleteLogHandler
* 摘　　要：   
* 全局影响：   public static 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
***************************************************************************/
void LogHandler::DeleteLogHandler()
{
	if (NULL != m_logHandler)
		delete m_logHandler;

	m_logHandler = NULL;
}

/***************************************************************************
* 函数名称：   Warning
* 摘　　要：   
* 全局影响：   public static 
* 参　　数：   [in]  char * info
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
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
* 函数名称：   Error
* 摘　　要：   
* 全局影响：   public static 
* 参　　数：   [in]  char * info
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
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
* 函数名称：   Debug
* 摘　　要：   
* 全局影响：   public static 
* 参　　数：   [in]  char * info
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
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
* 函数名称：   Info
* 摘　　要：   
* 全局影响：   public static 
* 参　　数：   [in]  char * info
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
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
* 函数名称：   Init
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  char * fileName
* 参　　数：   [in]  char * filePath
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
***************************************************************************/
void LogHandler::Init(const char* fileName /* = NULL */, const char* filePath /* = NULL */)
{
	OpenFile(fileName, filePath);
}

/***************************************************************************
* 函数名称：   SetFilePath
* 摘　　要：   
* 全局影响：   public static 
* 参　　数：   [in]  char * fileName
* 参　　数：   [in]  char * filePath
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
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
* 函数名称：   WriteFile
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  char * info
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/25      饶智博        添加
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
* 函数名称：   SetLogHandlerState
* 摘　　要：   
* 全局影响：   public static 
* 参　　数：   [in]  bool ok
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
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
* 函数名称：   OpenFile
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  const char * fileName
* 参　　数：   [in]  const char * filePath
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
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
* 函数名称：   CloseFile
* 摘　　要：   
* 全局影响：   private static 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
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
* 函数名称：   GetLocationTime
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  char * currentTime
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
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
* 函数名称：   GenerateInfo
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  MSGType type
* 参　　数：   [in]  char * msg
* 参　　数：   [in]  const char * info
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
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
* 函数名称：   DecodeMSG
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  const char * pfmt
* 参　　数：   [in]  va_list arg_ptr
* 参　　数：   [in]  char * msg
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
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
* 函数名称：   ResetBuffer
* 摘　　要：   
* 全局影响：   private static 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
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
* 函数名称：   ShowInfo
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  const char * info
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
***************************************************************************/
void LogHandler::ShowInfo(const char* info)
{
	OutputDebugString(m_infoBuffer);
	if (!CheckInfoEnd(info,"\n"))
		OutputDebugString("\n");
}

/***************************************************************************
* 函数名称：   CheckInfoEnd
* 摘　　要：   
* 全局影响：   private static 
* 参　　数：   [in]  const char * info
* 参　　数：   [in]  const char * end
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/26      饶智博        添加
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