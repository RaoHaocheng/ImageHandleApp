#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

// 包含必要的头文件
#include <QWidget>
#include <QMouseEvent>
#include <QImage>
#include <QPoint>
#include <QVector>

class PaintWidget : public QWidget
{
	Q_OBJECT

public:
	PaintWidget(QWidget *parent = 0);
	~PaintWidget();

public:
	void clearPaintWidget();				                // 清除控件中显示的内容
	void setImg(QImage img);							    // 设置图片
	inline QImage Img() const { return m_image; }           // 返回图片

// 定义的信号
signals:
	void reSize();										    // 发送图片大小改变的信号

// 私有的结构类型
protected:
	enum PaintState
	{
		DRAWING,
		COMPLETE
	};

protected:
	void paintEvent(QPaintEvent *);                                         // 系统绘图响应事件
	inline PaintState currentState() const { return m_paintState; }         // 当前控件工作状态
	void setPaintState(PaintState state);                                   // 设置画笔状态


// 私有成员变量
private:
	QImage m_image;						 // 用于显示图片
	PaintState m_paintState;             // 当前画图状态

};

#endif // FINDCIRCLEAPP_H
