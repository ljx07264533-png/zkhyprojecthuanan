#include <windows.h>
#include <numeric>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

#include "alglibrary/zkhyProjectHuaNan/BGABall2dDefectDet.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	//  拟合直线方式
	//int padEdgeCompute(vector<HTuple> hUpRow, vector<HTuple> hUpCol, vector<HTuple> hLeftRow, vector<HTuple> hLeftCol, vector<HTuple> hDownRow, vector<HTuple> hDownCol, vector<HTuple> hRightRow, vector<HTuple> hRightCol, QFNDetInfo &pad)
	//{
	//	int crossMethod = 0;    //交点计算方式
	//	if (pad.padLocated == "top")
	//	{
	//		HTuple hUpRowTmp, hUpColTmp, hLeftRowTmp, hLeftColTmp, hDownRowTmp, hDownColTmp, hRightRowTmp, hRightColTmp, hUpLineStartRow, hUpLineStartCol, hUpLineEndRow, hUpLineEndCol, hUpLineKX, hUpLineKY, hUpLineD, hLeftLineStartRow, hLeftLineStartCol, hLeftLineEndRow, hLeftLineEndCol, hLeftLineKX, hLeftLineKY, hLeftLineD, hRightLineStartRow, hRightLineStartCol, hRightLineEndRow, hRightLineEndCol, hRightLineKX, hRightLineKY, hRightLineD;
	//		HObject hUpLine, hLeftLine, hDownLine, hRightLine;
	//		// 上直线
	//		vector2HTuple(hUpRow, hUpRowTmp);
	//		vector2HTuple(hUpCol, hUpColTmp);
	//		GenContourPolygonXld(&hUpLine, hUpRowTmp, hUpColTmp);
	//		FitLineContourXld(hUpLine, "tukey", -1, 0, 5, 2.0, &hUpLineStartRow, &hUpLineStartCol, &hUpLineEndRow, &hUpLineEndCol, &hUpLineKX, &hUpLineKY, &hUpLineD);
	//		// 左直线
	//		vector2HTuple(hLeftRow, hLeftRowTmp);
	//		vector2HTuple(hLeftCol, hLeftColTmp);
	//		GenContourPolygonXld(&hLeftLine, hLeftRowTmp, hLeftColTmp);
	//		FitLineContourXld(hLeftLine, "tukey", -1, 0, 5, 2.0, &hLeftLineStartRow, &hLeftLineStartCol, &hLeftLineEndRow, &hLeftLineEndCol, &hLeftLineKX, &hLeftLineKY, &hLeftLineD);
	//		// 右直线
	//		vector2HTuple(hRightRow, hRightRowTmp);
	//		vector2HTuple(hRightCol, hRightColTmp);
	//		GenContourPolygonXld(&hRightLine, hRightRowTmp, hRightColTmp);
	//		FitLineContourXld(hRightLine, "tukey", -1, 0, 5, 2.0, &hRightLineStartRow, &hRightLineStartCol, &hRightLineEndRow, &hRightLineEndCol, &hRightLineKX, &hRightLineKY, &hRightLineD);
	//		// 计算交点
	//		//使用经过上直线中心点坐标的直线作为参考直线-0 || 可以不平行-1
	//		Point2f pointLT, pointRT;
	//		if (crossMethod == 0)
	//		{
	//			Point2f upLineCenter;
	//			upLineCenter.x = (hUpLineStartCol + hUpLineEndCol) * 0.5;
	//			upLineCenter.y = (hUpLineStartRow + hUpLineEndRow) * 0.5;
	//			//左上交点    //y坐标不变
	//			double kL = hLeftLineKX.D() / (hLeftLineKY.D() + 1e-4);
	//			double bL = hLeftLineStartCol.D() - kL * hLeftLineStartRow.D();
	//			pointLT.x = 1 / kL * (upLineCenter.y - bL);
	//			pointLT.y = upLineCenter.y;
	//			//右上交点
	//
	//		}
	//		
	//
	//		//左上交点
	//		//右上交点
	//		// 记录Pad参数
	//	}
	//	
	//	return 0;
	//}

	// 重载，使用阈值分割获得锡球区域
	int findBGABallThresholdSeg(HObject& hDetImg, int thresholdLow, int thresholdHigh, double dBallAreaMin, double dBallAreaMax, const double dBallDetRadius, const double stdBallRadius, BGADetInfo& detball) {
		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj;
		GenCircle(&hTemplateBallRegion, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius);
		//根据坐标和锡球半径生成模板图锡球轮廓
		ReduceDomain(hDetImg, hTemplateBallRegion, &hImgReduced);
		Threshold(hImgReduced, &hDetBallRegion, thresholdLow, thresholdHigh);
		//Intersection(hSegRegions, hTemplateBallRegion, &hDetBallRegion);
		Connection(hDetBallRegion, &hDetBallRegions);
		SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);
		//判断是否找到锡球  Select后如果没有区域被选中，返回的为空对象（HObject）而不是空区域（Region）
		GenEmptyObj(&hEmptyObj);
		TestEqualObj(hDetBallRegionsSelected, hEmptyObj, &hIsEmpty);

		if (hIsEmpty.I() != 1)   //找到了锡球
		{
			Union1(hDetBallRegionsSelected, &hDetBallRegion);
			Intensity(hDetBallRegion, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算分割出来锡球的灰度均值
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius;
			SmallestCircle(hDetBallRegion, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius);
			detball.detBallCenter.x = hBallCircleCol.D();
			detball.detBallCenter.y = hBallCircleRow.D();
			detball.radius = hBallCircleRadius.D();
			detball.contrast = hBallGrayMean.D();
			detball.area = myPi * hBallCircleRadius.D() * hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			detball.detBallCenter.x = detball.templateBallCenter.x;
			detball.detBallCenter.y = detball.templateBallCenter.y;
			detball.isMissing = true;
			detball.radius = stdBallRadius;
			detball.contrast = 0;
		}
		return 0;
	}

	// 使用阈值分割获得锡球区域。重载，对局部小图进行缩放
	int findBGABallThresholdSeg(HObject& hDetImg, int thresholdLow, int thresholdHigh, double dBallAreaMin, double dBallAreaMax, const double dBallDetRadius, const double stdBallRadius, double scale, BGADetInfo& detball) {
		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty, hImgWidth, hImgHeight, hHomMat2DScale;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj, hImgTmp;

		// 获取裁剪区域
		double centerX, centerY, rowTmp1, colTmp1, rowTmp2, colTmp2;
		// 裁剪出锡球小图
		centerX = detball.templateBallCenter.x;
		centerY = detball.templateBallCenter.y;
		rowTmp1 = centerY - dBallDetRadius;
		colTmp1 = centerX - dBallDetRadius;
		rowTmp2 = centerY + dBallDetRadius;
		colTmp2 = centerX + dBallDetRadius;
		CropRectangle1(hDetImg, &hImgTmp, rowTmp1, colTmp1, rowTmp2, colTmp2);

		GetImageSize(hImgTmp, &hImgWidth, &hImgHeight);
		HomMat2dIdentity(&hHomMat2DScale);
		HomMat2dScale(hHomMat2DScale, scale, scale, 0, 0, &hHomMat2DScale); //以图像左上角点为基准
		AffineTransImage(hImgTmp, &hImgTmp, hHomMat2DScale, "nearest_neighbor", "true");
		// TODO 缩放后的坐标变换关系

		GenCircle(&hTemplateBallRegion, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius);
		//根据坐标和锡球半径生成模板图锡球轮廓
		ReduceDomain(hDetImg, hTemplateBallRegion, &hImgReduced);
		Threshold(hImgReduced, &hDetBallRegion, thresholdLow, thresholdHigh);
		Connection(hDetBallRegion, &hDetBallRegions);
		SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);
		//判断是否找到锡球  Select后如果没有区域被选中，返回的为空对象（HObject）而不是空区域（Region）
		GenEmptyObj(&hEmptyObj);
		TestEqualObj(hDetBallRegionsSelected, hEmptyObj, &hIsEmpty);

		if (hIsEmpty.I() != 1)   //找到了锡球
		{
			Union1(hDetBallRegionsSelected, &hDetBallRegion);
			Intensity(hDetBallRegion, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算分割出来锡球的灰度均值
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius;
			SmallestCircle(hDetBallRegion, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius);
			detball.detBallCenter.x = hBallCircleCol.D();
			detball.detBallCenter.y = hBallCircleRow.D();
			detball.radius = hBallCircleRadius.D();
			detball.contrast = hBallGrayMean.D();
			detball.area = myPi * hBallCircleRadius.D() * hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			detball.detBallCenter.x = detball.templateBallCenter.x;
			detball.detBallCenter.y = detball.templateBallCenter.y;
			detball.isMissing = true;
			detball.radius = stdBallRadius;
			detball.contrast = 0;
		}
		return 0;
	}

	// 重载，使用阈值分割获得锡球区域，返回中心坐标和半径
	int findBGABallThresholdSeg(HObject& hDetImg, HObject& hSegRegions, const double& dBallAreaMin, const double& dBallAreaMax, const double& dBallDetRadius, const double& stdBallRadius, Point2f& centerPoint, double& detRadius, BGADetInfo& detball) {
		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj;
		GenCircle(&hTemplateBallRegion, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius);
		//根据坐标和锡球半径生成模板图锡球轮廓

		Intersection(hSegRegions, hTemplateBallRegion, &hDetBallRegion);
		Connection(hDetBallRegion, &hDetBallRegions);
		SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);
		//判断是否找到锡球  Select后如果没有区域被选中，返回的为空对象（HObject）而不是空区域（Region）
		GenEmptyObj(&hEmptyObj);
		TestEqualObj(hDetBallRegionsSelected, hEmptyObj, &hIsEmpty);

		if (hIsEmpty.I() != 1)   //找到了锡球
		{
			Union1(hDetBallRegionsSelected, &hDetBallRegion);
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius;
			SmallestCircle(hDetBallRegion, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius);
			centerPoint.x = hBallCircleCol.D();
			centerPoint.y = hBallCircleRow.D();
			detRadius = hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			centerPoint.x = detball.templateBallCenter.x;
			centerPoint.y = detball.templateBallCenter.y;
			detRadius = -1;
		}
		return 0;
	}

	// 使用阈值分割后再拟合圆获取锡球区域
	int findBGABallThresholdSegFitCircle(HObject& hDetImg, HObject& hSegRegions, double dBallAreaMin, double dBallAreaMax, const double dBallDetRadius, const double stdBallRadius, BGADetInfo& detball) {
		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj;
		GenCircle(&hTemplateBallRegion, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius);
		//根据坐标和锡球半径生成模板图锡球轮廓
		ReduceDomain(hDetImg, hTemplateBallRegion, &hImgReduced);
		Intersection(hSegRegions, hTemplateBallRegion, &hDetBallRegion);
		Connection(hDetBallRegion, &hDetBallRegions);
		SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);
		//判断是否找到锡球  Select后如果没有区域被选中，返回的为空对象（HObject）而不是空区域（Region）
		GenEmptyObj(&hEmptyObj);
		TestEqualObj(hDetBallRegionsSelected, hEmptyObj, &hIsEmpty);

		if (hIsEmpty.I() != 1)   //找到了锡球
		{
			Union1(hDetBallRegionsSelected, &hDetBallRegion);
			FillUp(hDetBallRegion, &hDetBallRegion);
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius;
			HTuple hBallArea, hCountourNum, hStartPhi, hEndPhi, hPointOrder, hBallCircleRowTmp, hBallCircleColTmp, hBallCircleRadiusTmp, hCountourNumTmp, hStartPhiTmp, hEndPhiTmp, hPointOrderTmp;
			HObject hDetBallContour, hCircleTmp;
			GenContourRegionXld(hDetBallRegion, &hDetBallContour, "border");
			FitCircleContourXld(hDetBallContour, "geotukey", -1, 2, 0, 3, 2, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius, &hStartPhi, &hEndPhi, &hPointOrder);
			GenCircle(&hCircleTmp, hBallCircleRow, hBallCircleCol, hBallCircleRadius);
			Intensity(hCircleTmp, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算分割出来锡球的灰度均值
			detball.detBallCenter.x = hBallCircleCol.D();
			detball.detBallCenter.y = hBallCircleRow.D();
			detball.radius = hBallCircleRadius.D();
			detball.contrast = hBallGrayMean.D();
			detball.area = myPi * hBallCircleRadius.D() * hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			detball.detBallCenter.x = detball.templateBallCenter.x;
			detball.detBallCenter.y = detball.templateBallCenter.y;
			detball.isMissing = true;
			detball.radius = stdBallRadius;
			detball.contrast = 0;
		}
		return 0;
	}


	// 使用亚像素获取锡球轮廓
	int findBGABallSubpixelFitCircle(HObject& hDetImg, HObject& hSegRegions, double dBallAreaMin, double dBallAreaMax, const double dBallDetRadius, const double stdBallRadius, BGADetInfo& detball)
	{
		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj;
		GenCircle(&hTemplateBallRegion, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius);
		//根据坐标和锡球半径生成模板图锡球轮廓
		ReduceDomain(hDetImg, hTemplateBallRegion, &hImgReduced);
		Intersection(hSegRegions, hTemplateBallRegion, &hDetBallRegion);
		Connection(hDetBallRegion, &hDetBallRegions);
		SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);
		//判断是否找到锡球  Select后如果没有区域被选中，返回的为空对象（HObject）而不是空区域（Region）
		GenEmptyObj(&hEmptyObj);
		TestEqualObj(hDetBallRegionsSelected, hEmptyObj, &hIsEmpty);

		if (hIsEmpty.I() != 1)   //找到了锡球
		{
			Union1(hDetBallRegionsSelected, &hDetBallRegion);
			FillUp(hDetBallRegion, &hDetBallRegion);
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius;
			HTuple hBallArea, hCountourNum, hStartPhi, hEndPhi, hPointOrder, hBallCircleRowTmp, hBallCircleColTmp, hBallCircleRadiusTmp, hCountourNumTmp, hStartPhiTmp, hEndPhiTmp, hPointOrderTmp;
			HObject hDetBallContour, hCircleTmp;
			GenContourRegionXld(hDetBallRegion, &hDetBallContour, "border");
			FitCircleContourXld(hDetBallContour, "geotukey", -1, 2, 0, 3, 2, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius, &hStartPhi, &hEndPhi, &hPointOrder);
			GenCircle(&hCircleTmp, hBallCircleRow, hBallCircleCol, hBallCircleRadius);
			Intensity(hCircleTmp, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算分割出来锡球的灰度均值
			detball.detBallCenter.x = hBallCircleCol.D();
			detball.detBallCenter.y = hBallCircleRow.D();
			detball.radius = hBallCircleRadius.D();
			detball.contrast = hBallGrayMean.D();
			detball.area = myPi * hBallCircleRadius.D() * hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			detball.detBallCenter.x = detball.templateBallCenter.x;
			detball.detBallCenter.y = detball.templateBallCenter.y;
			detball.isMissing = true;
			detball.radius = stdBallRadius;
			detball.contrast = 0;
		}
		return 0;
	}

	int packingDetInputTrans(HObject srcImg, CSYSTransPara transPara, vector<Point2f> vertexs, double coordOffset, HObject& imgTrans, HTuple& layout2DetectTrans, vector<Point2f>& vertexsTrans)
	{
		HObject detImg;
		HTuple hChannelNum;
		CountChannels(srcImg, &hChannelNum);
		if (hChannelNum.I() == 3)
			Rgb1ToGray(srcImg, &detImg);
		else
			detImg = srcImg;
		Point2f affineTransValue = transPara.translationValue;
		float theta = transPara.angle;

		// Step 1，将待测图转正
		HTuple hDetect2LayoutTrans, hImgWidth, hImgHeight;
		VectorAngleToRigid(coordOffset, coordOffset, 0, affineTransValue.y, affineTransValue.x, (HTuple)(theta), &layout2DetectTrans);
		HomMat2dInvert(layout2DetectTrans, &hDetect2LayoutTrans);

		GetImageSize(detImg, &hImgWidth, &hImgHeight);
		AffineTransImageSize(detImg, &imgTrans, hDetect2LayoutTrans, "nearest_neighbor", hImgWidth, hImgHeight);     // 待测图旋转后大小保持不变
		vertexsTrans.resize(5);
		// Step 2，将产品的四个顶点信息映射到Layout上
		for (int i = 0; i < 5; i++)
		{
			HTuple hPointTmpY, hPointTmpX;
			Point2f pointTmp;
			AffineTransPixel(hDetect2LayoutTrans, vertexs[i].y, vertexs[i].x, &hPointTmpY, &hPointTmpX);
			pointTmp.x = hPointTmpX;
			pointTmp.y = hPointTmpY;
			vertexsTrans[i] = pointTmp;
		}
		return 0;

	}

	// 计算模板锡球的坐标
	int BGATemplateBallCoordCalculation(const int& ballNumRow, const int& ballNumCol, const double& coordOffset, const Point2f& offsetDelta, const BGAParam& layoutInfo, const vector<cv::Point2f>& inputVertexs, vector<vector<BGADetInfo>>& detBalls, vector<existedTarget>& ballExisted, HObject& templateRegions) {

		double scaleX, scaleY;
		// 计算待测图和Layout的缩放系数
		// 计算待测芯片尺寸
		auto computeDistance = [](Point2f p1, Point2f p2) {return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)); };
		double bodySizeX = computeDistance(inputVertexs[0], inputVertexs[3]);
		double bodySizeY = computeDistance(inputVertexs[2], inputVertexs[3]);
		// 计算缩放系数
		double deltaX, deltaY, stdBallRadius;
		deltaX = offsetDelta.x;
		deltaY = offsetDelta.y;
		scaleX = bodySizeX / layoutInfo.bodySizeX;
		scaleY = bodySizeY / layoutInfo.bodySizeY;
		//scaleX = 1;
		//scaleY = 1;

		stdBallRadius = layoutInfo.ballWidth * 0.5;

		if (layoutInfo.posInfo.size() != ballNumRow || layoutInfo.posInfo[0].size() != ballNumCol)
			return 2;         //Layout坐标信息与锡球行列数不匹配

		// 避免将产品移动到贴近图像边缘的位置，需要在仿射变换和计算坐标时增加相同的偏移量以对layout信息中的坐标进行修正
		detBalls.resize(ballNumRow);
		for (int i = 0; i < ballNumRow; ++i)
		{
			vector<BGADetInfo> detBallsTmp;
			detBallsTmp.resize(ballNumCol);
			for (int j = 0; j < ballNumCol; ++j)
			{
				BGADetInfo detBallTmp;
				detBallTmp.templateBallCenter.x = layoutInfo.posInfo[i][j].pos.x * scaleX + coordOffset + deltaX;
				detBallTmp.templateBallCenter.y = layoutInfo.posInfo[i][j].pos.y * scaleY + coordOffset + deltaY;
				detBallTmp.isExist = layoutInfo.posInfo[i][j].existFlag;
				detBallsTmp[j] = detBallTmp;
				if (detBallTmp.isExist == 1) {

					existedTarget tmp;
					tmp.row = i;
					tmp.col = j;
					ballExisted.push_back(tmp);

					HObject hBallRegionTmp1;
					GenCircle(&hBallRegionTmp1, detBallTmp.templateBallCenter.y, detBallTmp.templateBallCenter.x, stdBallRadius);
					ConcatObj(templateRegions, hBallRegionTmp1, &templateRegions);
				}

			}
			detBalls[i] = detBallsTmp;
		}
		return 0;
	}

	// 使用阈值分割获得锡球区域
	// TODO 逻辑优化，接口调整
	int findBGABallThresholdSeg(HObject& hDetImg, HObject& hSegRegions, double dBallAreaMin, double dBallAreaMax, const double dBallDetRadius, const double stdBallRadius, BGADetInfo& detball, HObject& hBallRegions) {
		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj;
		GenCircle(&hTemplateBallRegion, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius);
		//根据坐标和锡球半径生成模板图锡球轮廓
		//ReduceDomain(hDetImg, hTemplateBallRegion, &hImgReduced);
		//Threshold(hImgReduced, &hDetBallRegion, thresholdLow, thresholdHigh);
		Intersection(hSegRegions, hTemplateBallRegion, &hDetBallRegion);
		Connection(hDetBallRegion, &hDetBallRegions);
		SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);
		//判断是否找到锡球  Select后如果没有区域被选中，返回的为空对象（HObject）而不是空区域（Region）
		GenEmptyObj(&hEmptyObj);
		TestEqualObj(hDetBallRegionsSelected, hEmptyObj, &hIsEmpty);

		if (hIsEmpty.I() != 1)   //找到了锡球
		{
			Union1(hDetBallRegionsSelected, &hDetBallRegion);
			Intensity(hDetBallRegion, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算分割出来锡球的灰度均值

			Union2(hDetBallRegion, hBallRegions, &hBallRegions);		// 输出分割的轮廓

			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius;
			SmallestCircle(hDetBallRegion, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius);
			detball.detBallCenter.x = hBallCircleCol.D();
			detball.detBallCenter.y = hBallCircleRow.D();
			detball.radius = hBallCircleRadius.D();
			detball.radiusNorm = hBallCircleRadius.D();
			detball.contrast = hBallGrayMean.D();
			detball.area = myPi * hBallCircleRadius.D() * hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			detball.detBallCenter.x = detball.templateBallCenter.x;
			detball.detBallCenter.y = detball.templateBallCenter.y;
			detball.isMissing = true;
			detball.radius = stdBallRadius;
			detball.radiusNorm = stdBallRadius;
			detball.contrast = 0;
		}
		return 0;
	}

	// 使用阈值分割获得锡球区域，对区域操作后获取亚像素边缘
	int findBGABallThresholdSegSubpixel(HObject& hDetImg, HObject& hSegRegions, int thresholdLow, double dBallAreaMin, double dBallAreaMax, const double dBallDetRadius, const double stdBallRadius, double morphologyRadius, BGADetInfo& detball, HObject& hBallRegions) {
		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj;
		GenCircle(&hTemplateBallRegion, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius);
		//根据坐标和锡球半径生成模板图锡球轮廓
		//ReduceDomain(hDetImg, hTemplateBallRegion, &hImgReduced);
		Intersection(hSegRegions, hTemplateBallRegion, &hDetBallRegion);
		Connection(hDetBallRegion, &hDetBallRegions);
		SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);
		//判断是否找到锡球  Select后如果没有区域被选中，返回的为空对象（HObject）而不是空区域（Region）
		GenEmptyObj(&hEmptyObj);
		TestEqualObj(hDetBallRegionsSelected, hEmptyObj, &hIsEmpty);

		if (hIsEmpty.I() != 1)   //找到了锡球
		{
			Union1(hDetBallRegionsSelected, &hDetBallRegion);

			Union2(hDetBallRegion, hBallRegions, &hBallRegions);		// 输出分割的轮廓
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius, hBallArea, hCountourNum, hStartPhi, hEndPhi, hPointOrder, hBallCircleRowTmp, hBallCircleColTmp, hBallCircleRadiusTmp, hCountourNumTmp, hStartPhiTmp, hEndPhiTmp, hPointOrderTmp;
			HObject hDetBallContour, hCircleTmp, hCircleTmpDilation, hCircleTmpErosion, hRingTmp, hImgReducedTmp, hBallContourSplited, hDetBallContourUnioned, hDetBallContourSelected;
			// 生成粗定位区域
			SmallestCircle(hDetBallRegion, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius);
			GenCircle(&hCircleTmp, hBallCircleRow, hBallCircleCol, hBallCircleRadius);
			DilationCircle(hCircleTmp, &hCircleTmpDilation, morphologyRadius);
			ErosionCircle(hCircleTmp, &hCircleTmpErosion, morphologyRadius);
			Difference(hCircleTmpDilation, hCircleTmpErosion, &hRingTmp);
			// 亚像素分割
			ReduceDomain(hDetImg, hRingTmp, &hImgReducedTmp);
			ThresholdSubPix(hImgReducedTmp, &hDetBallContour, thresholdLow);
			//SegmentContoursXld(hDetBallContour, &hBallContourSplited, "lines_circles", 5, stdBallRadius * 0.2, stdBallRadius * 0.1);
			UnionAdjacentContoursXld(hDetBallContour, &hDetBallContourUnioned, dBallDetRadius, dBallDetRadius, "attr_keep");

			CountObj(hDetBallContourUnioned, &hCountourNum);
			double contourLongest = 0;
			GenEmptyObj(&hDetBallContourSelected);
			for (int k = 1; k <= hCountourNum.I(); k++)
			{
				HObject hContourTmp;
				HTuple hContourLength;
				SelectObj(hDetBallContourUnioned, &hContourTmp, k);
				LengthXld(hContourTmp, &hContourLength);
				if (hContourLength.D() > contourLongest) {
					contourLongest = hContourLength.D();
					hDetBallContourSelected = hContourTmp;
				}
			}

			// fit circle
			FitCircleContourXld(hDetBallContourSelected, "geotukey", -1, 2, 0, 3, 2, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius, &hStartPhi, &hEndPhi, &hPointOrder);
			// 最小外接圆
			//SmallestCircleXld(hDetBallContourSelected, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius);
			GenCircle(&hCircleTmp, hBallCircleRow, hBallCircleCol, hBallCircleRadius);
			Intensity(hCircleTmp, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算锡球区域的灰度均值

			detball.detBallCenter.x = hBallCircleCol.D();
			detball.detBallCenter.y = hBallCircleRow.D();
			detball.radius = hBallCircleRadius.D();
			detball.radiusNorm = hBallCircleRadius.D();
			detball.contrast = hBallGrayMean.D();
			detball.area = myPi * hBallCircleRadius.D() * hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			detball.detBallCenter.x = detball.templateBallCenter.x;
			detball.detBallCenter.y = detball.templateBallCenter.y;
			detball.isMissing = true;
			detball.radius = stdBallRadius;
			detball.radiusNorm = stdBallRadius;
			detball.contrast = 0;
		}
		return 0;
	}

	// 使用2D测量获取锡球轮廓
	int findBGABallMetrology(HObject& hDetImg, HTuple hImgWidth, HTuple hImgHeight, double iThresholdSegBallLow, const double dBallDetRadius, const double stdBallRadius, double measureLen1, double measureLen2, double sigma, BGADetInfo& detball)
	{
		HTuple hMetrologyHandle, hGenParamName, hGenParamValue, hMetrologyCircleIndices, hBallParameter, hIsFindBall;
		CreateMetrologyModel(&hMetrologyHandle);
		SetMetrologyModelImageSize(hMetrologyHandle, hImgWidth, hImgHeight);
		AddMetrologyObjectCircleMeasure(hMetrologyHandle, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius, measureLen1, measureLen2, sigma, iThresholdSegBallLow, hGenParamName, hGenParamValue, &hMetrologyCircleIndices);
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "num_instances", 1);
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "measure_transition", "uniform");
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "min_score", 0.5);

		ApplyMetrologyModel(hDetImg, hMetrologyHandle);
		GetMetrologyObjectResult(hMetrologyHandle, hMetrologyCircleIndices, "all", "result_type", "all_param", &hBallParameter);
		TupleLength(hBallParameter, &hIsFindBall);

		HObject hContour;
		HTuple hRowTmp, hColTmp;
		GetMetrologyObjectMeasures(&hContour, hMetrologyHandle, "all", "all", &hRowTmp, &hColTmp);
		if (hIsFindBall.I() != 0)   //找到了锡球
		{
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius, hBallGrayMean, hBallGrayDeviation;
			HObject hCircleTmp;
			hBallCircleRow = hBallParameter[0];
			hBallCircleCol = hBallParameter[1];
			hBallCircleRadius = hBallParameter[2];
			GenCircle(&hCircleTmp, hBallCircleRow, hBallCircleCol, hBallCircleRadius);
			Intensity(hCircleTmp, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算分割出来锡球的灰度均值
			detball.detBallCenter.x = hBallCircleCol.D();
			detball.detBallCenter.y = hBallCircleRow.D();
			detball.radius = hBallCircleRadius.D();
			detball.contrast = hBallGrayMean.D();
			detball.area = myPi * hBallCircleRadius.D() * hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			detball.detBallCenter.x = detball.templateBallCenter.x;
			detball.detBallCenter.y = detball.templateBallCenter.y;
			detball.isMissing = true;
			detball.radius = stdBallRadius;
			detball.contrast = 0;
		}
		ClearMetrologyModel(hMetrologyHandle);
		return 0;
	}

	// 重载，使用2D测量获取锡球轮廓
	int findBGABallMetrology(HObject& hDetImg, HTuple& hMetrologyHandle, const double thresholdMeasure, const double dBallDetRadius, const double stdBallRadius, double measureLen1, double measureLen2, double sigma, BGADetInfo& detball)
	{
		HTuple hGenParamName, hGenParamValue, hMetrologyCircleIndices, hBallParameter, hIsFindBall;
		AddMetrologyObjectCircleMeasure(hMetrologyHandle, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius, measureLen1, measureLen2, sigma, thresholdMeasure, hGenParamName, hGenParamValue, &hMetrologyCircleIndices);
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "num_instances", 1);
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "measure_transition", "uniform");
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "min_score", 0.5);

		ApplyMetrologyModel(hDetImg, hMetrologyHandle);
		GetMetrologyObjectResult(hMetrologyHandle, hMetrologyCircleIndices, "all", "result_type", "all_param", &hBallParameter);
		TupleLength(hBallParameter, &hIsFindBall);

		HObject hContour;
		HTuple hRowTmp, hColTmp;
		GetMetrologyObjectMeasures(&hContour, hMetrologyHandle, "all", "all", &hRowTmp, &hColTmp);
		if (hIsFindBall.I() != 0)   //找到了锡球
		{
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius, hBallGrayMean, hBallGrayDeviation;
			HObject hCircleTmp;
			hBallCircleRow = hBallParameter[0];
			hBallCircleCol = hBallParameter[1];
			hBallCircleRadius = hBallParameter[2];
			GenCircle(&hCircleTmp, hBallCircleRow, hBallCircleCol, hBallCircleRadius);
			Intensity(hCircleTmp, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算分割出来锡球的灰度均值
			detball.detBallCenter.x = hBallCircleCol.D();
			detball.detBallCenter.y = hBallCircleRow.D();
			detball.radius = hBallCircleRadius.D();
			detball.radiusNorm = hBallCircleRadius.D();
			detball.contrast = hBallGrayMean.D();
			detball.area = myPi * hBallCircleRadius.D() * hBallCircleRadius.D();
		}
		else     //没找到锡球
		{
			detball.detBallCenter.x = detball.templateBallCenter.x;
			detball.detBallCenter.y = detball.templateBallCenter.y;
			detball.isMissing = true;
			detball.radius = stdBallRadius;
			detball.radiusNorm = stdBallRadius;
			detball.contrast = 0;
		}
		ClearMetrologyObject(hMetrologyHandle, "all");
		return 0;
	}

	// 重载，阈值分割粗定位+2D Measure 获取锡球轮廓
	int findBGABallMetrology(HObject& hDetImg, HTuple& hMetrologyHandle, HTuple& ballCoarseRow, HTuple& ballCoarseCol, HTuple& ballCoarseRadius, const double& thresholdMeasure, const double& measureLen1, const double& measureLen2, const double& sigma, const int& measureInstances, const int& measureNum, HTuple& transition, HTuple& select, const double& score, const long& randSeed, const double& dBallDetRadius, const double& stdBallRadius, vector<vector<BGADetInfo>>& detBalls)
	{
		HTuple hGenParamName, hGenParamValue, hMetrologyCircleIndices, hBallParameter, hBallFindNum;
		//设置测量参数
		hGenParamName.Clear();
		hGenParamValue.Clear();
		AddMetrologyObjectCircleMeasure(hMetrologyHandle, ballCoarseRow, ballCoarseCol, ballCoarseRadius, measureLen1, measureLen2, sigma, thresholdMeasure, hGenParamName, hGenParamValue, &hMetrologyCircleIndices);
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "num_instances", measureInstances);    //每个位置找几个圆
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "num_measures", measureNum);    //每个圆用几个测量矩形
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "measure_transition", transition);    //边缘点极性
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "measure_select", select);    //边缘点极性
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "min_score", score);   //分数
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, "rand_seed", (double)randSeed);    //随机种子

		// 一次设置所有需要设置的测量参数
		/*HTuple hMetrologyObjName, hMetrologyObjValue;
		hMetrologyObjName.Clear();
		hMetrologyObjValue.Clear();
		string metrologyObjName[6] = {"num_instances", "num_measures", "measure_transition", "measure_select", "min_score", "rand_seed"};
		for (int i = 0; i < 6; i++)
		{
			hMetrologyObjName[i] = metrologyObjName[i].c_str();
		}
		hMetrologyObjValue[0] = measureInstances;
		hMetrologyObjValue[1] = measureNum;
		hMetrologyObjValue[2] = transition;
		hMetrologyObjValue[3] = select;
		hMetrologyObjValue[4] = score;
		hMetrologyObjValue[5] = randSeed;
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, hMetrologyObjName, hMetrologyObjValue);*/
		ApplyMetrologyModel(hDetImg, hMetrologyHandle);
		GetMetrologyObjectResult(hMetrologyHandle, hMetrologyCircleIndices, "all", "result_type", "all_param", &hBallParameter);
		TupleLength(hBallParameter, &hBallFindNum);
		int ballNum = hBallFindNum / 3;  //hBallParameter依次存储每个找到的圆的row, col和radius;

		// Debug，获取Measure矩形的轮廓
		/*HObject hContour, hContourCross;
		HTuple hRowTmp, hColTmp;
		GetMetrologyObjectMeasures(&hContour, hMetrologyHandle, "all", "all", &hRowTmp, &hColTmp);
		GenCrossContourXld(&hContourCross, hRowTmp, hColTmp, 3, 0.785398);*/

		// 填充数据，判断哪些锡球不存在
		int ballNumRow = detBalls.size();
		int ballNumCol = detBalls[0].size();
		auto computeDistance = [](Point2f p1, Point2f p2) {return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)); };
		int k = 0;
		int ballid = 0;
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
					for (; k < ballNum; ) //遍历找到的所有锡球
					{
						Point2f ballTmp;
						ballTmp.y = hBallParameter[3 * k];
						ballTmp.x = hBallParameter[3 * k + 1];
						double radiusTmp = hBallParameter[3 * k + 2];
						double disTmp = computeDistance(detBalls[i][j].detBallCenter, ballTmp);

						if (disTmp < stdBallRadius) {  //更新查找到的锡球数据
							detBalls[i][j].detBallCenter = ballTmp;
							detBalls[i][j].radius = radiusTmp;
							detBalls[i][j].area = myPi * radiusTmp * radiusTmp;
							detBalls[i][j].isMissing = false;

							// 获取轮廓，计算球质量
							HObject hContour, hContourCross;
							HTuple hRowTmp, hColTmp, hPointsNumTmp;
							GetMetrologyObjectMeasures(&hContour, hMetrologyHandle, ballid, "all", &hRowTmp, &hColTmp);
							TupleLength(hRowTmp, &hPointsNumTmp);
							//GenCrossContourXld(&hContourCross, hRowTmp, hColTmp, 3, 0.785398);
							double qualiryTmp = hPointsNumTmp.D() / double(measureNum) * 100;
							detBalls[i][j].quality = (qualiryTmp > 100) ? 100 : qualiryTmp;
							++k;
							++ballid;
							break;   // 跳出内层循环
						}
						else  //该位置处没有锡球
						{
							detBalls[i][j].detBallCenter = detBalls[i][j].templateBallCenter;
							detBalls[i][j].radius = stdBallRadius;
							detBalls[i][j].area = 0;
							detBalls[i][j].isMissing = true;
							detBalls[i][j].contrast = 0;
							++ballid;
							break;   // 跳出内层循环
						}
					}
					if (k > ballNum)  //对应位置没有找到锡球
					{
						detBalls[i][j].detBallCenter = detBalls[i][j].templateBallCenter;
						detBalls[i][j].radius = stdBallRadius;
						detBalls[i][j].area = 0;
						detBalls[i][j].isMissing = true;
						detBalls[i][j].contrast = 0;
					}
				}
			}
		}
		return 0;
	}

	// 重载，阈值分割粗定位+2D Measure 获取锡球轮廓,优化输入输出，降低运算时间
	int findBGABallMetrology(HObject& hDetImg, HTuple& hMetrologyHandle, HTuple& ballCoarseRow, HTuple& ballCoarseCol, HTuple& ballCoarseRadius, const double& thresholdMeasure, const double& measureLen1, const double& measureLen2, const double& sigma, const HTuple& hGenParamName, const HTuple& hGenParamValue, const HTuple& hMetrologyObjName, const HTuple& hMetrologyObjValue, const int& measureNum, const double& dBallDetRadius, const double& stdBallRadius, vector<vector<BGADetInfo>>& detBalls)
	{
		HTuple hMetrologyCircleIndices, hBallParameter, hBallFindNum;

		AddMetrologyObjectCircleMeasure(hMetrologyHandle, ballCoarseRow, ballCoarseCol, ballCoarseRadius, measureLen1, measureLen2, sigma, thresholdMeasure, hMetrologyObjName, hMetrologyObjValue, &hMetrologyCircleIndices);

		ApplyMetrologyModel(hDetImg, hMetrologyHandle);
		GetMetrologyObjectResult(hMetrologyHandle, hMetrologyCircleIndices, "all", "result_type", "all_param", &hBallParameter);

		TupleLength(hBallParameter, &hBallFindNum);
		int ballNum = hBallFindNum / 3;  //hBallParameter依次存储每个找到的圆的row, col和radius;

		// Debug，获取Measure矩形的轮廓
#if 0
		HObject hContour, hContourCross;
		HTuple hRowTmp, hColTmp;
		GetMetrologyObjectMeasures(&hContour, hMetrologyHandle, "all", "all", &hRowTmp, &hColTmp);
		GenCrossContourXld(&hContourCross, hRowTmp, hColTmp, 3, 0.785398);
#endif

		// 填充数据，判断哪些锡球不存在
		int ballNumRow = detBalls.size();
		int ballNumCol = detBalls[0].size();
		auto computeDistance = [](Point2f p1, Point2f p2) {return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)); };
		int k = 0;
		int ballid = 0;
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
					for (; k < ballNum; ) //遍历找到的所有锡球
					{
						Point2f ballTmp;
						ballTmp.y = hBallParameter[3 * k];
						ballTmp.x = hBallParameter[3 * k + 1];
						double radiusTmp = hBallParameter[3 * k + 2];
						double disTmp = computeDistance(detBalls[i][j].detBallCenter, ballTmp);

						if (disTmp < stdBallRadius) {  //更新查找到的锡球数据
							detBalls[i][j].detBallCenter = ballTmp;
							detBalls[i][j].radius = radiusTmp;
							detBalls[i][j].area = myPi * radiusTmp * radiusTmp;
							detBalls[i][j].isMissing = false;

							// 获取轮廓，计算球质量
							HObject hContour;
							HTuple hRowTmp, hColTmp, hPointsNumTmp;
							GetMetrologyObjectMeasures(&hContour, hMetrologyHandle, ballid, "all", &hRowTmp, &hColTmp);
							TupleLength(hRowTmp, &hPointsNumTmp);
#if 0
							// Debug, 显示轮廓和边缘点
							HObject hCircleTmp, hContourCross;
							GenCrossContourXld(&hContourCross, hRowTmp, hColTmp, 3, 0.785398);
							GenCircleContourXld(&hCircleTmp, ballTmp.y, ballTmp.x, radiusTmp, 0, 6.28318, "positive", 1);
#endif
							double qualiryTmp = hPointsNumTmp.D() / double(measureNum) * 100;
							detBalls[i][j].quality = (qualiryTmp > 100) ? 100 : qualiryTmp;
							++k;
							++ballid;
							break;   // 跳出内层循环
						}
						else  //该位置处没有锡球
						{
							detBalls[i][j].detBallCenter = detBalls[i][j].templateBallCenter;
							detBalls[i][j].radius = stdBallRadius;
							detBalls[i][j].area = 0;
							detBalls[i][j].isMissing = true;
							detBalls[i][j].contrast = 0;
							++ballid;
							break;   // 跳出内层循环
						}
					}
					if (k > ballNum)  //找到的锡球已遍历完成，处理剩余的锡球。Layout中存在的锡球在对应位置上没有找到锡球。
					{
						detBalls[i][j].detBallCenter = detBalls[i][j].templateBallCenter;
						detBalls[i][j].radius = stdBallRadius;
						detBalls[i][j].area = 0;
						detBalls[i][j].isMissing = true;
						detBalls[i][j].contrast = 0;
					}
				}
			}
		}
		hMetrologyCircleIndices.Clear();
		ClearMetrologyObject(hMetrologyHandle, "all");
		return 0;
	}

	// 重载，阈值分割粗定位+2D Measure 获取锡球轮廓,优化输入输出，降低运算时间
	int findBGABallMetrology(HObject& hDetImg, HTuple& hMetrologyHandle, HTuple& ballCoarseRow, HTuple& ballCoarseCol, HTuple& ballCoarseRadius, const double& thresholdMeasure, const double& measureLen1, const double& measureLen2, const double& sigma, const HTuple& hMetrologyObjName, const HTuple& hMetrologyObjValue, const int& measureNum, const double& dBallDetRadius, const double& stdBallRadius, const vector<existedTarget>& ballsExisted, vector<vector<BGADetInfo>>& detBalls)
	{
		HTuple hMetrologyCircleIndices, hBallParameter, hBallFindNum;
		//设置测量参数
		AddMetrologyObjectCircleMeasure(hMetrologyHandle, ballCoarseRow, ballCoarseCol, ballCoarseRadius, measureLen1, measureLen2, sigma, thresholdMeasure, hMetrologyObjName, hMetrologyObjValue, &hMetrologyCircleIndices);

		ApplyMetrologyModel(hDetImg, hMetrologyHandle);
		GetMetrologyObjectResult(hMetrologyHandle, hMetrologyCircleIndices, "all", "result_type", "all_param", &hBallParameter);

		TupleLength(hBallParameter, &hBallFindNum);
		int ballNum = hBallFindNum / 3;  //hBallParameter依次存储每个找到的圆的row, col和radius;

		// Debug，获取Measure矩形的轮廓
#if isDebugPT2D
		HObject hContour, hContourCross;
		HTuple hRowTmp, hColTmp;
		GetMetrologyObjectMeasures(&hContour, hMetrologyHandle, "all", "all", &hRowTmp, &hColTmp);
		GenCrossContourXld(&hContourCross, hRowTmp, hColTmp, 3, 0.785398);
#endif

		// 填充数据，判断哪些锡球不存在
		//int ballNumRow = detBalls.size();
		//int ballNumCol = detBalls[0].size();
		auto computeDistance = [](Point2f p1, Point2f p2) {return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)); };
		int k = 0;
		int ballid = 0;
