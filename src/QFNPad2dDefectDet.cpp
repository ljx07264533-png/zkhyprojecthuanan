#include "alglibrary/zkhyProjectHuaNan/QFNPad2dDefectDet.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/BGABall2dDefectDet.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;
using namespace zkhyPublicFuncHN;

namespace alglib::ops::zkhyProHN {
	void findOutPoint(HTuple a, int fun, int& idx)
	{ // a 带查找数组，fun 需要查找最大值还是最小值，idx 查找到目标值的索引
		idx = -1;
		if (fun == 0)      // 查找最大值
		{
			int aNum = a.Length();
			double valueMax = -9999999;
			for (int i = 0; i < aNum; i++)
			{
				double b = a[i].D();
				if (b > valueMax)
				{
					valueMax = b;
					idx = i;
				}
			}
		}
		else if (fun == 1)    // 查找最小值
		{
			int aNum = a.Length();
			double valueMin = 9999999;
			for (int i = 0; i < aNum; i++)
			{
				double b = a[i].D();
				if (b < valueMin)
				{
					valueMin = b;
					idx = i;
				}
			}
		}
	};

	//  选取最靠外的边缘点的方式
	int padEdgeFind(HTuple hUpRow, HTuple hUpCol, HTuple hLeftRow, HTuple hLeftCol, HTuple hDownRow, HTuple hDownCol, HTuple hRightRow, HTuple hRightCol, QFNDetInfo& pad, vector<Point2f>& edgePoints, HTuple hHomMat2D)
	{
		int idxU, idxL, idxD, idxR;
		Point2f edgePointU, edgePointL, edgePointD, edgePointR;
		// 上边缘点-找最靠上的点
		findOutPoint(hUpRow, 1, idxU);
		edgePointU.x = hUpCol[idxU].D();
		edgePointU.y = hUpRow[idxU].D();
		HTuple rowTrans, colTrans;
		Point2f pointTmp;
		// 对查找到的边缘点进行坐标变换
		AffineTransPixel(hHomMat2D, hUpRow[idxU], hUpCol[idxU], &rowTrans, &colTrans);
		pointTmp.x = colTrans;
		pointTmp.y = rowTrans;
		edgePoints.emplace_back(pointTmp);

		// 左边缘点-找最靠左的点
		findOutPoint(hLeftCol, 1, idxL);
		edgePointL.x = hLeftCol[idxL].D();
		edgePointL.y = hLeftRow[idxL].D();
		AffineTransPixel(hHomMat2D, hLeftRow[idxU], hLeftCol[idxU], &rowTrans, &colTrans);
		pointTmp.x = colTrans;
		pointTmp.y = rowTrans;
		//edgePoints[1] = pointTmp;
		edgePoints.emplace_back(pointTmp);

		// 下边缘点-找最靠下的点
		findOutPoint(hDownRow, 0, idxD);
		edgePointD.x = hDownCol[idxD].D();
		edgePointD.y = hDownRow[idxD].D();
		AffineTransPixel(hHomMat2D, hDownRow[idxU], hDownCol[idxU], &rowTrans, &colTrans);
		pointTmp.x = colTrans;
		pointTmp.y = rowTrans;
		//edgePoints[2] = pointTmp;
		edgePoints.emplace_back(pointTmp);

		// 右边缘点-找最靠右的点
		findOutPoint(hRightCol, 0, idxR);
		edgePointR.x = hRightCol[idxR].D();
		edgePointR.y = hRightRow[idxR].D();
		AffineTransPixel(hHomMat2D, hRightRow[idxU], hRightCol[idxU], &rowTrans, &colTrans);
		pointTmp.x = colTrans;
		pointTmp.y = rowTrans;
		//edgePoints[3] = pointTmp;
		edgePoints.emplace_back(pointTmp);

		// 计算左上顶点和右下顶点
		pad.pointLT.x = edgePointL.x;
		pad.pointLT.y = edgePointU.y;
		pad.pointRD.x = edgePointR.x;
		pad.pointRD.y = edgePointD.y;
		pad.isMissing = false;
		double padCircleR = min(pad.pointRD.x - pad.pointLT.x, pad.pointRD.y - pad.pointLT.y) * 0.5;    // 取矩形短边作为半径
		if (pad.padLocated == "top")
		{
			// Pad位于芯片顶部
			if (pad.padType == "normal" && pad.useCircle == true)   // norml类型的Pad需要计算顶部的圆 
			{
				pad.circleCenter.x = (pad.pointLT.x + pad.pointRD.x) * 0.5;
				pad.circleCenter.y = pad.pointRD.y - padCircleR;
				pad.radius = padCircleR;
			}
			else
			{
				pad.circleCenter.x = 0;
				pad.circleCenter.y = 0;
				pad.radius = 0;
			}
		}
		if (pad.padLocated == "down")
		{
			// Pad位于芯片底部
			if (pad.padType == "normal" && pad.useCircle == true)   // norml类型的Pad需要计算顶部的圆 
			{
				pad.circleCenter.x = (pad.pointLT.x + pad.pointRD.x) * 0.5;
				pad.circleCenter.y = pad.pointLT.y + padCircleR;
				pad.radius = padCircleR;
			}
			else
			{
				pad.circleCenter.x = 0;
				pad.circleCenter.y = 0;
				pad.radius = 0;
			}
		}
		if (pad.padLocated == "left")
		{
			// Pad位于芯片左侧
			if (pad.padType == "normal" && pad.useCircle == true)   // norml类型的Pad需要计算顶部的圆
			{
				pad.circleCenter.x = pad.pointRD.x - padCircleR;
				pad.circleCenter.y = (pad.pointLT.y + pad.pointRD.y) * 0.5;
				pad.radius = padCircleR;
			}
			else
			{
				pad.circleCenter.x = 0;
				pad.circleCenter.y = 0;
				pad.radius = 0;
			}
		}
		if (pad.padLocated == "right")
		{
			// Pad位于芯片右侧
			if (pad.padType == "normal" && pad.useCircle == true)   // norml类型的Pad需要计算顶部的圆
			{
				pad.circleCenter.x = pad.pointLT.x + padCircleR;
				pad.circleCenter.y = (pad.pointLT.y + pad.pointRD.y) * 0.5;
				pad.radius = padCircleR;
			}
			else
			{
				pad.circleCenter.x = 0;
				pad.circleCenter.y = 0;
				pad.radius = 0;
			}
		}
		return 0;
	}

