#include <windows.h>
#include <numeric>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <fstream>

#include "alglibrary/zkhyProjectHuaNan/BGAInfoSetup.h"
#include "alglibrary/zkhyProjectHuaNan/alglibmisc.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {

	int pinMissing(HObject& modelRegion, HObject& testRegion, HTuple& area, HObject& hRegionMissing, double dilationRadius)
	{
		HObject hModelRegionDilation, hRegionDiff, hRegionDiffs;
		HTuple hAreaMean;

		DilationCircle(modelRegion, &hModelRegionDilation, dilationRadius);
		Difference(hModelRegionDilation, testRegion, &hRegionDiff);
		Connection(hRegionDiff, &hRegionDiffs);
		hAreaMean = area.TupleMean();
		SelectShape(hRegionDiffs, &hRegionMissing, "area", "and", hAreaMean * 0.5, hAreaMean * 1.5);

		return 0;
	}


	void hTuple2Points(HTuple& x, HTuple& y, vector<Point2f>& points, HTuple& hHomMat2D, const bool& isShuffer)
	{
		HTuple hIsTrans;
		TupleLength(hHomMat2D, &hIsTrans);

		HTuple hSize;
		TupleLength(x, &hSize);
		points.resize(hSize.I());

		HTuple hPointsX, hPointsY;
		hPointsX = x;
		hPointsY = y;
		if (hIsTrans.I() != 0)
		{   // 进行仿射变换
			AffineTransPixel(hHomMat2D, y, x, &hPointsY, &hPointsX);
		}

		for (int i = 0; i < hSize.I(); i++)
		{
			Point2f pointTmp;
			pointTmp.x = hPointsX[i].D();
			pointTmp.y = hPointsY[i].D();
			points[i] = pointTmp;
		}
		if (isShuffer)
		{
			std::random_device rd;
			std::mt19937_64 randSeed(rd());
			shuffle(points.begin(), points.end(), randSeed);
		}
	}


	void pointKdTreeBuild(vector<Point2f>& points, kdtree& kdTreerootMask)
	{
		int size = (int)points.size();
		ae_int_t nx = 2;
		ae_int_t ny = 0;
		ae_int_t normtype = 2;
		ae_int_t cols = 2;
		ae_int_t rows = size;
		double* pContent = new double[size * 2];
		for (int i = 0; i < size; i++) {
			pContent[i * 2] = points[i].x;

			pContent[i * 2 + 1] = points[i].y;
		}
		real_2d_array pointsMaskArray;
		pointsMaskArray.setcontent(rows, cols, pContent);
		kdtreebuild(pointsMaskArray, nx, ny, normtype, kdTreerootMask);
		delete[]pContent;
	}


	void searchNearestNew(kdtree& kdTreerootMask, Point2f target, Point2f& nearestPoint, double& distance, int numNearest, int funcId)
	{
		real_1d_array targetArray = "[0,0]";
		real_2d_array nearestPointArray = "[[]]";
		ae_int_t resultNum;
		targetArray[0] = target.x;
		targetArray[1] = target.y;
		ae_int_t N = numNearest;

		resultNum = kdtreequeryknn(kdTreerootMask, targetArray, N);
		kdtreequeryresultsx(kdTreerootMask, nearestPointArray);
		nearestPoint.x = 0;
		nearestPoint.y = 0;
		//for (int i = 0; i < resultNum; i++) {
		//	/*nearestPoint.x = nearestPoint.x + (float)nearestPointArray[i][0] / (float)resultNum;
		//	nearestPoint.y = nearestPoint.y + (float)nearestPointArray[i][1] / (float)resultNum;*/
		//}
		if (funcId == 1)    // 待查找的目标点不位于KD树上，求解除其到KD树中最邻近N个点的平均距离
		{
			for (int i = 0; i < resultNum; i++)
			{
				nearestPoint.x = nearestPoint.x + (float)nearestPointArray[i][0] / (float)resultNum;
				nearestPoint.y = nearestPoint.y + (float)nearestPointArray[i][1] / (float)resultNum;
			}
		}
		else if (funcId == 2) // 求待查找的目标点距离最近的N-1个点(除去本身)的平均距离
		{
			for (int i = 1; i < resultNum; i++)
			{
				nearestPoint.x = nearestPoint.x + (float)nearestPointArray[i][0] / (float)(resultNum - 1);
				nearestPoint.y = nearestPoint.y + (float)nearestPointArray[i][1] / (float)(resultNum - 1);
			}
		}
		distance = sqrt((nearestPoint.x - target.x) * (nearestPoint.x - target.x) + (nearestPoint.y - target.y) * (nearestPoint.y - target.y));
	}


	int partialDataSum(HTuple inputData, double ignoreRatio, double& result)
	{
		HTuple dataTmp;
		dataTmp = inputData;
		return 0;
	}

	int ballQualityNormal(HObject& img, HTuple& hImgWidth, HTuple& hImgHeight, const int& amplitudeThre, HTuple hTransition, const double& startX, const double& startY, const double& angle, const double& innerCircleR, const double& outerCircleR, const double& roiWidth, HTuple hSigma, HTuple hSelect, HTuple& hRowMeasure, HTuple& hColMeasure, HTuple& hAmplitudeMeasure, HTuple& hDistanceMeasure)
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
		hTmpCtrlDr = lineStartY - lineEndY;
		hTmpCtrlDc = lineStartX - lineEndX;
		//hTmpCtrlPhi = hTmpCtrlDr.TupleAtan2(hTmpCtrlDc);  计算出来角度不正确
		hTmpCtrlPhi = angle;
		hTmpCtrlLen1 = 0.5 * ((hTmpCtrlDr * hTmpCtrlDr + hTmpCtrlDc * hTmpCtrlDc).TupleSqrt());
		hTmpCtrlLen2 = roiWidth;
		GenMeasureRectangle2(hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2, hImgWidth, hImgHeight, "nearest_neighbor", &hMsrHandleMeasure);
		/*HObject rectangle1, circle1, circle2;
		GenRectangle2(&rectangle1, hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2);
		GenCircle(&circle1, lineStartY, lineStartX, 1);
		GenCircle(&circle2, lineEndY, lineEndX, 1);*/
		if (hSigma.D() >= 0.5 * hTmpCtrlLen1)
			hSigma = 0.4;
		// 计算构建的矩形和内外圆的交点坐标
		try {
			MeasurePos(img, hMsrHandleMeasure, hSigma, amplitudeThre, hTransition, hSelect, &hRowMeasure, &hColMeasure, &hAmplitudeMeasure, &hDistanceMeasure);
		}
		catch (HException& error) {
			string errormsg = error.ErrorMessage().Text();
			return 8;
			//Sleep(10);
		}
		return 0;
	}

	// 重载，计算球质量，调整输入参数
	int ballQualityNormal(HObject& img, HTuple& measureHandle, int amplitudeThre, HTuple hTransition, double startX, double startY, double angle, double innerCircleR, double outerCircleR, HTuple hSigma, HTuple hSelect, HTuple& hRowMeasure, HTuple& hColMeasure, HTuple& hAmplitudeMeasure, HTuple& hDistanceMeasure)
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


	// 计算模板锡球的坐标
	void BGATemplateBallCoordCalculation(const HTuple& ballPosX, const HTuple& ballPosY, const BGAParam& layoutInfo,
		const vector<cv::Point2f>& inputVertexs, HTuple& newBallPosX, HTuple& newBallPosY, HObject& templateRegions)
	{
		newBallPosX = ballPosX;
		newBallPosY = ballPosY;
		HTuple stdBallRadius;
		int ballNum = newBallPosX.TupleLength().I();
		double scaleX, scaleY;
		// 计算待测图和Layout的缩放系数
		// 计算待测芯片尺寸
		auto computeDistance = [](Point2f p1, Point2f p2) {return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)); };
		double bodySizeX = computeDistance(inputVertexs[0], inputVertexs[3]);
		double bodySizeY = computeDistance(inputVertexs[2], inputVertexs[3]);
		// 计算缩放系数
		scaleX = bodySizeX / layoutInfo.bodySizeX;
		scaleY = bodySizeY / layoutInfo.bodySizeY;

		// 根据缩放系数对layout信息中的BGA坐标进行修正
		for (int i = 0; i < ballNum; ++i)
		{
			newBallPosX[i] = ballPosX[i] * scaleX;
			newBallPosY[i] = ballPosY[i] * scaleY;
		}
		TupleGenConst(ballNum, layoutInfo.ballWidth / 2.0, &stdBallRadius);
		GenCircle(&templateRegions, newBallPosY, newBallPosX, stdBallRadius);
		return;
	}

	void pointKdTreeBuild3D(vector<Point2f>& points, kdtree& kdTreerootMask)
	{
		int size = (int)points.size();
		ae_int_t nx = 2;
		ae_int_t ny = 0;
		ae_int_t normtype = 2;
		ae_int_t cols = 2;
		ae_int_t rows = size;
		double* pContent = new double[size * 2];
		for (int i = 0; i < size; i++) {
			pContent[i * 2] = points[i].x;

			pContent[i * 2 + 1] = points[i].y;
		}
		real_2d_array pointsMaskArray;
		pointsMaskArray.setcontent(rows, cols, pContent);
		kdtreebuild(pointsMaskArray, nx, ny, normtype, kdTreerootMask);
		delete[]pContent;
	}

	void searchNearestNew3D(kdtree& kdTreerootMask, Point2f target, Point2f& nearestPoint, double& distance, int numNearest, int funcId)
	{
		real_1d_array targetArray = "[0,0]";
		real_2d_array nearestPointArray = "[[]]";
		ae_int_t resultNum;
		targetArray[0] = target.x;
		targetArray[1] = target.y;
		ae_int_t N = numNearest;

		resultNum = kdtreequeryknn(kdTreerootMask, targetArray, N);
		kdtreequeryresultsx(kdTreerootMask, nearestPointArray);
		nearestPoint.x = 0;
		nearestPoint.y = 0;
		//for (int i = 0; i < resultNum; i++) {
		//	/*nearestPoint.x = nearestPoint.x + (float)nearestPointArray[i][0] / (float)resultNum;
		//	nearestPoint.y = nearestPoint.y + (float)nearestPointArray[i][1] / (float)resultNum;*/
		//}
		if (funcId == 1)    // 待查找的目标点不位于KD树上，求解除其到KD树中最邻近N个点的平均距离
		{
			for (int i = 0; i < resultNum; i++)
			{
				nearestPoint.x = nearestPoint.x + (float)nearestPointArray[i][0] / (float)resultNum;
				nearestPoint.y = nearestPoint.y + (float)nearestPointArray[i][1] / (float)resultNum;
			}
		}
		else if (funcId == 2) // 求待查找的目标点距离最近的N-1个点(除去本身)的平均距离
		{
			for (int i = 1; i < resultNum; i++)
			{
				nearestPoint.x = nearestPoint.x + (float)nearestPointArray[i][0] / (float)(resultNum - 1);
				nearestPoint.y = nearestPoint.y + (float)nearestPointArray[i][1] / (float)(resultNum - 1);
			}
		}
		distance = sqrt((nearestPoint.x - target.x) * (nearestPoint.x - target.x) + (nearestPoint.y - target.y) * (nearestPoint.y - target.y));
	}


	int realBGAInfoSetup(const BGASetupInput& input, BGASetupOutput& output)
	{
		AlgParamBGASetup* inputAlgParam = (AlgParamBGASetup*)&input.alg;
		double bodySizeX = inputAlgParam->d_params.block.bodySizeX;    //封测尺寸x 长度
		double bodySizeY = inputAlgParam->d_params.block.bodySizeY;    //封测尺寸y 宽度
		double bodySizeZ = inputAlgParam->d_params.block.bodySizeZ;    //封测尺寸z 厚度                      
		int gridNumX = inputAlgParam->i_params.block.gridNumY;        //行个数
		int gridNumY = inputAlgParam->i_params.block.gridNumX;        //列个数  
		double gridOffsetX = inputAlgParam->d_params.block.gridOffsetX;  //网格中心偏移x距离
		double gridOffsetY = inputAlgParam->d_params.block.gridOffsetY;  //网格中心偏移y距离
		double gridPitchX = inputAlgParam->d_params.block.gridPitchX;   //网格x方向间距
		double gridPitchY = inputAlgParam->d_params.block.gridPitchY;   //网格y方向间距
		double ballWidth = inputAlgParam->d_params.block.ballWidth;    //球宽
		double ballHeight = inputAlgParam->d_params.block.ballHeight;   //球高
		double leftUpCornerX = inputAlgParam->d_params.block.leftUpCornerX;  //左上角点x距离
		double leftUpCornerY = inputAlgParam->d_params.block.leftUpCornerY;  //左上角点y距离
		double mmOfPixelX = inputAlgParam->d_params.block.mmOfPixelX;    //像素当量 mm/pixel
		double mmOfPixelY = inputAlgParam->d_params.block.mmOfPixelY;    //像素当量 mm/pixel
		vector<cv::Point2f> inputPoints = input.inputPoints;  //软件输入中心点坐标

		if (false)
		{
			bodySizeX = 14.89;    //封测尺寸x 长度
			bodySizeY = 9.93;    //封测尺寸y 宽度
			bodySizeZ = 1;    //封测尺寸z 厚度    
			gridNumX = 22;        //行个数
			gridNumY = 12;        //列个数 
			gridOffsetX = 0;  //网格中心偏移x距离
			gridOffsetY = 0;  //网格中心偏移y距离
			gridPitchX = 0.64;   //横向间距
			gridPitchY = 0.8;   //纵向间距
			ballWidth = 0.317;    //球宽
			ballHeight = 0.22;   //球高
			mmOfPixelX = 0.01405;   //x方向像素当量 mm/pixel
			mmOfPixelY = 0.01405;   //y方向像素当量 mm/pixel
			//vector<cv::Point2f> inputPoints = input.inputPoints;  //软件输入中心点坐标
		}
		if (input.inputPoints.size() < 1)
		{
			return 1;//输入参数错误
		}
		//软件输入参数判定
		if (bodySizeX <= 0 || bodySizeY <= 0 || bodySizeZ <= 0 || gridNumX <= 0 || gridNumY <= 0 || gridOffsetX < 0 || gridOffsetY < 0 ||
			gridPitchX <= 0 || gridPitchY <= 0 || ballWidth <= 0 || ballHeight <= 0 || mmOfPixelX <= 0 || mmOfPixelY <= 0)
		{
			return 1;//输入参数错误
		}
		//1、计算起始点第一个球的坐标
		//double startPointX = (bodySizeX - (gridNumY - 1)*gridPitchX) / 2.0 + gridOffsetX;
		//double startPointY = (bodySizeY - (gridNumX - 1)*gridPitchY) / 2.0 + gridOffsetY;
		double startPointX = leftUpCornerX;
		double startPointY = leftUpCornerY;

		//2、计算每一列BGA球的坐标
		vector<vector<PKGInfo>> posInfo;//中心坐标点+标志位
		for (int i = 0; i < gridNumX; i++)
		{
			vector<PKGInfo> oneRowPoints;
			double pointX, pointY;
			pointY = startPointY + gridPitchY * i;
			for (int j = 0; j < gridNumY; j++)
			{
				PKGInfo onePKG;
				pointX = startPointX + gridPitchX * j;
				onePKG.pos = cv::Point2f(pointX, pointY);
				onePKG.existFlag = 1;
				oneRowPoints.emplace_back(onePKG);
			}
			posInfo.emplace_back(oneRowPoints);
		}

		//3、软件输入的坐标与算法排布的坐标进行比对，找到锡球缺失索引，给出标志位（KD-Tree方法）	
		kdtree kdTreerootDetectBall;
		pointKdTreeBuild3D(inputPoints, kdTreerootDetectBall);  // 建立KD树
		HTuple missPointR = HTuple(), missPointC = HTuple();
		HTuple existPointR = HTuple(), existPointC = HTuple();
		//HTuple pointRows = HTuple(), pointCols = HTuple();
	//#pragma omp parallel for
		int index = 0;
		for (int j = 0; j < gridNumY; j++)
		{
			for (int i = 0; i < gridNumX; i++)
			{
				Point2f nearestpoint, detectPoint;
				double disTmp = 0;
				detectPoint.x = posInfo[i][j].pos.x;
				detectPoint.y = posInfo[i][j].pos.y;
				//计算球距、最近点的坐标
				searchNearestNew3D(kdTreerootDetectBall, detectPoint, nearestpoint, disTmp, 1, 1);
				posInfo[i][j].pos.x = detectPoint.x / mmOfPixelX;
				posInfo[i][j].pos.y = detectPoint.y / mmOfPixelY;
				if (disTmp > ballWidth * 0.1)
				{
					posInfo[i][j].existFlag = 0;
					TupleConcat(missPointR, posInfo[i][j].pos.y, &missPointR);
					TupleConcat(missPointC, posInfo[i][j].pos.x, &missPointC);
				}
				else
				{
					//existPointR[index] = posInfo[i][j].pos.y;
					//existPointC[index] = posInfo[i][j].pos.x;
					//index++;
					TupleConcat(existPointR, posInfo[i][j].pos.y, &existPointR);
					TupleConcat(existPointC, posInfo[i][j].pos.x, &existPointC);
				}
			}
		}
		output.BGAParam.posInfo = posInfo;

		//计算图像像素的大小
		int allPixelsX = (int)(bodySizeX / mmOfPixelX);
		int allPixelsY = (int)(bodySizeY / mmOfPixelY);
		//4、创建一张类型为'byte'的图像，用于软件显示使用
		HObject ho_BigImageConstByte, ho_Region, ho_RegionDil, ho_BallImage, ho_Rectangle;
		GenImageConst(&ho_BigImageConstByte, "byte", allPixelsX, allPixelsY);
		GenRectangle1(&ho_Rectangle, 0, 0, allPixelsY, allPixelsX);
		PaintRegion(ho_Rectangle, ho_BigImageConstByte, &ho_BallImage, 100, "fill");//背景颜色填充
		GenRegionPoints(&ho_Region, existPointR, existPointC);
		DilationCircle(ho_Region, &ho_RegionDil, ballWidth / mmOfPixelX / 2.0);
		PaintRegion(ho_RegionDil, ho_BallImage, &ho_BallImage, 255, "fill");//ball颜色填充
		GenRegionPoints(&ho_Region, missPointR, missPointC);
		DilationCircle(ho_Region, &ho_RegionDil, ballWidth / mmOfPixelX / 2.0);
		PaintRegion(ho_RegionDil, ho_BallImage, &ho_BallImage, 50, "fill");//空球颜色填充

		//WriteImage(ho_BallImage, "bmp", 0, "C:\\Users\\yi.qiu\\Desktop\\BGAtest.bmp");
		//5、输出参数
		//output.showImg = zkhyHalconFunc::HObject2Mat(ho_BallImage).clone();
		// @gxl Test
		output.showImg = HObject2Mat(ho_BallImage);
		output.BGAParam.mmOfPixelX = mmOfPixelX;
		output.BGAParam.mmOfPixelY = mmOfPixelY;
		output.BGAParam.bodySizeX = bodySizeX / mmOfPixelX;
		output.BGAParam.bodySizeY = bodySizeY / mmOfPixelY;
		output.BGAParam.bodySizeZ = bodySizeZ;
		output.BGAParam.gridNumX = gridNumX;
		output.BGAParam.gridNumY = gridNumY;
		output.BGAParam.gridOffsetX = gridOffsetX / mmOfPixelX;
		output.BGAParam.gridOffsetY = gridOffsetY / mmOfPixelY;
		output.BGAParam.gridPitchX = gridPitchX / mmOfPixelX;
		output.BGAParam.gridPitchY = gridPitchY / mmOfPixelY;
		output.BGAParam.ballWidth = ballWidth / mmOfPixelX;
		output.BGAParam.ballHeight = ballHeight;
		output.BGAParam.ballPosY = existPointR;
		output.BGAParam.ballPosX = existPointC;
		output.BGAParam.leftUpCornerX = leftUpCornerX / mmOfPixelX;
		output.BGAParam.leftUpCornerY = leftUpCornerY / mmOfPixelY;
		output.inputPoints = inputPoints;

		// @gxl clear vector
		inputPoints.clear();
		posInfo.clear();

		inputPoints.shrink_to_fit();
		posInfo.shrink_to_fit();

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int BGAInfoSetup(const BGASetupInput& input, BGASetupOutput& output)
	{
		try
		{
			return realBGAInfoSetup(input, output);
		}
		catch (...)
		{
			return 3; //PKG计算异常
		}
	}
}