#if 0
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
#endif
					int ballExistNum = ballsExisted.size();
					for (int indexTmp = 0; indexTmp < ballExistNum; ++indexTmp)
					{
						int i = ballsExisted[indexTmp].row;
						int j = ballsExisted[indexTmp].col;
						BGADetInfo ballDetTmp = detBalls[i][j];

						for (; k < ballNum; ) //遍历找到的所有锡球
						{
							Point2f ballTmp;
							ballTmp.y = hBallParameter[3 * k];
							ballTmp.x = hBallParameter[3 * k + 1];
							double radiusTmp = hBallParameter[3 * k + 2];
							double disTmp = computeDistance(ballDetTmp.detBallCenter, ballTmp);

							if (disTmp < stdBallRadius) {  //更新查找到的锡球数据
								detBalls[i][j].detBallCenter = ballTmp;
								detBalls[i][j].radius = radiusTmp;
								//detBalls[i][j].area = myPi * radiusTmp * radiusTmp;
								detBalls[i][j].isMissing = false;
								detBalls[i][j].radiusNorm = radiusTmp;

								// 获取轮廓，计算球质量
								HObject hContour;
								HTuple hRowTmp, hColTmp, hPointsNumTmp;
								GetMetrologyObjectMeasures(&hContour, hMetrologyHandle, ballid, "all", &hRowTmp, &hColTmp);
								TupleLength(hRowTmp, &hPointsNumTmp);
#if isDebugPT2D
								// Debug, 显示轮廓和边缘点
								HObject hCircleTmp, hContourCross;
								GenCrossContourXld(&hContourCross, hRowTmp, hColTmp, 3, 0.785398);
								GenCircleContourXld(&hCircleTmp, ballTmp.y, ballTmp.x, radiusTmp, 0, 6.28318, "positive", 1);
#endif
								double qualiryTmp = hPointsNumTmp.D() / double(measureNum + Inf) * 100;
								detBalls[i][j].quality = (qualiryTmp > 100) ? 100 : qualiryTmp;

#if 0
								// 计算球挤压度
								HObject hContourFindedTmp;
								HTuple hContourCenterRow, hContourCenterCol, hContourAngle, hContourLength, hContourWidth;
								GenContourPolygonXld(&hContourFindedTmp, hRowTmp, hColTmp);
								SmallestRectangle2Xld(hContourFindedTmp, &hContourCenterRow, &hContourCenterCol, &hContourAngle, &hContourLength, &hContourWidth);

								detBalls[i][j].squash = abs(hContourLength.D() - radiusTmp) / (radiusTmp + Inf) * 100;
								detBalls[i][j].squash = hContourWidth.D() / (hContourLength.D() + Inf) * 100;
#endif
								++k;
								++ballid;
								break;   // 跳出内层循环
							}
							else  //该位置处没有锡球
							{
								detBalls[i][j].detBallCenter = ballDetTmp.templateBallCenter;
								detBalls[i][j].radius = stdBallRadius;
								detBalls[i][j].radiusNorm = stdBallRadius;
								detBalls[i][j].area = 0;
								detBalls[i][j].isMissing = true;
								detBalls[i][j].contrast = 0;
								detBalls[i][j].squash = 0;
								++ballid;
								break;   // 跳出内层循环
							}
						}
						if (k > ballNum)  //找到的锡球已遍历完成，处理剩余的锡球。Layout中存在的锡球在对应位置上没有找到锡球。
						{
							detBalls[i][j].detBallCenter = ballDetTmp.templateBallCenter;
							detBalls[i][j].radius = stdBallRadius;
							detBalls[i][j].radiusNorm = stdBallRadius;
							detBalls[i][j].area = 0;
							detBalls[i][j].isMissing = true;
							detBalls[i][j].contrast = 0;
							detBalls[i][j].squash = 0;
						}
					}
					return 0;
				}

	int BGABallCoordinateTune(int ballExistNum, double begin, double end, int ballNumRow, int ballNumCol, double stdBallPitchX, double stdBallPitchY, vector < vector<BGADetInfo>> &detBalls) {
		// 计算锡球到其左邻和下邻的距离
		int ballIndexMin = ballExistNum * begin;
		int ballIndexMax = ballExistNum * end;
		int ballIndx = 0;
		int indxTmp = 0;
		vector<double> ballDisDetX(ballIndexMax - ballIndexMin + 1);
		vector<double> ballDisTemX(ballIndexMax - ballIndexMin + 1);
		vector<double> ballDisDetY(ballIndexMax - ballIndexMin + 1);
		vector<double> ballDisTemY(ballIndexMax - ballIndexMin + 1);
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
					++ballIndx;
					if ((ballIndx >= ballIndexMin) && (ballIndx <= ballIndexMax))
					{
						if (detBalls[i][j].isMissing == false)
						{
							if ((j - 1) <= 0)           //左邻
							{
								ballDisDetX[indxTmp] = 0;
								ballDisTemX[indxTmp] = 0;
							}
							else if (detBalls[i][j - 1].isExist == 0)
							{
								ballDisDetX[indxTmp] = 0;
								ballDisTemX[indxTmp] = 0;
							}
							else
							{
								ballDisDetX[indxTmp] = detBalls[i][j].detBallCenter.x - detBalls[i][j - 1].detBallCenter.x;
								ballDisTemX[indxTmp] = stdBallPitchX;
							}
							if ((i + 1) >= ballNumRow)        //下邻
							{
								ballDisDetY[indxTmp] = 0;
								ballDisTemY[indxTmp] = 0;
							}
							else if (detBalls[i + 1][j].isExist == 0)
							{
								ballDisDetY[indxTmp] = 0;
								ballDisTemY[indxTmp] = 0;
							}
							else
							{
								ballDisDetY[indxTmp] = detBalls[i + 1][j].detBallCenter.y - detBalls[i][j].detBallCenter.y;
								ballDisTemY[indxTmp] = stdBallPitchY;
							}
							++indxTmp;
						}
					}
				}
			}
		}
		double ballDisDetSumX = std::accumulate(ballDisDetX.begin(), ballDisDetX.end(), 0.0);
		double ballDisTemSumX = std::accumulate(ballDisTemX.begin(), ballDisTemX.end(), 0.0);
		double ballDisDetSumY = std::accumulate(ballDisDetY.begin(), ballDisDetY.end(), 0.0);
		double ballDisTemSumY = std::accumulate(ballDisTemY.begin(), ballDisTemY.end(), 0.0);
		double scaleX = ballDisDetSumX / ballDisTemSumX;
		double scaleY = ballDisDetSumY / ballDisTemSumY;
		// 使用缩放系数对检测到的锡球坐标进行缩放
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
					if (detBalls[i][j].isMissing == false)
					{
						detBalls[i][j].detBallCenter.x = detBalls[i][j].detBallCenter.x / scaleX;
						detBalls[i][j].detBallCenter.y = detBalls[i][j].detBallCenter.y / scaleY;
						//detBalls[i][j].templateBallCenter.x = detBalls[i][j].templateBallCenter.x * scaleX;
						//detBalls[i][j].templateBallCenter.y = detBalls[i][j].templateBallCenter.y * scaleY;
					}
				}
			}
		}
		return 0;
	}

	int BGABallRadiusNormalization(int ballExistNum, double begin, double end, int ballNumRow, int ballNumCol, double stdBallRadius, vector<vector<BGADetInfo>> &detBalls) {
		// 计算锡球半径的均值
		int ballIndexMin = ballExistNum * begin;
		int ballIndexMax = ballExistNum * end;
		int ballIndx = 0;
		double ballRadiusMin = 999999;
		double ballRadiusMax = 0;
		vector<double> detBallRadius(ballExistNum);
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
					double radiusTmp = detBalls[i][j].radius;
					detBallRadius[ballIndx] = radiusTmp;
					if (radiusTmp < ballRadiusMin)
					{
						ballRadiusMin = radiusTmp;
					}
					if (radiusTmp > ballRadiusMax)
					{
						ballRadiusMax = radiusTmp;
					}
					++ballIndx;
				}
			}
		}
		sort(detBallRadius.begin(), detBallRadius.end());
		double ballRadiusDetAvg = std::accumulate(detBallRadius.begin() + ballIndexMin, detBallRadius.begin() + ballIndexMax + 1, 0.0) / (ballIndexMax - ballIndexMin + 1);