	int padMissing(QFNDetInfo& pad, HTuple hTemplateCenterRow, HTuple hTemplateCenterCol, double dPadWidth, double dPadLength, double padCircleR)
	{
		pad.isMissing = true;  //判断该处Pad缺失
		if (pad.padLocated == "top")
		{
			// Pad位于芯片顶部
			//使用模板图中Pad的坐标作为缺失Pad的坐标
			pad.pointLT.x = hTemplateCenterCol - dPadWidth * 0.5;
			pad.pointLT.y = hTemplateCenterRow - dPadLength * 0.5;
			pad.pointRD.x = hTemplateCenterCol + dPadWidth * 0.5;
			pad.pointRD.y = hTemplateCenterRow + dPadLength * 0.5;
			if (pad.padType == "normal" && pad.useCircle == true)   // norml类型的Pad需要计算顶部的圆 
			{
				pad.circleCenter.x = hTemplateCenterCol;
				pad.circleCenter.y = pad.pointRD.y - padCircleR;
				pad.radius = padCircleR;
			}
			else
			{
				pad.circleCenter.x = 0;
				pad.circleCenter.y = 0;
				pad.radius = 0;
			}
		}
		else if (pad.padLocated == "down")
		{
			// Pad位于芯片底部
			//使用模板图中Pad的坐标作为缺失Pad的坐标
			pad.pointLT.x = hTemplateCenterCol - dPadWidth * 0.5;
			pad.pointLT.y = hTemplateCenterRow - dPadLength * 0.5;
			pad.pointRD.x = hTemplateCenterCol + dPadWidth * 0.5;
			pad.pointRD.y = hTemplateCenterRow + dPadLength * 0.5;
			if (pad.padType == "normal" && pad.useCircle == true)   // norml类型的Pad需要计算顶部的圆 
			{
				pad.circleCenter.x = hTemplateCenterCol;
				pad.circleCenter.y = pad.pointLT.y + padCircleR;
				pad.radius = padCircleR;
			}
			else
			{
				pad.circleCenter.x = 0;
				pad.circleCenter.y = 0;
				pad.radius = 0;
			}
		}
		else if (pad.padLocated == "left")
		{
			// Pad位于芯片左侧
			//使用模板图中Pad的坐标作为缺失Pad的坐标
			pad.pointLT.x = hTemplateCenterCol - dPadLength * 0.5;
			pad.pointLT.y = hTemplateCenterRow - dPadWidth * 0.5;
			pad.pointRD.x = hTemplateCenterCol + dPadLength * 0.5;
			pad.pointRD.y = hTemplateCenterRow + dPadWidth * 0.5;
			if (pad.padType == "normal" && pad.useCircle == true)   // norml类型的Pad需要计算顶部的圆
			{
				pad.circleCenter.x = pad.pointRD.x - padCircleR;
				pad.circleCenter.y = hTemplateCenterCol;
				pad.radius = padCircleR;
			}
			else
			{
				pad.circleCenter.x = 0;
				pad.circleCenter.y = 0;
				pad.radius = 0;
			}
		}
		else if (pad.padLocated == "right")
		{
			// Pad位于芯片右侧
			//使用模板图中Pad的坐标作为缺失Pad的坐标
			pad.pointLT.x = hTemplateCenterCol - dPadLength * 0.5;
			pad.pointLT.y = hTemplateCenterRow - dPadWidth * 0.5;
			pad.pointRD.x = hTemplateCenterCol + dPadLength * 0.5;
			pad.pointRD.y = hTemplateCenterRow + dPadWidth * 0.5;
			if (pad.padType == "normal" && pad.useCircle == true)   // norml类型的Pad需要计算顶部的圆
			{
				pad.circleCenter.x = pad.pointLT.x + padCircleR;
				pad.circleCenter.y = hTemplateCenterCol;
				pad.radius = padCircleR;
			}
			else
			{
				pad.circleCenter.x = 0;
				pad.circleCenter.y = 0;
				pad.radius = 0;
			}
		}
		else if (pad.padLocated == "center")
		{
			// Pad位于芯片中心
			//使用模板图中Pad的坐标作为缺失Pad的坐标
			pad.pointLT.x = hTemplateCenterCol - dPadLength * 0.5;
			pad.pointLT.y = hTemplateCenterRow - dPadWidth * 0.5;
			pad.pointRD.x = hTemplateCenterCol + dPadLength * 0.5;
			pad.pointRD.y = hTemplateCenterRow + dPadWidth * 0.5;
			pad.circleCenter.x = 0;
			pad.circleCenter.y = 0;
			pad.radius = 0;
		}
		return 0;
	}

	int vector2HTuple(vector<HTuple> input, HTuple& output)
	{
		int vectorSize = input.size();
		for (int i = 0; i < vectorSize; i++)
		{
			int tupleLen = input[i].Length();
			for (int j = 0; j < tupleLen; j++)
				output.Append(input[i][j].D());
		}
		return 0;
	};

