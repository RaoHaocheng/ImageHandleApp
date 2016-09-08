#include "findCircleWin.h"

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
	: QMainWindow(parent)
	, m_paintWidget(nullptr)
	, m_verticalScroll(nullptr)
	, m_horizontalScroll(nullptr)
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

	if (nullptr != m_verticalScroll)
		delete m_verticalScroll;
	m_verticalScroll = nullptr;

	if (nullptr != m_horizontalScroll)
		delete m_horizontalScroll;
	m_horizontalScroll = nullptr;
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
		m_currentPath, "Image files (*.jpg *.bmp *.png)");
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
	QString fileName;
	if (path.isNull())
		fileName = QFileDialog::getSaveFileName(this, QStringLiteral("保存图片文件"),
		m_currentPath, "Image files (*.jpg *.bmp *.png)");
	else
		fileName = path;

	if (!fileName.isEmpty()) {
		QFile file(fileName);
		if (file.exists()) {
			QMessageBox::critical(this, QStringLiteral("保存图片文件"),
				QStringLiteral("保存图片文件") +
				QString("'%1'").arg(fileName) + QStringLiteral("失败！"));
			return;
		}

		QImage img = m_paintWidget->Img();
		img.save(fileName);
	}
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
	connect(m_paintWidget, SIGNAL(reSize(QSize)), this, SLOT(checkSize(QSize)));
	setCentralWidget(m_paintWidget);

	// 初始化滚动条
	m_verticalScroll = new QScrollBar(Qt::Vertical, this);
	m_horizontalScroll = new QScrollBar(Qt::Horizontal, this);
	connect(m_verticalScroll, SIGNAL(valueChanged(int)), this, SLOT(changeVScrollVaule(int)));
	connect(m_horizontalScroll, SIGNAL(valueChanged(int)), this, SLOT(changeHScrollVaule(int)));
	m_verticalScroll->hide();
	m_horizontalScroll->hide();

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
	QMenu *findCircleMenu = new QMenu(QStringLiteral("圆形定位方法"), this);
	QAction *CICAction = findCircleMenu->addAction(QStringLiteral("CIC"));
	QAction *EDPFAction = findCircleMenu->addAction(QStringLiteral("iTCiD"));
	QAction *findCircleThresholdAction = findCircleMenu->addAction(QStringLiteral("threshold"));

	menuBar()->addMenu(findCircleMenu);
	connect(CICAction, SIGNAL(triggered()), this, SLOT(findCircleCIC()));
	connect(EDPFAction, SIGNAL(triggered()), this, SLOT(findCircleEDPF()));
	connect(findCircleThresholdAction, SIGNAL(triggered()), this, SLOT(findCircleThreadshold()));

	// 图像处理
	QMenu *imgHandleMenu = new QMenu(QStringLiteral("图片处理方法"), this);
	QAction *gaussianAction = imgHandleMenu->addAction(QStringLiteral("高斯噪声"));
	QAction *autoCannyAction = imgHandleMenu->addAction(QStringLiteral("自动化Canny"));
	QAction *drawCircleAction = imgHandleMenu->addAction(QStringLiteral("画圆"));
	QAction *thresholdAction = imgHandleMenu->addAction(QStringLiteral("阈值分割"));
	QAction *batAction = imgHandleMenu->addAction(QStringLiteral("批处理"));

	menuBar()->addMenu(imgHandleMenu);
	connect(gaussianAction, SIGNAL(triggered()), this, SLOT(addGaussianNoise()));
	connect(autoCannyAction, SIGNAL(triggered()), this, SLOT(autoCanny()));
	connect(drawCircleAction, SIGNAL(triggered()), this, SLOT(drawCircle()));
	connect(thresholdAction, SIGNAL(triggered()), this, SLOT(threadshold()));
	connect(batAction, SIGNAL(triggered()), this, SLOT(batImgHandle()));
}

/***************************************************************************
* 函数名称：   findCircleEDPF
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/28      饶智博        添加
***************************************************************************/
void FindCircleWin::findCircleEDPF()
{
// 	FindContoursMethod f;
// 	QImage img = m_paintWidget->Img();
// 	IplImage pImage = QImage2cvIplImage(img);
// 	cv::Mat dst;
	//f.findContoursByEDPF(&pImage, dst);


	QImage img = m_paintWidget->Img();
	IplImage pImage = QImage2cvIplImage(img);

	FindCircularMarker f;
	cv::Mat keypointsImage;
	std::vector<ST_CENTER> centers;

	f.FindCircleBySamplingTriangles(&pImage, centers, false);
	DrawCircle(cv::Mat(&pImage), keypointsImage, centers);

	img = Mat2QImage(keypointsImage);
	m_paintWidget->setImg(img);
}


