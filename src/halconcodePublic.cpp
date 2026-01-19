//20230421 增加genFlawInfoFromRegion, computeRectangle1Plygons, computeRectangle1Plygons(重载), hTuple2Points, points2HTuple, thresholdSeg函数。--edited by Guo Xulong
//20230426 增加genDefectRegion模块。--edited by Wang Nannan
//20230504 移除computeRectangle1Plygons(重载)函数。--edited by Guo Xulong
//20230606 1. 修复genFlawInfoFromRegion函数的BUG；2. 增加genContourFromRegion函数用于将输入的HObject区域转换为轮廓点集。--edited by Guo Xulong
//20230610 优化genFlawInfoFromRegion函数，增加对空区域的处理。--edited by Guo Xulong
//20230626 1. 新增genFlawInfoFromRegions函数，用于计算多个缺陷区域的基本信息；2. 更改genFlawInfoFromRegion函数部分内部变量名称(不影响原有接口调用)。--edited by Guo Xulong
//20230628 1. 新增hTuple2Points3D、points3D2HTuple函数，用于HTuple和 points3f的转换。--edited by Qiu Yi
//20230705 1. 新增isHObjectEmpty函数，用于判断输入的HObject是否为空；2. 修复genFlawInfoFromRegion函数和genFlawInfoFromRegions函数输入缺陷区域为空时导致的算法异常。--edited by Guo Xulong
//20230725 1. 新增HObject2Mat函数，用于将HObject图像转换为Mat图像。--edited by Guo Xulong
//20230803 1. 新增isHRegionEmpty，用于判断H区域是否初始化以及是否为空。--edited by Guo Xulong
//20230824 1. 新增genRegionFromContour，用于将输入的轮廓点集（<vector<vector<Point2f>>>或<vector<Point2f>>）转换为HObject区域。--edited by Guo Xulong
//20230829 1. 修复genRegionFromContour中的BUG。--edited by Guo Xulong
//20231118 1. 新增numericalStatic，用于对输入的数值进行统计。--edited by Guo Xulong
//		   2. 新增createDirectory，用于创建多级目录。 -- --edited by Wang Nannan
//20231130 1. 新增genFlawBasicInfo，用于对输入缺陷特征结构体的基础特征进行初始化赋值操作。--edited by Guo Xulong
//20231228 genContourFromRegion函数：1. 增加对输入数据的判断，手动释放部分变量空间。--edited by Guo Xulong
//20240218 hTuple2Points函数：1. 代码优化。 hTuple2Points重载：1. 支持选择转换后的轮廓是否为闭合点集。新增genContourFromHContour函数：1. 将输入的HObject区域转换为轮廓点集。--edited by Guo Xulong
//20240226 genContourFromRegion函数：1. 更新轮廓计算方法，支持转换内外轮廓。--edited by Guo Xulong
//20240409 genContourFromRegion函数，hTuple2Points函数，genContourFromHContour函数：1. 增加采样间隔，用于控制轮廓点数量。--edited by Guo Xulong

#include <algorithm>
#include <direct.h>

#include <numeric>
#include <stdio.h>
#include <vector>
#include <windows.h>
#include <io.h>

#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;


//--------------------------------------regionsToInfo Start--------------------------------------------------//
void zkhyPublicFuncHN::regionsToInfo(HObject ho_defectRegions, HTuple* hv_Rows, HTuple* hv_Columns,
	HTuple* hv_Len1s, HTuple* hv_Len2s, HTuple* hv_Areas, HTuple* hv_row1s, HTuple* hv_col1s,
	HTuple* hv_row2s, HTuple* hv_col2s, HTuple* hv_row3s, HTuple* hv_col3s, HTuple* hv_row4s,
	HTuple* hv_col4s)
{

	// Local iconic variables
	HObject  ho_ObjectSelected;

	// Local control variables
	HTuple  hv__, hv_i, hv_Row11, hv_Column11, hv_Phi;
	HTuple  hv_Length11, hv_Length12, hv_Cos, hv_Sin, hv_pt_x;
	HTuple  hv_pt_y;

	(*hv_Len1s) = HTuple();
	(*hv_Len2s) = HTuple();
	(*hv_Areas) = HTuple();
	(*hv_Rows) = HTuple();
	(*hv_Columns) = HTuple();
	AreaCenter(ho_defectRegions, &(*hv_Areas), &(*hv_Rows), &(*hv_Columns));
	SmallestRectangle2(ho_defectRegions, &hv__, &hv__, &hv__, &(*hv_Len1s), &(*hv_Len2s));
	*hv_Len1s = *hv_Len1s * 2 + 1;
	*hv_Len2s = *hv_Len2s * 2 + 1;
	(*hv_row1s) = HTuple();
	(*hv_col1s) = HTuple();
	(*hv_row2s) = HTuple();
	(*hv_col2s) = HTuple();
	(*hv_row3s) = HTuple();
	(*hv_col3s) = HTuple();
	(*hv_row4s) = HTuple();
	(*hv_col4s) = HTuple();
	{
		HTuple end_val15 = (*hv_Rows).TupleLength();
		HTuple step_val15 = 1;
		for (hv_i = 1; hv_i.Continue(end_val15, step_val15); hv_i += step_val15)
		{
			SelectObj(ho_defectRegions, &ho_ObjectSelected, hv_i);
			SmallestRectangle2(ho_ObjectSelected, &hv_Row11, &hv_Column11, &hv_Phi, &hv_Length11,
				&hv_Length12);
			TupleCos(hv_Phi, &hv_Cos);
			TupleSin(hv_Phi, &hv_Sin);
			//dev_set_color ('green')
			hv_pt_x.Clear();
			hv_pt_x[0] = 0;
			hv_pt_x[1] = 0;
			hv_pt_x[2] = 0;
			hv_pt_x[3] = 0;
			hv_pt_y.Clear();
			hv_pt_y[0] = 0;
			hv_pt_y[1] = 0;
			hv_pt_y[2] = 0;
			hv_pt_y[3] = 0;
			hv_pt_x[0] = hv_Column11 - ((hv_Length11 * hv_Cos) + (hv_Length12 * hv_Sin));
			hv_pt_y[0] = hv_Row11 + ((hv_Length11 * hv_Sin) - (hv_Length12 * hv_Cos));
			//gen_cross_contour_xld (Cross, pt_y[0], pt_x[0], 6, Phi)

			hv_pt_x[1] = hv_Column11 + ((hv_Length11 * hv_Cos) - (hv_Length12 * hv_Sin));
			hv_pt_y[1] = hv_Row11 - ((hv_Length11 * hv_Sin) + (hv_Length12 * hv_Cos));
			//gen_cross_contour_xld (Cross, pt_y[1], pt_x[1], 6, Phi)

			hv_pt_x[2] = hv_Column11 + ((hv_Length11 * hv_Cos) + (hv_Length12 * hv_Sin));
			hv_pt_y[2] = hv_Row11 - ((hv_Length11 * hv_Sin) - (hv_Length12 * hv_Cos));
			//gen_cross_contour_xld (Cross, pt_y[2], pt_x[2], 6, Phi)

			hv_pt_x[3] = hv_Column11 + (((-hv_Length11) * hv_Cos) + (hv_Length12 * hv_Sin));
			hv_pt_y[3] = hv_Row11 - (((-hv_Length11) * hv_Sin) - (hv_Length12 * hv_Cos));
			//gen_cross_contour_xld (Cross, pt_y[2], pt_x[2], 6, Phi)

			(*hv_row1s) = (*hv_row1s).TupleConcat(HTuple(hv_pt_y[0]));
			(*hv_col1s) = (*hv_col1s).TupleConcat(HTuple(hv_pt_x[0]));
			(*hv_row2s) = (*hv_row2s).TupleConcat(HTuple(hv_pt_y[1]));
			(*hv_col2s) = (*hv_col2s).TupleConcat(HTuple(hv_pt_x[1]));
			(*hv_row3s) = (*hv_row3s).TupleConcat(HTuple(hv_pt_y[2]));
			(*hv_col3s) = (*hv_col3s).TupleConcat(HTuple(hv_pt_x[2]));
			(*hv_row4s) = (*hv_row4s).TupleConcat(HTuple(hv_pt_y[3]));
			(*hv_col4s) = (*hv_col4s).TupleConcat(HTuple(hv_pt_x[3]));
		}
	}

	//
	//Display the results
	//stop ()
	return;
}
//--------------------------------------regionsToInfo End---------------------------------------------------//

