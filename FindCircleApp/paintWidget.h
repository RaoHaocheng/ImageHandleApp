#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H

// 包含必要的头文件
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

// 保护类型接口
protected:
	typedef struct IMAGE_MANAGE
	{
		std::vector<QImage> images;
		int index;
	}ST_IMAGE_MANAGE, *PST_IMAGE_MANAGE;
	enum PaintState
	{
		DRAWING,        // 画图状态
		COMPLETE        // 完成状态
	};
	enum MouseState
	{
		IMAGE,          // 图片模式
		VSCROLL,        // V滚轮模式
		HSCROLL         // H滚轮模式
	};

public:
	void clearPaintWidget();				                  // 清除控件中显示的内容
	void setImg(QImage img);							      // 设置图片
	inline QImage Img() const { return m_image; }             // 返回图片
	void setSize(QSize);                                      // 设置控件的大小
	void setPos(QRect);                                       // 设置位置
	inline void setImgScale(float scale) { m_scale = scale; } // 设置倍率
	inline float ImgScale() const { return m_scale; }         // 返回当前倍率
	void scaleImg(float scale = m_scale);                     // 放缩中间中的图片
	bool checkSize(QSize);                                    // 检查当前的图片大小是否超过控件大小
	void setImgPosX(int);                                     // 设置图片的显示位置
	void setImgPosY(int);                                     
	void setImgPos(QPoint);                                   
	bool goBack();                                            // 返回上一层
	bool goFront();                                           // 返回该一层
	void clearImageBuffer();                                  // 清除图片的缓存
	void setShowScroll(bool bShow) { m_bShowScroll = bShow; } // 设置是否显示滚动条
	inline bool isShowScroll() const { return m_bShowScroll; }// 返回是否显示滚动条

// 定义的信号
signals:
	void reSize(QSize imgSize);							      // 发送图片大小改变的信号


// 一些可用的接口工具
protected:
	inline PaintState currentState() const { return m_paintState; }         // 当前控件工作状态
	void setPaintState(PaintState state);                                   // 设置画笔状态
	void setImg(QImage img, bool bInsert);							        // 设置图片
	inline ST_IMAGE_MANAGE currentImageManage() const { return m_vcImage; } // 返回图片管理结构体
	void resetImageManage(QImage img);                                      // 重新设置图片管理控件
	void updateUI();                                                        // 更新UI界面
	void init();                                                            // 初始化相关参数
	void initUI();                                                          // 初始化UI
	inline void setMouseState(MouseState state){ m_mouseState = state; }    // 设置鼠标状态
	inline MouseState getMouseState() const { return m_mouseState; }        // 返回鼠标状态

// 受保护的槽函数
protected slots:
	void changeHScrollVaule(int);                                           // H条的数值发生变化的时候
	void changeVScrollVaule(int);                                           // V条的数值发生变化的时候
	void clickHScroll();                                                    // 点击滚动条函数
	void clickVScroll();                                                    // 点击滚动条函数

// 控件事件的保护
protected:
	virtual void paintEvent(QPaintEvent *);                                 // 系统绘图响应事件
	virtual void wheelEvent(QWheelEvent *);                                 // 滚轮事件
	void resizeEvent(QResizeEvent *event);                                  // 大小变化事件 
	virtual void mouseReleaseEvent(QMouseEvent* event);                     // 鼠标释放事件

// 私有成员变量
private:
	QImage m_image;						 // 用于显示图片
	QImage m_srcImg;                     // 用于保存原始图片
	PaintState m_paintState;             // 当前画图状态
	QWidget* m_parent;                   // 副窗口指针
	QPoint m_imgPos;                     // 先前图片的显示位置
	ST_IMAGE_MANAGE m_vcImage;           // 图片缓存
	QScrollBar* m_verticalScroll;        // 滚动条
	QScrollBar* m_horizontalScroll;
	static float m_scale;                // 当前当前放大的状态
	static float m_times;                // 当前倍率
	bool m_bShowScroll;                  // 是否显示滚动条
	MouseState m_mouseState;             // 当前鼠标状态
};

#endif // FINDCIRCLEAPP_H