/***************************************************************************
* 函数名称：   findCircleCIC
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/28      饶智博        添加
***************************************************************************/
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
//    	params.minConvexity = 0.8f;
//    	params.minInertiaRatio = 0.73f;
//    	params.minCircularity = 0.8f;
 	params.minConvexity = 0.82f;
 	params.minInertiaRatio = 0.70f;
 	params.minCircularity = 0.75f;
 	params.maxArea = 10000000000;
 	params.blobColor = 0;
 	params.maxThreshold = 65;
 	params.maxConvexity = 1.2f;
 	params.maxCircularity = 1.2f;
 	params.maxInertiaRatio = 1.2f;
	params.minDistBetweenBlobs = 1; 

	FindCircularMarker f;
	cv::Mat keypointsImage;
	

	f.FindCircleByCICImproved(cv::Mat(&pImage), params, centers, false);
	//f.FindCircleByThreshold(cv::Mat(&pImage), params, centers, false);
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

/***************************************************************************
* 函数名称：   checkSize
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  QSize size
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/02      饶智博        添加
***************************************************************************/
void FindCircleWin::checkSize(QSize size)
{
	QSize winSize = this->size();
	
	if (size.width() > winSize.width())
	{
		m_horizontalScroll->setMaximum(size.width()-winSize.width());
		m_horizontalScroll->setMinimum(0);
		m_horizontalScroll->setPageStep(size.width());
		m_horizontalScroll->show();
	}
	else
	{
		m_horizontalScroll->hide();
	}

	if (size.height() > winSize.height())
	{
		m_verticalScroll->setMaximum(size.height() - winSize.height());
		m_verticalScroll->setMinimum(0);
		m_verticalScroll->setPageStep(size.height() );
		m_verticalScroll->show();
	}
	else
	{
		m_verticalScroll->hide();
	}
}


/***************************************************************************
* 函数名称：   resizeEvent
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  QResizeEvent * event
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/02      饶智博        添加
***************************************************************************/
void FindCircleWin::resizeEvent(QResizeEvent *event)
{
	// 使用过的控件重新布局
	const int SCROLL_WIDGET_WIDTH = 20;
	m_verticalScroll->setGeometry(this->width() - SCROLL_WIDGET_WIDTH, 
		SCROLL_WIDGET_WIDTH, SCROLL_WIDGET_WIDTH, this->height() - SCROLL_WIDGET_WIDTH);
	m_horizontalScroll->setGeometry(0, this->height() - SCROLL_WIDGET_WIDTH, 
		this->width() - SCROLL_WIDGET_WIDTH, SCROLL_WIDGET_WIDTH);

	// 改变了窗口后，在检查其中的中间是否有超过的问题
	m_paintWidget->checkSize(this->size());

	event->accept();
}

/***************************************************************************
* 函数名称：   changeHScrollVaule
* 摘　　要：   
* 全局影响：   private 
* 参　　数：   [in]  int vaule
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/23      饶智博        添加
***************************************************************************/
void FindCircleWin::changeHScrollVaule(int vaule)
{
	m_paintWidget->setImgPosX(-vaule);
}

/***************************************************************************
* 函数名称：   changeVScrollVaule
* 摘　　要：   
* 全局影响：   private 
* 参　　数：   [in]  int vaule
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/23      饶智博        添加
***************************************************************************/
void FindCircleWin::changeVScrollVaule(int vaule)
{
	m_paintWidget->setImgPosY(-vaule);
}

/***************************************************************************
* 函数名称：   addGaussianNoise
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/23      饶智博        添加
***************************************************************************/
void FindCircleWin::addGaussianNoise()
{
	QImage img = m_paintWidget->Img();
	IplImage pImage = QImage2cvIplImage(img);

	IplImage* dst = AddGaussianNoise(&pImage, 0, 0.02);

	img = IplImage2QImage(*dst);
	m_paintWidget->setImg(img);
}

/***************************************************************************
* 函数名称：   autoCanny
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/28      饶智博        添加
***************************************************************************/
void FindCircleWin::autoCanny()
{
	QImage img = m_paintWidget->Img();
	IplImage pImage = QImage2cvIplImage(img);
	cv::Mat dst;

	AutoCanny(cv::Mat(&pImage), dst);


	img = Mat2QImage(dst);
	m_paintWidget->setImg(img);

}

