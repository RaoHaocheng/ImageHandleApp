#include "findCircleWin.h"

#include <QDesktopWidget>
#include <QtWidgets>
#include <QImage>
#include <vector>
#include "../Communications/communications.h"
#include "../Communications/ShareMemory.h"
#pragma comment(lib,"communicationsd.lib")


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
	openImgFile(path);
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
	saveImgFile(path);
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

	// 基本的功能，比如返回上一步和下一步
	QMenu *operatorMenu = new QMenu(QStringLiteral("图片操作"), this);
	QAction *goBackAction = operatorMenu->addAction(QStringLiteral("撤销"));
	QAction *goFrontAction = operatorMenu->addAction(QStringLiteral("向后"));
	menuBar()->addMenu(operatorMenu);
	connect(goBackAction, SIGNAL(triggered()), this, SLOT(goBack()));
	connect(goFrontAction, SIGNAL(triggered()), this, SLOT(goFront()));

	// 初始化找圆方法函数
	QMenu *findCircleMenu = new QMenu(QStringLiteral("圆形定位方法"), this);
	QAction *CICAction = findCircleMenu->addAction(QStringLiteral("CIC"));
	QAction *iTCiDAction = findCircleMenu->addAction(QStringLiteral("iTCiD"));
	QAction *findCircleThresholdAction = findCircleMenu->addAction(QStringLiteral("threshold"));

	menuBar()->addMenu(findCircleMenu);
	connect(CICAction, SIGNAL(triggered()), this, SLOT(findCircleCIC()));
	connect(iTCiDAction, SIGNAL(triggered()), this, SLOT(findCircleiTCiD()));
	connect(findCircleThresholdAction, SIGNAL(triggered()), this, SLOT(findCircleThreadshold()));

	// 图像处理
	QMenu *imgHandleMenu = new QMenu(QStringLiteral("图片处理方法"), this);
	QAction *gaussianAction = imgHandleMenu->addAction(QStringLiteral("高斯噪声"));
	QAction *autoCannyAction = imgHandleMenu->addAction(QStringLiteral("自动化Canny"));
	QAction *batCannyAction = imgHandleMenu->addAction(QStringLiteral("Canny批处理"));
	QAction *drawCircleAction = imgHandleMenu->addAction(QStringLiteral("画圆"));
	QAction *thresholdAction = imgHandleMenu->addAction(QStringLiteral("阈值分割"));
	QAction *batAction = imgHandleMenu->addAction(QStringLiteral("批处理"));
	QAction *edpfAction = imgHandleMenu->addAction(QStringLiteral("edpf"));
	QAction *batEdpfAction = imgHandleMenu->addAction(QStringLiteral("edpf批处理"));
	QAction *drawConnectedDomainAction = imgHandleMenu->addAction(QStringLiteral("描绘连通域"));
	
	menuBar()->addMenu(imgHandleMenu);
	connect(gaussianAction, SIGNAL(triggered()), this, SLOT(addGaussianNoise()));
	connect(autoCannyAction, SIGNAL(triggered()), this, SLOT(autoCanny()));
	connect(drawCircleAction, SIGNAL(triggered()), this, SLOT(drawCircle()));
	connect(thresholdAction, SIGNAL(triggered()), this, SLOT(threadshold()));
	connect(batAction, SIGNAL(triggered()), this, SLOT(batImgHandle()));
	connect(batCannyAction, SIGNAL(triggered()), this, SLOT(batCannyImgHandle()));
	connect(edpfAction, SIGNAL(triggered()), this, SLOT(edpf()));
	connect(batEdpfAction, SIGNAL(triggered()), this, SLOT(batEdpf()));
	connect(drawConnectedDomainAction, SIGNAL(triggered()), this, SLOT(drawConnectedDomain()));


	// 测试按钮
	QMenu *testMenu = new QMenu(QStringLiteral("测试"), this);
	QAction *testAction = testMenu->addAction(QStringLiteral("测试"));

	menuBar()->addMenu(testMenu);
	connect(testAction, SIGNAL(triggered()), this, SLOT(test()));

	// 右键菜单
	m_popMenu = new QMenu(this); //popMenu为类私有成员
	goBackAction = m_popMenu->addAction(QStringLiteral("撤销"));
	connect(goBackAction, SIGNAL(triggered(bool)), this, SLOT(goBack()));
	goFrontAction = m_popMenu->addAction(QStringLiteral("重做"));
	connect(goFrontAction, SIGNAL(triggered(bool)), this, SLOT(goFront()));

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(sltShowPopMenu(const QPoint&)));

}

