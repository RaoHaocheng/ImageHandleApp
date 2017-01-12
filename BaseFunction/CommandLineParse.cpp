#include "CommandLineParse.h"

/***************************************************************************
* 函数名称：   CommandLineParse
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  int argc
* 参　　数：   [in]  char * * argv
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/12      饶智博        添加
***************************************************************************/
CommandLineParse::CommandLineParse(int argc, char** argv)
{
	ParseCommandLine(argc, argv);
}

/***************************************************************************
* 函数名称：   ~CommandLineParse
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/12      饶智博        添加
***************************************************************************/
CommandLineParse::~CommandLineParse()
{
}

/***************************************************************************
* 函数名称：   ParseCommandLine
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  int argc
* 参　　数：   [in]  char * * argv
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/12      饶智博        添加
***************************************************************************/
void CommandLineParse::ParseCommandLine(int argc, char** argv)
{
	m_commandLine.clear();

	// 将数据放置到我们的容器中去
	for (int i = 0; i < argc; i++)
	{
		m_commandLine.push_back(argv[i]);
	}
}


/***************************************************************************
* 函数名称：   IsSet
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const char * command
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/12      饶智博        添加
***************************************************************************/
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

/***************************************************************************
* 函数名称：   Value
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  const char * command
* 返回值　：   char*
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2017/01/12      饶智博        添加
***************************************************************************/
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