//--------------------------------------thresholdSeg Start--------------------------------------------------//
int zkhyPublicFuncHN::thresholdSeg(HObject& image, HObject& region, HTuple& thresholdLow, const HTuple& thresholdHigh, HTuple& targetProperty, const int& func)
{
	HTuple hThresholdUsed;
	if (func == 0)
	{	//固定阈值
		Threshold(image, &region, thresholdLow, thresholdHigh);
	}
	else if (func == 1)
	{	//大津法 Otsu
		BinaryThreshold(image, &region, "max_separability", targetProperty, &hThresholdUsed);
		thresholdLow = hThresholdUsed;
	}
	return 0;
}
//--------------------------------------thresholdSeg End---------------------------------------------------//

//--------------------------------------hTuple2Points Start--------------------------------------------------//
void zkhyPublicFuncHN::hTuple2Points(HTuple& x, HTuple& y, vector<Point2f>& points, int sampleInterval)
{
	HTuple hSize;
	TupleLength(x, &hSize);
	int pointNum = hSize.I() / sampleInterval;
	points.resize(pointNum);
	int i = 0;
	int j = 0;
	for (int i = 0; j < pointNum; i += sampleInterval, ++j)
	{
		Point2f pointTmp;
		pointTmp.x = x[i].D();
		pointTmp.y = y[i].D();
		points[j] = pointTmp;
	}

	if (i != hSize.I())
	{
		Point2f pointTmp;
		pointTmp.x = x[hSize.I() - 1].D();
		pointTmp.y = y[hSize.I() - 1].D();
		points.push_back(pointTmp);
	}
}
//--------------------------------------hTuple2Points End---------------------------------------------------//
//--------------------------------------hTuple2Points 重载 Start--------------------------------------------------//
void zkhyPublicFuncHN::hTuple2Points(HTuple& x, HTuple& y, vector<Point2f>& points, bool& isClosedInput)
{
	if (isClosedInput == false)
		zkhyPublicFuncHN::hTuple2Points(x, y, points);
	else
	{
		HTuple hSize;
		TupleLength(x, &hSize);
		int pointNum = hSize.I();
		points.resize(pointNum + 1);
		for (int i = 0; i < pointNum; ++i)
		{
			Point2f pointTmp;
			pointTmp.x = x[i].D();
			pointTmp.y = y[i].D();
			points[i] = pointTmp;
		}
		// 最后一个点与第一个点相同
		points[pointNum] = points[0];
	}
}
//--------------------------------------hTuple2Points 重载End------------------------------------------------//

//--------------------------------------points2HTuple Start--------------------------------------------------//
void zkhyPublicFuncHN::points2HTuple(vector<Point2f>& points, HTuple& x, HTuple& y)
{
	int pointsNum = points.size();
	HTuple hTmpX(pointsNum, -1), hTmpY(pointsNum, -1);
	for (int i = 0; i < points.size(); i++)
	{
		hTmpX[i] = points[i].x;
		hTmpY[i] = points[i].y;
	}
	x = hTmpX.Clone();
	y = hTmpY.Clone();
}
//--------------------------------------points2HTuple End---------------------------------------------------//

//--------------------------------------hTuple2Points3D Start--------------------------------------------------//
void zkhyPublicFuncHN::hTuple2Points3D(HTuple x, HTuple y, HTuple z, vector<Point3f>& points)
{
	HTuple hSize;
	TupleLength(x, &hSize);
	for (int i = 0; i < hSize.I(); i++)
	{
		Point3f pointTmp;
		pointTmp.x = x[i].D();
		pointTmp.y = y[i].D();
		pointTmp.z = z[i].D();
		points.push_back(pointTmp);
	}
}

//--------------------------------------points3D2HTuple Start--------------------------------------------------//
void zkhyPublicFuncHN::points3D2HTuple(vector<Point3f> points, HTuple& x, HTuple& y, HTuple& z)
{

	int pointsNum = points.size();
	HTuple pointsX, pointsY, pointsZ;
	//for (int i = 0; i < pointsNum; i++)
	//{
	//	pointsX.TupleConcat(points[i].x);
	//	pointsY.TupleConcat(points[i].y);
	//	pointsZ.TupleConcat(points[i].z);
	//}

	TupleGenConst(pointsNum, 0, &pointsX);
	TupleGenConst(pointsNum, 0, &pointsY);
	TupleGenConst(pointsNum, 0, &pointsZ);
	for (int i = 0; i < pointsNum; i++)
	{
		pointsX[i] = points[i].x;
		pointsY[i] = points[i].y;
		pointsZ[i] = points[i].z;
	}

	x = pointsX;
	y = pointsY;
	z = pointsZ;
}