/***************************************************************************
* 函数名称：   drawConnectedDomain
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/22      饶智博        添加
***************************************************************************/
void FindCircleWin::drawConnectedDomain()
{
	FindContoursMethod f;
	QImage img = m_paintWidget->Img();
	cv::Mat src = QImage2Mat(img);

	cv::Mat dst;
	int iTotal;
	f.findConnectedDomain(src, dst, iTotal);

	img = Mat2QImage(dst);
	m_paintWidget->setImg(img);
}

/***************************************************************************
* 函数名称：   sltShowPopMenu
* 摘　　要：   
* 全局影响：   private 
* 参　　数：   [in]  const QPoint & point
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/04      饶智博        添加
***************************************************************************/
void FindCircleWin::sltShowPopMenu(const QPoint& point)
{
	if (m_popMenu){
		m_popMenu->exec(QCursor::pos());
	}
}

/***************************************************************************
* 函数名称：   test
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/04      饶智博        添加
***************************************************************************/
void FindCircleWin::test()
{
	LogHandler::SetLogHandlerState(false);
	Communications* c = Communications::CreateCommunications(COMMUNICATIONS_TYPE::TCP);

	PST_IP_INFO ipInfo = new ST_IP_INFO;
	UINT i;
	size_t frameNum = 0;
	c->InitCom();
	c->GetIpAddr(ipInfo, i);

	char dst[4];
	dst[0] = 115;
	dst[1] = 28;
	dst[2] = 14;
	dst[3] = 79;
	char buf[5069];
	if (!c->SetSender(ipInfo->abyIPAddr, 6000))
	{
		std::cout << "set Sender fail!";
	}
	while (true)
	{
		memset(buf, 0, sizeof(buf));
		sprintf_s(buf, 5069, "Given the complex background, small circular marker, and slow and inaccurate locating in measuring the pyramid angle of helicopter blades, a new locating method for the circular marker is proposed.First, the Edge Drawing Parameter - Free Edge Segment Detector Algorithm is adopted to inspect the image's edge and to extract the connected domain of the edge image.Second, the circularity, inertia, and convexity of the connected domain are employed to lter the non - circular contour from the connected domain in the image.Finally, least - square circle tting is used to complete the accurate locating of the circular marker.The simulation and real experiments show that the circular markers can be inspected and located with the proposed algorithm in complex scenes with high speed and good accuracy.In addition, the proposed algorithm had strong anti - interference performance and could be used to improve the speed and accuracy of measurement for the helicopter blades pyramid angle against a complex background.");
		c->Send((BYTE*)dst, 6000, (BYTE*)buf, 5069, frameNum);
		frameNum++;
		Sleep(1000);
		//WARNING_LOG(buf);
		//std::cout << buf;
	}
	    
	return;

	//////////////////////////////
// 	LogHandler::SetLogHandlerState(false);
// 	Communications* c = Communications::CreateCommunications(UDP);
// 	char* ipChar = "127.168.12.1";
// 	char ipchar[16];
// 	BYTE ip[4];
// 	PST_IP_INFO ipInfo = new ST_IP_INFO;
// 	UINT i;
// 	c->InitCom();
// 	c->GetIpAddr(ipInfo, i);
// 	c->GetIpInfo(ipInfo);
// 	WARNING_LOG((char*)ipInfo->abyIPAddr);
// 	ipInfo->abyGateWay;
// 	c->IPStringtoByteArray((char*)&ipInfo->abyIPAddr, ip);
// 	c->ByteIpToStringIp(ip, ipchar);
// 
// 	return;
// 
// 	ShareMemory* sharemem = new ShareMemory();
// 	//sharemem.WriteMem((BYTE*)"sa", sizeof("sa"));
// 	char buf[255];
// 	bool ok;
// 	ok = sharemem->OpenShareMemory("hello");
// 	int x=0;
// 	while (true)
// 	{
// 		x++;
// 		if (ok)
// 			sharemem->ReadMem((BYTE*)buf, 255);
// 		else
// 		{
// 			sprintf_s(buf, 255, "%d", x);
// 			sharemem->WriteMem((BYTE*)buf, 255);
// 		}
// 
// 		WARNING_LOG(buf);
// 	}
// 
// 	delete sharemem;
// 	return;
// 
// 	//"LogFile.log", "D:/Program/ImageHandleApp/x64/Debug/"
// 	//LogHandler::SetFilePath();
// 	LogHandler::SetWinLogHandlerState(false);
// 	WARNING_LOG("%s 1%d 2%f 3%c 4%x 5%o 6%%\n", "hello", 10, 28.888, 'm', 50, 16);
// 	LogHandler::Warning("%s 1%d 2%f 3%c 4%x 5%o 6%%\n", "hello", 10, 28.888, 'm', 50, 16);
// 	LogHandler::Error("%s 1%d 2%f 3%c 4%x 5%o 6%%", "hello", 10, 28.888, 'm', 50, 16);
// 	LogHandler::Info("%s 1%d 2%f 3%c 4%x 5%o 6%%\n", "hello", 10, 28.888, 'm', 50, 16);
// 	LogHandler::Debug("%s 1%d 2%f 3%c 4%x 5%o 6%%", "hello", 10, 28.888, 'm', 50, 16);
// 
// 
// 	return;
// 
//  	FindCircularMarker f;
//  	QImage img = m_paintWidget->Img();
//  	//IplImage pImage = QImage2cvIplImage(img);
//  	cv::Mat src = QImage2Mat(img);
//  
//  	cv::Mat dst;
//  	//f.findContoursByEDPF(src, dst, 1.0);
//  	//f.findContoursByED(src, dst);
// 	std::vector<ST_CENTER> centers;
// 	ST_CENTER center;
// 	center.location.x = 436;
// 	center.location.y = 350;
// 	center.radius = 200;
// 	centers.push_back(center);
// 	f.AccuaryCircleLocation(src, centers);
// 	//f.findContoursByCannyLine(src, dst);
//  
//  	img = Mat2QImage(dst);
// 	m_paintWidget->setImg(img);
}

