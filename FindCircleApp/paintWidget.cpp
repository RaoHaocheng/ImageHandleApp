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
{
	clearImageBuffer();
}

PaintWidget::~PaintWidget()
{

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

	emit reSize(m_image.size());
}


bool PaintWidget::goBack()
{
	// �����һ���Ƿ����
	if (m_vcImage.index - 1 < 0)
		return false;

	setImg(m_vcImage.images.at(--m_vcImage.index), false);
	return true;
}

bool PaintWidget::goFront()
{
	// �����һ���Ƿ����
	if (m_vcImage.index + 1 >= m_vcImage.images.size())
		return false;

	setImg(m_vcImage.images.at(++m_vcImage.index),false);
	return true;
}

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
	int numDegrees = event->delta() / 8;//�����ĽǶȣ�*8�����������ľ���
	int numSteps = numDegrees / 15;//�����Ĳ�����*15�����������ĽǶ�

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

		if (m_scale >MAX_SIZE)
			m_scale = MAX_SIZE;

		scaleImg();
	}

	// ����ͼƬ
	update();
	emit reSize(m_image.size());	// ����ͼƬ��Ϣ�ĸı�
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