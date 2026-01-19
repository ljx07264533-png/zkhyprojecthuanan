//说明：以下添加的为halcon共用的代码
//20230421 增加genFlawInfoFromRegion, computeRectangle1Plygons, computeRectangle1Plygons(重载), hTuple2Points, points2HalconCpp::HTuple, thresholdSeg函数。--edited by Guo Xulong
//20230504 移除computeRectangle1Plygons(重载)函数。--edited by Guo Xulong
//20230606 1. 修复genFlawInfoFromRegion函数的BUG；2. 增加genContourFromRegion函数用于将输入的HalconCpp::HObject区域转换为轮廓点集。--edited by Guo Xulong
//20230610 优化genFlawInfoFromRegion函数，增加对空区域的处理。--edited by Guo Xulong
//20230626 1. 新增genFlawInfoFromRegions函数，用于计算多个缺陷区域的基本信息；2. 更改genFlawInfoFromRegion函数部分内部变量名称(不影响原有接口调用)。--edited by Guo Xulong
//20230705 1. 新增isHalconCpp::HObjectEmpty函数，用于判断输入的HalconCpp::HObject是否为空。--edited by Guo Xulong
//20230725 1. 新增HalconCpp::HObject2Mat函数，用于将HalconCpp::HObject图像转换为Mat图像。--edited by Guo Xulong
//20230803 1. 新增isHRegionEmpty，用于判断H区域是否初始化以及是否为空。--edited by Guo Xulong
//20230824 1. 新增genRegionFromContour，用于将输入的轮廓点集（<std::vector<std::vector<cv::Point2f>>>或<std::vector<cv::Point2f>>）转换为HalconCpp::HObject区域。--edited by Guo Xulong
//20231118 1. 新增numericalStatic，用于对输入的数值进行统计。--edited by Guo Xulong
//		   2. 新增createDirectory，用于创建多级目录。 -- --edited by Wang Nannan
//20231130 1. 新增genFlawBasicInfo，用于对输入缺陷特征结构体的基础特征进行初始化赋值操作。--edited by Guo Xulong
#ifndef ZKHYPROJECTHUANAN_HALCONCODEPUBLIC_H
#define ZKHYPROJECTHUANAN_HALCONCODEPUBLIC_H

#include "alglibrary/alglibCore.h"


namespace zkhyPublicFuncHN
{
	//--------------------------------------regionsToInfo--------------------------------------------------//
	//功能：输入多个缺陷区域（每个缺陷区域都包含多个缺陷融合后的结果），获取缺陷区域对应的尺寸和外接矩形5个点信息
	//输入：缺陷区域
	//输出 缺陷区域中心点（多维）、缺陷的长半轴、缺陷的面积、缺陷的外接矩形四个角点坐标
	void regionsToInfo(HalconCpp::HObject ho_defectRegions, HalconCpp::HTuple* hv_Rows, HalconCpp::HTuple* hv_Columns,
		HalconCpp::HTuple* hv_Len1s, HalconCpp::HTuple* hv_Len2s, HalconCpp::HTuple* hv_Areas, HalconCpp::HTuple* hv_row1s, HalconCpp::HTuple* hv_col1s,
		HalconCpp::HTuple* hv_row2s, HalconCpp::HTuple* hv_col2s, HalconCpp::HTuple* hv_row3s, HalconCpp::HTuple* hv_col3s, HalconCpp::HTuple* hv_row4s,
		HalconCpp::HTuple* hv_col4s);

	//--------------------------------------thresholdSeg--------------------------------------------------//
	//功能：集成不同的阈值分割方法
	//输入：image 待测H图像
	//		   thresholdLow 阈值分割低阈值
	//		   thresholdHigh 阈值分割高阈值
	//		   targetProperty 目标相较于背景是亮还是暗，可选"dark"和"light"
	//		   func 阈值分割方法，可选单区间固定阈值和大津法
	//输出：region 分割获得的区域
	int thresholdSeg(HalconCpp::HObject& image, HalconCpp::HObject& region, HalconCpp::HTuple& thresholdLow,const HalconCpp::HTuple& thresholdHigh, HalconCpp::HTuple& targetProperty, const int& func);

