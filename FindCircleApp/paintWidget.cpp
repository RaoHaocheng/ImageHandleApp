#include "paintWidget.h"
#include <QPainter>


 float PaintWidget::m_scale = 1;
 float PaintWidget::m_times = 1.2;

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
{

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
	painter.drawImage(0, 0, m_image);
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
	// ��ͼ
	m_image = img;
	m_srcImg = img;
	m_scale = 1;

	// ��������
	update();
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

	if (numSteps < 0)
	{
		if (m_scale > 0.1)
			m_scale /= m_times;

		if (m_scale < 0.1)
			m_scale = 0.1;

		scaleImg();
	}
	else if (numSteps > 0)
	{
		if (m_scale < 5)
			m_scale *= m_times;

		if (m_scale >5)
			m_scale = 5;

		scaleImg();
	}

	update();
	event->accept();
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