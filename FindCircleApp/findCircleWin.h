#ifndef FINDCIRCLEWIN_H
#define FINDCIRCLEWIN_H

#include <QtWidgets/QMainWindow>
#include <QScrollBar>
#include <vector>
#include "ui_findcircleapp.h"
#include "paintWidget.h"
#include "cvToQt.h"
#include "..\\TestTools\\LogHandler.h"


// 默认窗口大小
const int WIN_WIDTH = 800;
const int WIN_HEIGHT = 600;

// 使用的类
class QImage;

class FindCircleWin : public QMainWindow
{
	Q_OBJECT

public:
	FindCircleWin(QWidget *parent = 0);
	virtual ~FindCircleWin();


protected:
	void resizeEvent(QResizeEvent *event);

private:
	void iniUi();

protected slots:
	void checkSize(QSize);
	bool openImgFile(const QString &path = QString());
	bool saveImgFile(const QString &path = QString());

private slots:
	void openFile(const QString &path = QString());
	void saveFile(const QString &path = QString());
	void clearImg();
	void findCircleCIC();
	void findCircleThreadshold();
	void findCircleiTCiD();
	void changeHScrollVaule(int);
	void changeVScrollVaule(int);
	void addGaussianNoise();
	void autoCanny();
	void drawCircle();
	void threadshold();
	void batImgHandle();		   // 需要修改，目前该功能仅用于测试
	void batCannyImgHandle();      // 需要修改，目前该功能仅用于测试
	void edgeDrawing();
	void edpf();
	void batEdpf();                // FIXME：need more para;
	void goBack();
	void goFront();
	void test();
	void sltShowPopMenu(const QPoint&);
	void drawConnectedDomain();

private:
	PaintWidget* m_paintWidget;
	QString m_currentPath;
	QScrollBar* m_verticalScroll;
	QScrollBar* m_horizontalScroll;
	std::vector<ST_CENTER> centers;
	QMenu* m_popMenu;                      // 弹出菜单

};

#endif // FINDCIRCLEAPP_H
