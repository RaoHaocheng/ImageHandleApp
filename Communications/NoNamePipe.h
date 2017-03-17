#ifndef NO_NAME_PIPE
#define NO_NAME_PIPE

#include "define.h"

enum ProcessType
{
	ParentProcess=0,
	ChildProcess=1
};

class DLLS_PORT NoNamePipe
{
public:
	NoNamePipe(ProcessType type);
	~NoNamePipe();

public:
	bool InitNoNamePipe();
	bool WriteToPipe(const char* buf, size_t& len);
	bool ReadFromPipe(char* buf, size_t& len);
	bool StartProcess(char* szCmdLine);

protected:
	bool CreateNoNamePipe();
	bool GetNoNamePipe();

private:
	ProcessType m_type;
	HANDLE m_hPipeRead;
	HANDLE m_hPipeWrite;
	SECURITY_ATTRIBUTES m_saAttr;
	STARTUPINFO m_siStartInfo;
	PROCESS_INFORMATION m_piProcInfo;
}

#endif