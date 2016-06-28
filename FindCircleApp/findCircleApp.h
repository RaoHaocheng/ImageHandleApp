#ifndef FINDCIRCLEAPP_H
#define FINDCIRCLEAPP_H

#include <QtWidgets/QApplication>
#include <QtGui/QIcon>

class FindCircleWin;

class FindCircleApp : public QApplication
{
	Q_OBJECT

public:
	FindCircleApp(int &argc, char **argv);
	~FindCircleApp();

public slots:
	FindCircleWin* newMainWindow();


private:
	FindCircleWin* m_mainWin;

};


#endif // !FINDCIRCLEAPP_H