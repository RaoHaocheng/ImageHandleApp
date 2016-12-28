#include "CommandLineParse.h"

CommandLineParse::CommandLineParse(int argc, char** argv)
{
	ParseCommandLine(argc, argv);
}

CommandLineParse::~CommandLineParse()
{
}

void CommandLineParse::ParseCommandLine(int argc, char** argv)
{
	m_commandLine.clear();

	// 将数据放置到我们的容器中去
	for (int i = 0; i < argc; i++)
	{
		m_commandLine.push_back(argv[i]);
	}
}


bool CommandLineParse::IsSet(const char* command)
{
	for (int i = 0; i < (int)m_commandLine.size(); i++)
	{
		// 说明有相等的
		if (!strcmp(command, m_commandLine.at(i)))
			return true;
	}

	return false;
}

char* CommandLineParse::Value(const char* command)
{
	// 说明没有内容
	if (!IsSet(command))
		return NULL;

	for (int i = 0; i < (int)m_commandLine.size(); i++)
	{
		// 说明有相等的
		if (!strcmp(command, m_commandLine.at(i)))
		{
			// 说明后一位是有数据的
			if (i + 1 < (int)m_commandLine.size())
			{
				return m_commandLine.at(++i);
			}
			else
				return NULL;
		}
	}

	return NULL;
}