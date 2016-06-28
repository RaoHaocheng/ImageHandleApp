#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

// ������Ҫ��ͷ�ļ�
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
	void clearPaintWidget();				                // ����ؼ�����ʾ������
	void setImg(QImage img);							    // ����ͼƬ
	inline QImage Img() const { return m_image; }           // ����ͼƬ

// ������ź�
signals:
	void reSize();										    // ����ͼƬ��С�ı���ź�

// ˽�еĽṹ����
protected:
	enum PaintState
	{
		DRAWING,
		COMPLETE
	};

protected:
	void paintEvent(QPaintEvent *);                                         // ϵͳ��ͼ��Ӧ�¼�
	inline PaintState currentState() const { return m_paintState; }         // ��ǰ�ؼ�����״̬
	void setPaintState(PaintState state);                                   // ���û���״̬


// ˽�г�Ա����
private:
	QImage m_image;						 // ������ʾͼƬ
	PaintState m_paintState;             // ��ǰ��ͼ״̬

};

#endif // FINDCIRCLEAPP_H
