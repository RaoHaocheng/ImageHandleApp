#include "paintWidget.h"
#include <QPainter>


float PaintWidget::m_scale = 1;
float PaintWidget::m_times = 1.2;

const float MIN_SIZE = 0.1;
const float MAX_SIZE = 5.0;

/***************************************************************************
* 函数名称：   PaintWidget
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QWidget * parent
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
PaintWidget::PaintWidget(QWidget *parent, QRect pos)
	: QWidget(parent)
	, m_paintState(COMPLETE)
	, m_parent(parent)
	, m_imgPos(QPoint(0,0))
	, m_verticalScroll(nullptr)
	, m_horizontalScroll(nullptr)
	, m_bShowScroll(true)
	, m_mouseState(IMAGE)
{
	init();
}

/***************************************************************************
* 函数名称：   ~PaintWidget
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
PaintWidget::~PaintWidget()
{
	if (nullptr != m_verticalScroll)
		delete m_verticalScroll;
	m_verticalScroll = nullptr;

	if (nullptr != m_horizontalScroll)
		delete m_horizontalScroll;
	m_horizontalScroll = nullptr;

}

/***************************************************************************
* 函数名称：   init
* 摘　　要：   
* 全局影响：   protected 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::init()
{
	initUI();
	clearImageBuffer();
}

/***************************************************************************
* 函数名称：   initUI
* 摘　　要：   
* 全局影响：   protected 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::initUI()
{
	// 初始化滚动条
	m_verticalScroll = new QScrollBar(Qt::Vertical, this);
	m_horizontalScroll = new QScrollBar(Qt::Horizontal, this);
	connect(m_verticalScroll, SIGNAL(valueChanged(int)), this, SLOT(changeVScrollVaule(int)));
	connect(m_horizontalScroll, SIGNAL(valueChanged(int)), this, SLOT(changeHScrollVaule(int)));
	connect(m_verticalScroll, SIGNAL(sliderReleased()), this, SLOT(clickVScroll()));
	connect(m_horizontalScroll, SIGNAL(sliderReleased()), this, SLOT(clickHScroll()));
	m_verticalScroll->hide();
	m_horizontalScroll->hide();

}

void PaintWidget::mouseReleaseEvent(QMouseEvent* event)
{
	// 看是否是鼠标左键
	if (event->button()==Qt::LeftButton)
	{
		// 设置成图像模式
		setMouseState(IMAGE);
	}


	event->accept();
}

/***************************************************************************
* 函数名称：   clickHScroll
* 摘　　要：   
* 全局影响：   protected 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::clickHScroll()
{
	setMouseState(HSCROLL);
}

/***************************************************************************
* 函数名称：   clickVScroll
* 摘　　要：   
* 全局影响：   protected 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::clickVScroll()
{
	setMouseState(VSCROLL);
}

/***************************************************************************
* 函数名称：   changeHScrollVaule
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  int vaule
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::changeHScrollVaule(int vaule)
{
	setImgPosX(-vaule);
	update();
}

/***************************************************************************
* 函数名称：   changeVScrollVaule
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  int vaule
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::changeVScrollVaule(int vaule)
{
	setImgPosY(-vaule);
	update();
}

/***************************************************************************
* 函数名称：   paintEvent
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  QPaintEvent * event
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void PaintWidget::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);

	if (currentState() == PaintState::DRAWING)
		return;

	// 画出图片
	setPaintState(PaintState::DRAWING);
	painter.drawImage(m_imgPos.x(), m_imgPos.y(), m_image);
	setPaintState(PaintState::COMPLETE);

	// 更新界面
	updateUI();
}

/***************************************************************************
* 函数名称：   setPaintState
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  PaintState state
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void PaintWidget::setPaintState(PaintState state)
{
	m_paintState = state;
}

/***************************************************************************
* 函数名称：   clearPaintWidget
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void PaintWidget::clearPaintWidget()
{
	// 设置成白色
	m_image.fill(qRgb(255, 255, 255));

	// 更新数据
	update();
}

/***************************************************************************
* 函数名称：   setImg
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QImage img
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void PaintWidget::setImg(QImage img)
{
	setImg(img, true);
}

/***************************************************************************
* 函数名称：   setImg
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  QImage img
* 参　　数：   [in]  bool bInsert
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/11/17      饶智博        添加
***************************************************************************/
void PaintWidget::setImg(QImage img, bool bInsert)
{
	// 画图
	m_image = img;
	m_srcImg = img;
	m_scale = 1;
	setImgPos(QPoint(0, 0));

	if (bInsert)
	{
		QImage imgBuf = img;

		if (m_vcImage.images.size() == 0 || (m_vcImage.images.size() == m_vcImage.index + 1))
		{
			m_vcImage.images.push_back(imgBuf);
			m_vcImage.index = m_vcImage.images.size() - 1;// 指向最后一张
		}
		else
		{
			// 在对应的位置插入相应的数据，并删除对应的数据
			m_vcImage.images.insert(m_vcImage.images.begin() + (++m_vcImage.index), imgBuf);

			// 删除这后面的数据
			for (int i = m_vcImage.index + 1; i < m_vcImage.images.size();)
			{
				m_vcImage.images.erase(m_vcImage.images.begin() + i);
			}
		}
	}

	update();
}