	//--------------------------------------hTuple2Points--------------------------------------------------//
	//功能：将HalconCpp::HTuple格式的点集转换为cv::Point2f格式的点集
	//输入：x x坐标 <HalconCpp::HTuple>
	//		y y坐标 <HalconCpp::HTuple>
	//		sampleInterval 采样间隔，用于控制轮廓点数量
	//输出：points 转换后的点集 <std::vector<cv::Point2f>>
	void hTuple2Points(HalconCpp::HTuple& x, HalconCpp::HTuple& y, std::vector<cv::Point2f>& points, int sampleInterval = 1);
	//--------------------------------------hTuple2Points 重载--------------------------------------------------//
	//功能：将HalconCpp::HTuple格式的点集转换为cv::Point2f格式的点集，将输入点集转换为闭合的轮廓
	//输入：x x坐标 <HalconCpp::HTuple>
	//		y y坐标 <HalconCpp::HTuple>
	//		isClosedInput 是否闭合轮廓
	//输出：points 转换后的点集 <std::vector<cv::Point2f>>
	void hTuple2Points(HalconCpp::HTuple& x, HalconCpp::HTuple& y, std::vector<cv::Point2f>& points, bool& isClosedInput);

	//--------------------------------------points2HalconCpp::HTuple--------------------------------------------------//
	//功能：将cv::Point2f格式的点集转换为HalconCpp::HTuple格式的点集
	//输入：points 转换后的点集 <std::vector<cv::Point2f>>
	//输出：x x坐标 <HalconCpp::HTuple>
	//		   y y坐标 <HalconCpp::HTuple>
	void points2HTuple(std::vector<cv::Point2f>& points, HalconCpp::HTuple& x, HalconCpp::HTuple& y);

	//--------------------------------------hTuple2Points3D --------------------------------------------//
	//功能：将HalconCpp::HTuple格式的点集转换为cv::Point3f格式的点集
	//输入：x x坐标 <HalconCpp::HTuple>，y y坐标 <HalconCpp::HTuple>，z z坐标 <HalconCpp::HTuple>
	//输出：points 转换后的点集 <std::vector<cv::Point3f>>
	void hTuple2Points3D(HalconCpp::HTuple x, HalconCpp::HTuple y, HalconCpp::HTuple z, std::vector<cv::Point3f>& points);

	//--------------------------------------points3D2HalconCpp::HTuple --------------------------------------------//
	//功能：将cv::Point3f格式的点集转换为HalconCpp::HTuple格式的点集
	//输入：points 转换后的点集 <std::vector<cv::Point3f>>
	//输出：x x坐标 <HalconCpp::HTuple>，y y坐标 <HalconCpp::HTuple>，z z坐标 <HalconCpp::HTuple>
	void points3D2HTuple(std::vector<cv::Point3f> points, HalconCpp::HTuple& x, HalconCpp::HTuple& y, HalconCpp::HTuple& z);


	//--------------------------------------computeRectangle1Plygons--------------------------------------------------//
	//功能：根据正矩形的左上和右下角点坐标计算其余顶点坐标，并生成闭合的多边形，输入坐标变换关系时会对轮廓点集进行坐标变换
	//输入：row1 矩形左上角点行坐标（y）
	//		   col1 矩形左上角点列坐标（x）
	//		   row2 矩形右下角点行坐标（y）
	//		   col2 矩形右下角点列坐标（x）
	//		   homMat2D 坐标变换矩阵，用于对点集进行坐标变换，默认为空的HalconCpp::HTuple
	//输出：polygon 矩形轮廓点集，生成的为闭合轮廓，即polygon[4]=polygon[0] <std::vector<cv::Point2f>>
	void computeRectangle1Plygon(HalconCpp::HTuple row1, HalconCpp::HTuple col1, HalconCpp::HTuple row2, HalconCpp::HTuple col2, std::vector<cv::Point2f>& polygon, HalconCpp::HTuple homMat2D = HalconCpp::HTuple());

	//--------------------------------------genFlawInfoFromRegion--------------------------------------------------//
	//功能：计算输入区域的面积、角度、外接矩形以及轮廓等基本特征数据
	//输入：defectRegion 缺陷区域，输入为单个缺陷的区域
	//		   useContour 是否计算缺陷轮廓，true为计算轮廓，false为不计算轮廓，默认为计算
	//		   useRotateRect 计算区域的外接旋转矩形还是正矩形，true为旋转矩形，false为正矩形，默认为旋转矩形
	//		   homMat2D 坐标变换矩阵，用于对缺陷区域进行映射，默认为空
	//输出：area 缺陷面积 <double>
	//		   angle 缺陷角度 <double>
	//		   flawWidth 缺陷宽度 <double>
	//		   flawHeight 缺陷高度 <double>
	//		   centerPoint 缺陷中心点坐标 <cv::Point2f>
	//		   rectCountour 缺陷外接矩形坐标 <std::vector<cv::Point2f>>
	//		   defectContours 缺陷区域轮廓坐标 <std::vector<std::vector<cv::Point2f>>>
	int  genFlawInfoFromRegion(HalconCpp::HObject& defectRegion, double& area, double& angle, double& flawWidth, double& flawHeight, cv::Point2f& centerPoint, std::vector<cv::Point2f>& rectContour, std::vector<std::vector<cv::Point2f>>& defectContours, const bool& useContour = true, const bool& useRotateRect = true, const HalconCpp::HTuple& homMat2D = HalconCpp::HTuple());