/***************************************************************************
* 函数名称：   drawCircle
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/08/23      饶智博        添加
***************************************************************************/
void FindCircleWin::drawCircle()
{
	QImage img = m_paintWidget->Img();
	IplImage pImage = QImage2cvIplImage(img);

	cv::Mat keypointsImage;
	DrawCircle(cv::Mat(&pImage), keypointsImage, centers);
	//cvtColor(cv::Mat(&pImage), keypointsImage, CV_GRAY2RGB);
//	cv::threshold(cv::Mat(&pImage), keypointsImage, 40, 255, cv::THRESH_BINARY);


	img = Mat2QImage(keypointsImage);
	m_paintWidget->setImg(img);
}

void FindCircleWin::threadshold()
{
	QImage img = m_paintWidget->Img();
	IplImage pImage = QImage2cvIplImage(img);

	cv::Mat keypointsImage;
	int ithreadshold = Otsu(&pImage);
	cv::threshold(cv::Mat(&pImage), keypointsImage, ithreadshold, 255, cv::THRESH_BINARY);

	img = Mat2QImage(keypointsImage);
	m_paintWidget->setImg(img);
}


void FindCircleWin::findCircleThreadshold()
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
	params.minArea = 200;
	//   params.minConvexity = 0.8f;
	//   params.minInertiaRatio = 0.73f;
	//   params.minCircularity = 0.8f;
	params.minConvexity = 0.82f;
	params.minInertiaRatio = 0.70f;
	params.minCircularity = 0.75f;
	params.maxArea = 10000000000;
	params.blobColor = 0;
	params.maxThreshold = 65;
	params.maxConvexity = 1.2f;
	params.maxCircularity = 1.2f;
	params.maxInertiaRatio = 1.2f;
	params.minDistBetweenBlobs = 1;

	FindCircularMarker f;
	cv::Mat keypointsImage;

	f.FindCircleByThreadshold(cv::Mat(&pImage), params, centers, FALSE);
	DrawCircle(cv::Mat(&pImage), keypointsImage, centers);

	img = Mat2QImage(keypointsImage);

	m_paintWidget->setImg(img);
}

void FindCircleWin::batImgHandle()
{
	QString fileName;
	QString openFilePath = "C:/Users/haocheng/Desktop/bmp/";
	QString saveFilePath = "C:/Users/haocheng/Desktop/10/";

	QFile file(saveFilePath + "result.txt");
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream result(&file);


	for (int i = 1; i <= 4; i++)
	{
		for (int j = 1; j <= 5; j++)
		{
			// 做V的
			fileName = QString("v%1-%2.bmp").arg(i).arg(j);
			openFile(openFilePath + fileName);
			findCircleThreadshold();
			saveFile(saveFilePath + fileName);

			result << (i-1) * 4 + j << ":" << fileName << " " << "\n";

			double av = 0;
			double errAv = 0;

			// 写文件
			for (int k = 0; k < (int)centers.size(); k++)
			{
				double radius = centers.at(k).radius;
				double ans = 2 * radius*0.0371660830989318;
				double err = ans - 10.4;
				result << radius << " " << ans << " " << err << " " << "\n";
				av = av + ans;
				errAv = errAv + abs(err);
			}
			result << av / (int)centers.size() << " " << errAv / (int)centers.size() << 0 << "\n";


			fileName = QString("h%1-%2.bmp").arg(i).arg(j);
			openFile(openFilePath + fileName);
			findCircleThreadshold();
			saveFile(saveFilePath + fileName);

			av = 0;
			errAv = 0;
			result << (i - 1) * 4 + j << ":" << fileName << " " << "\n";
			for (int k = 0; k < (int)centers.size(); k++)
			{
				double radius = centers.at(k).radius;
				double ans = 2 * radius*0.0371660830989318;
				double err = ans - 10.4;
				result << radius << " " << ans << " " << err << " " << "\n";
				av = av + ans;
				errAv = errAv + abs(err);
			}
			result << av / (int)centers.size() << " " << errAv / (int)centers.size() << 0<<"\n";
		}
	}

	// 不好的图片
  	for (int i = 1; i <= 3; i++)
  	{
  		fileName = QString("Error_%1.bmp").arg(i);
  		openFile(openFilePath + fileName);
  		findCircleThreadshold();
  		saveFile(saveFilePath + fileName);
  
  		result << i<< fileName << " " << "\n";
  		for (int k = 0; k < (int)centers.size(); k++)
  		{
  			double radius = centers.at(k).radius;
  			double ans = 2 * radius*0.0371660830989318;
  			double err = ans - 10.4;
  			result << radius << " " << ans << " " << err << " " << "\n";
  		}
  	}
}