#if 0
		double ballRadiusDetDev = 0;
		for (int i = ballIndexMin; i < ballIndexMax; ++i)
		{
			ballRadiusDetDev += (detBallRadius[i] - ballRadiusDetAvg) * (detBallRadius[i] - ballRadiusDetAvg);
		}
		ballRadiusDetDev = sqrt(ballRadiusDetDev / (ballIndexMax - ballIndexMin));
#endif

		double radiusMaxMin = ballRadiusMax - ballRadiusMin;
		// 使用半径均值对半径进行normalization
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
					if (detBalls[i][j].isMissing == false)
					{
						//double exponent = -1.0 * pow(detBalls[i][j].radius - ballRadiusDetAvg, 2.0) / (2.0 * ballRadiusDetDev * ballRadiusDetDev);

						//detBalls[i][j].radiusNorm = (1.0 / (sqrt(2.0*myPi)*ballRadiusDetDev))*exp(exponent);
						//detBalls[i][j].radiusNorm = ballRadiusDetAvg * (0.5 * (1 + (erf((detBalls[i][j].radius - ballRadiusDetAvg) / (ballRadiusDetDev * sqrt(2.0))))) + 0.5);

						//detBalls[i][j].radiusNorm = (detBalls[i][j].radius - ballRadiusDetAvg) / radiusMaxMin + stdBallRadius;

						//detBalls[i][j].radiusNorm = (detBalls[i][j].radius - ballRadiusDetAvg) / ballRadiusDetDev + stdBallRadius;
						detBalls[i][j].radiusNorm = detBalls[i][j].radius / ballRadiusDetAvg * stdBallRadius;
						//detBalls[i][j].radius = detBalls[i][j].radius / ballRadiusDetAvg * stdBallRadius;
					}
				}
			}
		}
		return 0;
	}


	void pkgRegion2Flaw(HObject & defectRegion, FlawInfoStruct & outFlaw, const bool& useContour, const bool& useRotateRect, HTuple & homMat2D)
	{
		double areaTmp, angleTmp, flawWidth, flawHeight;
		Point2f centerPoint;
		vector<Point2f> rectCountour;
		vector<vector<Point2f>> defectContours;
		zkhyPublicFuncHN::genFlawInfoFromRegion(defectRegion, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectCountour, defectContours, false, true);
		outFlaw.FlawBasicInfo.flawArea = areaTmp;
		outFlaw.FlawBasicInfo.angle = angleTmp;
		outFlaw.FlawBasicInfo.flawWidth = flawWidth;
		outFlaw.FlawBasicInfo.flawHeight = flawHeight;
		outFlaw.FlawBasicInfo.centerPt = centerPoint;
		outFlaw.FlawBasicInfo.pts = rectCountour;
		outFlaw.FlawBasicInfo.flawContours = defectContours;
	}

	// TODO	使用halcon1D测量实现2D测量的功能
	int metrologyCircle(HObject & hDetImg, HTuple & hMetrologyHandle, HTuple & targetRow, HTuple & targetCol, HTuple & targetRadius)
	{
		return 0;
	}
	int findBGABallMetrologyd(HObject & hDetImg, HTuple & hMetrologyHandle, HTuple & ballCoarseRow, HTuple & ballCoarseCol, HTuple & ballCoarseRadius, const double& thresholdMeasure, const double& measureLen1, const double& measureLen2, const double& sigma, const HTuple & hGenParamName, const HTuple & hGenParamValue, const HTuple & hMetrologyObjName, const HTuple & hMetrologyObjValue, const int& measureNum, const double& dBallDetRadius, const double& stdBallRadius, vector<vector<BGADetInfo>>&detBalls)
	{
		HTuple hMetrologyCircleIndices, hBallParameter, hBallFindNum;
		//设置测量参数
		AddMetrologyObjectCircleMeasure(hMetrologyHandle, ballCoarseRow, ballCoarseCol, ballCoarseRadius, measureLen1, measureLen2, sigma, thresholdMeasure, hGenParamName, hGenParamValue, &hMetrologyCircleIndices);

		// 一次设置所有需要设置的测量参数
		SetMetrologyObjectParam(hMetrologyHandle, hMetrologyCircleIndices, hMetrologyObjName, hMetrologyObjValue);
		ApplyMetrologyModel(hDetImg, hMetrologyHandle);
		GetMetrologyObjectResult(hMetrologyHandle, hMetrologyCircleIndices, "all", "result_type", "all_param", &hBallParameter);
		return 0;
	}

	// 重载，计算球质量，调整输入参数
	int ballQualityNormald(HObject & img, HTuple & measureHandle, int amplitudeThre, HTuple hTransition, double startX, double startY, double angle, double innerCircleR, double outerCircleR, HTuple hSigma, HTuple hSelect, HTuple & hRowMeasure, HTuple & hColMeasure, HTuple & hAmplitudeMeasure, HTuple & hDistanceMeasure)
	{
		// 计算直线与内圆和外圆的交点，圆心坐标+偏移量
		double lineStartX = startX + innerCircleR * cos(angle);
		double lineStartY = startY - innerCircleR * sin(angle);
		double lineEndX = startX + outerCircleR * cos(angle);
		double lineEndY = startY - outerCircleR * sin(angle);
		//cout << "start:" << lineStartX << ", " << lineStartY << "   end:" << lineEndX << ", " << lineEndY << endl;
		// 构建测量矩形
		HTuple hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlDr, hTmpCtrlDc, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2, hMsrHandleMeasure;
		hTmpCtrlRow = 0.5 * (lineStartY + lineEndY);
		hTmpCtrlCol = 0.5 * (lineStartX + lineEndX);
		//hTmpCtrlDr = lineStartY - lineEndY;
		//hTmpCtrlDc = lineStartX - lineEndX;
		//hTmpCtrlPhi = hTmpCtrlDr.TupleAtan2(hTmpCtrlDc);  计算出来角度不正确
		//hTmpCtrlPhi = angle;
		//hTmpCtrlLen1 = 0.5 * ((hTmpCtrlDr * hTmpCtrlDr + hTmpCtrlDc * hTmpCtrlDc).TupleSqrt());
		//hTmpCtrlLen2 = roiWidth;
		TranslateMeasure(measureHandle, hTmpCtrlRow, hTmpCtrlCol);
		//GenMeasureRectangle2(hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2, hImgWidth, hImgHeight, "nearest_neighbor", &hMsrHandleMeasure);
		/*HObject rectangle1, circle1, circle2;
		GenRectangle2(&rectangle1, hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2);
		GenCircle(&circle1, lineStartY, lineStartX, 1);
		GenCircle(&circle2, lineEndY, lineEndX, 1);*/
		if (hSigma.D() >= 0.5 * hTmpCtrlLen1)
			hSigma = 0.4;
		// 计算构建的矩形和内外圆的交点坐标
		try {
			MeasurePos(img, measureHandle, hSigma, amplitudeThre, hTransition, hSelect, &hRowMeasure, &hColMeasure, &hAmplitudeMeasure, &hDistanceMeasure);
		}
		catch (HException& error) {
			string errormsg = error.ErrorMessage().Text();
			return 8;
			//Sleep(10);
		}
		return 0;
	}


	int BGABallFeatureStatic(const vector<vector<BGADetInfo>>&balls, const int ballNum, vector<FlawInfoStruct>&staticRes)
	{
		// 遍历计算 balls 中每个特征的最小值、最大值、均值
		int ballNumRow = balls.size();
		int ballNumCol = balls[0].size();
		vector<double> ballRadius(ballNum), ballContrast(ballNum), ballQuality(ballNum), ballSquash(ballNum), ballPitchX(ballNum), ballPitchY(ballNum), ballOffsetX(ballNum), ballOffsetY(ballNum), ballOffsetXY(ballNum);
		if (ballNum > 0) {
			int ballIndx = 0;
			for (int i = 0; i < ballNumRow; ++i)
			{
				for (int j = 0; j < ballNumCol; ++j)
				{
					if (balls[i][j].isExist == 1 && balls[i][j].isMissing == false)  //锡球存在时执行运算
					{
						ballRadius[ballIndx] = balls[i][j].radiusNorm;
						ballContrast[ballIndx] = balls[i][j].contrast;
						ballQuality[ballIndx] = balls[i][j].quality;
						ballSquash[ballIndx] = balls[i][j].squash;
						ballPitchX[ballIndx] = balls[i][j].pitchX;
						ballPitchY[ballIndx] = balls[i][j].pitchY;
						ballOffsetX[ballIndx] = balls[i][j].offsetX;
						ballOffsetY[ballIndx] = balls[i][j].offsetY;
						ballOffsetXY[ballIndx] = balls[i][j].offsetXY;
						++ballIndx;
					}
				}
			}
		}

		// 计算最小值、最大值、均值
		vector<double> ballRadiusStatic, ballContrastStatic, ballQualityStatic, ballSquashStatic, ballPitchXStatic, ballPitchYStatic, ballOffsetXStatic, ballOffsetYStatic, ballOffsetXYStatic;

		zkhyPublicFuncHN::numericalStatic(ballRadius, ballRadiusStatic);
		zkhyPublicFuncHN::numericalStatic(ballContrast, ballContrastStatic);
		zkhyPublicFuncHN::numericalStatic(ballQuality, ballQualityStatic);
		zkhyPublicFuncHN::numericalStatic(ballSquash, ballSquashStatic);
		zkhyPublicFuncHN::numericalStatic(ballPitchX, ballPitchXStatic);
		zkhyPublicFuncHN::numericalStatic(ballPitchY, ballPitchYStatic);
		zkhyPublicFuncHN::numericalStatic(ballOffsetX, ballOffsetXStatic);
		zkhyPublicFuncHN::numericalStatic(ballOffsetY, ballOffsetYStatic);
		zkhyPublicFuncHN::numericalStatic(ballOffsetXY, ballOffsetXYStatic);

		// 填充输出 staticRes 第一行 为最小值，第二行为均值，第三行为最大值
		int staticNum = staticRes.size();
		for (int i = 0; i < staticNum; ++i)
		{
			staticRes[i].extendParameters.d_params.block.ballWidth = ballRadiusStatic[i] * 2;
			staticRes[i].extendParameters.d_params.block.ballContrast = ballContrastStatic[i];
			staticRes[i].extendParameters.d_params.block.ballQuality = ballQualityStatic[i];
			staticRes[i].extendParameters.d_params.block.ballSquash = ballSquashStatic[i];
			staticRes[i].extendParameters.d_params.block.ballXPitch = ballPitchXStatic[i];
			staticRes[i].extendParameters.d_params.block.ballYPitch = ballPitchYStatic[i];
			staticRes[i].extendParameters.d_params.block.ballXOffset = ballOffsetXStatic[i];
			staticRes[i].extendParameters.d_params.block.ballYOffset = ballOffsetYStatic[i];
			staticRes[i].extendParameters.d_params.block.ballROffset = ballOffsetXYStatic[i];
		}

		return 0;
	}

	int ballQualityPatternMatching(HObject & hBallSeged, const double radiusCofficient, const int ballNumRow, const int ballNumCol, vector<vector<BGADetInfo>>&balls)
	{
		// 获取 锡球 在 半径 * radiusCofficient 处的灰度值，若其大于 lowThreshold 则认为锡球在该处的点是有效点，否则认为是无效点。记录有效点占全部点的壁纸作为锡球的质量

		// 生成锡球区域
		HTuple hBallRow, hBallCol, hBallRadius;
		hBallRow.Clear();
		hBallCol.Clear();
		hBallRadius.Clear();
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (balls[i][j].isExist == 1)
					if (balls[i][j].isMissing == 0)
					{
						hBallRow.Append(balls[i][j].detBallCenter.y);
						hBallCol.Append(balls[i][j].detBallCenter.x);
						hBallRadius.Append(balls[i][j].radius * radiusCofficient);
					}
			}
		}
		HObject hBallsRegion, hBallsRegionInner, hBallsRegionOutter, hBallsRegionUnion, hBallsRegionTmp, hBallsQualityRegion, hBallsRegionOutterUnion;
		HTuple hBallsQUalityRegionArea, hRingsRegionArea;
		GenCircle(&hBallsRegion, hBallRow, hBallCol, hBallRadius);

		// 外扩1像素，内缩1像素
		DilationCircle(hBallsRegion, &hBallsRegionOutter, 1);
		ErosionCircle(hBallsRegion, &hBallsRegionInner, 1);
		Union1(hBallsRegionOutter, &hBallsRegionOutterUnion);
		Difference(hBallsRegionOutterUnion, hBallsRegionInner, &hBallsRegionTmp);
		// 计算交集
		Intersection(hBallsRegionTmp, hBallSeged, &hBallsQualityRegion);
		Intersection(hBallsRegion, hBallsQualityRegion, &hBallsQualityRegion);
		AreaCenter(hBallsQualityRegion, &hBallsQUalityRegionArea, NULL, NULL);
		Intersection(hBallsRegion, hBallsRegionTmp, &hBallsRegionTmp);
		AreaCenter(hBallsRegion, &hRingsRegionArea, NULL, NULL);

		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (balls[i][j].isExist == 1)
					if (balls[i][j].isMissing == 0)
					{
						int indexTmp = i * ballNumRow + j;
						balls[i][j].quality = hBallsQUalityRegionArea[indexTmp].D() / hRingsRegionArea[indexTmp].D() * 100;
					}
			}
		}

		return 0;
	}

	int normWithStdValue(const vector<double>&oriValue, const vector<double>&stdValue, vector<double>&normValue, const double scaleCoef) {

		int dataNum = oriValue.size();
		if (dataNum != stdValue.size())
			return 1;	// 标准值需要和原始值数量一样

		// 计算均值
		double dataMean = std::accumulate(oriValue.begin(), oriValue.end(), 0.0) / dataNum;
		for (int i = 0; i < dataNum; ++i)
		{
			//normValue[i] = oriValue[i] * stdValue[i] / dataMean;
			//normValue[i] = (normValue[i] - stdValue[i]) * scaleCoef + stdValue[i];
			normValue[i] = (oriValue[i] - dataMean) * scaleCoef + stdValue[i];
		}

		return 0;
	}

	void physic2Pixel(vector<double>&inputData, const double pixelEquivalent)
	{
		int dataNum = inputData.size();
		for (int i = 0; i < dataNum; ++i)
		{
			inputData[i] = inputData[i] / pixelEquivalent;
		}
	}



	int pointsRansac(const vector<Point2f>&oriPoints, vector<Point2f>&outPoints, const int iteration, const double margin, const double error)
	{
		// ransac 算法，对输入的点集进行筛选，剔除到拟合的直线范围大于margin的点，输出筛选后的点集
		// 输入：oriPoints，输入的点集
		// 输出：outPoints，筛选后的点集

		int pointNum = oriPoints.size();
		if (pointNum < 2)
			return 1;	// 输入点集数量不足

		// 设置随机数生成器
		random_device rd;
		mt19937 gen(rd());
		uniform_int_distribution<> uniformRand(0, pointNum - 1);

		double errorTmp = 99999;
		vector<bool> inlierFlag(pointNum, false);
		for (int i = 0; i < iteration && errorTmp < error; ++i)
		{
			// 随机选取两个点，作为直线
			int pointIdx1, pointIdx2;
			pointIdx1 = uniformRand(gen);
			pointIdx2 = uniformRand(gen);
			while (pointIdx1 == pointIdx2)
			{
				pointIdx2 = uniformRand(gen);
			}
		}

		return 0;
	}


	// 锡球粗定位，使用阈值分割获得锡球区域
	int BGABallCoarseLocate(HObject& hDetImg, HObject& hSegRegions, const double dBallDetRadius, const double stdBallRadius, const double dOpenRadius, const int ballExistNum, const vector<existedTarget>& ballsExisted, vector<vector<BGADetInfo>>& detBalls, HObject& hBallRegions, HTuple& hBallCenterRow, HTuple& hBallCenterCol) {
		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj;

		int k = 0;
		for (int k = 0; k < ballExistNum; ++k)
		{
			int i = ballsExisted[k].row;
			int j = ballsExisted[k].col;
			BGADetInfo detball = detBalls[i][j];
			if (detball.isExist == 1)  //锡球存在时执行运算
			{
				GenCircle(&hTemplateBallRegion, detball.templateBallCenter.y, detball.templateBallCenter.x, dBallDetRadius);
				//根据坐标和锡球半径生成模板图锡球轮廓
				//ReduceDomain(hDetImg, hTemplateBallRegion, &hImgReduced);
				//Threshold(hImgReduced, &hDetBallRegion, thresholdLow, thresholdHigh);
				Intersection(hTemplateBallRegion, hSegRegions, &hDetBallRegion);
				//Connection(hDetBallRegion, &hDetBallRegions);
				//SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);

				if (zkhyPublicFuncHN::isHObjectEmpty(hDetBallRegion))
				{	//没找到锡球
					detball.detBallCenter.x = detball.templateBallCenter.x;
					detball.detBallCenter.y = detball.templateBallCenter.y;
					detball.radius = stdBallRadius;
					HObject hEmptyRegion;
					GenEmptyRegion(&hEmptyRegion);
					ConcatObj(hBallRegions, hEmptyRegion, &hBallRegions);		// 输出分割的轮廓
				}
				else
				{
					HTuple hSegArea;
					Union1(hDetBallRegion, &hDetBallRegion);
					Intensity(hDetBallRegion, hDetImg, &hBallGrayMean, &hBallGrayDeviation);             // 计算分割出来锡球的灰度均值
					AreaCenter(hDetBallRegion, &hSegArea, NULL, NULL);

					FillUp(hDetBallRegion, &hDetBallRegion);		// 填充锡球区域内部的空洞)
					//ConcatObj(hBallRegions, hDetBallRegion, &hBallRegions);		// 输出分割的轮廓
					//Union2(hDetBallRegion, hBallRegions, &hBallRegions);		// 输出分割的轮廓

					//ClosingCircle(hDetBallRegion, &hDetBallRegion, stdBallRadius);   //闭运算，填充锡球内部的空洞
					OpeningCircle(hDetBallRegion, &hDetBallRegion, dOpenRadius);  //开运算，去除锡球外部的小斑点				
					ConcatObj(hBallRegions, hDetBallRegion, &hBallRegions);		// 输出分割的轮廓

					HTuple hCircularity;
					Circularity(hDetBallRegion, &hCircularity);

					if (zkhyPublicFuncHN::isHRegionEmpty(hDetBallRegion))
					{	//没找到锡球
						detball.detBallCenter.x = detball.templateBallCenter.x;
						detball.detBallCenter.y = detball.templateBallCenter.y;
						detball.radius = stdBallRadius;
						detball.squash = 1;
					}
					else
					{	//找到了锡球
						// 获取锡球区域轮廓
						HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius;
						SmallestCircle(hDetBallRegion, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius);
						detball.detBallCenter.x = hBallCircleCol.D();
						detball.detBallCenter.y = hBallCircleRow.D();
						detball.radius = hBallCircleRadius.D();
						detball.contrast = hBallGrayMean.D();
						detball.area = hSegArea.D();
						detball.squash = 1 - hCircularity.D();
					}
				}
				detBalls[i][j] = detball;
				hBallCenterCol[k] = detball.detBallCenter.x;
				hBallCenterRow[k] = detball.detBallCenter.y;
			}
		}
		return 0;
	}

	void computeCirclePlygon(Point2f centerPoint, double radius, vector<Point2f>& edgePoints, HObject& hCircleRegion, HTuple hHomMat2D)
	{
		HObject hCircleRegionTrans, hCircleContour, hCircleContourTrans;
		HTuple hRowTmp, hColTmp;
		//GenCircle(&hCircleRegion, centerPoint.y, centerPoint.x, radius);
		//AffineTransRegion(hCircleRegion, &hCircleRegionTrans, hHomMat2D, "nearest_neighbor");
		//GetRegionContour(hCircleRegionTrans, &hRowTmp, &hColTmp);
		GenCircleContourXld(&hCircleContour, centerPoint.y, centerPoint.x, radius, 0, myPi * 2, "positive", 1);
		AffineTransContourXld(hCircleContour, &hCircleContourTrans, hHomMat2D);
		GetContourXld(hCircleContourTrans, &hRowTmp, &hColTmp);
		GenRegionContourXld(hCircleContour, &hCircleRegion, "filled");
		zkhyPublicFuncHN::hTuple2Points(hColTmp, hRowTmp, edgePoints);
	}

	//----------------------------------------------realBGABall2dDefectDet--------------------------//
	//功能：封测底部2D缺陷检测，计算待测图中的pin脚特征或锡球特征（如是否有缺失、锡球在X轴和Y轴上的位置偏移、相邻锡球之间的最小距离、锡球和其外接圆的面积比（球质量）以及球对比度）
	int realBGABall2dDefectDet(const BGABall2dDefectDetInput& input, BGABall2dDefectDetOutput& output)
	{
		// 解析输入参数
		HObject hSrcImg = input.srcImg;     // 输入图像为两幅图像相减的结果
		CSYSTransPara layout2DetectTransPara = input.layout2DetectTransPara;
		// 使用halcon解析坐标系时需要将角度取反
		layout2DetectTransPara.angle = -layout2DetectTransPara.angle;
		vector<Point2f> inputVertexs = input.vertexs;
		BGAParam layoutInfo = input.layoutInfo;
		// 算法参数
		AlgParamBGABall2dDefectDet* pParams = (AlgParamBGABall2dDefectDet*)&input.algPara;
		int iQualityMethod = pParams->i_params.block.iQualityMethod;
		int iSamplingStep = pParams->i_params.block.iSamplingStep;
		int iThresholdSegBallLow = pParams->i_params.block.iThresholdSegBallLow;
		int iThresholdSegBallHigh = pParams->i_params.block.iThresholdSegBallHigh;
		int iBallPitchFunc = pParams->i_params.block.iBallPitchFunc;
		int isNormalizeBallWidth = pParams->i_params.block.isNormalizeBallWidth;
		int iBallRegionMode = pParams->i_params.block.iBallRegionMode;		// 0: 使用阈值分割获取锡球区域，1：使用2D测量获取锡球区域
		int ballDetFunc = pParams->i_params.block.iBallDetFunc;			// 0: 使用阈值分割获取锡球区域，1：使用阈值分割进行粗定位，随后再进行亚像素分割，2：使用2D测量获取锡球区域，3：使用阈值分割进行粗定位，然后使用2D测量获取锡球区域 
		double dInnerCircle = pParams->d_params.block.dInnerCircle;
		double dOuterCircle = pParams->d_params.block.dOuterCircle;
		double dThresholdMeasure = pParams->d_params.block.dThresholdMeasure;
		double dBallAreaMin = pParams->d_params.block.dBallAreaMin;
		double dBallAreaMax = pParams->d_params.block.dBallAreaMax;
		double dBallSquashInner = pParams->d_params.block.dBallSquashInner;
		double timeOutThreshold = pParams->d_params.block.dTimeOutThreshold;

		double dBallBegin = 0.25;
		double dBallEnd = 0.75;

		// 判断输入参数合法性
		// 需要先判断对象是否初始化，再判断是否为空对象，否则判断对象为空时会产生异常
		if (zkhyPublicFuncHN::isHObjectEmpty(hSrcImg))
			return 6;
		if (timeOutThreshold > 999999 || timeOutThreshold <= 0)
			return 1;           // 算法超时参数输入有误
		if (inputVertexs.size() != 5)
			return 7;           // 输入产品顶点信息有误
		if (iQualityMethod > 1)
			return 3;           //球质量计算方法有误

		/*string fileRoot = "D:\\code\\数据解析\\";
		string filename = "BGABall2dDet-0330.txt";
		ofstream ofs;
		ofs.open((fileRoot + filename), ios::out | ios::app);
		ofs.setf(ios::fixed, ios::floatfield);
		ofs.precision(20);
		if (ofs.is_open())
			ofs << "[Alg Debug] Alg_name:[BGABall2dDet] TransPara: " << layout2DetectTransPara.translationValue.x << ", " << layout2DetectTransPara.translationValue.y << ", " << layout2DetectTransPara.angle << endl;
		ofs.close();*/

#if 0
		// prepare GPU
		HTuple hDeviceHandle, hDeviceIdentifer;
		QueryAvailableComputeDevices(&hDeviceIdentifer);
		if (hDeviceIdentifer.Length() > 0)
		{
			OpenComputeDevice(hDeviceIdentifer[0], &hDeviceHandle);
			SetComputeDeviceParam(hDeviceHandle, "asynchronous_execution", "false");
			InitComputeDevice(hDeviceHandle, "all");
			ActivateComputeDevice(hDeviceHandle);
		}
#endif

		// 开始计时
		DWORD startTime = GetTickCount64();
		// 对输入图像及点集进行坐标变换，并获取坐标变换矩阵（HTuple）
		// 计算坐标偏移量，防止图像移到(0,0)导致计算出错
		double coordOffset = input.layoutInfo.ballWidth * 2;
		HTuple layout2DetectTrans;
		vector<Point2f>  vertexs;
		HObject hDetImg, hDetBallRegionAll, hTemplateBallRegionAll, hDetBallRegionSeg, hSegRegions, hSegRegionUnion;
		GenEmptyObj(&hDetImg);
		GenEmptyObj(&hDetBallRegionAll);     //记录所有的锡球区域
		GenEmptyObj(&hTemplateBallRegionAll);     //记录模板锡球区域
		GenEmptyObj(&hDetBallRegionSeg);     //记录阈值分割后的锡球区域，进行了填充和形态学操作
		GenEmptyObj(&hSegRegions);           //记录阈值分割获取的区域，打散
		GenEmptyObj(&hSegRegionUnion);		 //记录阈值分割获取的区域

		int resFlag = packingDetInputTrans(hSrcImg, layout2DetectTransPara, inputVertexs, coordOffset, hDetImg, layout2DetectTrans, vertexs);
		if (resFlag == 1)
			return 6;
		else if (resFlag != 0)
			return 5;
		int iSectionDivisionNum = int(360 / iSamplingStep);

		// Crop Image, using the Transed vertexs !! The speed of this method is slower than using original image.
		//CropRectangle1(hDetImg, &hDetImg, 0, 0, vertexs[2].y + coordOffset, vertexs[2].x + coordOffset);

		//int iSectionDivisionNum = iSamplingStep;
		// Step 1, 解析PKG Information
		int ballNumRow, ballNumCol;
		double stdBallRadius, stdBallPitchX, stdBallPitchY, stdGridOffsetX, stdGridOffsetY, stdLeftUpCornerX, stdLeftUpCornerY;
		HTuple hImgWidth, hImgHeight;
		GetImageSize(hDetImg, &hImgWidth, &hImgHeight);
		vector<vector<BGADetInfo>> detBalls;
		ballNumRow = layoutInfo.gridNumX;
		ballNumCol = layoutInfo.gridNumY;
		stdBallRadius = layoutInfo.ballWidth * 0.5;
		stdBallPitchX = layoutInfo.gridPitchX;
		stdBallPitchY = layoutInfo.gridPitchY;
		stdGridOffsetX = layoutInfo.gridOffsetX;
		stdGridOffsetY = layoutInfo.gridOffsetY;
		stdLeftUpCornerX = layoutInfo.leftUpCornerX;
		stdLeftUpCornerY = layoutInfo.leftUpCornerY;

		// 超参数
		double dRadiusCoef = 1.5;
		double dBallDetRadius = stdBallRadius * dRadiusCoef;

		// 获取左上角锡球坐标偏移量
		Point2f ballLocOffset(0, 0);
#if 0		// 对齐第一颗锡球的位置，该方法会导致右下角锡球模板图中位置和实际位置的误差累加，即误差会被累积到右下角锡球的位置
		Point2f ballFirstTemplateLoc = layoutInfo.posInfo[0][0].pos;
		ballFirstTemplateLoc.x = ballFirstTemplateLoc.x + coordOffset;
		ballFirstTemplateLoc.y = ballFirstTemplateLoc.y + coordOffset;

		HTuple hTemplateBallRow, hTemplateBallCol, hTemplateBallRadius, hBallGrayMean, hBallGrayDeviation, hIsEmpty;
		HObject hImgReduced, hTemplateBallRegion, hDetBallRegion, hDetBallRegions, hDetBallRegionsSelected, hEmptyObj;
		GenCircle(&hTemplateBallRegion, ballFirstTemplateLoc.y, ballFirstTemplateLoc.x, dBallDetRadius);
		//根据坐标和锡球半径生成模板图锡球轮廓
		ReduceDomain(hDetImg, hTemplateBallRegion, &hImgReduced);
		Threshold(hImgReduced, &hDetBallRegion, iThresholdSegBallLow, iThresholdSegBallHigh);

		Connection(hDetBallRegion, &hDetBallRegions);
		SelectShape(hDetBallRegions, &hDetBallRegionsSelected, "area", "and", dBallAreaMin, dBallAreaMax);

		GenEmptyObj(&hEmptyObj);
		TestEqualObj(hDetBallRegionsSelected, hEmptyObj, &hIsEmpty);

		if (hIsEmpty.I() != 1)   //找到了锡球
		{
			Union1(hDetBallRegionsSelected, &hDetBallRegion);
			// 获取锡球区域轮廓
			HTuple hBallCircleRow, hBallCircleCol, hBallCircleRadius;
			SmallestCircle(hDetBallRegion, &hBallCircleRow, &hBallCircleCol, &hBallCircleRadius);

			ballLocOffset.x = hBallCircleCol.D() - ballFirstTemplateLoc.x;
			ballLocOffset.y = hBallCircleRow.D() - ballFirstTemplateLoc.y;
		}
#endif

#if 0
		// 使用模板IC中心和待测图IC中心的偏移量作为锡球模板坐标的偏移量
		Point2f ICTemplateCenter((layoutInfo.bodySizeX * 0.5 + coordOffset), (layoutInfo.bodySizeY * 0.5 + coordOffset));
		Point2f ICCenter = vertexs[4];
		ballLocOffset.x = ICCenter.x - ICTemplateCenter.x;
		ballLocOffset.y = ICCenter.y - ICTemplateCenter.y;
#endif

#if 1
		// 使用待测图IC左上顶点以及第一颗锡球到左上两边的距离计算锡球模板坐标的偏移量
		Point2f ballFirstTemplateLoc = layoutInfo.posInfo[0][0].pos;
		ballFirstTemplateLoc.x = ballFirstTemplateLoc.x + coordOffset;
		ballFirstTemplateLoc.y = ballFirstTemplateLoc.y + coordOffset;

		ballLocOffset.x = vertexs[0].x + stdLeftUpCornerX - ballFirstTemplateLoc.x;
		ballLocOffset.y = vertexs[0].y + stdLeftUpCornerY - ballFirstTemplateLoc.y;
#endif

		// 计算模板锡球的坐标
		vector<existedTarget> ballExisted;
		resFlag = BGATemplateBallCoordCalculation(ballNumRow, ballNumCol, coordOffset, ballLocOffset, layoutInfo, vertexs, detBalls, ballExisted, hTemplateBallRegionAll);

		int ballExistNum = ballExisted.size();

		if (resFlag != 0)
			return resFlag;
		int flawNum = ballExistNum + 1;
		output.flawsData.resize(flawNum);
		output.ballContour.resize(flawNum);

		// Step 2, 获取待测图中锡球区域

		//获取Layout中锡球的真实行列坐标
		switch (ballDetFunc)
		{
		case 0:	 // 阈值分割
		{
			// 对图像进行分割获取整图分割区域
			Threshold(hDetImg, &hSegRegions, iThresholdSegBallLow, iThresholdSegBallHigh);
			for (int i = 0; i < ballNumRow; ++i)
			{
				for (int j = 0; j < ballNumCol; ++j)
				{
					if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
					{
						findBGABallThresholdSeg(hDetImg, hSegRegions, dBallAreaMin, dBallAreaMax, dBallDetRadius, stdBallRadius, detBalls[i][j], hDetBallRegionSeg);
					}
				}
			}
			break;
		}
		case 1:	 // 阈值分割粗定位，随后再进行亚像素分割
		{
			double morphologyRadius = stdBallRadius * 0.3;
			// 对图像进行分割获取整图分割区域
			Threshold(hDetImg, &hSegRegions, iThresholdSegBallLow, iThresholdSegBallHigh);
			for (int i = 0; i < ballNumRow; ++i)
			{
				for (int j = 0; j < ballNumCol; ++j)
				{
					if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
					{
						// 阈值分割进行粗定位，随后在生成的区域内使用最小外接圆生成轮廓锡球
						findBGABallThresholdSegSubpixel(hDetImg, hSegRegions, iThresholdSegBallLow, dBallAreaMin, dBallAreaMax, dBallDetRadius, stdBallRadius, morphologyRadius, detBalls[i][j], hDetBallRegionSeg);
					}
				}
			}
			break;
		}
		case 2:	 // 2D Measure
		{
			// 设置测量参数
			double measureLen1, measureLen2, sigma;
			measureLen1 = stdBallRadius * 0.5;
			measureLen2 = stdBallRadius * 0.1;
			sigma = 1.5;
			// 设置测量句柄
			HTuple hMetrologyHandle;
			CreateMetrologyModel(&hMetrologyHandle);
			SetMetrologyModelImageSize(hMetrologyHandle, hImgWidth, hImgHeight);
			for (int i = 0; i < ballNumRow; ++i)
			{
				for (int j = 0; j < ballNumCol; ++j)
				{
					if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
					{
						findBGABallMetrology(hDetImg, hMetrologyHandle, iThresholdSegBallLow, dBallDetRadius, stdBallRadius, measureLen1, measureLen2, sigma, detBalls[i][j]);
					}
				}
			}
			break;
		}
		case 3:	 // 阈值分割粗定位+2D Measure
		{
			// 对图像进行分割获取整图分割区域
			HObject hSegRegionTmp, hSegRegion;
			Threshold(hDetImg, &hSegRegion, iThresholdSegBallLow, iThresholdSegBallHigh);
			Connection(hSegRegion, &hSegRegionTmp);
			SelectShape(hSegRegionTmp, &hSegRegions, "area", "and", dBallAreaMin, dBallAreaMax);
			Union1(hSegRegions, &hSegRegionUnion);
			// 粗定位获得锡球中心点坐标
			HTuple hBallCoarseRow, hBallCoarseCol, hBallCoarseRadius;
			hBallCoarseRow.Clear();
			hBallCoarseCol.Clear();
			hBallCoarseRadius.Clear();
			TupleGenConst(ballExistNum, 0, &hBallCoarseRow);
			TupleGenConst(ballExistNum, 0, &hBallCoarseCol);

			double dOpenRadius = stdBallRadius * 0.1;
			BGABallCoarseLocate(hDetImg, hSegRegionUnion, dBallDetRadius, stdBallRadius, dOpenRadius, ballExistNum, ballExisted, detBalls, hDetBallRegionSeg, hBallCoarseRow, hBallCoarseCol);
			// 2D Measure
			// 设置测量参数
			double measureLen1, measureLen2, sigma, ransacDist;
			// 设置的内外圆检测范围无效，只能以圆周为基准，生成对称的扫描矩形
			measureLen1 = (stdBallRadius * dOuterCircle - stdBallRadius * dInnerCircle) * 0.5;
			measureLen2 = myPi * stdBallRadius * iSamplingStep / 180 * 0.5;

			// 根据扫描圆环的内外环半径设置测量圆的半径
			double dBallRadiusMeasure = stdBallRadius * dInnerCircle + measureLen1;
			TupleGenConst(ballExistNum, dBallRadiusMeasure, &hBallCoarseRadius);

			sigma = 1.5;
			ransacDist = 2.0;
			int numInstance = 1;
			// 由亮到暗，第一个点
			HTuple transition = "negative";
			HTuple select = "first";
			double minScore = 0.5;
			long randSeed = 2023042417253234;  //设置为0时是完全随机 2023042417253234
			// 设置测量句柄
			HTuple hMetrologyHandle;
			CreateMetrologyModel(&hMetrologyHandle);
			SetMetrologyModelImageSize(hMetrologyHandle, hImgWidth, hImgHeight);

			// 设置测量参数

			// 将所有需要设置的测量参数写入HTuple，便于后续设置
			HTuple hMetrologyObjName, hMetrologyObjValue;
			hMetrologyObjName.Clear();
			hMetrologyObjValue.Clear();
			hMetrologyObjName[0] = "num_instances";
			hMetrologyObjName[1] = "num_measures";
			hMetrologyObjName[2] = "measure_transition";
			hMetrologyObjName[3] = "measure_select";
			hMetrologyObjName[4] = "min_score";
			hMetrologyObjName[5] = "rand_seed";
			hMetrologyObjName[6] = "distance_threshold";		// Ransac距离
			//hMetrologyObjName[7] = "measure_interpolation";
			hMetrologyObjValue[0] = numInstance;
			hMetrologyObjValue[1] = iSectionDivisionNum;
			hMetrologyObjValue[2] = transition;
			hMetrologyObjValue[3] = select;
			hMetrologyObjValue[4] = minScore;
			hMetrologyObjValue[5] = (double)randSeed;
			hMetrologyObjValue[6] = ransacDist;
			//hMetrologyObjValue[7] = "bilinear";
			// 2D Measure
			//findBGABallMetrology(hDetImg, hMetrologyHandle, hBallCoarseRow, hBallCoarseCol, hBallCoarseRadius, dThresholdMeasure,  measureLen1, measureLen2, sigma, numInstance, iSectionDivisionNum, transition, select, minScore, randSeed, dBallDetRadius, stdBallRadius, detBalls);
			//findBGABallMetrology(hDetImg, hMetrologyHandle, hBallCoarseRow, hBallCoarseCol, hBallCoarseRadius, dThresholdMeasure, measureLen1, measureLen2, sigma, hGenParamName, hGenParamValue, hMetrologyObjName, hMetrologyObjValue, iSectionDivisionNum, dBallDetRadius, stdBallRadius, detBalls);
			findBGABallMetrology(hDetImg, hMetrologyHandle, hBallCoarseRow, hBallCoarseCol, hBallCoarseRadius, dThresholdMeasure, measureLen1, measureLen2, sigma, hMetrologyObjName, hMetrologyObjValue, iSectionDivisionNum, dBallDetRadius, stdBallRadius, ballExisted, detBalls);
			ClearMetrologyModel(hMetrologyHandle);
			hMetrologyObjName.Clear();
			hMetrologyObjValue.Clear();
			break;
		}
		case 4:	 // 阈值分割粗定位+1D Measure
		{
			double angleTmp = 0.0;
			// 计算直线与内圆和外圆的交点，圆心坐标+偏移量
			double lineStartX = detBalls[0][0].templateBallCenter.x + dInnerCircle * cos(angleTmp);
			double lineStartY = detBalls[0][0].templateBallCenter.y + dInnerCircle * sin(angleTmp);
			double lineEndX = detBalls[0][0].templateBallCenter.x + dOuterCircle * cos(angleTmp);
			double lineEndY = detBalls[0][0].templateBallCenter.y + dOuterCircle * sin(angleTmp);
			// 生成MeasureHandle
			break;
		}
		default:
			return 5;
		}
#if isDebugPT2D

		HObject hBallTemplateAll, hBallDetected;
		GenEmptyObj(&hBallTemplateAll);
		GenEmptyObj(&hBallDetected);
		for (int i = 0; i < ballNumRow; ++i)
		{
			for (int j = 0; j < ballNumCol; ++j)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
					HObject hBallRegionTmp1, hBallRegionTmp2;
					GenCircle(&hBallRegionTmp1, detBalls[i][j].templateBallCenter.y, detBalls[i][j].templateBallCenter.x, stdBallRadius * 0.8);
					ConcatObj(hBallRegionTmp1, hBallTemplateAll, &hBallTemplateAll);
					if (detBalls[i][j].isMissing == false)
					{
						GenCircle(&hBallRegionTmp2, detBalls[i][j].detBallCenter.y, detBalls[i][j].detBallCenter.x, detBalls[i][j].radius);
						ConcatObj(hBallRegionTmp2, hBallDetected, &hBallDetected);
					}
				}
			}
		}
		Sleep(10);
