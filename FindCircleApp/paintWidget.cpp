#include "paintWidget.h"
#include <QPainter>


 float PaintWidget::m_scale = 1;
 float PaintWidget::m_times = 1.2;

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
{

}

PaintWidget::~PaintWidget()
{

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
	painter.drawImage(0, 0, m_image);
	setPaintState(PaintState::COMPLETE);

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
	// 画图
	m_image = img;
	m_srcImg = img;
	m_scale = 1;

	// 更新数据
	update();
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
	int numDegrees = event->delta() / 8;//滚动的角度，*8就是鼠标滚动的距离
	int numSteps = numDegrees / 15;//滚动的步数，*15就是鼠标滚动的角度

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