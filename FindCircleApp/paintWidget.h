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
	PaintWidget(QWidget *parent = 0, QRect size = QRect(0, 0, -1, -1));
	~PaintWidget();

protected:
	typedef struct IMAGE_MANAGE
	{
		std::vector<QImage> images;
		int index;
	}ST_IMAGE_MANAGE, *PST_IMAGE_MANAGE;

public:
	void clearPaintWidget();				                  // ����ؼ�����ʾ������
	void setImg(QImage img);							      // ����ͼƬ
	inline QImage Img() const { return m_image; }             // ����ͼƬ
	void setSize(QSize);                                      // ���ÿؼ��Ĵ�С
	void setPos(QRect);                                       // ����λ��
	inline void setImgScale(float scale) { m_scale = scale; } // ���ñ���
	inline float ImgScale() const { return m_scale; }         // ���ص�ǰ����
	void scaleImg(float scale = m_scale);                     // �����м��е�ͼƬ
	bool checkSize(QSize);                                    // ��鵱ǰ��ͼƬ��С�Ƿ񳬹��ؼ���С
	void setImgPosX(int);                                     // ����ͼƬ����ʾλ��
	void setImgPosY(int);                                     
	void setImgPos(QPoint);                                   
	bool goBack();                                            // ������һ��
	bool goFront();                                           // ���ظ�һ��
	void clearImageBuffer();                                  // ���ͼƬ�Ļ���

// ������ź�
signals:
	void reSize(QSize imgSize);							      // ����ͼƬ��С�ı���ź�

// ˽�еĽṹ����
protected:
	enum PaintState
	{
		DRAWING,
		COMPLETE
	};

// һЩ���õĽӿڹ���
protected:
	inline PaintState currentState() const { return m_paintState; }         // ��ǰ�ؼ�����״̬
	void setPaintState(PaintState state);                                   // ���û���״̬
	void setImg(QImage img, bool bInsert);							        // ����ͼƬ
	inline ST_IMAGE_MANAGE currentImageManage() const { return m_vcImage; }
	void resetImageManage(QImage img);

// �ؼ��¼��ı���
protected:
	virtual void paintEvent(QPaintEvent *);                                 // ϵͳ��ͼ��Ӧ�¼�
	virtual void wheelEvent(QWheelEvent *);                                 // �����¼�

// ˽�г�Ա����
private:
	QImage m_image;						 // ������ʾͼƬ
	QImage m_srcImg;                     // ���ڱ���ԭʼͼƬ
	PaintState m_paintState;             // ��ǰ��ͼ״̬
	QWidget* m_parent;                   // ������ָ��
	static float m_scale;                // ��ǰ��ǰ�Ŵ��״̬
	static float m_times;                // ��ǰ����
	QPoint m_imgPos;                     // ��ǰͼƬ����ʾλ��
	ST_IMAGE_MANAGE m_vcImage;           // ͼƬ����

};

#endif // FINDCIRCLEAPP_H