#endif
		//----------------------------------------------------------------//
		// 显示轮廓用
#if isDebugPT2D

		Mat imgTmp1 = HObject2Mat(hDetImg);
		Mat imgTmp2;
		cv::cvtColor(imgTmp1, imgTmp2, COLOR_GRAY2BGR);
		Mat imgTmp3 = HObject2Mat(hSrcImg);
		cv::cvtColor(imgTmp3, imgTmp3, COLOR_GRAY2BGR);
#endif
		// 对坐标进行微调
		/*bool isTuneCoordinate = false;
		if(isTuneCoordinate)
			BGABallCoordinateTune(ballExistNum, dBallBegin, dBallEnd, ballNumRow, ballNumCol, stdBallPitchX, stdBallPitchY, detBalls);
		bool isNormalizationRadius = false;*/
		// 对锡球半径进行Normalization，只影响 特征值-锡球宽度 不影响实际输出的区域或轮廓
		if (isNormalizeBallWidth == 1)
			BGABallRadiusNormalization(ballExistNum, dBallBegin, dBallEnd, ballNumRow, ballNumCol, stdBallRadius, detBalls);

		// Step 3，计算BGA锡球缺陷特征
		int ballIndx = 0;
		int ballDetectedNum = 0;
		double stdBallArea = myPi * stdBallRadius * stdBallRadius;
		// Parper for computing the ball suqash
