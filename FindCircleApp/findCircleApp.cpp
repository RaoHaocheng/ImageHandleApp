#include "findCircleApp.h"

#include "findCircleWin.h"


FindCircleApp::FindCircleApp(int &argc, char **argv)
 : QApplication(argc, argv)
 , m_mainWin(nullptr)
{
	//³õÊ¼»¯Ãû×Ö
	QCoreApplication::setOrganizationName(QLatin1String("NCHU"));
	QCoreApplication::setApplicationName(QLatin1String("FindCircleApp"));
	QCoreApplication::setApplicationVersion(QLatin1String("0.0"));
}

FindCircleApp::~FindCircleApp()
{
	if (nullptr != m_mainWin)
		delete m_mainWin;
	m_mainWin = nullptr;
}


FindCircleWin* FindCircleApp::newMainWindow()
{
	if (nullptr != m_mainWin)
		delete m_mainWin;
	m_mainWin = new FindCircleWin();

	m_mainWin->show();

	return m_mainWin;
}