	int edgePointsSearch(HObject img, HTuple hImgWidth, HTuple hImgHeight, int amplitudeThre, HTuple hTransition, HTuple roiRow1, HTuple roiCol1, HTuple roiLength1, HTuple roiLength2, HTuple hRectAngle, double searchStep, int searchDirection, double recWidth, HTuple hSigma, HTuple hSelect, vector<HTuple>& hRowMeasure, vector<HTuple>& hColMeasure, vector<HTuple>& hAmplitudeMeasure, vector<HTuple>& hDistanceMeasure)
	{
		roiLength1 = roiLength1 * 0.5;
		roiLength2 = roiLength2 * 0.5;
		// 测试，ROI
		//HObject hRoiTmp;
		//GenRectangle2ContourXld(&hRoiTmp, roiRow1, roiCol1, hRectAngle, roiLength1, roiLength2);

		if (searchDirection == 0)            //searchDirection=0 扫描线方向与输入ROI长轴方向一致
		{
			// 计算输入RoI短轴与矩形的两个交点
			double searchStartX, searchStartY; //searchEndX, searchEndY;
			searchStartX = roiCol1 - roiLength2.D() * cos(myPi * 0.5 - hRectAngle.D());
			searchStartY = roiRow1 - roiLength2.D() * sin(myPi * 0.5 - hRectAngle.D());
			//searchEndX = roiCol1 + 0.5 * roiHeight.D() * cos(myPi * 0.5 + hRectAngle.D());
			//searchEndY = roiRow1 + 0.5 * roiHeight.D() * sin(myPi * 0.5 + hRectAngle.D());
			double searchAngle = hRectAngle.D();               //搜索矩形的角度
			double realStepX = searchStep * cos(myPi * 0.5 - hRectAngle.D());  //搜索时的步长
			double realStepY = searchStep * sin(myPi * 0.5 - hRectAngle.D());  //搜索时的步长
			HTuple lineRefStartRow, lineRefStartCol, lineRefEndRow, lineRefEndCol;
			// 计算搜索起始点扫描线与RoI的交点坐标
			lineRefStartRow = searchStartY + roiLength1.D() * sin(searchAngle);
			lineRefStartCol = searchStartX - roiLength1.D() * cos(searchAngle);
			lineRefEndRow = searchStartY - roiLength1.D() * sin(searchAngle);
			lineRefEndCol = searchStartX + roiLength1.D() * cos(searchAngle);
			// 根据步长计算循环次数
			int loopNum = int(roiLength2.D() * 2 / searchStep);
			for (int i = 0; i < loopNum; i = i + 1)
			{
				HTuple lineStartRow, lineStartCol, lineEndRow, lineEndCol, hRowMeasureTmp, hColMeasureTmp, hAmplitudeMeasureTmp, hDistanceMeasureTmp;
				// 根据步长计算扫描线的起止点坐标
				lineStartRow = lineRefStartRow + i * realStepY;
				lineStartCol = lineRefStartCol + i * realStepX;
				lineEndRow = lineRefEndRow + i * realStepY;
				lineEndCol = lineRefEndCol + i * realStepX;
				// TODO 检查数据是否越界
				// 构建测量矩形
				HTuple hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlDr, hTmpCtrlDc, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2, hMsrHandleMeasure;
				hTmpCtrlRow = 0.5 * (lineStartRow + lineEndRow);
				hTmpCtrlCol = 0.5 * (lineStartCol + lineEndCol);
				hTmpCtrlDr = lineEndRow - lineStartRow;
				hTmpCtrlDc = lineEndCol - lineStartCol;
				hTmpCtrlPhi = searchAngle;
				hTmpCtrlLen1 = 0.5 * ((hTmpCtrlDr * hTmpCtrlDr + hTmpCtrlDc * hTmpCtrlDc).TupleSqrt());
				hTmpCtrlLen2 = recWidth;
				GenMeasureRectangle2(hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2, hImgWidth, hImgHeight, "nearest_neighbor", &hMsrHandleMeasure);

				// 测试，检查扫描线
				HObject hRectTmp;
				GenRectangle2ContourXld(&hRectTmp, hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2);

				// 计算构建的测量矩形是否包含灰度值变化
				MeasurePos(img, hMsrHandleMeasure, hSigma, amplitudeThre, hTransition, hSelect, &hRowMeasureTmp, &hColMeasureTmp, &hAmplitudeMeasureTmp, &hDistanceMeasureTmp);
				// 赋值
				hRowMeasure.push_back(hRowMeasureTmp);
				hColMeasure.push_back(hColMeasureTmp);
				hAmplitudeMeasure.push_back(hAmplitudeMeasureTmp);
				hDistanceMeasure.push_back(hDistanceMeasureTmp);
			}
		}
		else if (searchDirection == 1)       //searchDirection=1 扫描线方向与输入ROI长轴方向垂直（逆时针旋转90度）
		{
			// 计算输入RoI长轴与矩形的两个交点
			double searchStartX, searchStartY; //searchEndX, searchEndY;
			searchStartX = roiCol1 - roiLength1.D() * cos(-hRectAngle.D());
			searchStartY = roiRow1 - roiLength1.D() * sin(-hRectAngle.D());
			//searchEndX = roiCol1 + 0.5 * roiWidth.D() * cos(hRectAngle.D());
			//searchEndY = roiRow1 + 0.5 * roiWidth.D() * sin(hRectAngle.D());
			double searchAngle = hRectAngle.D() + myPi * 0.5;  //搜索矩形的角度
			double realStepX = searchStep * cos(-hRectAngle.D());  //搜索时的步长
			double realStepY = searchStep * sin(-hRectAngle.D());  //搜索时的步长
			HTuple lineRefStartRow, lineRefStartCol, lineRefEndRow, lineRefEndCol;
			// 计算搜索起始点扫描线与RoI的交点坐标
			lineRefStartRow = searchStartY + roiLength2.D() * sin(searchAngle);
			lineRefStartCol = searchStartX - roiLength2.D() * cos(searchAngle);
			lineRefEndRow = searchStartY - roiLength2.D() * sin(searchAngle);
			lineRefEndCol = searchStartX + roiLength2.D() * cos(searchAngle);
			// 根据步长计算循环次数
			int loopNum = int(roiLength1.D() * 2 / searchStep);
			for (int i = 0; i < loopNum; i = i + 1)
			{
				HTuple lineStartRow, lineStartCol, lineEndRow, lineEndCol, hRowMeasureTmp, hColMeasureTmp, hAmplitudeMeasureTmp, hDistanceMeasureTmp;
				// 根据步长计算扫描线的起止点坐标
				lineStartRow = lineRefStartRow + i * realStepY;
				lineStartCol = lineRefStartCol + i * realStepX;
				lineEndRow = lineRefEndRow + i * realStepY;
				lineEndCol = lineRefEndCol + i * realStepX;
				// 构建测量矩形
				HTuple hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlDr, hTmpCtrlDc, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2, hMsrHandleMeasure;
				hTmpCtrlRow = 0.5 * (lineStartRow + lineEndRow);
				hTmpCtrlCol = 0.5 * (lineStartCol + lineEndCol);
				hTmpCtrlDr = lineEndRow - lineStartRow;
				hTmpCtrlDc = lineEndCol - lineStartCol;
				hTmpCtrlPhi = searchAngle;
				hTmpCtrlLen1 = 0.5 * ((hTmpCtrlDr * hTmpCtrlDr + hTmpCtrlDc * hTmpCtrlDc).TupleSqrt());
				hTmpCtrlLen2 = recWidth;
				GenMeasureRectangle2(hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2, hImgWidth, hImgHeight, "nearest_neighbor", &hMsrHandleMeasure);

				// 计算构建的测量矩形是否包含灰度值变化
				MeasurePos(img, hMsrHandleMeasure, hSigma, amplitudeThre, hTransition, hSelect, &hRowMeasureTmp, &hColMeasureTmp, &hAmplitudeMeasureTmp, &hDistanceMeasureTmp);
				// 测试，检查扫描线
				//HObject hRectTmp;
				//GenRectangle2ContourXld(&hRectTmp, hTmpCtrlRow, hTmpCtrlCol, hTmpCtrlPhi, hTmpCtrlLen1, hTmpCtrlLen2);
				// 赋值
				hRowMeasure.push_back(hRowMeasureTmp);
				hColMeasure.push_back(hColMeasureTmp);
				hAmplitudeMeasure.push_back(hAmplitudeMeasureTmp);
				hDistanceMeasure.push_back(hDistanceMeasureTmp);
			}

		}
		else
			return 1;   // 不支持的搜索方向
		return 0;
	}

	void computePadRegionFeature(HObject img, HObject regionMask, double thresholdLow, double thresholdHigh, QFNDetInfo& pad)
	{
		HObject segRegion, segMaskIntersection, segRegions, padRegion;
		HTuple grayMean, grayDeviation, area, row, col;
		Threshold(img, &segRegion, thresholdLow, thresholdHigh);
		Intersection(segRegion, regionMask, &segMaskIntersection);
		// 选取分割后面积最大的区域作为Pad区域
		Connection(segMaskIntersection, &segRegions);
		SelectShapeStd(segRegions, &padRegion, "max_area", 70);
		Intensity(padRegion, img, &grayMean, &grayDeviation);
		AreaCenter(padRegion, &area, &row, &col);
		pad.contrast = grayMean.D();
		pad.area = area.D();
		pad.padRegion = segMaskIntersection;
	}

	void computePadJLenth(Point2f ICLT, Point2f ICRD, QFNDetInfo& pad)
	{
		if (pad.padLocated == "top")        // Y方向距离
		{
			pad.JLength = pad.pointRD.y - ICLT.y;
		}
		else if (pad.padLocated == "Left")        // X方向距离
		{
			pad.JLength = pad.pointRD.x - ICLT.x;
		}
		else if (pad.padLocated == "Down")        // Y方向距离
		{
			pad.JLength = abs(pad.pointLT.y - ICRD.y);
		}
		if (pad.padLocated == "Right")        // X方向距离
		{
			pad.JLength = abs(pad.pointLT.x - ICRD.x);
		}
	}