#if 1
		HObject hBallSquashInner, hBallSquashOuter, hBallSquashRing, hBallDetectedinRing, hBallSquashOuterUnion;
		HTuple hBallSquashInnerRadius, hBallSquashOuterRadius, hBallRaw, hBallCol;
		hBallSquashInnerRadius.Clear();
		hBallSquashOuterRadius.Clear();
		hBallRaw.Clear();
		hBallCol.Clear();
		int indexTmp = 0;
		for (int indexTmp = 0; indexTmp < ballExistNum; ++indexTmp)
		{
			int i = ballExisted[indexTmp].row;
			int j = ballExisted[indexTmp].col;
			BGADetInfo ballTmp = detBalls[i][j];
			hBallRaw[indexTmp] = ballTmp.detBallCenter.y;
			hBallCol[indexTmp] = ballTmp.detBallCenter.x;
			hBallSquashInnerRadius[indexTmp] = ballTmp.radius * dBallSquashInner;
			hBallSquashOuterRadius[indexTmp] = ballTmp.radius;
		}
		GenCircle(&hBallSquashInner, hBallRaw, hBallCol, hBallSquashInnerRadius);
		GenCircle(&hBallSquashOuter, hBallRaw, hBallCol, hBallSquashOuterRadius);
		// 圆环
		Union1(hBallSquashOuter, &hBallSquashOuterUnion);
		Difference(hBallSquashOuterUnion, hBallSquashInner, &hBallSquashRing);
		Intersection(hBallSquashRing, hSegRegionUnion, &hBallDetectedinRing);
		Intersection(hBallSquashOuter, hBallDetectedinRing, &hBallDetectedinRing);
