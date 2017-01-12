#include "CommandLineParse.h"

/***************************************************************************
* �������ƣ�   CommandLineParse
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  int argc
* �Ρ�������   [in]  char * * argv
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/12      ���ǲ�        ���
***************************************************************************/
CommandLineParse::CommandLineParse(int argc, char** argv)
{
	ParseCommandLine(argc, argv);
}

/***************************************************************************
* �������ƣ�   ~CommandLineParse
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/12      ���ǲ�        ���
***************************************************************************/
CommandLineParse::~CommandLineParse()
{
}

/***************************************************************************
* �������ƣ�   ParseCommandLine
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  int argc
* �Ρ�������   [in]  char * * argv
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/12      ���ǲ�        ���
***************************************************************************/
void CommandLineParse::ParseCommandLine(int argc, char** argv)
{
	m_commandLine.clear();

	// �����ݷ��õ����ǵ�������ȥ
	for (int i = 0; i < argc; i++)
	{
		m_commandLine.push_back(argv[i]);
	}
}


/***************************************************************************
* �������ƣ�   IsSet
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const char * command
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/12      ���ǲ�        ���
***************************************************************************/
bool CommandLineParse::IsSet(const char* command)
{
	for (int i = 0; i < (int)m_commandLine.size(); i++)
	{
		// ˵������ȵ�
		if (!strcmp(command, m_commandLine.at(i)))
			return true;
	}

	return false;
}

/***************************************************************************
* �������ƣ�   Value
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  const char * command
* ����ֵ����   char*
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2017/01/12      ���ǲ�        ���
***************************************************************************/
char* CommandLineParse::Value(const char* command)
{
	// ˵��û������
	if (!IsSet(command))
		return NULL;

	for (int i = 0; i < (int)m_commandLine.size(); i++)
	{
		// ˵������ȵ�
		if (!strcmp(command, m_commandLine.at(i)))
		{
			// ˵����һλ�������ݵ�
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