//--------------------------------------computeRectangle1Plygons Start--------------------------------------------------//
void zkhyPublicFuncHN::computeRectangle1Plygon(HTuple row1, HTuple col1, HTuple row2, HTuple col2, vector<Point2f>& polygon, HTuple hHomMat2D)
{
	HTuple hIsTrans;
	TupleLength(hHomMat2D, &hIsTrans);
	Point2f pointTmp;
	polygon.resize(5);
	if (hIsTrans.I() == 0)
	{			// 不进行仿射变换
		pointTmp.x = col1;            //左上
		pointTmp.y = row1;
		polygon[0] = pointTmp;
		pointTmp.x = col1;            //左下
		pointTmp.y = row2;
		polygon[1] = pointTmp;
		pointTmp.x = col2;            //右下
		pointTmp.y = row2;
		polygon[2] = pointTmp;
		pointTmp.x = col2;            //右上
		pointTmp.y = row1;
		polygon[3] = pointTmp;
		pointTmp.x = col1;            //左上，需要闭合曲线
		pointTmp.y = row1;
		polygon[4] = pointTmp;
	}
	else
	{
		HTuple row1Trans, col1Trans, row2Trans, col2Trans, row3Trans, col3Trans, row4Trans, col4Trans;

		//左上
		AffineTransPixel(hHomMat2D, row1, col1, &row1Trans, &col1Trans);
		pointTmp.x = col1Trans;
		pointTmp.y = row1Trans;
		polygon[0] = pointTmp;

		//左下
		AffineTransPixel(hHomMat2D, row2, col1, &row2Trans, &col2Trans);
		pointTmp.x = col2Trans;
		pointTmp.y = row2Trans;
		polygon[1] = pointTmp;

		//右下
		AffineTransPixel(hHomMat2D, row2, col2, &row3Trans, &col3Trans);
		pointTmp.x = col3Trans;
		pointTmp.y = row3Trans;
		polygon[2] = pointTmp;

		//右上
		AffineTransPixel(hHomMat2D, row1, col2, &row4Trans, &col4Trans);
		pointTmp.x = col4Trans;
		pointTmp.y = row4Trans;
		polygon[3] = pointTmp;

		pointTmp.x = col1Trans;            //左上，需要闭合曲线
		pointTmp.y = row1Trans;
		polygon[4] = pointTmp;
	}
}
//--------------------------------------computeRectangle1Plygons End---------------------------------------------------//

// test

