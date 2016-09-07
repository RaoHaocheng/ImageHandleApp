#ifndef COMMANDLINE_PARSE_H
#define COMMANDLINE_PARSE_H

// ���ǵĻ�����
#include "define.h"

// ʹ��ģ����ĵ�������
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