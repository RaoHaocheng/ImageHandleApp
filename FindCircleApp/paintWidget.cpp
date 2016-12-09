#include "paintWidget.h"
#include <QPainter>


float PaintWidget::m_scale = 1;
float PaintWidget::m_times = 1.2;

const float MIN_SIZE = 0.1;
const float MAX_SIZE = 5.0;

/***************************************************************************
* �������ƣ�   PaintWidget
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  QWidget * parent
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
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
* �������ƣ�   ~PaintWidget
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
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
* �������ƣ�   init
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::init()
{
	initUI();
	clearImageBuffer();
}

/***************************************************************************
* �������ƣ�   initUI
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::initUI()
{
	// ��ʼ��������
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
	// ���Ƿ���������
	if (event->button()==Qt::LeftButton)
	{
		// ���ó�ͼ��ģʽ
		setMouseState(IMAGE);
	}


	event->accept();
}

/***************************************************************************
* �������ƣ�   clickHScroll
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::clickHScroll()
{
	setMouseState(HSCROLL);
}

/***************************************************************************
* �������ƣ�   clickVScroll
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::clickVScroll()
{
	setMouseState(VSCROLL);
}

/***************************************************************************
* �������ƣ�   changeHScrollVaule
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  int vaule
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::changeHScrollVaule(int vaule)
{
	setImgPosX(-vaule);
	update();
}

/***************************************************************************
* �������ƣ�   changeVScrollVaule
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  int vaule
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::changeVScrollVaule(int vaule)
{
	setImgPosY(-vaule);
	update();
}

/***************************************************************************
* �������ƣ�   paintEvent
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  QPaintEvent * event
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void PaintWidget::paintEvent(QPaintEvent * event)
{
	QPainter painter(this);

	if (currentState() == PaintState::DRAWING)
		return;

	// ����ͼƬ
	setPaintState(PaintState::DRAWING);
	painter.drawImage(m_imgPos.x(), m_imgPos.y(), m_image);
	setPaintState(PaintState::COMPLETE);

	// ���½���
	updateUI();
}

/***************************************************************************
* �������ƣ�   setPaintState
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  PaintState state
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void PaintWidget::setPaintState(PaintState state)
{
	m_paintState = state;
}

/***************************************************************************
* �������ƣ�   clearPaintWidget
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void PaintWidget::clearPaintWidget()
{
	// ���óɰ�ɫ
	m_image.fill(qRgb(255, 255, 255));

	// ��������
	update();
}

/***************************************************************************
* �������ƣ�   setImg
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  QImage img
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void PaintWidget::setImg(QImage img)
{
	setImg(img, true);
}

/***************************************************************************
* �������ƣ�   setImg
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  QImage img
* �Ρ�������   [in]  bool bInsert
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/11/17      ���ǲ�        ���
***************************************************************************/
void PaintWidget::setImg(QImage img, bool bInsert)
{
	// ��ͼ
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
			m_vcImage.index = m_vcImage.images.size() - 1;// ָ�����һ��
		}
		else
		{
			// �ڶ�Ӧ��λ�ò�����Ӧ�����ݣ���ɾ����Ӧ������
			m_vcImage.images.insert(m_vcImage.images.begin() + (++m_vcImage.index), imgBuf);

			// ɾ������������
			for (int i = m_vcImage.index + 1; i < m_vcImage.images.size();)
			{
				m_vcImage.images.erase(m_vcImage.images.begin() + i);
			}
		}
	}

	update();
}