//--------------------------------------genFlawInfoFromRegion Start--------------------------------------------------//
int  zkhyPublicFuncHN::genFlawInfoFromRegion(HObject& defectRegion, double& area, double& angle, double& flawWidth, double& flawHeight, Point2f& centerPoint, vector<Point2f>& rectContour, vector<vector<Point2f>>& defectContours, const bool& useContour, const bool& useRotateRect, const HTuple& homMat2D)
{	// 单个缺陷生成缺陷信息
	/* 使用示例：
		FlawInfoStruct flawTmp;
		double areaTmp, angleTmp, flawWidth, flawHeight;
		Point2f centerPoint;
		vector<Point2f> rectContour;
		vector<vector<Point2f>> defectContours;
		genFlawInfoFromRegion(defectRegion, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectContour, defectContours, useContour, useRotateRect, homMat2D);
		flawTmp.FlawBasicInfo.flawArea = areaTmp;
		flawTmp.FlawBasicInfo.angle = angleTmp;
		flawTmp.FlawBasicInfo.flawWidth = flawWidth;
		flawTmp.FlawBasicInfo.flawHeight = flawHeight;
		flawTmp.FlawBasicInfo.centerPt = centerPoint;
		flawTmp.FlawBasicInfo.pts = rectContour;
		flawTmp.FlawBasicInfo.flawContours = defectContours;
		flawTmp.FlawBasicInfo.type = 0;
	*/
	if (isHObjectEmpty(defectRegion))
	{	// 区域为空
		area = 0;
		angle = 0;
		flawWidth = 0;
		flawHeight = 0;
		centerPoint = cv::Point2f(0, 0);
		rectContour = vector<cv::Point2f>(0);
		defectContours = vector<vector<cv::Point2f>>(0);
		return 0;
	}
	HTuple isAffineTrans;
	TupleLength(homMat2D, &isAffineTrans);
	HObject defectRegionTrans = defectRegion;
	if (isAffineTrans.I() != 0)
		AffineTransRegion(defectRegion, &defectRegionTrans, homMat2D, "nearest_neighbor");

	try
	{
		// 计算外接矩形
		if (useRotateRect)
		{	// 计算区域的外接旋转矩形
			HTuple rowTmp, colTmp, len1Tmp, len2Tmp, angleTmp, cosAngle, sinAngle;
			SmallestRectangle2(defectRegionTrans, &rowTmp, &colTmp, &angleTmp, &len1Tmp, &len2Tmp);
			vector<Point2f> rectContourTmp(5);
			TupleCos(angleTmp, &cosAngle);
			TupleSin(angleTmp, &sinAngle);
			// 依次计算缺陷的角点
			rectContourTmp[0].x = colTmp.D() - (len1Tmp.D() * cosAngle.D() + len2Tmp.D() * sinAngle.D());
			rectContourTmp[0].y = rowTmp.D() + (len1Tmp.D() * sinAngle.D() - len2Tmp.D() * cosAngle.D());
			rectContourTmp[1].x = colTmp.D() + (len1Tmp.D() * cosAngle.D() - len2Tmp.D() * sinAngle.D());
			rectContourTmp[1].y = rowTmp.D() - (len1Tmp.D() * sinAngle.D() + len2Tmp.D() * cosAngle.D());
			rectContourTmp[2].x = colTmp.D() + (len1Tmp.D() * cosAngle.D() + len2Tmp.D() * sinAngle.D());
			rectContourTmp[2].y = rowTmp.D() - (len1Tmp.D() * sinAngle.D() - len2Tmp.D() * cosAngle.D());
			rectContourTmp[3].x = colTmp.D() - (len1Tmp.D() * cosAngle.D() - len2Tmp.D() * sinAngle.D());
			rectContourTmp[3].y = rowTmp.D() + (len1Tmp.D() * sinAngle.D() + len2Tmp.D() * cosAngle.D());
			rectContourTmp[4].x = rectContourTmp[0].x;
			rectContourTmp[4].y = rectContourTmp[0].y;
			rectContour = rectContourTmp;
			angle = angleTmp.D();
			flawWidth = 2 * len1Tmp.D() + 1;
			flawHeight = 2 * len2Tmp.D() + 1;
		}
		else
		{	// 计算区域的正外接矩形
			HTuple rowTmp1, colTmp1, rowTmp2, colTmp2, isAffineTransTmp;
			SmallestRectangle1(defectRegion, &rowTmp1, &colTmp1, &rowTmp2, &colTmp2);
			vector<Point2f> rectContourTmp(5);
			TupleLength(homMat2D, &isAffineTransTmp);
			if (isAffineTransTmp.I() == 0)
				computeRectangle1Plygon(rowTmp1, colTmp1, rowTmp2, colTmp2, rectContourTmp);
			else
				computeRectangle1Plygon(rowTmp1, colTmp1, rowTmp2, colTmp2, rectContourTmp, homMat2D);
			rectContour = rectContourTmp;
			HTuple rowTmp, colTmp, len1Tmp, len2Tmp, angleTmp;
			SmallestRectangle2(defectRegion, &rowTmp, &colTmp, &angleTmp, &len1Tmp, &len2Tmp);
			angle = angleTmp.D();
			flawWidth = 2 * len1Tmp.D() + 1;
			flawHeight = 2 * len2Tmp.D() + 1;
		}

		// 计算中心点，角度
		HTuple rowCenter, colCenter, areaTmp;
		AreaCenter(defectRegionTrans, &areaTmp, &rowCenter, &colCenter);
		area = areaTmp.D();
		centerPoint = Point2f(colCenter.D(), rowCenter.D());

		// 计算轮廓点
		if (useContour)
		{
			genContourFromRegion(defectRegionTrans, defectContours);
		}
	}
	catch (HException& except)
	{
		char debugInfo[256];
		sprintf_s(debugInfo, "[Alg Debug][Alg Error] Alg_Name:[genFlawInfoFromRegion] errMsg: %s\n", except.ErrorMessage().Text());
		OutputDebugStringA(debugInfo);
		return 1;
	}
	return 0;
}
//--------------------------------------genFlawInfoFromRegion End---------------------------------------------------//
// 
// //--------------------------------------genFlawInfoFromRegions Start--------------------------------------------------//
int  zkhyPublicFuncHN::genFlawInfoFromRegions(HObject& defectRegions, vector<double>& area, vector<double>& angle, vector<double>& flawWidth, vector<double>& flawHeight, vector<Point2f>& centerPoint, vector<vector<Point2f>>& rectContour, vector<vector<vector<Point2f>>>& defectContours, const bool& useContour, const bool& useRotateRect, const HTuple& homMat2D)
{	// 多个缺陷生成缺陷信息
	/* 使用示例：
		HTuple hFlawNum;
		// 缺陷个数
		CountObj(defectRegions, &hFlawNum);
		int flawNum = hFlawNum.I();

		vector<FlawInfoStruct> flawTmp(flawNum);
		vector<double> areaTmp(flawNum);
		vector<double> angleTmp(flawNum);
		vector<double> flawWidth(flawNum);
		vector<double> flawHeight(flawNum);
		vector<Point2f> centerPoint;
		vector<vector<Point2f>> rectContour;
		vector<vector<vector<Point2f>>> defectContours;
		genFlawInfoFromRegions(defectRegions, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectContour, defectContours, useContour, useRotateRect, homMat2D);

		for(int i = 0; i < flawNum; i++)
		{
			flawTmp[i].FlawBasicInfo.flawArea = areaTmp[i];
			flawTmp[i].FlawBasicInfo.angle = angleTmp[i];
			flawTmp[i].FlawBasicInfo.flawWidth = flawWidth[i];
			flawTmp[i].FlawBasicInfo.flawHeight = flawHeight[i];
			flawTmp[i].FlawBasicInfo.centerPt = centerPoint[i];
			flawTmp[i].FlawBasicInfo.pts = rectContour[i];
			flawTmp[i].FlawBasicInfo.flawContours = defectContours[i];
			flawTmp[i].FlawBasicInfo.type = 0;
		}
	*/
	if (isHObjectEmpty(defectRegions))
	{	// 区域为空
		area.clear();
		angle.clear();
		flawWidth.clear();
		flawHeight.clear();
		centerPoint.clear();
		rectContour.clear();
		defectContours.clear();
		return 0;
	}
	HTuple isAffineTrans, hFlawNum;
	TupleLength(homMat2D, &isAffineTrans);
	HObject defectRegionTrans = defectRegions;
	if (isAffineTrans.I() != 0)
		AffineTransRegion(defectRegions, &defectRegionTrans, homMat2D, "nearest_neighbor");

	CountObj(defectRegionTrans, &hFlawNum);   // 缺陷个数
	int flawNum = hFlawNum.I();
	// 初始化输出向量
	area.resize(flawNum);
	angle.resize(flawNum);
	flawWidth.resize(flawNum);
	flawHeight.resize(flawNum);
	centerPoint.resize(flawNum);
	rectContour.resize(flawNum);
	defectContours.resize(flawNum);
	try
	{
		if (useRotateRect)
		{	// 计算区域的外接旋转矩形
			HTuple rowTmp, colTmp, len1Tmp, len2Tmp, angleTmp, cosAngle, sinAngle, rectContourPoints1X, rectContourPoints1Y, rectContourPoints2X, rectContourPoints2Y, rectContourPoints3X, rectContourPoints3Y, rectContourPoints4X, rectContourPoints4Y;
			SmallestRectangle2(defectRegionTrans, &rowTmp, &colTmp, &angleTmp, &len1Tmp, &len2Tmp);
			TupleCos(angleTmp, &cosAngle);
			TupleSin(angleTmp, &sinAngle);
			// 依次计算缺陷的角点
			rectContourPoints1X = colTmp - (len1Tmp.D() * cosAngle.D() + len2Tmp.D() * sinAngle.D());
			rectContourPoints1Y = rowTmp + (len1Tmp.D() * sinAngle.D() - len2Tmp.D() * cosAngle.D());
			rectContourPoints2X = colTmp + (len1Tmp.D() * cosAngle.D() - len2Tmp.D() * sinAngle.D());
			rectContourPoints2Y = rowTmp - (len1Tmp.D() * sinAngle.D() + len2Tmp.D() * cosAngle.D());
			rectContourPoints3X = colTmp + (len1Tmp.D() * cosAngle.D() + len2Tmp.D() * sinAngle.D());
			rectContourPoints3Y = rowTmp - (len1Tmp.D() * sinAngle.D() - len2Tmp.D() * cosAngle.D());
			rectContourPoints4X = colTmp - (len1Tmp.D() * cosAngle.D() - len2Tmp.D() * sinAngle.D());
			rectContourPoints4Y = rowTmp + (len1Tmp.D() * sinAngle.D() + len2Tmp.D() * cosAngle.D());

			// 填充缺陷信息
			for (int i = 0; i < flawNum; ++i)
			{
				vector<Point2f> rectContourTmp(5);
				rectContourTmp[0].x = rectContourPoints1X[i].D();
				rectContourTmp[0].y = rectContourPoints1Y[i].D();
				rectContourTmp[1].x = rectContourPoints2X[i].D();
				rectContourTmp[1].y = rectContourPoints2Y[i].D();
				rectContourTmp[2].x = rectContourPoints3X[i].D();
				rectContourTmp[2].y = rectContourPoints3Y[i].D();
				rectContourTmp[3].x = rectContourPoints4X[i].D();
				rectContourTmp[3].y = rectContourPoints4Y[i].D();
				rectContourTmp[4].x = rectContourPoints1X[i].D();
				rectContourTmp[4].y = rectContourPoints1Y[i].D();
				rectContour[i] = rectContourTmp;
				angle[i] = angleTmp[i].D();
				flawWidth[i] = 2 * len1Tmp[i].D() + 1;
				flawHeight[i] = 2 * len2Tmp[i].D() + 1;
			}

		}
		else
		{	// 计算区域的正外接矩形
			HTuple rowTmp1, colTmp1, rowTmp2, colTmp2, isAffineTransTmp;
			SmallestRectangle1(defectRegions, &rowTmp1, &colTmp1, &rowTmp2, &colTmp2);
			HTuple rowTmp, colTmp, len1Tmp, len2Tmp, angleTmp;
			SmallestRectangle2(defectRegions, &rowTmp, &colTmp, &angleTmp, &len1Tmp, &len2Tmp);
			// 填充缺陷信息
			for (int i = 0; i < flawNum; ++i)
			{
				vector<Point2f> rectContourTmp(5);
				TupleLength(homMat2D, &isAffineTransTmp);
				if (isAffineTransTmp.I() == 0)
					computeRectangle1Plygon(rowTmp1[i], colTmp1[i], rowTmp2[i], colTmp2[i], rectContourTmp);
				else
					computeRectangle1Plygon(rowTmp1[i], colTmp1[i], rowTmp2[i], colTmp2[i], rectContourTmp, homMat2D);
				rectContour[i] = rectContourTmp;
				angle[i] = angleTmp[i].D();
				flawWidth[i] = 2 * len1Tmp[i].D() + 1;
				flawHeight[i] = 2 * len2Tmp[i].D() + 1;
			}
		}

		// 计算中心点，角度
		HTuple rowCenter, colCenter, areaTmp;
		AreaCenter(defectRegionTrans, &areaTmp, &rowCenter, &colCenter);
		for (int i = 0; i < flawNum; ++i)
		{
			area[i] = areaTmp[i].D();
			centerPoint[i] = Point2f(colCenter[i].D(), rowCenter[i].D());
		}

		// 计算轮廓点
		if (useContour)
		{
			for (int i = 0; i < flawNum; ++i)
			{
				vector<vector<Point2f>> defectContoursTmp;
				HObject defectRegionTransTmp;
				SelectObj(defectRegionTrans, &defectRegionTransTmp, i + 1);
				genContourFromRegion(defectRegionTransTmp, defectContoursTmp);
				defectContours[i] = defectContoursTmp;
			}
		}
	}
	catch (HException& except)
	{
		char debugInfo[256];
		sprintf_s(debugInfo, "[Alg Debug][Alg Error] Alg_Name:[genFlawInfoFromRegions] errMsg: %s\n", except.ErrorMessage().Text());
		OutputDebugStringA(debugInfo);
		return 1;
	}
	return 0;
}
//--------------------------------------genFlawInfoFromRegions End---------------------------------------------------//
//--------------------------------------genDefectRegion Begin--------------------------------------------//
//功能：将绘制的ROI点集转换为检测区域，优先采用ROI点集的形式,若没有传入点集则采用输入的region信息 ，如果点集为空并且检测区域未被初始化，则输出的区域为整图区域
//输入：原图、检测区域、检测ROI（二维点集）
//输出：检测区域
void zkhyPublicFuncHN::genDefectRegion(HObject ho_srcImage, HObject region, vector<vector<Point2f>>& inputRois, HObject* defectRegion)
{
	GenEmptyObj(&(*defectRegion));
	//计算正向ROI
	if (inputRois.size() != 0) {
		for (int i = 0; i < inputRois.size(); i++) {
			HObject subRegion;
			GenEmptyObj(&subRegion);
			HTuple hv_rows, hv_cols;
			for (int j = 0; j < inputRois[i].size(); j++) {
				hv_rows.Append(inputRois[i][j].y);
				hv_cols.Append(inputRois[i][j].x);
			}
			GenRegionPolygonFilled(&subRegion, hv_rows, hv_cols);
			ConcatObj(*defectRegion, subRegion, &(*defectRegion));
		}
	}
	else if (region.IsInitialized()) {
		*defectRegion = region;
	}
	else
		GetDomain(ho_srcImage, &(*defectRegion));
	return;
}
//--------------------------------------genDefectRegion End--------------------------------------------//