#endif

		if (iQualityMethod == 1)
		{

			ballQualityPatternMatching(hSegRegions, 0.8, ballNumRow, ballNumCol, detBalls);
		}
		// 按列遍历，从上到下，从左到右，满足黄金样本的要求
		// 注意 ballExisted 中的压入顺序
#if 1
	// Alg 1，Ball Pitch 球距
		if (iBallPitchFunc == 0)
		{   // 0, 计算当前锡球到其两侧锡球的距离，取和标准值偏差最大的作为锡球间距
			double pitchX1, pitchX2, pitchY1, pitchY2;   //依次对应左右上下
			for (int indexTmp = 0; indexTmp < ballExistNum; ++indexTmp)
			{

				int i = ballExisted[indexTmp].row;
				int j = ballExisted[indexTmp].col;
				// 计算锡球的特征
				if (detBalls[i][j].isMissing == false)
				{
					int idxRight, idxLeft, idxUp, idxDown;
					idxRight = j + 1;
					idxLeft = j - 1;
					idxUp = i - 1;
					idxDown = i + 1;
					if ((idxLeft) <= 0)           //x方向
						pitchX1 = 0;
					else
						pitchX1 = detBalls[i][j].detBallCenter.x * detBalls[i][idxLeft].isExist - detBalls[i][idxLeft].detBallCenter.x * detBalls[i][idxLeft].isExist;
					if ((idxRight) >= ballNumCol)
						pitchX2 = 0;
					else
						pitchX2 = detBalls[i][idxRight].detBallCenter.x * detBalls[i][idxRight].isExist - detBalls[i][j].detBallCenter.x * detBalls[i][idxRight].isExist;
					if ((idxUp) <= 0)           //y方向
						pitchY1 = 0;
					else
						pitchY1 = detBalls[i][j].detBallCenter.y * detBalls[idxUp][j].isExist - detBalls[idxUp][j].detBallCenter.y * detBalls[idxUp][j].isExist;
					if ((idxDown) >= ballNumRow)
						pitchY2 = 0;
					else
						pitchY2 = detBalls[idxDown][j].detBallCenter.y * detBalls[idxDown][j].isExist - detBalls[i][j].detBallCenter.y * detBalls[idxDown][j].isExist;
					// PitchX1和PitchX2均为0时，表明其不存在左右邻居，此时使用标准值对其进行填充
					if (pitchX1 == 0 && pitchX2 == 0)
						detBalls[i][j].pitchX = stdBallPitchX;
					else if (pitchX1 == 0 || pitchX2 == 0)
						detBalls[i][j].pitchX = max(pitchX1, pitchX2);
					else if (abs(pitchX1 - stdBallPitchX) > abs(pitchX2 - stdBallPitchX))
						detBalls[i][j].pitchX = pitchX1;
					else
						detBalls[i][j].pitchX = pitchX2;

					if (pitchY1 == 0 && pitchY2 == 0)
						detBalls[i][j].pitchY = stdBallPitchY;
					else if (pitchY1 == 0 || pitchY2 == 0)
						detBalls[i][j].pitchY = max(pitchY1, pitchY2);
					else if (abs(pitchY1 - stdBallPitchY) > abs(pitchY2 - stdBallPitchY))
						detBalls[i][j].pitchY = pitchY1;
					else
						detBalls[i][j].pitchY = pitchY2;
				}
			}
		}
		else if (iBallPitchFunc == 1)
		{	// 1, 计算当前锡球到其右侧锡球和下方锡球的距离作为锡球间距
			double pitchX1, pitchX2, pitchY1, pitchY2;   //依次对应左右上下
			for (int indexTmp = 0; indexTmp < ballExistNum; ++indexTmp)
			{

				int i = ballExisted[indexTmp].row;
				int j = ballExisted[indexTmp].col;
				// 计算锡球的特征
				if (detBalls[i][j].isMissing == false)
				{
					if ((j + 1) >= ballNumCol)  //x方向
						pitchX2 = 0;
					else
						pitchX2 = detBalls[i][j + 1].detBallCenter.x * detBalls[i][j + 1].isExist - detBalls[i][j].detBallCenter.x * detBalls[i][j + 1].isExist;
					if ((i + 1) >= ballNumRow)  //y方向
						pitchY2 = 0;
					else
						pitchY2 = detBalls[i + 1][j].detBallCenter.y * detBalls[i + 1][j].isExist - detBalls[i][j].detBallCenter.y * detBalls[i + 1][j].isExist;
					if (pitchX2 == 0)
						detBalls[i][j].pitchX = -9999;
					else
						detBalls[i][j].pitchX = pitchX2;

					if (pitchY2 == 0)
						detBalls[i][j].pitchY = -9999;
					else
						detBalls[i][j].pitchY = pitchY2;
				}
			}
		}
		else
			return 9;
