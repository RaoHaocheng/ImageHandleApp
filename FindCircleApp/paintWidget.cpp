#include "paintWidget.h"
#include <QPainter>

PaintWidget::PaintWidget(QWidget *parent)
	: QWidget(parent),
	m_paintState(COMPLETE)
{

}

PaintWidget::~PaintWidget()
{

}

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

void PaintWidget::setPaintState(PaintState state)
{
	m_paintState = state;
}

void PaintWidget::clearPaintWidget()
{
	// 设置成白色
	m_image.fill(qRgb(255, 255, 255));

	// 更新数据
	update();
}

void PaintWidget::setImg(QImage img)
{
	m_image = img;
	update();
}