//--------------------------------------genContourFromRegion Begin--------------------------------------------//
//功能：将输入的HObject区域转换为轮廓点集
//输入：inputRegion 待转换的HObject区域，可以为单个区域，也可以为多个区域 <HObject>
//输出：outputContours 轮廓点集 <vector<vector<Point2f>>>
void zkhyPublicFuncHN::genContourFromRegion(HObject inputRegion, vector<vector<Point2f>>& outputContours, int sampleInterval)
{
	if (isHObjectEmpty(inputRegion) || isHRegionEmpty(inputRegion))
		return;

	HObject regionContours;
	GenContourRegionXld(inputRegion, &regionContours, "border_holes");
	genContourFromHContour(regionContours, outputContours, sampleInterval);
#if 0
	HTuple hRegionNum;
	HObject regionsConnected;
	Connection(inputRegion, &regionsConnected);
	CountObj(regionsConnected, &hRegionNum);   // 区域数
	int regionNum = hRegionNum.I();
	outputContours.resize(regionNum);
	for (int i = 1; i <= hRegionNum; ++i) {
		HObject selectRegion;
		vector<Point2f> contourTmp;
		SelectObj(regionsConnected, &selectRegion, i);
		HTuple rowContour, colContour;
		GetRegionContour(selectRegion, &rowContour, &colContour);
		hTuple2Points(colContour, rowContour, contourTmp);
		outputContours[i - 1] = contourTmp;

		// release contourTmp
		vector<Point2f>().swap(contourTmp);
	}
#endif
}
//--------------------------------------genContourFromRegion End--------------------------------------------//

//--------------------------------------isHObjectEmpty Begin--------------------------------------------//
//功能：判断HObject是否初始化以及是否为空
// 输入：hObjectInput 输入HObject
// 输出：true ：为空或者未初始化
//       false：不为空
bool zkhyPublicFuncHN::isHObjectEmpty(HObject hObjectInput)
{
	if (hObjectInput.IsInitialized() == false)
		return true;           // 输入HObject为空，HObject未初始化
	HObject hEmpty;
	HTuple hIsEuqal;
	GenEmptyObj(&hEmpty);
	TestEqualObj(hObjectInput, hEmpty, &hIsEuqal);
	if (hIsEuqal.I() == 1)
	{
		return true;           // 输入图像为空
	}
	return false;
}
//--------------------------------------isHObjectEmpty End--------------------------------------------//