/***************************************************************************
* 函数名称：   goBack
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
bool PaintWidget::goBack()
{
	// 检查上一个是否存在
	if (m_vcImage.index - 1 < 0)
		return false;

	setImg(m_vcImage.images.at(--m_vcImage.index), false);
	return true;
}

/***************************************************************************
* 函数名称：   goFront
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
bool PaintWidget::goFront()
{
	// 检查下一个是否存在
	if (m_vcImage.index + 1 >= m_vcImage.images.size())
		return false;

	setImg(m_vcImage.images.at(++m_vcImage.index),false);
	return true;
}

/***************************************************************************
* 函数名称：   clearImageBuffer
* 摘　　要：   
* 全局影响：   public 
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::clearImageBuffer()
{
	m_vcImage.images.clear();
	m_vcImage.index = 0;
}

/***************************************************************************
* 函数名称：   wheelEvent
* 摘　　要：   
* 全局影响：   virtual protected 
* 参　　数：   [in]  QWheelEvent * event
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void PaintWidget::wheelEvent(QWheelEvent *event)
{
	const int times = 15;                // 倍率为10
	int numDegrees = event->delta() / 8;  //滚动的角度，*8就是鼠标滚动的距离
	int numSteps = numDegrees / times;       //滚动的步数，*15就是鼠标滚动的角度
	int scrollValue = 0;


	switch (m_mouseState)
	{
	case PaintWidget::IMAGE:
		// 放大或则缩小图片
		if (numSteps < 0)
		{
			if (m_scale > MIN_SIZE)
				m_scale /= m_times;

			if (m_scale < MIN_SIZE)
				m_scale = MIN_SIZE;

			scaleImg();
		}
		else if (numSteps > 0)
		{
			if (m_scale < MAX_SIZE)
				m_scale *= m_times;

			if (m_scale > MAX_SIZE)
				m_scale = MAX_SIZE;

			scaleImg();
		}
		break;
	case PaintWidget::VSCROLL:
		scrollValue = m_verticalScroll->value();
		scrollValue = scrollValue - numSteps*times;

		if (scrollValue < 0)
			scrollValue = 0;
		else if (scrollValue > m_verticalScroll->maximum())
			scrollValue = m_verticalScroll->maximum();

		m_verticalScroll->setValue(scrollValue);
		break;
	case PaintWidget::HSCROLL:
		scrollValue = m_horizontalScroll->value();
		scrollValue = scrollValue - numSteps*times;

		if (scrollValue < 0)
			scrollValue = 0;
		else if (scrollValue > m_horizontalScroll->maximum())
			scrollValue = m_horizontalScroll->maximum();

		m_horizontalScroll->setValue(scrollValue);
		break;
	default:
		break;
	}

	// 更新图片
	update();

	// 表明事件已经被接受
	event->accept();				// 信息处理完了
}

/***************************************************************************
* 函数名称：   setSize
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QSize size
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void PaintWidget::setSize(QSize size)
{
	QPoint p = pos();
	setPos(QRect(p.x(), p.y(), size.width(), size.height()));
}

/***************************************************************************
* 函数名称：   setPos
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QRect pos
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/06/30      饶智博        添加
***************************************************************************/
void PaintWidget::setPos(QRect pos)
{
	setGeometry(pos);
}