/***************************************************************************
* �������ƣ�   goBack
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
bool PaintWidget::goBack()
{
	// �����һ���Ƿ����
	if (m_vcImage.index - 1 < 0)
		return false;

	setImg(m_vcImage.images.at(--m_vcImage.index), false);
	return true;
}

/***************************************************************************
* �������ƣ�   goFront
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
bool PaintWidget::goFront()
{
	// �����һ���Ƿ����
	if (m_vcImage.index + 1 >= m_vcImage.images.size())
		return false;

	setImg(m_vcImage.images.at(++m_vcImage.index),false);
	return true;
}

/***************************************************************************
* �������ƣ�   clearImageBuffer
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::clearImageBuffer()
{
	m_vcImage.images.clear();
	m_vcImage.index = 0;
}

/***************************************************************************
* �������ƣ�   wheelEvent
* ժ����Ҫ��   
* ȫ��Ӱ�죺   virtual protected 
* �Ρ�������   [in]  QWheelEvent * event
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void PaintWidget::wheelEvent(QWheelEvent *event)
{
	const int times = 15;                // ����Ϊ10
	int numDegrees = event->delta() / 8;  //�����ĽǶȣ�*8�����������ľ���
	int numSteps = numDegrees / times;       //�����Ĳ�����*15�����������ĽǶ�
	int scrollValue = 0;


	switch (m_mouseState)
	{
	case PaintWidget::IMAGE:
		// �Ŵ������СͼƬ
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

	// ����ͼƬ
	update();

	// �����¼��Ѿ�������
	event->accept();				// ��Ϣ��������
}

/***************************************************************************
* �������ƣ�   setSize
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  QSize size
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void PaintWidget::setSize(QSize size)
{
	QPoint p = pos();
	setPos(QRect(p.x(), p.y(), size.width(), size.height()));
}

/***************************************************************************
* �������ƣ�   setPos
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  QRect pos
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/06/30      ���ǲ�        ���
***************************************************************************/
void PaintWidget::setPos(QRect pos)
{
	setGeometry(pos);
}

/***************************************************************************
* �������ƣ�   scaleImg
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  float scale
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/01      ���ǲ�        ���
***************************************************************************/
void PaintWidget::scaleImg(float scale)
{
	m_image = m_srcImg.scaled(QSize(m_srcImg.size().width() * scale, m_srcImg.size().height() * scale));
}

/***************************************************************************
* �������ƣ�   checkSize
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* ����ֵ����   bool
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/02      ���ǲ�        ���
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
* �������ƣ�   setImgPos
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  QPoint point
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/03      ���ǲ�        ���
***************************************************************************/
void PaintWidget::setImgPos(QPoint point)
{
	m_imgPos.setX(point.x());
	m_imgPos.setY(point.y());
	update();
}

/***************************************************************************
* �������ƣ�   setImgPosX
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  int x
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/03      ���ǲ�        ���
***************************************************************************/
void PaintWidget::setImgPosX(int x)
{
	m_imgPos.setX(x);
	update();
}


/***************************************************************************
* �������ƣ�   setImgPosY
* ժ����Ҫ��   
* ȫ��Ӱ�죺   public 
* �Ρ�������   [in]  int y
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/07/03      ���ǲ�        ���
***************************************************************************/
void PaintWidget::setImgPosY(int y)
{
	m_imgPos.setY(y);
	update();
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
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::resizeEvent(QResizeEvent *event)
{
	// ʹ�ù��Ŀؼ����²���
	const int SCROLL_WIDGET_WIDTH = 20;
	m_verticalScroll->setGeometry(this->width() - SCROLL_WIDGET_WIDTH,
		0, SCROLL_WIDGET_WIDTH, this->height() - SCROLL_WIDGET_WIDTH);
	m_horizontalScroll->setGeometry(0, this->height() - SCROLL_WIDGET_WIDTH,
		this->width() - SCROLL_WIDGET_WIDTH, SCROLL_WIDGET_WIDTH);

	// �ı��˴��ں��ڼ�����е��м��Ƿ��г���������
	update();

	event->accept();
}

/***************************************************************************
* �������ƣ�   updateUI
* ժ����Ҫ��   
* ȫ��Ӱ�죺   protected 
* �Ρ�������   [in]  QSize size
* ����ֵ����   void
*
* �޸ļ�¼��
*  [����]     [����/�޸���]  [�޸�ԭ��]
*2016/12/09      ���ǲ�        ���
***************************************************************************/
void PaintWidget::updateUI()
{
	QSize winSize = this->size();
	QSize size = m_image.size();

	// ����С
	checkSize(winSize);

	// ˢ�¹�������ʾ
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
		else// �Ѿ���ʾ�ˣ����Ƿ���Ҫ�ص�
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