#include "findCircleWin.h"

#include <QDesktopWidget>
#include <QtWidgets>
#include <QImage>
#include <vector>


/***************************************************************************
* �������ƣ�   FindCircleWin
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  QWidget * parent
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
FindCircleWin::FindCircleWin(QWidget *parent)
	: QMainWindow(parent)
	, m_paintWidget(nullptr)
	, m_verticalScroll(nullptr)
	, m_horizontalScroll(nullptr)
{
	// ��ʼ������
	iniUi();
}

/***************************************************************************
* �������ƣ�   ~FindCircleWin
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
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
* �������ƣ�   openFile
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* �Ρ�������   [in]  const QString & path
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void FindCircleWin::openFile(const QString &path)
{
	QString fileName;
	if (path.isNull())
		fileName = QFileDialog::getOpenFileName(this, QStringLiteral("��ͼƬ�ļ�"),
		m_currentPath, "Image files (*.jpg *.bmp *.png)");
	else
		fileName = path;

	if (!fileName.isEmpty()) {
		QFile file(fileName);
		if (!file.exists()) {
			QMessageBox::critical(this, QStringLiteral("��ͼƬ�ļ�"),
				QStringLiteral("��ͼƬ�ļ�") +
				QString("'%1'").arg(fileName) + QStringLiteral("ʧ�ܣ�"));
			return;
		}

		QImage img(fileName);
		m_paintWidget->setImg(img);
	}
}

/***************************************************************************
* �������ƣ�   saveFile
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* �Ρ�������   [in]  const QString & path
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void FindCircleWin::saveFile(const QString &path)
{
	QString fileName;
	if (path.isNull())
		fileName = QFileDialog::getSaveFileName(this, QStringLiteral("����ͼƬ�ļ�"),
		m_currentPath, "Image files (*.jpg *.bmp *.png)");
	else
		fileName = path;

	if (!fileName.isEmpty()) {
		QFile file(fileName);
		if (file.exists()) {
			QMessageBox::critical(this, QStringLiteral("����ͼƬ�ļ�"),
				QStringLiteral("����ͼƬ�ļ�") +
				QString("'%1'").arg(fileName) + QStringLiteral("ʧ�ܣ�"));
			return;
		}

		QImage img = m_paintWidget->Img();
		img.save(fileName);
	}
}

/***************************************************************************
* �������ƣ�   clearImg
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void FindCircleWin::clearImg()
{
	m_paintWidget->clearPaintWidget();
	m_currentPath = "";
}

/***************************************************************************
* �������ƣ�   iniUi
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void FindCircleWin::iniUi()
{
	// ���ƽ����С����ʹ�����
	setGeometry((QApplication::desktop()->width() - WIN_WIDTH) / 2, 
		(QApplication::desktop()->height() - WIN_HEIGHT) / 2, WIN_WIDTH, WIN_HEIGHT);

	// ��ʼ�����
	if (nullptr != m_paintWidget)
		delete m_paintWidget;

	m_paintWidget = new PaintWidget(this);
	connect(m_paintWidget, SIGNAL(reSize(QSize)), this, SLOT(checkSize(QSize)));
	setCentralWidget(m_paintWidget);

	// ��ʼ��������
	m_verticalScroll = new QScrollBar(Qt::Vertical, this);
	m_horizontalScroll = new QScrollBar(Qt::Horizontal, this);
	connect(m_verticalScroll, SIGNAL(valueChanged(int)), this, SLOT(changeVScrollVaule(int)));
	connect(m_horizontalScroll, SIGNAL(valueChanged(int)), this, SLOT(changeHScrollVaule(int)));
	m_verticalScroll->hide();
	m_horizontalScroll->hide();

	// ��ʼ������
	// ��ʼ���˵���,��ʼ���ļ��˵���
	QMenu *fileMenu = new QMenu(QStringLiteral("�ļ�"), this);
	QAction *openAction = fileMenu->addAction(QStringLiteral("��..."));
	openAction->setShortcut(QKeySequence(tr("Ctrl+O")));
	QAction *saveAction = fileMenu->addAction(QStringLiteral("����..."));
	saveAction->setShortcut(QKeySequence(tr("Ctrl+S")));
	QAction *clearImgAction = fileMenu->addAction(QStringLiteral("���..."));
	clearImgAction->setShortcut(QKeySequence(tr("Ctrl+C")));
	QAction *quitAction = fileMenu->addAction(QStringLiteral("�˳�"));
	quitAction->setShortcuts(QKeySequence::Quit);

	// ����ʵ�ֺ�������
	menuBar()->addMenu(fileMenu);
	connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
	connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
	connect(clearImgAction, SIGNAL(triggered()), this, SLOT(clearImg()));
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	// ��ʼ����Բ��������
	QMenu *findCircleMenu = new QMenu(QStringLiteral("Բ�ζ�λ����"), this);
	QAction *CICAction = findCircleMenu->addAction(QStringLiteral("CIC"));
	QAction *EDPFAction = findCircleMenu->addAction(QStringLiteral("iTCiD"));
	QAction *findCircleThresholdAction = findCircleMenu->addAction(QStringLiteral("threshold"));

	menuBar()->addMenu(findCircleMenu);
	connect(CICAction, SIGNAL(triggered()), this, SLOT(findCircleCIC()));
	connect(EDPFAction, SIGNAL(triggered()), this, SLOT(findCircleEDPF()));
	connect(findCircleThresholdAction, SIGNAL(triggered()), this, SLOT(findCircleThreadshold()));

	// ͼ����
	QMenu *imgHandleMenu = new QMenu(QStringLiteral("ͼƬ������"), this);
	QAction *gaussianAction = imgHandleMenu->addAction(QStringLiteral("��˹����"));
	QAction *autoCannyAction = imgHandleMenu->addAction(QStringLiteral("�Զ���Canny"));
	QAction *drawCircleAction = imgHandleMenu->addAction(QStringLiteral("��Բ"));
	QAction *thresholdAction = imgHandleMenu->addAction(QStringLiteral("��ֵ�ָ�"));
	QAction *batAction = imgHandleMenu->addAction(QStringLiteral("������"));

	menuBar()->addMenu(imgHandleMenu);
	connect(gaussianAction, SIGNAL(triggered()), this, SLOT(addGaussianNoise()));
	connect(autoCannyAction, SIGNAL(triggered()), this, SLOT(autoCanny()));
	connect(drawCircleAction, SIGNAL(triggered()), this, SLOT(drawCircle()));
	connect(thresholdAction, SIGNAL(triggered()), this, SLOT(threadshold()));
	connect(batAction, SIGNAL(triggered()), this, SLOT(batImgHandle()));
}

/***************************************************************************
* �������ƣ�   findCircleEDPF
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/28      ���ǲ�        ���
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
* �������ƣ�   findCircleCIC
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/28      ���ǲ�        ���
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
// 		cv::circle(keypointsImage, center.location, (int)center.radius, cv::Scalar(0, 0, 255)); // ��һ��Բ 
// 		cv::circle(keypointsImage, center.location, (int)1, cv::Scalar(0, 255, 255)); // ��һ��Բ 
// 	}

	img = Mat2QImage(keypointsImage);
	m_paintWidget->setImg(img);
}

/***************************************************************************
* �������ƣ�   checkSize
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  QSize size
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/02      ���ǲ�        ���
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
* �������ƣ�   resizeEvent
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  QResizeEvent * event
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/02      ���ǲ�        ���
***************************************************************************/
void FindCircleWin::resizeEvent(QResizeEvent *event)
{
	// ʹ�ù��Ŀؼ����²���
	const int SCROLL_WIDGET_WIDTH = 20;
	m_verticalScroll->setGeometry(this->width() - SCROLL_WIDGET_WIDTH, 
		SCROLL_WIDGET_WIDTH, SCROLL_WIDGET_WIDTH, this->height() - SCROLL_WIDGET_WIDTH);
	m_horizontalScroll->setGeometry(0, this->height() - SCROLL_WIDGET_WIDTH, 
		this->width() - SCROLL_WIDGET_WIDTH, SCROLL_WIDGET_WIDTH);

	// �ı��˴��ں��ڼ�����е��м��Ƿ��г���������
	m_paintWidget->checkSize(this->size());

	event->accept();
}