	void computePadSpan(Point2f ICLT, Point2f ICRD, QFNDetInfo& pad)
	{
		double centerX, centerY;
		centerX = (ICLT.x + ICRD.x) * 0.5;
		centerY = (ICLT.y + ICRD.y) * 0.5;
		if (pad.padLocated == "top")        // Y方向距离
		{
			pad.span = abs(pad.pointRD.y - centerY);
		}
		else if (pad.padLocated == "Left")        // X方向距离
		{
			pad.span = abs(pad.pointRD.x - centerX);
		}
		else if (pad.padLocated == "Down")        // Y方向距离
		{
			pad.span = abs(pad.pointLT.y - centerY);
		}
		if (pad.padLocated == "Right")        // X方向距离
		{
			pad.span = abs(pad.pointLT.x - centerX);
		}
	}


	//----------------------------------------------realQFNPad2dDefectDet--------------------------------//
	//功能：计算QFN封装产品的Pad特征
	int realQFNPad2dDefectDet(const QFNPad2dDefectDetInput& input, QFNPad2dDefectDetOutput& output)
	{
		/*-----------------------------解析参数包----------------------------*/
		HObject hSrcImg = input.srcImg;
		CSYSTransPara layout2DetectTransPara = input.layout2DetectTransPara;
		layout2DetectTransPara.angle = -layout2DetectTransPara.angle;
		vector<Point2f>  inputVertexs = input.vertexs;
		QFNParam layoutInfo = input.layoutInfo;
		// 算法参数
		AlgParamQFNPad2dDefectDet* pParams = (AlgParamQFNPad2dDefectDet*)&input.algPara;
		int isFitCircle = pParams->i_params.block.isFitCircle;
		int iThresholdSegmPadLow = pParams->i_params.block.iThresholdSegmPadLow;
		int iThresholdSegmPadHigh = pParams->i_params.block.iThresholdSegmPadHigh;
		int iDetectMode = pParams->i_params.block.iDetectMode;
		double dEdgeSearchRateL = pParams->d_params.block.dEdgeSearchRateL;
		double dEdgeSearchRateW = pParams->d_params.block.dEdgeSearchRateW;
		double dThresholdPadAlign = pParams->d_params.block.dThresholdPadAlign;
		double dPadEdgePointNumMin = pParams->d_params.block.dPadEdgePointNumMin;
		double timeOutThreshold = pParams->d_params.block.dTimeOutThreshold;

		// 判断输入合法性
		// 需要先判断对象是否初始化，再判断是否为空对象，否则判断对象为空时会产生异常
		if (hSrcImg.IsInitialized() == false)
			return 2;           // 输入图像为空，图像未初始化
		HObject hEmpty;
		HTuple hIsEuqal;
		GenEmptyObj(&hEmpty);
		TestEqualObj(hSrcImg, hEmpty, &hIsEuqal);
		if (hIsEuqal.I() == 1)
		{
			return 2;           // 输入图像为空
		}
		if (timeOutThreshold > 999999 || timeOutThreshold <= 0)
			return 1;           // 算法超时参数输入有误
		if (inputVertexs.size() != 5)
			return 5;           // 输入的产品顶点信息有误
		if (input.layoutInfo.posInfo.size() != 4 && input.layoutInfo.posInfo.size() != 5)
			return 6;           // 输入的layout信息有误
		// 开始计时
		DWORD startTime = GetTickCount64();
		//对输入图像和IC芯片点集进行坐标变换
		// 计算坐标偏移量，防止图像移到(0,0)导致计算出错
		double coordOffset = input.layoutInfo.padLenght * 2.0;
		HTuple hHomMatLayout2Detect;
		vector<Point2f>  vertexs;
		HObject hDetImg, hPadRegions;
		int resFlag = packingDetInputTrans(hSrcImg, layout2DetectTransPara, inputVertexs, coordOffset, hDetImg, hHomMatLayout2Detect, vertexs);
		if (resFlag != 0)
			return 4;
		GenEmptyObj(&hPadRegions);
		// Step 1, 解析PKG information
		HTuple hDetectImgWidth, hDetectImgHeight;
		double stdPadLength, stdPadWidth, stdDist2TipX, stdDist2TipY, stdCenterPadLength, stdCenterPadWidth, stdPadPitchX, stdPadPitchY, stdGridOffsetX, stdGridOffsetY;
		Point2f stdCenterPad;
		stdPadLength = layoutInfo.padLenght;
		stdPadWidth = layoutInfo.padWidth;
		stdDist2TipX = layoutInfo.distToTipX;
		stdDist2TipY = layoutInfo.distToTipY;
		stdCenterPadLength = layoutInfo.centerPadLenght;
		stdCenterPadWidth = layoutInfo.centerPadWidth;
		stdCenterPad.x = layoutInfo.centerPadPoint.x + coordOffset;
		stdCenterPad.y = layoutInfo.centerPadPoint.y + coordOffset;
		stdPadPitchX = layoutInfo.gridPitchX;
		stdPadPitchY = layoutInfo.gridPitchY;
		stdGridOffsetX = layoutInfo.gridOffsetX;
		stdGridOffsetY = layoutInfo.gridOffsetY;

		// 计算待测图和Layout的缩放系数
		// 计算待测芯片尺寸
		double scaleX, scaleY;
		auto computeDistance = [](Point2f p1, Point2f p2) {return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)); };
		double bodySizeX = computeDistance(inputVertexs[0], inputVertexs[3]);
		double bodySizeY = computeDistance(inputVertexs[2], inputVertexs[3]);
		// 计算缩放系数
		//scaleX = bodySizeX / layoutInfo.bodySizeX;
		//scaleY = bodySizeY / layoutInfo.bodySizeY;
		scaleX = 1;
		scaleY = 1;

		// 避免将产品移动到贴近图像边缘的位置，需要在仿射变换和计算坐标时增加相同的偏移量以对layout信息中的坐标进行修正
		GetImageSize(hDetImg, &hDetectImgWidth, &hDetectImgHeight);
		int templatePadNumRow[4];
		int templatePadNum = 0;
		for (int i = 0; i < 4; i++)
		{
			templatePadNumRow[i] = layoutInfo.posInfo[i].size();
			templatePadNum += templatePadNumRow[i];
		}

		if (stdCenterPadWidth > 1e-4)   //判断是否有中心Pad
			templatePadNum += 1;

		//QFNDetInfo *detPads = new QFNDetInfo[templatePadNum];
		vector<QFNDetInfo> detPads(templatePadNum);
		// layoutInfo中Pad的坐标顺序第一维度依次对应 上-下-左-右 四条边
		int padBaseIndx = 0;             // 记录索引的递增量
		string padLocate[4] = { "top", "down", "left", "right" };
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < templatePadNumRow[i]; j++)
			{
				int indx = padBaseIndx + j;
				detPads[indx].templateCenter.x = layoutInfo.posInfo[i][j].pos.x * scaleX + coordOffset;
				detPads[indx].templateCenter.y = layoutInfo.posInfo[i][j].pos.y * scaleY + coordOffset;
				detPads[indx].isExist = layoutInfo.posInfo[i][j].existFlag;
				int TDPadindx;
				if (i % 2 == 0)
					TDPadindx = padBaseIndx + templatePadNumRow[i] + j;
				else
					TDPadindx = padBaseIndx - templatePadNumRow[i - 1] + j;
				detPads[indx].TDPadIndx = TDPadindx;
				detPads[indx].padLocated = padLocate[i];
				if (j == 0 || j + 1 >= templatePadNumRow[i])
					detPads[indx].padType = "corner";
				else
					detPads[indx].padType = "normal";
			}
			padBaseIndx += templatePadNumRow[i];
		}
		// 有中心pad时增加中心pad
		if (stdCenterPadWidth > 1e-4)
		{
			detPads[padBaseIndx].templateCenter.x = layoutInfo.centerPadPoint.x * scaleX + coordOffset;
			detPads[padBaseIndx].templateCenter.y = layoutInfo.centerPadPoint.y * scaleY + coordOffset;
			detPads[padBaseIndx].isExist = 1;
			detPads[padBaseIndx].padLocated = "center";
			detPads[padBaseIndx].padType = "center";
		}

		// 测试解析的Layout轮廓是否正确
		Mat imgTmp1 = HObject2Mat(hDetImg);
		Mat imgTmp2, imgTmp3, imgTmp4;
		cv::cvtColor(imgTmp1, imgTmp2, COLOR_GRAY2BGR);
		imgTmp3 = imgTmp2.clone();
		imgTmp4 = HObject2Mat(hSrcImg);
		cv::cvtColor(imgTmp4, imgTmp4, COLOR_GRAY2BGR);
		int PadExistNum = 0;
		for (int i = 0; i < templatePadNum; i++)
		{
			if (detPads[i].isExist == 1)
			{
				Point2f pointTmpLT, pointTmpRD;
				if (detPads[i].padLocated == "top" || detPads[i].padLocated == "down")
				{
					pointTmpLT.x = detPads[i].templateCenter.x - stdPadWidth * 0.5;
					pointTmpLT.y = detPads[i].templateCenter.y - stdPadLength * 0.5;
					pointTmpRD.x = detPads[i].templateCenter.x + stdPadWidth * 0.5;
					pointTmpRD.y = detPads[i].templateCenter.y + stdPadLength * 0.5;
				}
				else if (detPads[i].padLocated == "left" || detPads[i].padLocated == "right")
				{
					pointTmpLT.x = detPads[i].templateCenter.x - stdPadLength * 0.5;
					pointTmpLT.y = detPads[i].templateCenter.y - stdPadWidth * 0.5;
					pointTmpRD.x = detPads[i].templateCenter.x + stdPadLength * 0.5;
					pointTmpRD.y = detPads[i].templateCenter.y + stdPadWidth * 0.5;
				}
				else if (detPads[i].padLocated == "center") {
					pointTmpLT.x = stdCenterPad.x - stdCenterPadWidth * 0.5;
					pointTmpLT.y = stdCenterPad.y - stdCenterPadLength * 0.5;
					pointTmpRD.x = stdCenterPad.x + stdCenterPadWidth * 0.5;
					pointTmpRD.y = stdCenterPad.y + stdCenterPadLength * 0.5;
				}
				cv::rectangle(imgTmp2, pointTmpLT, pointTmpRD, Scalar(0, 0, 255));
				string info1 = std::to_string(i);
				//string info2 = string("angle2: ") + to_string(crossAngle[i * 2 + 1]);
				Point2f tmpPoint;
				tmpPoint.x = detPads[i].templateCenter.x;
				tmpPoint.y = detPads[i].templateCenter.y;
				cv::putText(imgTmp2, info1, tmpPoint, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 1);

				++PadExistNum;
			}
		}
		// Resize output vector
		output.padAlignRoI.resize(PadExistNum * 4);
		output.padAlignEdgePoints.resize(PadExistNum * 4);
		output.padAlignResults.resize(PadExistNum);
		output.flawsData.resize(PadExistNum);
		output.padContour.resize(PadExistNum);
		// Step 2, Pad Align
		double padCircleR, dEdgeSearchLength, dEdgeSearchWidth, padLength, padWidth;
		for (int i = 0, j = 0; i < templatePadNum; i++)
		{
			if (detPads[i].isExist == 1)    //Layout对应位置存在Pad时才执行计算
			{
				if (detPads[i].padLocated == "center")
				{
					dEdgeSearchLength = dEdgeSearchRateL * stdCenterPadLength;
					dEdgeSearchWidth = dEdgeSearchRateW * stdCenterPadWidth;
					padCircleR = 0;
					padLength = stdCenterPadLength;
					padWidth = stdCenterPadWidth;
				}
				else
				{
					dEdgeSearchLength = dEdgeSearchRateL * stdPadLength;
					dEdgeSearchWidth = dEdgeSearchRateW * stdPadWidth;
					padCircleR = stdPadWidth * 0.5;
					padLength = stdPadLength;
					padWidth = stdPadWidth;
				}
				//依次生成上、左、下、右四条边上Pad的检测ROI
				//up-"positive",left-"negative",buttom-"negative",right-"positive"
				HTuple hRoiUpRow1, hRoiUpCol1, hRoiUpRow2, hRoiUpCol2, hRoiLeftRow1, hRoiLeftCol1, hRoiLeftRow2, hRoiLeftCol2, hRoiDownRow1, hRoiDownCol1, hRoiDownRow2, hRoiDownCol2, hRoiRightRow1, hRoiRightCol1, hRoiRightRow2, hRoiRightCol2;
				vector<HTuple> hUpRow, hUpCol, hUpAmplitude, hUpDistance, hLeftRow, hLeftCol, hLeftAmplitude, hLeftDistance, hDownRow, hDownCol, hDownAmplitude, hDownDistance, hRightRow, hRightCol, hRightAmplitude, hRightDistance;
				// 上
				hRoiUpRow1 = detPads[i].templateCenter.y - padLength * 0.5 - dEdgeSearchLength * 0.5;
				hRoiUpCol1 = detPads[i].templateCenter.x - dEdgeSearchWidth * 0.5;
				hRoiUpRow2 = detPads[i].templateCenter.y - padLength * 0.5 + dEdgeSearchLength * 0.5;
				hRoiUpCol2 = detPads[i].templateCenter.x + dEdgeSearchWidth * 0.5;
				//HObject hRoiTmp;
				//GenRectangle1(&hRoiTmp, hRoiUpRow1, hRoiUpCol1, hRoiUpRow2, hRoiUpCol2);
				edgePointsSearch(hDetImg, hDetectImgWidth, hDetectImgHeight, dThresholdPadAlign, "negative", (hRoiUpRow1 + hRoiUpRow2) * 0.5, (hRoiUpCol1 + hRoiUpCol2) * 0.5, (hRoiUpRow1 - hRoiUpRow2).TupleAbs(), (hRoiUpCol1 - hRoiUpCol2).TupleAbs(), myPi * 0.5, 1, 0, 1, 1.0, "first", hUpRow, hUpCol, hUpAmplitude, hUpDistance);	//矩形方向垂直，搜索方向垂直	


				// 左
				hRoiLeftRow1 = detPads[i].templateCenter.y - dEdgeSearchWidth * 0.5;
				hRoiLeftCol1 = detPads[i].templateCenter.x - padWidth * 0.5 - dEdgeSearchLength * 0.5;
				hRoiLeftRow2 = detPads[i].templateCenter.y + dEdgeSearchWidth * 0.5;
				hRoiLeftCol2 = detPads[i].templateCenter.x - padWidth * 0.5 + dEdgeSearchLength * 0.5;
				edgePointsSearch(hDetImg, hDetectImgWidth, hDetectImgHeight, dThresholdPadAlign, "negative", (hRoiLeftRow1 + hRoiLeftRow2) * 0.5, (hRoiLeftCol1 + hRoiLeftCol2) * 0.5, (hRoiLeftCol1 - hRoiLeftCol2).TupleAbs(), (hRoiLeftRow1 - hRoiLeftRow2).TupleAbs(), -myPi, 1, 0, 1, 1.0, "first", hLeftRow, hLeftCol, hLeftAmplitude, hLeftDistance);  //矩形方向水平，搜索方向水平

				// 下
				hRoiDownRow1 = detPads[i].templateCenter.y + padLength * 0.5 - dEdgeSearchLength * 0.5;
				hRoiDownCol1 = detPads[i].templateCenter.x - dEdgeSearchWidth * 0.5;
				hRoiDownRow2 = detPads[i].templateCenter.y + padLength * 0.5 + dEdgeSearchLength * 0.5;
				hRoiDownCol2 = detPads[i].templateCenter.x + dEdgeSearchWidth * 0.5;
				edgePointsSearch(hDetImg, hDetectImgWidth, hDetectImgHeight, dThresholdPadAlign, "negative", (hRoiDownRow1 + hRoiDownRow2) * 0.5, (hRoiDownCol1 + hRoiDownCol2) * 0.5, (hRoiDownRow1 - hRoiDownRow2).TupleAbs(), (hRoiDownCol1 - hRoiDownCol2).TupleAbs(), -myPi * 0.5, 1, 0, 1, 1.0, "first", hDownRow, hDownCol, hDownAmplitude, hDownDistance);  //矩形方向垂直，搜索方向垂直

				// 右
				hRoiRightRow1 = detPads[i].templateCenter.y - dEdgeSearchWidth * 0.5;
				hRoiRightCol1 = detPads[i].templateCenter.x + padWidth * 0.5 - dEdgeSearchLength * 0.5;
				hRoiRightRow2 = detPads[i].templateCenter.y + dEdgeSearchWidth * 0.5;
				hRoiRightCol2 = detPads[i].templateCenter.x + padWidth * 0.5 + dEdgeSearchLength * 0.5;
				edgePointsSearch(hDetImg, hDetectImgWidth, hDetectImgHeight, dThresholdPadAlign, "negative", (hRoiRightRow1 + hRoiRightRow2) * 0.5, (hRoiRightCol1 + hRoiRightCol2) * 0.5, (hRoiRightCol1 - hRoiRightCol2).TupleAbs(), (hRoiRightRow1 - hRoiRightRow2).TupleAbs(), 0, 1, 0, 1, 1.0, "first", hRightRow, hRightCol, hRightAmplitude, hRightDistance);  //矩形方向水平，搜索方向水平

				// 存储边缘点，用于显示
				vector<Point2f> PadRoIUp, PadRoILeft, PadRoIDown, PadRoIRight, singlePadEdgePointsUp, singlePadEdgePointsDown, singlePadEdgePointsLeft, singlePadEdgePointsRight, singlePadResult;
				zkhyPublicFuncHN::computeRectangle1Plygon(hRoiUpRow1, hRoiUpCol1, hRoiUpRow2, hRoiUpCol2, PadRoIUp, hHomMatLayout2Detect);
				zkhyPublicFuncHN::computeRectangle1Plygon(hRoiLeftRow1, hRoiLeftCol1, hRoiLeftRow2, hRoiLeftCol2, PadRoILeft, hHomMatLayout2Detect);
				zkhyPublicFuncHN::computeRectangle1Plygon(hRoiDownRow1, hRoiDownCol1, hRoiDownRow2, hRoiDownCol2, PadRoIDown, hHomMatLayout2Detect);
				zkhyPublicFuncHN::computeRectangle1Plygon(hRoiRightRow1, hRoiRightCol1, hRoiRightRow2, hRoiRightCol2, PadRoIRight, hHomMatLayout2Detect);
				//output.padAlignRoI.push_back(PadRoIUp);
				//output.padAlignRoI.push_back(PadRoILeft);
				//output.padAlignRoI.push_back(PadRoIDown);
				//output.padAlignRoI.push_back(PadRoIRight);
				output.padAlignRoI[4 * j] = PadRoIUp;
				output.padAlignRoI[4 * j + 1] = PadRoILeft;
				output.padAlignRoI[4 * j + 2] = PadRoIDown;
				output.padAlignRoI[4 * j + 3] = PadRoIRight;
				//可视化
				/*cv::rectangle(imgTmp4, PadRoIUp[0], PadRoIUp[2], Scalar(255, 0, 0));
				cv::rectangle(imgTmp4, PadRoILeft[0], PadRoILeft[2], Scalar(255, 0, 0));
				cv::rectangle(imgTmp4, PadRoIDown[0], PadRoIDown[2], Scalar(255, 0, 0));
				cv::rectangle(imgTmp4, PadRoIRight[0], PadRoIRight[2], Scalar(255, 0, 0));*/

				HTuple hUpRowTmp, hUpColTmp, hLeftRowTmp, hLeftColTmp, hDownRowTmp, hDownColTmp, hRightRowTmp, hRightColTmp;
				// 解析vector<HTuple> 上
				vector2HTuple(hUpRow, hUpRowTmp);
				vector2HTuple(hUpCol, hUpColTmp);
				// 解析vector<HTuple> 左
				vector2HTuple(hLeftRow, hLeftRowTmp);
				vector2HTuple(hLeftCol, hLeftColTmp);
				// 解析vector<HTuple> 下
				vector2HTuple(hDownRow, hDownRowTmp);
				vector2HTuple(hDownCol, hDownColTmp);
				// 解析vector<HTuple> 右
				vector2HTuple(hRightRow, hRightRowTmp);
				vector2HTuple(hRightCol, hRightColTmp);
				hTuple2Points(hUpColTmp, hUpRowTmp, singlePadEdgePointsUp, hHomMatLayout2Detect);
				hTuple2Points(hLeftColTmp, hLeftRowTmp, singlePadEdgePointsLeft, hHomMatLayout2Detect);
				hTuple2Points(hDownColTmp, hDownRowTmp, singlePadEdgePointsDown, hHomMatLayout2Detect);
				hTuple2Points(hRightColTmp, hRightRowTmp, singlePadEdgePointsRight, hHomMatLayout2Detect);
				//output.padAlignEdgePoints.emplace_back(singlePadEdgePointsUp);
				//output.padAlignEdgePoints.emplace_back(singlePadEdgePointsLeft);
				//output.padAlignEdgePoints.emplace_back(singlePadEdgePointsDown);
				//output.padAlignEdgePoints.emplace_back(singlePadEdgePointsRight);
				output.padAlignEdgePoints[4 * j] = singlePadEdgePointsUp;
				output.padAlignEdgePoints[4 * j + 1] = singlePadEdgePointsLeft;
				output.padAlignEdgePoints[4 * j + 2] = singlePadEdgePointsDown;
				output.padAlignEdgePoints[4 * j + 3] = singlePadEdgePointsRight;
				// 可视化
				/*HObject hEdgeUp, hEdgeLeft, hEdgeDown, hEdgeRight;
				GenCrossContourXld(&hEdgeUp, hUpRowTmp, hUpColTmp, 3, 0.785398);
				GenCrossContourXld(&hEdgeLeft, hLeftRowTmp, hLeftColTmp, 3, 0.785398);
				GenCrossContourXld(&hEdgeDown, hDownRowTmp, hDownColTmp, 3, 0.785398);
				GenCrossContourXld(&hEdgeRight, hRightRowTmp, hRightColTmp, 3, 0.785398);*/

				// 查找到边缘点数量不足，判断该处Pad丢失
				if (hUpRowTmp.Length() < dPadEdgePointNumMin || hDownRowTmp.Length() < dPadEdgePointNumMin)
				{
					padMissing(detPads[i], detPads[i].templateCenter.y, detPads[i].templateCenter.x, padWidth, padLength, padCircleR);
				}
				else if (hLeftRowTmp.Length() < dPadEdgePointNumMin || hRightRowTmp.Length() < dPadEdgePointNumMin)
				{
					padMissing(detPads[i], detPads[i].templateCenter.y, detPads[i].templateCenter.x, padWidth, padLength, padCircleR);
				}
				else
				{
					// 找到边缘点，计算Pad的坐标
					singlePadResult.reserve(4);
					//detPads[i].isMissing = false;
					padEdgeFind(hUpRowTmp, hUpColTmp, hLeftRowTmp, hLeftColTmp, hDownRowTmp, hDownColTmp, hRightRowTmp, hRightColTmp, detPads[i], singlePadResult, hHomMatLayout2Detect);
					output.padAlignResults[j] = singlePadResult;

					// 可视化
					/*HTuple hPadAlignResRow, hPadAlignResCol;
					HObject hPadAlignRes;
					points2HTuple(singlePadResult, hPadAlignResCol, hPadAlignResRow);
					GenCrossContourXld(&hPadAlignRes, hPadAlignResRow, hPadAlignResCol, 3, 0.785398);
					Sleep(1);*/
				}
				cv::rectangle(imgTmp3, detPads[j].pointLT, detPads[j].pointRD, Scalar(0, 0, 255));
				//Sleep(1);
				++j;
			}
		}

		// 计算缺陷信息
		//double *padPith = new double[templatePadNum];
		for (int i = 0, j = 0; i < templatePadNum; i++)
		{
			if (detPads[i].isExist == 1)
			{
				if (detPads[i].isMissing == false)  // 当前Pad存在
				{
					//Alg 1, Pad Pitch; Corner Pad Pitch
					int prePadId, afterPadId;
					double pitchPre, pitchAfter;
					prePadId = i - 1;
					afterPadId = i + 1;
					if (prePadId < 0)
						prePadId = i;
					if (afterPadId >= templatePadNum)
						afterPadId = i;
					if (detPads[prePadId].isMissing == true || detPads[prePadId].padLocated != detPads[i].padLocated)
						pitchPre = 0;		//	前序pad丢失或者和待测Pad不在同一边
					else
						if (detPads[i].padLocated == "top" || detPads[i].padLocated == "down")
							pitchPre = abs((detPads[i].pointLT.x + detPads[i].pointRD.x) * 0.5 - (detPads[prePadId].pointLT.x + detPads[prePadId].pointRD.x) * 0.5);
						else
							pitchPre = abs((detPads[i].pointLT.y + detPads[i].pointRD.y) * 0.5 - (detPads[prePadId].pointLT.y + detPads[prePadId].pointRD.y) * 0.5);
					if (detPads[afterPadId].isMissing == true || detPads[afterPadId].padLocated != detPads[i].padLocated)
						pitchAfter = 0;     //	前序pad丢失或者和待测Pad不在同一边
					else
						if (detPads[i].padLocated == "top" || detPads[i].padLocated == "down")
							pitchAfter = abs(abs((detPads[i].pointLT.x + detPads[i].pointRD.x) * 0.5 - (detPads[afterPadId].pointLT.x + detPads[afterPadId].pointRD.x) * 0.5) - stdPadPitchX);
						else
							pitchAfter = abs(abs((detPads[i].pointLT.y + detPads[i].pointRD.y) * 0.5 - (detPads[afterPadId].pointLT.y + detPads[afterPadId].pointRD.y) * 0.5) - stdPadPitchY);
					if (pitchPre > pitchAfter)  //取较大的值作为pad的pitch值
						detPads[i].pitch = pitchPre;
					else
						detPads[i].pitch = pitchAfter;
					//Alg 2, Pad Offset; Corner Pad Offset Center Pad Offset
					if (detPads[i].padLocated == "top" || detPads[i].padLocated == "down")
						detPads[i].offset = detPads[i].templateCenter.x - (detPads[i].pointLT.x + detPads[i].pointRD.x) * 0.5;
					else if (detPads[i].padLocated == "left" || detPads[i].padLocated == "right")
						detPads[i].offset = detPads[i].templateCenter.y - (detPads[i].pointLT.y + detPads[i].pointRD.y) * 0.5;
					else if (detPads[i].padLocated == "center")
					{
						double centerPadOffsetX, centerPadOffsetY;
						centerPadOffsetX = detPads[i].templateCenter.x - (detPads[i].pointLT.x + detPads[i].pointRD.x) * 0.5;
						centerPadOffsetY = detPads[i].templateCenter.y - (detPads[i].pointLT.y + detPads[i].pointRD.y) * 0.5;
						if (abs(centerPadOffsetX) > abs(centerPadOffsetY))
							detPads[i].offset = centerPadOffsetX;
						else
							detPads[i].offset = centerPadOffsetY;
						//detPads[i].offset = max(abs(detPads[i].templateCenter.x - (detPads[i].pointLT.x + detPads[i].pointRD.x) * 0.5), abs(detPads[i].templateCenter.y - (detPads[i].pointLT.y + detPads[i].pointRD.y) * 0.5));
					}
					//Alg 3, Pad Missing (已计算)
					//Alg 4, Pad Contrast
					HObject hPadRegion;
					GenRectangle1(&hPadRegion, detPads[i].pointLT.y, detPads[i].pointLT.x, detPads[i].pointRD.y, detPads[i].pointRD.x);
					computePadRegionFeature(hDetImg, hPadRegion, iThresholdSegmPadLow, iThresholdSegmPadHigh, detPads[i]);
					//Alg 5, Pad Length / Width; corner Pad Length / Width; Center Pad Length / Width (给缺陷特征赋值时计算)
					//Alg 6, Pad J.Length: distance of edge of PKG and Pad tip
					// 对IC顶点坐标进行仿射变换
					Point2f ICLT, ICRD;
					ICLT = vertexs[0];
					ICRD = vertexs[2];
					computePadJLenth(ICLT, ICRD, detPads[i]);
					//Alg 7, Pad TD: distance between facing Pad tips
					if (detPads[i].padLocated == "top")
						detPads[i].TD = abs(detPads[i].pointRD.y - detPads[detPads[i].TDPadIndx].pointLT.y);
					else if (detPads[i].padLocated == "down")
						detPads[i].TD = abs(detPads[i].pointLT.y - detPads[detPads[i].TDPadIndx].pointRD.y);
					else if (detPads[i].padLocated == "left")
						detPads[i].TD = abs(detPads[i].pointRD.x - detPads[detPads[i].TDPadIndx].pointLT.x);
					else if (detPads[i].padLocated == "right")
						detPads[i].TD = abs(detPads[i].pointLT.x - detPads[detPads[i].TDPadIndx].pointRD.x);
					//TODO Alg 8, Pad Offset LT
					//Alg 9, Pad Span: distance from PKG center axis parallel to row on which Pad is arranged to Pad tip
					computePadSpan(ICLT, ICRD, detPads[i]);
				}
				// 封装缺陷
				FlawInfoStruct flawTmp;
				flawTmp.FlawBasicInfo.flawHeight = abs(detPads[i].pointRD.y - detPads[i].pointLT.y);
				flawTmp.FlawBasicInfo.flawWidth = abs(detPads[i].pointRD.x - detPads[i].pointLT.x);
				HTuple yTrans, xTrans;       //将结果变换回待测图
				AffineTransPixel(hHomMatLayout2Detect, (detPads[i].pointRD.y + detPads[i].pointLT.y) * 0.5, (detPads[i].pointRD.x + detPads[i].pointLT.x) * 0.5, &yTrans, &xTrans);
				flawTmp.FlawBasicInfo.centerPt.x = xTrans.D();
				flawTmp.FlawBasicInfo.centerPt.y = yTrans.D();
				// flawContours
				vector<Point2f> padContourTmp1;
				vector<vector<Point2f>> padContourTmp2(1);
				zkhyPublicFuncHN::computeRectangle1Plygon(detPads[i].pointLT.y, detPads[i].pointLT.x, detPads[i].pointRD.y, detPads[i].pointRD.x, padContourTmp1, hHomMatLayout2Detect);
				padContourTmp2[0] = padContourTmp1;
				flawTmp.FlawBasicInfo.flawContours = padContourTmp2;
				flawTmp.FlawBasicInfo.pts = padContourTmp1;
				// 显示用
				output.padContour[j] = padContourTmp1;
				flawTmp.FlawBasicInfo.flawArea = float(detPads[i].area);
				flawTmp.FlawBasicInfo.type = 0;

				// 使用Pad外接矩形构造区域
				HObject hPadRegion;
				HTuple edgePointRow, edgePointCol;
				zkhyPublicFuncHN::points2HTuple(padContourTmp1, edgePointCol, edgePointRow);
				GenRegionPolygon(&hPadRegion, edgePointRow, edgePointCol);
				ConcatObj(hPadRegions, hPadRegion, &hPadRegions);

				if (detPads[i].isMissing)
					flawTmp.extendParameters.d_params.block.PadMissing = 1;
				else
					flawTmp.extendParameters.d_params.block.PadMissing = 0;

				flawTmp.extendParameters.d_params.block.padContrast = detPads[i].contrast;
				flawTmp.extendParameters.d_params.block.padJLength = detPads[i].JLength;
				flawTmp.extendParameters.d_params.block.padSpan = detPads[i].span;
				flawTmp.extendParameters.d_params.block.padTD = detPads[i].TD;

				if (detPads[i].padType == "normal")
				{
					flawTmp.extendParameters.d_params.block.padLength = max(flawTmp.FlawBasicInfo.flawHeight, flawTmp.FlawBasicInfo.flawWidth);
					flawTmp.extendParameters.d_params.block.padWidth = min(flawTmp.FlawBasicInfo.flawHeight, flawTmp.FlawBasicInfo.flawWidth);
					flawTmp.extendParameters.d_params.block.padPitch = detPads[i].pitch;
					flawTmp.extendParameters.d_params.block.padOffset = detPads[i].offset;
				}
				else if (detPads[i].padType == "corner")
				{
					flawTmp.extendParameters.d_params.block.cornerPadLength = max(flawTmp.FlawBasicInfo.flawHeight, flawTmp.FlawBasicInfo.flawWidth);
					flawTmp.extendParameters.d_params.block.cornerPadWidth = min(flawTmp.FlawBasicInfo.flawHeight, flawTmp.FlawBasicInfo.flawWidth);
					flawTmp.extendParameters.d_params.block.cornerPadPitch = detPads[i].pitch;
					flawTmp.extendParameters.d_params.block.cornerPadOffset = detPads[i].offset;
				}
				else if (detPads[i].padType == "center")
				{
					flawTmp.extendParameters.d_params.block.centerPadLength = max(flawTmp.FlawBasicInfo.flawHeight, flawTmp.FlawBasicInfo.flawWidth);
					flawTmp.extendParameters.d_params.block.centerPadWidth = min(flawTmp.FlawBasicInfo.flawHeight, flawTmp.FlawBasicInfo.flawWidth);
					flawTmp.extendParameters.d_params.block.padPitch = detPads[i].pitch;
					flawTmp.extendParameters.d_params.block.centerPadOffset = detPads[i].offset;
				}
				output.flawsData[j] = flawTmp;
				++j;
			}
		}

		// TODO 增加黄金样本检测模式，部分数据计算方法和正常模式不同
		if (iDetectMode == 1)
		{
			for (int i = 0, j = 0; i < templatePadNum; i++)
			{
				if (detPads[i].isExist == 1)
				{
					if (detPads[i].isMissing == false)  // 当前Pad存在
					{
						//Alg 1, Pad Pitch，黄金样本模式下，Pad Pitch的计算方法为从左到右，从上到下计算相邻Pad之间的距离
						int afterPadId;
						double pitchAfter;
						afterPadId = i + 1;
						if (afterPadId >= templatePadNum)
							afterPadId = i;
						if (detPads[afterPadId].isMissing == true || detPads[afterPadId].padLocated != detPads[i].padLocated)
							pitchAfter = -9999;     //	后序pad丢失或者和待测Pad不在同一边
						else
							if (detPads[i].padLocated == "top" || detPads[i].padLocated == "down")
								pitchAfter = abs(abs((detPads[i].pointLT.x + detPads[i].pointRD.x) * 0.5 - (detPads[afterPadId].pointLT.x + detPads[afterPadId].pointRD.x) * 0.5));
							else
								pitchAfter = abs(abs((detPads[i].pointLT.y + detPads[i].pointRD.y) * 0.5 - (detPads[afterPadId].pointLT.y + detPads[afterPadId].pointRD.y) * 0.5));

						output.flawsData[j].extendParameters.d_params.block.padPitch = pitchAfter;

						output.flawsData[j].extendParameters.d_params.block.padLength = max(abs(detPads[i].pointRD.y - detPads[i].pointLT.y), abs(detPads[i].pointRD.x - detPads[i].pointLT.x));
						output.flawsData[j].extendParameters.d_params.block.padWidth = min(abs(detPads[i].pointRD.y - detPads[i].pointLT.y), abs(detPads[i].pointRD.x - detPads[i].pointLT.x));
						output.flawsData[j].extendParameters.d_params.block.padOffset = detPads[i].offset;
						++j;
					}
				}
			}
		}
		output.padRegions = hPadRegions;
		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 3;                   // 算法超时

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------QFNPad2dDefectDet--------------------------//
	//功能：封测底部QFNPad2D缺陷检测函数入口
	int QFNPad2dDefectDet(const QFNPad2dDefectDetInput& input, QFNPad2dDefectDetOutput& output)
	{
		try
		{
			return realQFNPad2dDefectDet(input, output);
		}
		catch (...)
		{
			return 4; //未知错误
		}
	}
}
