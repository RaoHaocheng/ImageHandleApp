#ifndef FINDCIRCLEWIN_H
#define FINDCIRCLEWIN_H

#include <QtWidgets/QMainWindow>
#include "ui_findcircleapp.h"
#include "paintWidget.h"

// 默认窗口大小
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

class FindCircleWin : public QMainWindow
{
	Q_OBJECT

public:
	FindCircleWin(QWidget *parent = 0);
	~FindCircleWin();

private slots:
	void openFile(const QString &path = QString());
	void saveFile(const QString &path = QString());
	void clearImg();

	void findCircleCIC();

private:
	void iniUi();

private:
	PaintWidget* m_paintWidget;
	QString m_currentPath;

};

#endif // FINDCIRCLEAPP_H
