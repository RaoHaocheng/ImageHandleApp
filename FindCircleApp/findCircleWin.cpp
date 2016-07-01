#include "findCircleWin.h"

#include "cvToQt.h"

#include <QDesktopWidget>
#include <QtWidgets>
#include <QImage>
#include <vector>


/***************************************************************************
* 函数名称：   FindCircleWin
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QWidget * parent
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
FindCircleWin::FindCircleWin(QWidget *parent)
	: QMainWindow(parent),
	m_paintWidget(nullptr)
{
	// 初始化界面
	iniUi();
}

/***************************************************************************
* 函数名称：   ~FindCircleWin
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
FindCircleWin::~FindCircleWin()
{
	if (nullptr != m_paintWidget)
		delete m_paintWidget;

	m_paintWidget = nullptr;
}

/***************************************************************************
* 函数名称：   openFile
* 摘　　要：   
* 全局影响：   private 
* 参　　数：   [in]  const QString & path
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void FindCircleWin::openFile(const QString &path)
{
	QString fileName;
	if (path.isNull())
		fileName = QFileDialog::getOpenFileName(this, QStringLiteral("打开图片文件"),
		m_currentPath, "SVG files (*.jpg *.bmp *.png)");
	else
		fileName = path;

	if (!fileName.isEmpty()) {
		QFile file(fileName);
		if (!file.exists()) {
			QMessageBox::critical(this, QStringLiteral("打开图片文件"),
				QStringLiteral("打开图片文件") +
				QString("'%1'").arg(fileName) + QStringLiteral("失败！"));
			return;
		}

		QImage img(fileName);
		m_paintWidget->setImg(img);
	}
}

/***************************************************************************
* 函数名称：   saveFile
* 摘　　要：   
* 全局影响：   private 
* 参　　数：   [in]  const QString & path
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void FindCircleWin::saveFile(const QString &path)
{

}

/***************************************************************************
* 函数名称：   clearImg
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void FindCircleWin::clearImg()
{
	m_paintWidget->clearPaintWidget();
	m_currentPath = "";
}

/***************************************************************************
* 函数名称：   iniUi
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void FindCircleWin::iniUi()
{
	// 控制界面大小，并使其居中
	setGeometry((QApplication::desktop()->width() - WIN_WIDTH) / 2, 
		(QApplication::desktop()->height() - WIN_HEIGHT) / 2, WIN_WIDTH, WIN_HEIGHT);

	// 初始化面板
	if (nullptr != m_paintWidget)
		delete m_paintWidget;

	m_paintWidget = new PaintWidget(this);
	setCentralWidget(m_paintWidget);

	// 初始化界面
	// 初始化菜单栏,初始化文件菜单栏
	QMenu *fileMenu = new QMenu(QStringLiteral("文件"), this);
	QAction *openAction = fileMenu->addAction(QStringLiteral("打开..."));
	openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
	QAction *saveAction = fileMenu->addAction(QStringLiteral("保存..."));
	saveAction->setShortcut(QKeySequence(tr("Ctrl+S")));
	QAction *clearImgAction = fileMenu->addAction(QStringLiteral("清空..."));
	clearImgAction->setShortcut(QKeySequence(tr("Ctrl+C")));
	QAction *quitAction = fileMenu->addAction(QStringLiteral("退出"));
	quitAction->setShortcuts(QKeySequence::Quit);

	// 连接实现函数函数
	menuBar()->addMenu(fileMenu);
	connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(clearImgAction, SIGNAL(triggered()), this, SLOT(clearImg()));
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	// 初始化找圆方法函数
	QMenu *findCircleMenu = new QMenu(QStringLiteral("找圆"), this);
	QAction *CICAction = findCircleMenu->addAction(QStringLiteral("CIC"));

	menuBar()->addMenu(findCircleMenu);
	connect(CICAction, SIGNAL(triggered()), this, SLOT(findCircleCIC()));
}

void FindCircleWin::findCircleCIC()
{
	QImage img = m_paintWidget->Img();
	IplImage pImage = QImage2cvIplImage(img);

	cv::SimpleBlobDetector::Params params;
	params.filterByArea = true;
	params.filterByCircularity = true;
	params.filterByInertia = true;
	params.filterByConvexity = true;
	params.filterByColor = false;
	params.minThreshold = 5;
	params.thresholdStep = 5;
	params.minArea = 40;
	params.minConvexity = 0.8f;
	params.minInertiaRatio = 0.73f;
	params.minCircularity = 0.8f;
	params.maxArea = 10000000000;
	params.blobColor = 0;
	params.maxThreshold = 65;
	params.maxConvexity = 1.2f;
	params.maxCircularity = 1.2f;
	params.maxInertiaRatio = 1.2f;
	params.minDistBetweenBlobs = 1;

	FindCircularMarker f;
	cv::Mat keypointsImage;
	std::vector<ST_CENTER> centers;

	f.FindCircleByCICImproved(&pImage, params, centers, false);

	DrawCircle(cv::Mat(&pImage), keypointsImage, centers);

// 	cvtColor(cv::Mat(&pImage), keypointsImage, CV_GRAY2RGB);
// 	for (int j = 0; j < (int)centers.size(); j++)
// 	{
// 		ST_CENTER center;
// 		center = centers.at(j);
// 		cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // 画一个圆 
// 		cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // 画一个圆 
// 	}

	img = Mat2QImage(keypointsImage);
	m_paintWidget->setImg(img);
}