	//--------------------------------------genFlawInfoFromRegions--------------------------------------------------//
	//功能：计算输入区域的面积、角度、外接矩形以及轮廓等基本特征数据
	//输入：defectRegions 缺陷区域，输入为多个缺陷区域
	//		   useContour 是否计算缺陷轮廓，true为计算轮廓，false为不计算轮廓，默认为计算
	//		   useRotateRect 计算区域的外接旋转矩形还是正矩形，true为旋转矩形，false为正矩形，默认为旋转矩形
	//		   homMat2D 坐标变换矩阵，用于对缺陷区域进行映射，默认为空
	//输出：area 缺陷面积  <std::vector<double>>
	//		   angle 缺陷角度  <std::vector<double>>
	//		   flawWidth 缺陷宽度  <std::vector<double>>
	//		   flawHeight 缺陷高度  <std::vector<double>>
	//		   centerPoint 缺陷中心点坐标  <std::vector<double>>
	//		   rectCountour 缺陷外接矩形坐标 <std::vector<std::vector<cv::Point2f>>>
	//		   defectContours 缺陷区域轮廓坐标 <std::vector<std::vector<std::vector<cv::Point2f>>>>
	int genFlawInfoFromRegions(HalconCpp::HObject& defectRegions, std::vector<double>& area, std::vector<double>& angle, std::vector<double>& flawWidth, std::vector<double>& flawHeight, std::vector<cv::Point2f>& centerPoint, std::vector<std::vector<cv::Point2f>>& rectCountour, std::vector<std::vector<std::vector<cv::Point2f>>>& defectContours, const bool& useContour = true, const bool& useRotateRect = true, const HalconCpp::HTuple& homMat2D = HalconCpp::HTuple());

	//--------------------------------------genDefectRegion--------------------------------------------------//
	//功能：将绘制的ROI点集转换为检测区域，优先采用ROI点集的形式,若没有传入点集则采用输入的region信息 ，如果点集为空并且检测区域未被初始化，则输出的区域为整图区域
	//输入：原图、检测区域、检测ROI（二维点集）
	//输出：检测区域
	void genDefectRegion(HalconCpp::HObject ho_srcImage, HalconCpp::HObject region, std::vector<std::vector<cv::Point2f>>& inputRois, HalconCpp::HObject* defectRegion);

	//--------------------------------------genContourFromRegion --------------------------------------------//
	//功能：将输入的HalconCpp::HObject区域转换为轮廓点集
	//输入：inputRegion 待转换的HalconCpp::HObject区域，可以为单个区域，也可以为多个区域 <HalconCpp::HObject>
	//		sampleInterval 采样间隔，用于控制轮廓点数量
	//输出：outputContours 轮廓点集 <std::vector<std::vector<cv::Point2f>>>
	void genContourFromRegion(HalconCpp::HObject inputRegion, std::vector<std::vector<cv::Point2f>>& outputContours, int sampleInterval = 1);

	//----------------------------------------------isHalconCpp::HObjectEmpty--------------------------------//
	//功能：判断HalconCpp::HObject是否初始化以及是否为空
	// 输入：hObjectInput 输入HalconCpp::HObject
	// 输出：true ：为空或者未初始化
	//       false：不为空
	bool isHObjectEmpty(HalconCpp::HObject hObjectInput);
	//----------------------------------------------HalconCpp::HObject2Mat--------------------------------//
	// 功能：将HalconCpp::HObject图像转换为Mat图像
	// 输入：_Hobj 输入HalconCpp::HObject
	// 输出：outMat : 输出Mat图
	void HObject2Mat(const HalconCpp::HObject& _Hobj, cv::Mat& outMat);

	//--------------------------------------isHRegionEmpty--------------------------------------------//
	//功能：判断H区域是否初始化以及是否为空
	// 输入：hRegionInput 输入H区域
	// 输出：true ：为空或者未初始化
	//       false：不为空
	bool isHRegionEmpty(HalconCpp::HObject hRegionInput);

