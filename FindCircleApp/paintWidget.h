#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

// ������Ҫ��ͷ�ļ�
#include <QWidget>
#include <QMouseEvent>
#include <QImage>
#include <QPoint>
#include <QVector>
#include <QScrollBar>

class PaintWidget : public QWidget
{
	Q_OBJECT

public:
	PaintWidget(QWidget *parent = 0, QRect size = QRect(0, 0, -1, -1));
	~PaintWidget();

// �������ͽӿ�
protected:
	typedef struct IMAGE_MANAGE
	{
		std::vector<QImage> images;
		int index;
	}ST_IMAGE_MANAGE, *PST_IMAGE_MANAGE;
	enum PaintState
	{
		DRAWING,        // ��ͼ״̬
		COMPLETE        // ���״̬
	};
	enum MouseState
	{
		IMAGE,          // ͼƬģʽ
		VSCROLL,        // V����ģʽ
		HSCROLL         // H����ģʽ
	};

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
	void setShowScroll(bool bShow) { m_bShowScroll = bShow; } // �����Ƿ���ʾ������
	inline bool isShowScroll() const { return m_bShowScroll; }// �����Ƿ���ʾ������

// ������ź�
signals:
	void reSize(QSize imgSize);							      // ����ͼƬ��С�ı���ź�


// һЩ���õĽӿڹ���
protected:
	inline PaintState currentState() const { return m_paintState; }         // ��ǰ�ؼ�����״̬
	void setPaintState(PaintState state);                                   // ���û���״̬
	void setImg(QImage img, bool bInsert);							        // ����ͼƬ
	inline ST_IMAGE_MANAGE currentImageManage() const { return m_vcImage; } // ����ͼƬ����ṹ��
	void resetImageManage(QImage img);                                      // ��������ͼƬ����ؼ�
	void updateUI();                                                        // ����UI����
	void init();                                                            // ��ʼ����ز���
	void initUI();                                                          // ��ʼ��UI
	inline void setMouseState(MouseState state){ m_mouseState = state; }    // �������״̬
	inline MouseState getMouseState() const { return m_mouseState; }        // �������״̬

// �ܱ����Ĳۺ���
protected slots:
	void changeHScrollVaule(int);                                           // H������ֵ�����仯��ʱ��
	void changeVScrollVaule(int);                                           // V������ֵ�����仯��ʱ��
	void clickHScroll();                                                    // �������������
	void clickVScroll();                                                    // �������������

// �ؼ��¼��ı���
protected:
	virtual void paintEvent(QPaintEvent *);                                 // ϵͳ��ͼ��Ӧ�¼�
	virtual void wheelEvent(QWheelEvent *);                                 // �����¼�
	void resizeEvent(QResizeEvent *event);                                  // ��С�仯�¼� 
	virtual void mouseReleaseEvent(QMouseEvent* event);                     // ����ͷ��¼�

// ˽�г�Ա����
private:
	QImage m_image;						 // ������ʾͼƬ
	QImage m_srcImg;                     // ���ڱ���ԭʼͼƬ
	PaintState m_paintState;             // ��ǰ��ͼ״̬
	QWidget* m_parent;                   // ������ָ��
	QPoint m_imgPos;                     // ��ǰͼƬ����ʾλ��
	ST_IMAGE_MANAGE m_vcImage;           // ͼƬ����
	QScrollBar* m_verticalScroll;        // ������
	QScrollBar* m_horizontalScroll;
	static float m_scale;                // ��ǰ��ǰ�Ŵ��״̬
	static float m_times;                // ��ǰ����
	bool m_bShowScroll;                  // �Ƿ���ʾ������
	MouseState m_mouseState;             // ��ǰ���״̬
};

#endif // FINDCIRCLEAPP_H