#endif
		for (int indexTmp = 0; indexTmp < ballExistNum; ++indexTmp)
		{

			int i = ballExisted[indexTmp].row;
			int j = ballExisted[indexTmp].col;
			BGADetInfo ballTmp = detBalls[i][j];
			// 计算锡球的特征
			if (ballTmp.isMissing == false)
			{
#if 0
				// Alg 1，Ball Pitch 球距
				if (iBallPitchFunc == 0)
				{   // 0, 计算当前锡球到其两侧锡球的距离，取和标准值偏差最大的作为锡球间距
					double pitchX1, pitchX2, pitchY1, pitchY2;   //依次对应左右上下
					// 计算锡球的特征
					int idxRight, idxLeft, idxUp, idxDown;
					idxRight = j + 1;
					idxLeft = j - 1;
					idxUp = i - 1;
					idxDown = i + 1;
					if ((idxLeft) <= 0)           //x方向
						pitchX1 = 0;
					else
						pitchX1 = detBalls[i][j].detBallCenter.x * detBalls[i][idxLeft].isExist - detBalls[i][idxLeft].detBallCenter.x * detBalls[i][idxLeft].isExist;
					if ((idxRight) >= ballNumCol)
						pitchX2 = 0;
					else
						pitchX2 = detBalls[i][idxRight].detBallCenter.x * detBalls[i][idxRight].isExist - detBalls[i][j].detBallCenter.x * detBalls[i][idxRight].isExist;
					if ((idxUp) <= 0)           //y方向
						pitchY1 = 0;
					else
						pitchY1 = detBalls[i][j].detBallCenter.y * detBalls[idxUp][j].isExist - detBalls[idxUp][j].detBallCenter.y * detBalls[idxUp][j].isExist;
					if ((idxDown) >= ballNumRow)
						pitchY2 = 0;
					else
						pitchY2 = detBalls[idxDown][j].detBallCenter.y * detBalls[idxDown][j].isExist - detBalls[i][j].detBallCenter.y * detBalls[idxDown][j].isExist;
					// PitchX1和PitchX2均为0时，表明其不存在左右邻居，此时使用标准值对其进行填充
					if (pitchX1 == 0 && pitchX2 == 0)
						detBalls[i][j].pitchX = stdBallPitchX;
					else if (pitchX1 == 0 || pitchX2 == 0)
						detBalls[i][j].pitchX = max(pitchX1, pitchX2);
					else if (abs(pitchX1 - stdBallPitchX) > abs(pitchX2 - stdBallPitchX))
						detBalls[i][j].pitchX = pitchX1;
					else
						detBalls[i][j].pitchX = pitchX2;

					if (pitchY1 == 0 && pitchY2 == 0)
						detBalls[i][j].pitchY = stdBallPitchY;
					else if (pitchY1 == 0 || pitchY2 == 0)
						detBalls[i][j].pitchY = max(pitchY1, pitchY2);
					else if (abs(pitchY1 - stdBallPitchY) > abs(pitchY2 - stdBallPitchY))
						detBalls[i][j].pitchY = pitchY1;
					else
						detBalls[i][j].pitchY = pitchY2;
				}
				else if (iBallPitchFunc == 1)
				{	// 1, 计算当前锡球到其右侧锡球和下方锡球的距离作为锡球间距
					double pitchX1, pitchX2, pitchY1, pitchY2;   //依次对应左右上下
					// 计算锡球的特征
					if ((j + 1) >= ballNumCol)  //x方向
						pitchX2 = 0;
					else
						pitchX2 = detBalls[i][j + 1].detBallCenter.x * detBalls[i][j + 1].isExist - detBalls[i][j].detBallCenter.x * detBalls[i][j + 1].isExist;
					if ((i + 1) >= ballNumRow)  //y方向
						pitchY2 = 0;
					else
						pitchY2 = detBalls[i + 1][j].detBallCenter.y * detBalls[i + 1][j].isExist - detBalls[i][j].detBallCenter.y * detBalls[i + 1][j].isExist;
					if (pitchX2 == 0)
						detBalls[i][j].pitchX = -9999;
					else
						detBalls[i][j].pitchX = pitchX2;

					if (pitchY2 == 0)
						detBalls[i][j].pitchY = -9999;
					else
						detBalls[i][j].pitchY = pitchY2;
				}
				else
					return 9;
#endif
				//Debug
				/*if (detBalls[i][j].pitchY <= 0 || detBalls[i][j].pitchX <= 0)
				{
					char debugInfo[512];
					sprintf_s(debugInfo, "[Alg Debug][Alg Error] Alg_Name:[BGABall2dDefectDet] BallXPitch=%f, BallYPitch=%f\n", detBalls[i][j].pitchX, detBalls[i][j].pitchY);
					OutputDebugStringA(debugInfo);
				}*/


				// Alg 2，Ball Offset 球偏移
				detBalls[i][j].offsetX = ballTmp.detBallCenter.x - ballTmp.templateBallCenter.x;
				detBalls[i][j].offsetY = ballTmp.detBallCenter.y - ballTmp.templateBallCenter.y;
				detBalls[i][j].offsetXY = sqrt(detBalls[i][j].offsetX * detBalls[i][j].offsetX + detBalls[i][j].offsetY * detBalls[i][j].offsetY);

				// Alg 3, Ball Quality球质量
				//计算划分角度，每个角度是多少
#if 0
				if (iQualityMethod == 0 && ballDetFunc < 3)
				{
					double anglePerDivision;
					anglePerDivision = 2 * myPi / iSectionDivisionNum;
					int normalNum = 0;
					for (int k = 0; k < iSectionDivisionNum; ++k)
					{
						HTuple hRowMeasure, hColMeasure, hAmplitudeMeasure, hDistanceMeasure;
						double angleTmp;
						angleTmp = anglePerDivision * k;
						//cout << k << " ";
						int ballQualityFlag;
						ballQualityFlag = ballQualityNormal(hDetImg, hImgWidth, hImgHeight, dThresholdMeasure, "all", detBalls[i][j].detBallCenter.x, detBalls[i][j].detBallCenter.y, angleTmp, stdBallRadius * dInnerCircle, stdBallRadius * dOuterCircle, 1.0, 1.0, "all", hRowMeasure, hColMeasure, hAmplitudeMeasure, hDistanceMeasure);
						if (ballQualityFlag > 0) {
							return ballQualityFlag;
						}
						if (hRowMeasure.Length() >= 1)
							++normalNum;
					}
					detBalls[i][j].quality = 100.0 * normalNum / (iSectionDivisionNum + Inf);
				}
#endif
				// TODO Alg4, Ball Squash 球挤压度

				HObject hBallRingRegionTmp;
				SelectObj(hBallDetectedinRing, &hBallRingRegionTmp, ballDetectedNum + 1);
				double innerCircleRadius = hBallSquashInnerRadius[ballDetectedNum];
				HTuple hBallAreaTmp;
				AreaCenter(hBallRingRegionTmp, &hBallAreaTmp, NULL, NULL);

				ballTmp.squash = hBallAreaTmp.D() / ((ballTmp.radius + innerCircleRadius) * (ballTmp.radius - innerCircleRadius) * myPi);
				detBalls[i][j].squash = (ballTmp.squash > 1) ? (0) : (1 - ballTmp.squash);

				++ballDetectedNum;
			}

			// 重新获取值
			ballTmp = detBalls[i][j];

			FlawInfoStruct flawTmp;
			double ballWidth = ballTmp.radiusNorm * 2;
			flawTmp.FlawBasicInfo.flawHeight = float(ballWidth);
			flawTmp.FlawBasicInfo.flawWidth = float(ballWidth);
			flawTmp.FlawBasicInfo.flawArea = float(ballTmp.area);
			flawTmp.FlawBasicInfo.type = 0;
			HTuple yTrans, xTrans;       //将结果变换回待测图
			AffineTransPixel(layout2DetectTrans, ballTmp.detBallCenter.y, ballTmp.detBallCenter.x, &yTrans, &xTrans);
			flawTmp.FlawBasicInfo.centerPt.x = xTrans.D();
			flawTmp.FlawBasicInfo.centerPt.y = yTrans.D();

			// flawContour
			vector<Point2f> contourTmp1;
			vector<vector<Point2f>> contourTmp2(1);
			HObject hBallCircleRegion;
			// 生成待测锡球区域，使用原始未归一化的锡球半径
			computeCirclePlygon(ballTmp.detBallCenter, ballTmp.radius, contourTmp1, hBallCircleRegion, layout2DetectTrans);
			ConcatObj(hDetBallRegionAll, hBallCircleRegion, &hDetBallRegionAll);

			contourTmp2[0] = contourTmp1;

			flawTmp.FlawBasicInfo.flawContours = contourTmp2;
			// 显示用
			output.ballContour[indexTmp] = contourTmp1;

			// 计算最小外接矩形顶点
			RotatedRect rectTmp;
			Point2f rect[4];
			rectTmp = minAreaRect(contourTmp1);  //最小外接矩形
			rectTmp.points(rect);
			vector<Point2f> rectContour(5);
			rectContour[0] = rect[0];
			rectContour[1] = rect[1];
			rectContour[2] = rect[2];
			rectContour[3] = rect[3];
			rectContour[4] = rect[0];
			flawTmp.FlawBasicInfo.pts = rectContour;
			flawTmp.FlawBasicInfo.angle = rectTmp.angle;

			// 显示多边形
			/*try {
				vector<Point> rectContourTmp(5);
				rectContourTmp[0].x = int(rect[0].x);
				rectContourTmp[0].y = int(rect[0].y);
				rectContourTmp[1].x = int(rect[1].x);
				rectContourTmp[1].y = int(rect[1].y);
				rectContourTmp[2].x = int(rect[2].x);
				rectContourTmp[2].y = int(rect[2].y);
				rectContourTmp[3].x = int(rect[3].x);
				rectContourTmp[3].y = int(rect[3].y);
				rectContourTmp[4].x = int(rect[0].x);
				rectContourTmp[4].y = int(rect[0].y);
				cv::polylines(imgTmp2, rectContourTmp, 1, Scalar(255, 0, 0));
			}
			catch (cv::Exception& err) {
				const char* err_msg = err.what();
				std::cout << "exception caught: " << err_msg << std::endl;
			}*/
			flawTmp.extendParameters.d_params.block.ballMissing = ballTmp.isMissing;
			flawTmp.extendParameters.d_params.block.ballWidth = ballWidth;
			flawTmp.extendParameters.d_params.block.ballXOffset = ballTmp.offsetX;
			flawTmp.extendParameters.d_params.block.ballYOffset = ballTmp.offsetY;
			flawTmp.extendParameters.d_params.block.ballROffset = ballTmp.offsetXY;
			flawTmp.extendParameters.d_params.block.ballXPitch = ballTmp.pitchX;
			flawTmp.extendParameters.d_params.block.ballYPitch = ballTmp.pitchY;
			flawTmp.extendParameters.d_params.block.ballQuality = ballTmp.quality;
			flawTmp.extendParameters.d_params.block.ballContrast = ballTmp.contrast;
			flawTmp.extendParameters.d_params.block.ballSquash = ballTmp.squash;
			// 其他特征赋值
			flawTmp.extendParameters.d_params.block.ballGridOffsetX = 0;
			flawTmp.extendParameters.d_params.block.ballGridOffsetY = 0;
			//output.flawsData.push_back(flawTmp);
			output.flawsData[indexTmp] = flawTmp;
			//if (abs(detBalls[i][j].pitchX) > 100)
			//{
			//	//return 100;
			//	char debugInfo[512];
			//	sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[BGABall2dDefectDet] BallXPitch=%f, BallYPitch=%f\n", detBalls[i][j].pitchX, detBalls[i][j].pitchY);
			//	OutputDebugStringA(debugInfo);
			//}

			// 清理vector<point2f>
			vector<Point2f>().swap(contourTmp1);
			vector<vector<Point2f>>().swap(contourTmp2);
			vector<Point2f>().swap(rectContour);

			// 绘制传入的锡球区域和计算得到的锡球区域
#if isDebugPT2D

			cv::circle(imgTmp2, Point2f(detBalls[i][j].templateBallCenter.x, detBalls[i][j].templateBallCenter.y), stdBallRadius, Scalar(255, 0, 0));
			cv::circle(imgTmp2, Point2f(detBalls[i][j].detBallCenter.x, detBalls[i][j].detBallCenter.y), detBalls[i][j].radius, Scalar(0, 0, 255));
			cv::circle(imgTmp3, Point2f(flawTmp.FlawBasicInfo.centerPt.x, flawTmp.FlawBasicInfo.centerPt.y), detBalls[i][j].radius, Scalar(0, 0, 255));
			vector<vector<Point2f>> ballCircleContour;
			zkhyPublicFuncHN::genContourFromRegion(hBallCircleRegion, ballCircleContour);
			// 使用opencv绘制转换得到的轮廓 ballCircleContour。颜色为黄色
			vector<vector<Point>> ballCircleContourTmp(ballCircleContour.size());
			for (int i = 0; i < ballCircleContour.size(); i++) {
				for (int j = 0; j < ballCircleContour[i].size(); j++) {
					ballCircleContourTmp[i].push_back(Point(ballCircleContour[i][j].x, ballCircleContour[i][j].y));
				}
			}
			cv::drawContours(imgTmp2, ballCircleContourTmp, -1, Scalar(0, 255, 255));

#endif
		}
		// 保存图片
		/*string fileRootImg = "D:\\code\\数据解析\\packagingtest\\imageSave0423\\";
		string filenameImg = "";
		for (int i = 0; i < 9999; i++) {
			filenameImg = fileRootImg + "\\ballDet" + to_string(i) + ".bmp";
			struct stat buffer;
			if (stat(filenameImg.c_str(), &buffer) != 0)
				break;
		}
		cv::imwrite(filenameImg, imgTmp2);*/
		// 黄金样本数据处理
		if (iBallPitchFunc == 1)
		{
			double scaleCoef = 0.5;
			vector<double> stdGoldBallsRaidus = { 250, 250.5, 251, 250, 250.5, 249, 250.5, 250, 251, 249, 249.5, 249.5, 248.5, 248, 248, 249 };
			vector<double> stdGoldBallsPitchX = { 2000, 2000, 1999, 2000, 2000, 2000, 2001, 2000, 2000, 2000, 2000, 2000 };
			vector<double> stdGoldBallsPitchY = { 1999, 2000, 1999, 1999, 1999, 1999, 1999, 1999, 2000, 2000, 1999, 2000 };

			vector<double> detBallRadius(ballDetectedNum), detBallPitchX(ballDetectedNum - ballNumRow), detBallPitchY(ballDetectedNum - ballNumCol), detBallNormRadius(ballDetectedNum), detBallNormPitchX(ballDetectedNum - ballNumRow), detBallNormPitchY(ballDetectedNum - ballNumCol);
			for (int j = 0; j < ballNumCol; ++j)
			{
				for (int i = 0; i < ballNumRow; ++i)
				{
					if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
					{
						// 计算锡球的特征
						if (detBalls[i][j].isMissing == false)
						{
							int indexTmp = j * ballNumCol + i;
							detBallRadius[indexTmp] = detBalls[i][j].radius;
							if (j < ballNumCol - 1)
								detBallPitchX[indexTmp] = detBalls[i][j].pitchX;
							if (i < ballNumRow - 1)
								detBallPitchY[indexTmp - j] = detBalls[i][j].pitchY;
						}
					}
				}
			}

			physic2Pixel(stdGoldBallsRaidus, layoutInfo.mmOfPixelX * 1000);
			physic2Pixel(stdGoldBallsPitchX, layoutInfo.mmOfPixelX * 1000);
			physic2Pixel(stdGoldBallsPitchY, layoutInfo.mmOfPixelX * 1000);
			normWithStdValue(detBallRadius, stdGoldBallsRaidus, detBallNormRadius, 0.6);
			normWithStdValue(detBallPitchX, stdGoldBallsPitchX, detBallNormPitchX);
			normWithStdValue(detBallPitchY, stdGoldBallsPitchY, detBallNormPitchY);
			for (int j = 0; j < ballNumCol; ++j)
			{
				for (int i = 0; i < ballNumRow; ++i)
				{
					if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
					{
						// 计算锡球的特征
						if (detBalls[i][j].isMissing == false)
						{
							int indexTmp = j * ballNumCol + i;
							output.flawsData[indexTmp].extendParameters.d_params.block.ballWidth = detBallNormRadius[indexTmp] * 2;
							if (j < ballNumCol - 1)
								output.flawsData[indexTmp].extendParameters.d_params.block.ballXPitch = detBallNormPitchX[indexTmp];
							else
								output.flawsData[indexTmp].extendParameters.d_params.block.ballXPitch = -9999;
							if (i < ballNumRow - 1)
								output.flawsData[indexTmp].extendParameters.d_params.block.ballYPitch = detBallNormPitchY[indexTmp - j];
							else
								output.flawsData[indexTmp].extendParameters.d_params.block.ballYPitch = -9999;
						}
					}
				}
			}
		}

		// Ball Grid Offset X/Y 相对于标准中心偏移的偏移量
		HTuple hBallRegionLTRow, hBallRegionLTCol, hBallRegionRDRow, hBallRegionRDCol;
		Union1(hDetBallRegionAll, &hDetBallRegionAll);
		SmallestRectangle1(hDetBallRegionAll, &hBallRegionLTRow, &hBallRegionLTCol, &hBallRegionRDRow, &hBallRegionRDCol);

		// 封装缺陷
		FlawInfoStruct flawTmp;
		flawTmp.FlawBasicInfo.flawHeight = float(hBallRegionRDRow.D() - hBallRegionLTRow.D());
		flawTmp.FlawBasicInfo.flawWidth = float(hBallRegionRDCol.D() - hBallRegionLTCol.D());
		flawTmp.FlawBasicInfo.flawArea = 0;
		flawTmp.FlawBasicInfo.type = 0;
		HTuple yTrans, xTrans;       //将结果变换回待测图
		AffineTransPixel(layout2DetectTrans, (hBallRegionRDRow.D() + hBallRegionLTRow.D()) * 0.5, (hBallRegionRDCol.D() + hBallRegionLTCol.D()) * 0.5, &yTrans, &xTrans);
		flawTmp.FlawBasicInfo.centerPt.x = xTrans.D();
		flawTmp.FlawBasicInfo.centerPt.y = yTrans.D();
		//flawContour
		vector<Point2f> contourTmp1;
		vector<vector<Point2f>> contourTmp2(1);
		zkhyPublicFuncHN::computeRectangle1Plygon(hBallRegionLTRow, hBallRegionLTCol, hBallRegionRDRow, hBallRegionRDCol, contourTmp1, layout2DetectTrans);
		contourTmp2[0] = contourTmp1;
		flawTmp.FlawBasicInfo.flawContours = contourTmp2;
		flawTmp.FlawBasicInfo.pts = contourTmp1;

		// 显示用
		output.ballContour[ballExistNum] = contourTmp1;
		// 计算 gridOffsetX, gridOffsetY
		Point2f ballTemplateCenter((detBalls[0][0].templateBallCenter.x + detBalls[ballNumRow - 1][ballNumCol - 1].templateBallCenter.x) * 0.5, (detBalls[0][0].templateBallCenter.y + detBalls[ballNumRow - 1][ballNumCol - 1].templateBallCenter.y) * 0.5);
		flawTmp.extendParameters.d_params.block.ballGridOffsetX = (hBallRegionLTCol.D() + hBallRegionRDCol.D()) * 0.5 - ballTemplateCenter.x - stdGridOffsetX;
		flawTmp.extendParameters.d_params.block.ballGridOffsetY = (hBallRegionLTRow.D() + hBallRegionRDRow.D()) * 0.5 - ballTemplateCenter.y - stdGridOffsetY;

		// 其他特征赋值
		flawTmp.extendParameters.d_params.block.ballMissing = 0;
		flawTmp.extendParameters.d_params.block.ballWidth = stdBallRadius * 2;
		flawTmp.extendParameters.d_params.block.ballXOffset = 0;
		flawTmp.extendParameters.d_params.block.ballYOffset = 0;
		flawTmp.extendParameters.d_params.block.ballROffset = 0;
		flawTmp.extendParameters.d_params.block.ballXPitch = stdBallPitchX;
		flawTmp.extendParameters.d_params.block.ballYPitch = stdBallPitchY;
		flawTmp.extendParameters.d_params.block.ballQuality = 100;
		flawTmp.extendParameters.d_params.block.ballContrast = 250;
		flawTmp.extendParameters.d_params.block.ballSquash = 0;


		HObject hDetBallRegionAllTrans, hDetBallRegions;
		GenEmptyObj(&hDetBallRegions);
		if (iBallRegionMode == 0 && ballDetFunc != 2)
		{
			//Union1(hDetBallRegionSeg, &hDetBallRegions);
			hDetBallRegions = hSegRegionUnion;
		}
		else
			hDetBallRegions = hDetBallRegionAll;

		AffineTransRegion(hDetBallRegions, &hDetBallRegionAllTrans, layout2DetectTrans, "nearest_neighbor");
		output.flawsData[ballExistNum] = flawTmp;
		output.ballRegions = hDetBallRegionAllTrans;

		// 模板锡球区域
		AffineTransRegion(hTemplateBallRegionAll, &hTemplateBallRegionAll, layout2DetectTrans, "nearest_neighbor");
		output.ballTemplateRegions = hTemplateBallRegionAll;

		// 计算特征的统计值
		// 第一行 为最小值，第二行为均值，第三行为最大值
		output.flawStatic.resize(3);
		zkhyPublicFuncHN::genFlawBasicInfo(output.flawStatic);
		BGABallFeatureStatic(detBalls, ballDetectedNum, output.flawStatic);

		// gridOffsetX, gridOffsetY 赋值

		for (int i = 0; i < 3; ++i)
		{
			output.flawStatic[i].extendParameters.d_params.block.ballGridOffsetX = flawTmp.extendParameters.d_params.block.ballGridOffsetX;
			output.flawStatic[i].extendParameters.d_params.block.ballGridOffsetY = flawTmp.extendParameters.d_params.block.ballGridOffsetY;
		}

		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 4;                   // 算法超时

		// 清理vector<point2f>

		vector<Point2f>().swap(inputVertexs);
		vector<Point2f>().swap(vertexs);
		vector<Point2f>().swap(contourTmp1);
		vector<vector<Point2f>>().swap(contourTmp2);
		vector<vector<BGADetInfo>>().swap(detBalls);
		vector<existedTarget>().swap(ballExisted);