	//--------------------------------------genRegionFromContour --------------------------------------------//
	//功能：将输入的轮廓点集转换为HalconCpp::HObject区域
	//输入：inputContour 轮廓点集 <std::vector<std::vector<cv::Point2f>>>
	//输出：outputRegions 转换后的HalconCpp::HObject区域，为多个区域 <HalconCpp::HObject>
	int genRegionFromContour(std::vector<std::vector<cv::Point2f>>& inputContours, HalconCpp::HObject* outputRegions);

	//--------------------------------------genRegionFromContour 重载 --------------------------------------------//
	//功能：将输入的轮廓点集转换为HalconCpp::HObject区域
	//输入：inputContour 轮廓点集 <std::vector<cv::Point2f>>
	//输出：outputRegions 转换后的HalconCpp::HObject区域，为单个区域 <HalconCpp::HObject>
	int genRegionFromContour(std::vector<cv::Point2f>& inputContour, HalconCpp::HObject* outputRegion);

	//--------------------------------------createDirectory --------------------------------------------//
	//功能：创建多级目录 ，要求路径的最后一个文件夹也带着/  比如：D:/wafer/00/
	//char dirPath2[100]; sprintf_s(dirPath2, "%s/%02d/", "D:/savewafer", 0);
	//string path; createDirectory(path)
	int createDirectory(const std::string& directoryPath);

	//--------------------------------------numericalStatic --------------------------------------------//
	//功能：对输入的数值进行统计
	//输入：inputValue 需要进行统计的数值向量 std::vector<double>
	//输出：staticValue 统计变量，包含最小值、平均值、最大值 <std::vector<double>>
	int numericalStatic(const std::vector<double>& inputValue, std::vector<double>& staticValue);

	//--------------------------------------genFlawBasicInfo --------------------------------------------//
	//功能：对输入缺陷特征结构体的基础特征进行初始化赋值操作
	//输入：flaws 输入需要初始化基础特征 std::vector<FlawInfoStruct>
	int  genFlawBasicInfo(std::vector<alglib::core::FlawInfoStruct>& flaws);

	void putTextZH(cv::Mat& dst, const char* str, cv::Point org, cv::Scalar color, int fontSize, const char* fn, bool italic, bool underline);
	cv::Mat wordNewLine(int input_width, int input_height, wchar_t* input_string);

	//--------------------------------------measureOf2DLineMeasure --------------------------------------------//
	//功能：使用2D线测量功能找直线
	//输入：inputValue-输入图像，lineFirstPoint-直线首点，lineLastPoint-直线末点，measureLenght1-测量矩形半长轴，measureLenght1-测量矩形半短轴
	//输入：measureSigma-平滑系数，measureThresh-对比度阈值，polarity-极性[positive-黑到白，negative-白到黑]，select-选点[first-第一个点，last-最后一个点，all-所有点]）
	//输出：hv_MetrologyHandle-测量矩形句柄，hv_lineUpParam-拟合后的直线
	int measureOf2DLineMeasure(HalconCpp::HObject& inputImage, cv::Point2f& lineFirstPoint, cv::Point2f& lineLastPoint, HalconCpp::HTuple measureLenght1, HalconCpp::HTuple measureLenght2,
		HalconCpp::HTuple measureSigma, HalconCpp::HTuple measureThresh, HalconCpp::HTuple polarity, HalconCpp::HTuple select, HalconCpp::HTuple& hv_MetrologyHandle, HalconCpp::HTuple& hv_lineUpParam);

	//--------------------------------------genContourFromHContour----------------------------------//
	//功能：将输入的HalconCpp::HObject轮廓转换为轮廓点集
	//输入：inputRegion 待转换的HalconCpp::HObject轮廓，可以为单个轮廓，也可以为多个轮廓 <HalconCpp::HObject>
	//		sampleInterval 采样间隔，用于控制轮廓点数量
	//输出：outputContours 轮廓点集 <std::vector<std::vector<cv::Point2f>>>
	void genContourFromHContour(HalconCpp::HObject inputContour, std::vector<std::vector<cv::Point2f>>& outputContours, int sampleInterval = 1);

	//--------------------------------------computeBoundingRect----------------------------------//
	//功能：计算输入轮廓的外接矩形,并将轮廓转换为OPENCV格式的轮廓
	//输入：contours 轮廓点集 <std::vector<std::vector<cv::Point2f>>>
	//输出：rect 外接矩形 <Rect>
	//		contoursForDraw OPENCV格式的轮廓 <std::vector<std::vector<Point>>>
	int computeBoundingRect(const std::vector<std::vector<cv::Point2f>>& contours, cv::Rect& rect, std::vector<std::vector<cv::Point>>& contoursForDraw);
}

#endif // ZKHYPROJECTHUANAN_HALCONCODEPUBLIC_H