/***************************************************************************
* 函数名称：   scaleImg
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  float scale
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/01      饶智博        添加
***************************************************************************/
void PaintWidget::scaleImg(float scale)
{
	m_image = m_srcImg.scaled(QSize(m_srcImg.size().width() * scale, m_srcImg.size().height() * scale));
}

/***************************************************************************
* 函数名称：   checkSize
* 摘　　要：   
* 全局影响：   protected 
* 返回值　：   bool
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/02      饶智博        添加
***************************************************************************/
bool PaintWidget::checkSize(QSize size)
{
	if (size.width() < m_image.width() || size.height() < m_image.height())
	{
		emit reSize(m_image.size());
		return true;
	}

	return false;
}

/***************************************************************************
* 函数名称：   setImgPos
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  QPoint point
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/03      饶智博        添加
***************************************************************************/
void PaintWidget::setImgPos(QPoint point)
{
	m_imgPos.setX(point.x());
	m_imgPos.setY(point.y());
	update();
}

/***************************************************************************
* 函数名称：   setImgPosX
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  int x
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/03      饶智博        添加
***************************************************************************/
void PaintWidget::setImgPosX(int x)
{
	m_imgPos.setX(x);
	update();
}


/***************************************************************************
* 函数名称：   setImgPosY
* 摘　　要：   
* 全局影响：   public 
* 参　　数：   [in]  int y
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/07/03      饶智博        添加
***************************************************************************/
void PaintWidget::setImgPosY(int y)
{
	m_imgPos.setY(y);
	update();
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
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::resizeEvent(QResizeEvent *event)
{
	// 使用过的控件重新布局
	const int SCROLL_WIDGET_WIDTH = 20;
	m_verticalScroll->setGeometry(this->width() - SCROLL_WIDGET_WIDTH,
		0, SCROLL_WIDGET_WIDTH, this->height() - SCROLL_WIDGET_WIDTH);
	m_horizontalScroll->setGeometry(0, this->height() - SCROLL_WIDGET_WIDTH,
		this->width() - SCROLL_WIDGET_WIDTH, SCROLL_WIDGET_WIDTH);

	// 改变了窗口后，在检查其中的中间是否有超过的问题
	update();

	event->accept();
}

/***************************************************************************
* 函数名称：   updateUI
* 摘　　要：   
* 全局影响：   protected 
* 参　　数：   [in]  QSize size
* 返回值　：   void
*
* 修改记录：
*  [日期]     [作者/修改者]  [修改原因]
*2016/12/09      饶智博        添加
***************************************************************************/
void PaintWidget::updateUI()
{
	QSize winSize = this->size();
	QSize size = m_image.size();

	// 检查大小
	checkSize(winSize);

	// 刷新滚动条显示
	if (m_bShowScroll)
	{
		if (size.width() > winSize.width() )
		{
			if (!m_horizontalScroll->isVisible())
			{
				m_horizontalScroll->setMaximum(size.width() - winSize.width());
				m_horizontalScroll->setMinimum(0);
				m_horizontalScroll->setPageStep(size.width());
				m_horizontalScroll->show();
			}
		}
		else// 已经显示了，看是否需要关掉
		{
			if (m_horizontalScroll->value() == 0)
			{
				m_horizontalScroll->hide();
			}
		}

		if (size.height() > winSize.height() )
		{
			if (!m_verticalScroll->isVisible())
			{
				m_verticalScroll->setMaximum(size.height() - winSize.height());
				m_verticalScroll->setMinimum(0);
				m_verticalScroll->setPageStep(size.height());
				m_verticalScroll->show();
			}
		}
		else
		{
			if (m_verticalScroll->value() == 0)
			{
				m_verticalScroll->hide();
			}
		}
	}
}