#if 0
		string fileRoot = "D:\\code\\数据解析\\packagingtest\\ballData0606\\";
		string filename = "GoldenSample.txt";
		ofstream ofs;
		ofs.open((fileRoot + filename), ios::out | ios::app);
		ofs.setf(ios::fixed, ios::floatfield);
		ofs.precision(2);
		if (ofs.is_open())
			ofs << "[Alg Debug] Alg_name:[BGABallDet] BallWidth: ";
		for (int j = 0; j < ballNumCol; ++j)
		{
			for (int i = 0; i < ballNumRow; ++i)
			{
				if (detBalls[i][j].isExist == 1)  //锡球存在时执行运算
				{
					if (i != 0 || j != 0)
						ofs << ",";
					if (detBalls[i][j].isMissing == 1)
						ofs << -9999;
					else
						ofs << detBalls[i][j].radius * 2 * layoutInfo.mmOfPixelX * 1000;
				}
			}
		}
		ofs << "\n";
		if (ofs.is_open())
			ofs << "[Alg Debug] Alg_name:[BGABallDet] PitchX: ";
		for (int j = 0; j < ballNumCol; ++j)
		{
			for (int i = 0; i < ballNumRow; ++i)
			{
				if (detBalls[i][j].isExist == 1 && detBalls[i][j].pitchX > 0)  //锡球存在时执行运算
				{
					if (i != 0 || j != 0)
						ofs << ",";
					if (detBalls[i][j].isMissing == 1)
						ofs << -9999;
					else
						ofs << detBalls[i][j].pitchX * layoutInfo.mmOfPixelX * 1000;
				}
			}
		}
		ofs << "\n";
		if (ofs.is_open())
			ofs << "[Alg Debug] Alg_name:[BGABallDet] PitchY: ";
		for (int j = 0; j < ballNumCol; ++j)
		{
			for (int i = 0; i < ballNumRow; ++i)
			{
				if (detBalls[i][j].isExist == 1 && detBalls[i][j].pitchY > 0)  //锡球存在时执行运算
				{
					if (i != 0 || j != 0)
						ofs << ",";
					if (detBalls[i][j].isMissing == 1)
						ofs << -9999;
					else
						ofs << detBalls[i][j].pitchY * layoutInfo.mmOfPixelY * 1000;
				}
			}
		}
		ofs << "\n";
		ofs.close();
#endif
#if 0
		string fileRoot = "D:\\code\\数据解析\\AlgCostTime\\";
		string filename = "BGA2DDetNewRle1129-2.txt";
		ofstream ofs;
		ofs.open((fileRoot + filename), ios::out | ios::app);
		ofs.setf(ios::fixed, ios::floatfield);
		ofs.precision(20);
		if (ofs.is_open())
			ofs << "[Alg Debug] Alg_name:[BGABallDet] AlgRunCostTimes: " << costTime << endl;
		ofs.close();
#endif

#if 0
		if (hDeviceIdentifer.Length() > 0)
		{
			DeactivateComputeDevice(hDeviceHandle);
		}
#endif

		return 0;
	}


}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------BGABall2dDefectDet--------------------------//
	//功能：封测底部BAG锡球2D缺陷检测函数入口
	int BGABall2dDefectDet(const BGABall2dDefectDetInput& input, BGABall2dDefectDetOutput& output)
	{
		try
		{
			return realBGABall2dDefectDet(input, output);
		}
		catch (...)
		{
			return 5; //未知错误
		}
	}
}
