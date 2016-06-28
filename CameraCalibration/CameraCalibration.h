// 标定类
#ifndef CAMERACALIBRATION_H
#define CAMERACALIBRATION_H

// 数据结构定义库以及在OpenCV库中必须使用的内容
#include "define.h"

typedef struct CALIBRATION_IMAGE_INFO   //图片信息
{
	int iImageWidth;      // 标定板的宽度
	int iImageHeight;     // 标定板的高度
	int iImgCounts;       // 标定图像个数
}ST_CALIBRATION_IMAGE_INFO, *PST_CALIBRATION_IMAGE_INFO;

typedef struct CALIBRATION_RESULT   //标定结果
{
	CvMat* cvMatLeftPoint;          // 左目相机角点坐标
	CvMat* cvMatRightPoint;         // 右目相机角点坐标
	CvMat* cvMatObjectPoint;		// 物理坐标
	CvMat* cvMatLeftPointCount;     // 左目棋盘格点数矩阵
	CvMat* cvMatRightPointCount;    // 右目相机棋盘格点数矩阵
	CvMat* cvMatLintrinsicMatrix;   // 左目相机内参数
	CvMat* cvMatRintrinsicMatrix;   // 右目相机内参数
	CvMat* cvMatLdistortionCoeff;   // 左目相机畸变系数
	CvMat* cvMatRdistortionCoeff;   // 右目相机畸变系数
	CvMat* cvMatLRotationVct;       // 棋盘格相对左目相机旋转向量
	CvMat* cvMatLTranslaVct;        // 棋盘格相对左目相机平移向量
	CvMat* cvMatRRotationVct;       // 棋盘格相对右目相机旋转向量
	CvMat* cvMatRTranslaVct;        // 棋盘格相对右目相机平移向量
	CvMat* cvMatRotationStereo;     // 立体旋转矩阵
	CvMat* cvMatTranslationStereo;  // 立体平移矩阵
	CvMat* cvMatFundamentalMat;     // 基础矩阵
	CvMat* cvEssentialValueMat;     // 本证矩阵
}ST_CALIBRATION_RESULT, *PST_CALIBRATION_RESULT;

typedef struct RECTIFY_IMG_PARAMETER
{
	CvMat* pcvProjectionMatLeft;       // 左投影矩阵
	CvMat* pcvProjectionMatRight;      // 右投影矩阵
	CvMat* pcvLeftParallelMat;         // 左目相机行对准矩阵
	CvMat* pcvRightParallelMat;        // 右目相机行对准矩阵
	CvMat* pcvReprojectionMat;         // 重投影矩阵
	CvMat* pcvMapXLeft;                // 左目相机x方向校正映射表，即mapx1
	CvMat* pcvMapYLeft;                // 左目相机y方向校正映射表，即mapy1
	CvMat* pcvMapXRight;               // 右目相机x方向校正映射表，即mapx1
	CvMat* pcvMapYRight;               // 右目相机y方向校正映射表，即mapy1 
}ST_RECTIFY_IMG_PARAMETER, *PST_RECTIFY_IMG_PARAMETER;

// 使用模板类的导出工作
template  class DLLS_PORT std::vector<IplImage*>;

class DLLS_PORT CameraCalibration
{
public:
	CameraCalibration();               // 构造函数
	virtual ~CameraCalibration();      // 析构函数

	// 为了防止用户跨越步骤调用函数
	enum CalibrationStep      // 标定的步骤
	{
		UNSTART,              // 未开始
		SET_INFO,             // 设置图片信息
		OPEN_IMAGE,           // 打开图片
		FIND_CORNER,          // 找寻胶垫
		COMPUTER_PARAMETER,   // 计算参数
		CALIBRATION,          // 标定
		CALIBRATION_ERROR     // 错误
	};

	void SetCalibrationImageInfo(const ST_CALIBRATION_IMAGE_INFO& stImageInfo);    // 设置相机的基本信息
	BOOL OpenCalibrationImage(const char* pcFileName, int flags);                  // 打开每个需要标定的图片
	BOOL Calibration(ST_CALIBRATION_RESULT& stResult);                             // 标定结果
	BOOL Calibration(ST_CALIBRATION_RESULT& stResult, const char* pcFileName);     // 标定结果以及保存校正后的图片
	CvSize GetImgSize() const;                                                     // 得到图片的大小
	inline CalibrationStep CurrentState() const { return m_CalibrationStep; }      // 返回当前需要执行的步骤
	inline void SetShowImage(const bool bShowImage) { m_bShowImage = bShowImage; } // 设置是否显示图片
	inline bool ShowImage() const { return m_bShowImage; }		                   // 当前是否显示图片
	BOOL ClearImageCache(const int flags);                                         // 清除照片
	inline std::vector<IplImage*> CameraLeftImgCache() { return m_vcLImage; }      // 返回图片缓存
	inline std::vector<IplImage*> CameraRightImgCache() { return m_vcRImage; }     // 返回图片缓存
	BOOL RectifyImg(IplImage* pImg, int flags);                                    // 校正图片
	BOOL Test();															       // 用于测试

protected:
	BOOL FindBoardCorner(IplImage* pImg, 
		CvPoint2D32f* pImgCornorPoint, int& iCornorPoints);                      // 查找标定板的角点
	BOOL ComputerParameter(ST_CALIBRATION_RESULT& stResult);                     // 计算内外参数
	BOOL RectifyImg(const char* pcFileName);                                     // 校正标定图片
	BOOL ComputerRectifyParameter(ST_RECTIFY_IMG_PARAMETER& stRectifyParameter,
		ST_CALIBRATION_RESULT& stResult);                                        // 计算校正参数
	
	BOOL NewCalibrationResult(ST_CALIBRATION_RESULT** stCalibrationResult, 
							  const ST_CALIBRATION_IMAGE_INFO& stImageInfo);     // 继承类使用的新建
	BOOL DeleteCalibrationResult(ST_CALIBRATION_RESULT** stCalibrationResult);   // 继承类使用的删除
	void ClearImageCache();                                                      // 清理图片双目相机的缓存

private:
	BOOL NewCalibrationResult();												  // 新建存储的数据
	BOOL DeleteCalibrationResult();												  // 删除存储的数据
	BOOL SaveLeftImageCache(const char* pcFileName);                              // 存放左相机缓存
	BOOL SaveRightImageCache(const char* pcFileName);                             // 存放右相机缓存
	BOOL FindBoardCorner();                                                       // 查找图片缓存中的所有数据
	BOOL CheckImgSize(IplImage* pImg);                                            // 检查图片的是否符合大小

private:
	CalibrationStep m_CalibrationStep;               // 标定的步骤
	ST_CALIBRATION_IMAGE_INFO* m_stImageInfo;        // 图片信息
	std::vector<IplImage*>  m_vcLImage;              // 左目相机图片容器
	std::vector<IplImage*>  m_vcRImage;              // 右目相机图片容器

	bool m_bShowImage;                               // 设置显示图片
	ST_CALIBRATION_RESULT* m_stCalibrationResult;    // 标定结果
	ST_RECTIFY_IMG_PARAMETER* m_stRecitfyPaprameter; // 修正参数
	CvSize* m_cvSize;                                // 图片大小
};
#endif