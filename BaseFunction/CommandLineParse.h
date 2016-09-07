#ifndef COMMANDLINE_PARSE_H
#define COMMANDLINE_PARSE_H

// 我们的基本库
#include "define.h"

// 使用模板类的导出工作
template class DLLS_PORT std::vector<char*>;

class DLLS_PORT CommandLineParse
{
public:
	CommandLineParse(int argc, char** argv);
	~CommandLineParse();

	bool IsSet(const char* command);
	char* Value(const char* command);

protected:
	void ParseCommandLine(int argc, char** argv);

private:
	std::vector<char*> m_commandLine;
};




#endif