/***************************************************************************
* �������ƣ�   changeHScrollVaule
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* �Ρ�������   [in]  int vaule
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/23      ���ǲ�        ���
***************************************************************************/
void FindCircleWin::changeHScrollVaule(int vaule)
{
	m_paintWidget->setImgPosX(-vaule);
}

/***************************************************************************
* �������ƣ�   changeVScrollVaule
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* �Ρ�������   [in]  int vaule
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/23      ���ǲ�        ���
***************************************************************************/
void FindCircleWin::changeVScrollVaule(int vaule)
{
	m_paintWidget->setImgPosY(-vaule);
}

/***************************************************************************
* �������ƣ�   addGaussianNoise
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/23      ���ǲ�        ���
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
* �������ƣ�   autoCanny
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/28      ���ǲ�        ���
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
* �������ƣ�   drawCircle
* ժ����Ҫ��   
* ȫ��Ӱ�죺   private 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/08/23      ���ǲ�        ���
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
			// ��V��
			fileName = QString("v%1-%2.bmp").arg(i).arg(j);
			openFile(openFilePath + fileName);
			findCircleThreadshold();
			saveFile(saveFilePath + fileName);

			result << (i-1) * 4 + j << ":" << fileName << " " << "\n";

			double av = 0;
			double errAv = 0;

			// д�ļ�
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

	// ���õ�ͼƬ
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