//--------------------------------------HObject2Mat Begin--------------------------------------------//
// 功能：将HObject图像转换为Mat图像
// 输入：_Hobj 输入HObject
// 输出：outMat : 输出Mat图
void zkhyPublicFuncHN::HObject2Mat(const HalconCpp::HObject& _Hobj, cv::Mat& outMat)
{
	HalconCpp::HTuple htCh = HalconCpp::HTuple();
	HalconCpp::HString cType;
	HalconCpp::HObject Hobj;
	HalconCpp::ConvertImageType(_Hobj, &Hobj, "byte");
	HalconCpp::CountChannels(Hobj, &htCh);
	Hlong w;
	Hlong h;
	if (htCh[0].I() == 1)
	{
		void* ptr = ((HalconCpp::HImage)Hobj).GetImagePointer1(&cType, &w, &h);
		outMat = Mat(h, w, CV_8UC1, static_cast<unsigned char*>(ptr));
	}
	else if (htCh[0].I() == 3)
	{
		HTuple tr, tg, tb, tType, tW, tH;
		GetImagePointer3(_Hobj, &tr, &tg, &tb, &tType, &tW, &tH);
		std::vector<cv::Mat> vecM(3);
		vecM[2] = Mat(tH.I(), tW.I(), CV_8UC1, (void*)tr.L());
		vecM[1] = Mat(tH.I(), tW.I(), CV_8UC1, (void*)tg.L());
		vecM[0] = Mat(tH.I(), tW.I(), CV_8UC1, (void*)tb.L());
		merge(vecM, outMat);
	}
}
//--------------------------------------HObject2Mat2 End--------------------------------------------//

//--------------------------------------isHRegionEmpty Begin--------------------------------------------//
//功能：判断H区域是否初始化以及是否为空
// 输入：hRegionInput 输入H区域
// 输出：true ：为空或者未初始化
//       false：不为空
bool zkhyPublicFuncHN::isHRegionEmpty(HObject hRegionInput)
{
	if (hRegionInput.IsInitialized() == false)
		return true;           // 输入区域为空，H区域未初始化
	HObject hEmpty;
	HTuple hIsEuqal;
	GenEmptyRegion(&hEmpty);
	TestEqualRegion(hRegionInput, hEmpty, &hIsEuqal);
	if (hIsEuqal.I() == 1)
	{
		return true;           // 输入区域为空
	}
	return false;
}
//--------------------------------------isHRegionEmpty End--------------------------------------------//

//--------------------------------------genRegionFromContour Begin--------------------------------------------//
//功能：将输入的轮廓点集转换为HObject区域
//输入：inputContour 轮廓点集 <vector<vector<Point2f>>>
//输出：outputRegions 转换后的HObject区域，为多个区域 <HObject>
int zkhyPublicFuncHN::genRegionFromContour(vector<vector<Point2f>>& inputContours, HObject* outputRegions)
{
	GenEmptyObj(outputRegions);
	int contourNum = inputContours.size();
	try {
		for (int i = 0; i < contourNum; ++i) {
			HObject regionTmp;
			genRegionFromContour(inputContours[i], &regionTmp);
			ConcatObj(*outputRegions, regionTmp, outputRegions);
		}
	}
	catch (HalconCpp::HException& e) {
		return 2;
	}
	return 0;
}
//--------------------------------------genContourFromRegion End--------------------------------------------//

//--------------------------------------genRegionFromContour 重载 Begin--------------------------------------------//
//功能：将输入的轮廓点集转换为HObject区域
//输入：inputContour 轮廓点集 <vector<Point2f>>
//输出：outputRegions 转换后的HObject区域，为单个区域 <HObject>
int zkhyPublicFuncHN::genRegionFromContour(vector<Point2f>& inputContour, HObject* outputRegion)
{
	GenEmptyObj(outputRegion);
	HTuple hPointsRow, hPointsCol;
	points2HTuple(inputContour, hPointsCol, hPointsRow);
	try {
		GenRegionPolygonFilled(outputRegion, hPointsRow, hPointsCol);
	}
	catch (HalconCpp::HException& e) {
		return 2;
	}
	return 0;
}
//--------------------------------------genContourFromRegion End--------------------------------------------//

//--------------------------------------createDirectory Begin--------------------------------------------// 
//创建多级目录
//char dirPath2[100]; sprintf_s(dirPath2, "%s/%02d/", "D:/savewafer", 0);
//string path; createDirectory(path)
int  zkhyPublicFuncHN::createDirectory(const std::string& directoryPath)
{
	int dirPathLen = directoryPath.length();
	if (dirPathLen > MAX_PATH)
	{
		return 1;
	}
	if (_access(directoryPath.data(), 0) == 0)
		return 0;
	char tmpDirPath[MAX_PATH] = { 0 };
	for (uint32_t i = 0; i < dirPathLen; ++i)
	{
		tmpDirPath[i] = directoryPath[i];
		if (tmpDirPath[i] == '\\' || tmpDirPath[i] == '/' || i == dirPathLen - 1)
		{
			if (_access(tmpDirPath, 0) != 0)
			{
				int ret = _mkdir(tmpDirPath);
				if (ret != 0)
				{
					return ret;
				}
			}
		}
	}
	return 0;
}
//--------------------------------------createDirectory End--------------------------------------------//

//--------------------------------------numericalStatic Begin--------------------------------------------//
	//功能：对输入的数值进行统计
	//输入：inputValue 需要进行统计的数值向量 vector<double>
	//输出：staticValue 统计变量，包含最小值、平均值、最大值 <vector<double>>
int zkhyPublicFuncHN::numericalStatic(const vector<double>& inputValue, vector<double>& staticValue)
{
	staticValue.resize(3);
	// 判断输入是否为空,如果为空则返回-9999
	if (inputValue.size() == 0)
	{
		// 计算最小值
		staticValue[0] = -9999;
		// 计算均值
		staticValue[1] = -9999;
		// 计算最大值
		staticValue[2] = -9999;
	}
	else
	{
		// 计算最小值
		staticValue[0] = *min_element(inputValue.begin(), inputValue.end());
		// 计算均值
		staticValue[1] = accumulate(inputValue.begin(), inputValue.end(), 0.0) / inputValue.size();
		// 计算最大值
		staticValue[2] = *max_element(inputValue.begin(), inputValue.end());
	}
	return 0;
}
//--------------------------------------numericalStatic End--------------------------------------------//

