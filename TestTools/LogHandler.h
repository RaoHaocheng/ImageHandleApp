#ifndef LOG_HANDLER_H
#define LOG_HANDLER_H
// Log handler
#include "Define.h"

// easy use
#define WARNING_LOG  LogHandler::Warning
#define ERROR_LOG    LogHandler::Error
#define INFO_LOG     LogHandler::Info
#define DEBUG_LOG    LogHandler::Debug


class DLLS_PORT LogHandler
{
private:
	LogHandler(const char* fileName = NULL, const char* filePath = NULL);
	~LogHandler();
	enum MSGType
	{
		WarningType = 0,
		ErrorType = 1,
		InfoType = 2,
		DebugType = 3,
		UnkownType = 4
	};

public:
	// use this function create or delete object, if filePath != null, we use it instead of fileName
	static LogHandler* CreateLogHandler(const char* fileName = NULL, const char* filePath = NULL);
	static void DeleteLogHandler();

public:
	static void SetFilePath(const char* fileName = NULL, const char* filePath = NULL);
	static void SetLogHandlerState(bool ok);
	static void SetWinLogHandlerState(bool ok);
	static void Warning(const char* msg, ...);
	static void Error(const char* msg, ...);
	static void Info(const char* msg, ...);
	static void Debug(const char* msg, ...);

private:
	// init object
	static void Init(const char* fileName = NULL, const char* filePath = NULL);
	static void WriteFile(const char* info);
	static void ShowInfo(const char* info);
	static void OpenFile(const char* fileName = NULL, const char* filePath = NULL);
	static void CloseFile();
	static void GetLocationTime(char* time);
	static void GenerateInfo(MSGType type, char* info, const char* msg);
	static void DecodeMSG(const char* fmt, va_list arg_ptr, char* msg);
	static void ResetBuffer();
	static bool CheckInfoEnd(const char* info, const char* end);

private:
	static LogHandler* m_logHandler;   // real object
	static std::ofstream* m_logFile;   // file object 
	static bool m_logStart;            // bool
	static bool m_winLogStart;         // bool
	static char* m_infoBuffer;         
	static char* m_msgBuffer;
};

#endif