/***************************************************************************
* 函数名称：   goBack
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/04      饶智博        添加
***************************************************************************/
void FindCircleWin::goBack()
{
	m_paintWidget->goBack();
}

/***************************************************************************
* 函数名称：   goFront
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/04      饶智博        添加
***************************************************************************/
void FindCircleWin::goFront()
{
	m_paintWidget->goFront();
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
void FindCircleWin::findCircleiTCiD()
{
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
	params.minArea = 10;
//    	params.minConvexity = 0.8f;
//    	params.minInertiaRatio = 0.73f;
//    	params.minCircularity = 0.8f;
 	params.minConvexity = 0.78f;
 	params.minInertiaRatio = 0.5f;
 	params.minCircularity = 0.7f;
 	params.maxArea = 10000000000;
 	params.blobColor = 0;
 	params.maxThreshold = 65;
 	params.maxConvexity = 1.0f;
 	params.maxCircularity = 1.0f;
 	params.maxInertiaRatio = 1.0f;
	params.minDistBetweenBlobs = 2; 

	FindCircularMarker f;
	cv::Mat keypointsImage;
	
	//f.FindCircle(cv::Mat(&pImage), params, centers);
	f.FindCircleByCICImproved(cv::Mat(&pImage), params, centers, 1.0, false);
	//f.FindCircleByCIC(cv::Mat(&pImage), params, centers, 1.0, false);
	//f.FindCircleByThreshold(cv::Mat(&pImage), params, centers, false);
	DrawCircle(cv::Mat(&pImage), keypointsImage, centers);

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
// 	QSize winSize = this->size();
// 	
// 	if (size.width() > winSize.width())
// 	{
// 		m_horizontalScroll->setMaximum(size.width()-winSize.width());
// 		m_horizontalScroll->setMinimum(0);
// 		m_horizontalScroll->setPageStep(size.width());
// 		m_horizontalScroll->show();
// 	}
// 	else
// 	{
// 		m_horizontalScroll->hide();
// 	}
// 
// 	if (size.height() > winSize.height())
// 	{
// 		m_verticalScroll->setMaximum(size.height() - winSize.height());
// 		m_verticalScroll->setMinimum(0);
// 		m_verticalScroll->setPageStep(size.height() );
// 		m_verticalScroll->show();
// 	}
// 	else
// 	{
// 		m_verticalScroll->hide();
// 	}
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
// 	// 使用过的控件重新布局
// 	const int SCROLL_WIDGET_WIDTH = 20;
// 	m_verticalScroll->setGeometry(this->width() - SCROLL_WIDGET_WIDTH, 
// 		SCROLL_WIDGET_WIDTH, SCROLL_WIDGET_WIDTH, this->height() - SCROLL_WIDGET_WIDTH);
// 	m_horizontalScroll->setGeometry(0, this->height() - SCROLL_WIDGET_WIDTH, 
// 		this->width() - SCROLL_WIDGET_WIDTH, SCROLL_WIDGET_WIDTH);
// 
// 	// 改变了窗口后，在检查其中的中间是否有超过的问题
// 	m_paintWidget->checkSize(this->size());
// 
// 	event->accept();
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
	//m_paintWidget->setImgPosX(-vaule);
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
//	m_paintWidget->setImgPosY(-vaule);
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

/***************************************************************************
* 函数名称：   threadshold
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/10/12      饶智博        添加
***************************************************************************/
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


/***************************************************************************
* 函数名称：   findCircleThreadshold
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/02      饶智博        添加
***************************************************************************/
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

/***************************************************************************
* 函数名称：   batImgHandle
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/10/12      饶智博        添加
***************************************************************************/
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

/***************************************************************************
* 函数名称：   batCannyImgHandle
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/02      饶智博        添加
***************************************************************************/
void FindCircleWin::batCannyImgHandle()
{
	QString fileName;
	QString openFilePath = "C:/Users/haocheng/Desktop/source/";
	QString saveFilePath = "C:/Users/haocheng/Desktop/result/";

	for (int i = 1; i <= 300; i++)
	{
		// 做V的
		fileName = QString("(%1).jpg").arg(i);
		openFile(openFilePath + fileName);
		autoCanny();
		saveFile(saveFilePath + fileName);
	}
}


/***************************************************************************
* 函数名称：   batEdpf
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/02      饶智博        添加
***************************************************************************/
void FindCircleWin::batEdpf()
{
	QString fileName;
	QString openFilePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/source/";//"C:/Users/haocheng/Desktop/source/";
	QString saveFilePath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/result/";

	for (int i = 1;; i++)
	{
		// 做V的
		fileName = QString("(%1).jpg").arg(i);
 		if (!openImgFile(openFilePath + fileName))
 			break;
		
		edpf();
		saveImgFile(saveFilePath + fileName);
	}
}


/***************************************************************************
* 函数名称：   edpf
* 摘　　要：   
* 全局影响：   private 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/02      饶智博        添加
***************************************************************************/
void FindCircleWin::edpf()
{
 	FindContoursMethod f;
	QImage img = m_paintWidget->Img();
	//IplImage pImage = QImage2cvIplImage(img);
	cv::Mat src = QImage2Mat(img);

	cv::Mat dst;
	f.findContoursByEDPF(src, dst, 1.0);
	//f.findContoursByED(&pImage, dst);

	img = Mat2QImage(dst);
	m_paintWidget->setImg(img);
}

/***************************************************************************
* 函数名称：   openImgFile
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  const QString & path
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/02      饶智博        添加
***************************************************************************/
bool FindCircleWin::openImgFile(const QString &path /* = QString() */)
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
			return false;
		}

		m_currentPath = fileName;
		QImage img(fileName);
		m_paintWidget->setImg(img);
		return true;
	}
 	else
 		return false;
}

/***************************************************************************
* 函数名称：   saveImgFile
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  const QString & path
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/02      饶智博        添加
***************************************************************************/
bool FindCircleWin::saveImgFile(const QString &path /* = QString() */)
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
			return false;
		}

		QImage img = m_paintWidget->Img();
		img.save(fileName);
		return true;
	}

	return false;
}