//--------------------------------------genFlawBasicInfo --------------------------------------------//
//功能：对输入缺陷特征结构体的基础特征进行初始化赋值操作
//输入：flaws 输入需要初始化基础特征 vector<FlawInfoStruct>
int  zkhyPublicFuncHN::genFlawBasicInfo(vector<FlawInfoStruct>& flaws)
{
	int flawNum = flaws.size();
	for (int i = 0; i < flawNum; i++)
	{
		flaws[i].FlawBasicInfo.flawArea = -9999;
		flaws[i].FlawBasicInfo.angle = -9999;
		flaws[i].FlawBasicInfo.flawWidth = -9999;
		flaws[i].FlawBasicInfo.flawHeight = -9999;
		flaws[i].FlawBasicInfo.centerPt = cv::Point2f(0, 0);
		flaws[i].FlawBasicInfo.pts = vector<cv::Point2f>(0);
		flaws[i].FlawBasicInfo.flawContours = vector<vector<cv::Point2f>>(0);
		flaws[i].FlawBasicInfo.type = 0;
	}
	return 0;
}
//--------------------------------------genFlawBasicInfo --------------------------------------------//

void GetStringSize(HDC hDC, const char* str, int* w, int* h)
{
	SIZE size;
	GetTextExtentPoint32A(hDC, str, strlen(str), &size);
	if (w != 0) *w = size.cx;
	if (h != 0) *h = size.cy;
}

void zkhyPublicFuncHN::putTextZH(Mat& dst, const char* str, Point org, Scalar color, int fontSize, const char* fn, bool italic, bool underline)
{
	CV_Assert(dst.data != 0 && (dst.channels() == 1 || dst.channels() == 3));

	int x, y, r, b;
	if (org.x > dst.cols || org.y > dst.rows) return;
	x = org.x < 0 ? -org.x : 0;
	y = org.y < 0 ? -org.y : 0;

	LOGFONTA lf;
	lf.lfHeight = -fontSize;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = 5;
	lf.lfItalic = italic;   //斜体
	lf.lfUnderline = underline; //下划线
	lf.lfStrikeOut = 0;
	lf.lfCharSet = DEFAULT_CHARSET;
	lf.lfOutPrecision = 0;
	lf.lfClipPrecision = 0;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = 0;
	strcpy_s(lf.lfFaceName, fn);

	HFONT hf = CreateFontIndirectA(&lf);
	HDC hDC = CreateCompatibleDC(0);
	HFONT hOldFont = (HFONT)SelectObject(hDC, hf);

	int strBaseW = 0, strBaseH = 0;
	int singleRow = 0;
	char buf[1 << 12];
	strcpy_s(buf, str);
	char* bufT[1 << 12];  // 这个用于分隔字符串后剩余的字符，可能会超出。
	//处理多行
	{
		int nnh = 0;
		int cw, ch;

		const char* ln = strtok_s(buf, "\n", bufT);
		while (ln != 0)
		{
			GetStringSize(hDC, ln, &cw, &ch);
			strBaseW = max(strBaseW, cw);
			strBaseH = max(strBaseH, ch);

			ln = strtok_s(0, "\n", bufT);
			nnh++;
		}
		singleRow = strBaseH;
		strBaseH *= nnh;
	}

	if (org.x + strBaseW < 0 || org.y + strBaseH < 0)
	{
		SelectObject(hDC, hOldFont);
		DeleteObject(hf);
		DeleteObject(hDC);
		return;
	}

	r = org.x + strBaseW > dst.cols ? dst.cols - org.x - 1 : strBaseW - 1;
	b = org.y + strBaseH > dst.rows ? dst.rows - org.y - 1 : strBaseH - 1;
	org.x = org.x < 0 ? 0 : org.x;
	org.y = org.y < 0 ? 0 : org.y;

	BITMAPINFO bmp = { 0 };
	BITMAPINFOHEADER& bih = bmp.bmiHeader;
	int strDrawLineStep = strBaseW * 3 % 4 == 0 ? strBaseW * 3 : (strBaseW * 3 + 4 - ((strBaseW * 3) % 4));

	bih.biSize = sizeof(BITMAPINFOHEADER);
	bih.biWidth = strBaseW;
	bih.biHeight = strBaseH;
	bih.biPlanes = 1;
	bih.biBitCount = 24;
	bih.biCompression = BI_RGB;
	bih.biSizeImage = strBaseH * strDrawLineStep;
	bih.biClrUsed = 0;
	bih.biClrImportant = 0;

	void* pDibData = 0;
	HBITMAP hBmp = CreateDIBSection(hDC, &bmp, DIB_RGB_COLORS, &pDibData, 0, 0);

	CV_Assert(pDibData != 0);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBmp);

	//color.val[2], color.val[1], color.val[0]
	SetTextColor(hDC, RGB(255, 255, 255));
	SetBkColor(hDC, 0);
	//SetStretchBltMode(hDC, COLORONCOLOR);

	strcpy_s(buf, str);
	const char* ln = strtok_s(buf, "\n", bufT);
	int outTextY = 0;
	while (ln != 0)
	{
		TextOutA(hDC, 0, outTextY, ln, strlen(ln));
		outTextY += singleRow;
		ln = strtok_s(0, "\n", bufT);
	}
	uchar* dstData = (uchar*)dst.data;
	int dstStep = dst.step / sizeof(dstData[0]);
	unsigned char* pImg = (unsigned char*)dst.data + org.x * dst.channels() + org.y * dstStep;
	unsigned char* pStr = (unsigned char*)pDibData + x * 3;
	for (int tty = y; tty <= b; ++tty)
	{
		unsigned char* subImg = pImg + (tty - y) * dstStep;
		unsigned char* subStr = pStr + (strBaseH - tty - 1) * strDrawLineStep;
		for (int ttx = x; ttx <= r; ++ttx)
		{
			for (int n = 0; n < dst.channels(); ++n) {
				double vtxt = subStr[n] / 255.0;
				int cvv = vtxt * color.val[n] + (1 - vtxt) * subImg[n];
				subImg[n] = cvv > 255 ? 255 : (cvv < 0 ? 0 : cvv);
			}

			subStr += 3;
			subImg += dst.channels();
		}
	}

	SelectObject(hDC, hOldBmp);
	SelectObject(hDC, hOldFont);
	DeleteObject(hf);
	DeleteObject(hBmp);
	DeleteDC(hDC);
}
//将宽字节wchar_t*转化为单字节char*  
char* UnicodeToAnsi(wchar_t* szStr)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, szStr, 1, NULL, 0, NULL, NULL);
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen + 1];
	WideCharToMultiByte(CP_ACP, 0, szStr, nLen, pResult, nLen, NULL, NULL);
	pResult[nLen] = '\0';
	return pResult;
}


//--------------------------------------measureOf2DLineMeasure Begin--------------------------------------------//
	//功能：使用2D线测量功能找直线
	//输入：inputValue-输入图像，lineFirstPoint-直线首点，lineLastPoint-直线末点，measureLenght1-测量矩形半长轴，measureLenght1-测量矩形半短轴
	//输入：measureSigma-平滑系数，measureThresh-对比度阈值，polarity-极性[positive-黑到白，negative-白到黑]，select-选点[first-第一个点，last-最后一个点，all-所有点]）
	//输出：hv_MetrologyHandle-测量直线句柄，hv_lineUpParam-拟合后的直线
