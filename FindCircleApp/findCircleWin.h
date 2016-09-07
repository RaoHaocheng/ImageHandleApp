#ifndef FINDCIRCLEWIN_H
#define FINDCIRCLEWIN_H

#include <QtWidgets/QMainWindow>
#include <QScrollBar>
#include "ui_findcircleapp.h"
#include "paintWidget.h"

#include "cvToQt.h"
#include <vector>

// Ĭ�ϴ��ڴ�С
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

class FindCircleWin : public QMainWindow
{
	Q_OBJECT

public:
	FindCircleWin(QWidget *parent = 0);
	~FindCircleWin();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	void iniUi();

protected slots:
	void checkSize(QSize);

private slots:
	void openFile(const QString &path = QString());
	void saveFile(const QString &path = QString());
	void clearImg();
	void findCircleCIC();
	void findCircleThreadshold();
	void findCircleEDPF();
	void changeHScrollVaule(int);
	void changeVScrollVaule(int);
	void addGaussianNoise();
	void autoCanny();
	void drawCircle();
	void threadshold();
	void batImgHandle();      // ��Ҫ�޸ģ�Ŀǰ�ù��ܽ����ڲ���

private:

private:
	PaintWidget* m_paintWidget;
	QString m_currentPath;
	QScrollBar* m_verticalScroll;
	QScrollBar* m_horizontalScroll;
	std::vector<ST_CENTER> centers;
};

#endif // FINDCIRCLEAPP_H