int zkhyPublicFuncHN::measureOf2DLineMeasure(HObject& inputImage, Point2f& lineFirstPoint, Point2f& lineLastPoint, HTuple measureLenght1, HTuple measureLenght2,
	HTuple measureSigma, HTuple measureThresh, HTuple polarity, HTuple select, HTuple& hv_MetrologyHandle, HTuple& hv_lineUpParam)
{
	HTuple hv_Width, hv_Height, hv_Index;
	GetImageSize(inputImage, &hv_Width, &hv_Height);
	CreateMetrologyModel(&hv_MetrologyHandle);
	SetMetrologyModelImageSize(hv_MetrologyHandle, hv_Width, hv_Height);
	AddMetrologyObjectLineMeasure(hv_MetrologyHandle, lineFirstPoint.y, lineFirstPoint.x, lineLastPoint.y, lineLastPoint.x,
		measureLenght1, measureLenght2, measureSigma, measureThresh, HTuple(), HTuple(), &hv_Index);
	SetMetrologyObjectParam(hv_MetrologyHandle, hv_Index, "num_instances", 1);
	SetMetrologyObjectParam(hv_MetrologyHandle, hv_Index, "measure_transition", polarity);
	SetMetrologyObjectParam(hv_MetrologyHandle, hv_Index, "measure_select", select);
	SetMetrologyObjectParam(hv_MetrologyHandle, hv_Index, "min_score", 0.5);
	ApplyMetrologyModel(inputImage, hv_MetrologyHandle);
	GetMetrologyObjectResult(hv_MetrologyHandle, hv_Index, "all", "result_type", "all_param", &hv_lineUpParam);
	return 0;
}
//--------------------------------------measureOf2DLineMeasure End--------------------------------------------//




/*
//字符自动换行算法，支持中文，自动适应字符大小
Mat wordNewLine(int input_width, int input_height, wchar_t* input_string)
{
	setlocale(LC_CTYPE, "chs");
	Mat dst(cv::Size(input_width, input_height), CV_8UC1, Scalar(255));

	int strlength = wcslen(input_string);
	int lineWidth = sqrt(strlength) + 1;

	int split_cols, split_rows;
	if (input_width >= input_height)
	{
		split_cols = lineWidth;
		split_rows = ceil(strlength * 1.0 / split_cols);
	}
	else
	{
		split_rows = lineWidth;
		split_cols = ceil(strlength * 1.0 / split_rows);
	}

	int character_w = input_width / split_cols;
	int character_h = input_height / split_rows;

	vector<int> center_x, center_y;
	for (int i = 1; i <= split_rows * split_cols; i++)
	{
		int x_i = i % split_cols;
		int x_j = i % split_rows;
		if (x_i == 0)
			x_i = split_cols;
		if (x_j == 0)
			x_j = split_rows;
		center_x.push_back((character_w * x_i) - character_w / 2);
		center_y.push_back((character_h * x_j) - character_h / 2);
	}
	sort(center_y.begin(), center_y.end());
	for (int i = 0; i < strlength; i++)
	{
		wchar_t str_single = input_string[i];
		char* single_char = UnicodeToAnsi(&str_single);
		int fontSize = character_w < character_h ? character_w : character_h;
		putTextZH(dst, single_char, cv::Point(center_x.at(i) - (fontSize * 72 / 96) / 2, center_y.at(i) - (fontSize * 72 / 96) / 2), Scalar(0), fontSize * 72 / 96, "宋体", false, false);
	}
	return dst;
}
*/

//--------------------------------------genContourFromRegion Begin--------------------------------------------//
//功能：将输入的HObject区域转换为轮廓点集
//输入：inputRegion 待转换的HObject区域，可以为单个区域，也可以为多个区域 <HObject>
//输出：outputContours 轮廓点集 <vector<vector<Point2f>>>
void zkhyPublicFuncHN::genContourFromHContour(HObject inputContour, vector<vector<Point2f>>& outputContours, int sampleInterval)
{
	if (isHObjectEmpty(inputContour))
		return;

	HTuple hContourNum;
	CountObj(inputContour, &hContourNum);   // 输入轮廓数
	int contourNum = hContourNum.I();
	outputContours.resize(contourNum);

	for (int i = 1; i <= contourNum; ++i) {
		HObject selectContour;
		vector<Point2f> contourTmp;
		SelectObj(inputContour, &selectContour, i);
		HTuple rowContour, colContour, contourPointsNum;
		GetContourXld(selectContour, &rowContour, &colContour);

		// 判断采样后的点是否过少
		int sampleStep = sampleInterval;
		TupleLength(rowContour, &contourPointsNum);
		if (contourPointsNum.I() / sampleStep < 3)
			sampleStep = contourPointsNum.I() / 3;

		hTuple2Points(colContour, rowContour, contourTmp, sampleStep);
		outputContours[i - 1] = contourTmp;

		// release contourTmp
		vector<Point2f>().swap(contourTmp);
	}
}
//--------------------------------------genContourFromRegion End--------------------------------------------//

//--------------------------------------computeBoundingRect Begin--------------------------------------------//
//功能：计算输入轮廓的外接矩形,并将轮廓转换为OPENCV格式的轮廓
//输入：contours 轮廓点集 <vector<vector<Point2f>>>
//输出：rect 外接矩形 <Rect>
//		contoursForDraw OPENCV格式的轮廓 <vector<vector<Point>>>
int zkhyPublicFuncHN::computeBoundingRect(const vector<vector<Point2f>>& contours, Rect& rect, vector<vector<Point>>& contoursForDraw)
{
	// 获取轮廓的个数
	int contoursNum = contours.size();
	if (contoursNum == 0)
		return 1;           // 轮廓个数为0

	vector<Point> contoursPoints;
	for (int i = 0; i < contoursNum; ++i)
	{
		vector<Point2f> contourTmp = contours[i];
		int pointsNum = contourTmp.size() - 1; // 最后一个点与第一个点重复
		vector<Point> contoursPointsTemp(pointsNum);
		for (int j = 0; j < pointsNum; ++j)
		{
			contoursPointsTemp[j] = contourTmp[j];
		}
		contoursPoints.insert(contoursPoints.end(), contoursPointsTemp.begin(), contoursPointsTemp.end());
	}
	contoursPoints.push_back(contoursPoints[0]);

	// 获取轮廓的正外接矩形
	rect = boundingRect(contoursPoints);

	contoursForDraw.push_back(contoursPoints);

	return 0;
}
//--------------------------------------computeBoundingRect End--------------------------------------------//