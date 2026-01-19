#include "alglibrary/zkhyProjectHuaNan/Packaging2dBodyMeasure.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	//----------------------------------------------realPackaging2dBodyMeasure--------------------------------//
//功能：计算封测产品的垂直度，平行度，封装宽度和高度信息
//输入：待测产品的Mat图像，四个扫描矩形ROI以及高精度拟合直线的参数
//输出：封测产品的垂直度，平行度，封装宽度和高度
//--------------new version-------------
	int realPackaging2dBodyMeasure(const Packaging2dBodyMeasureInput& input, Packaging2dBodyMeasureOutput& output)
	{
		//HObject hdetICRegion = input.detICRegion;
		CSYSTransPara detTransPara = input.detTransPara;
		AlgParamPackaging2dBodyMeasure* algBodyMeasure = (AlgParamPackaging2dBodyMeasure*)&input.algPara;

		vector<Point2f> intersections = input.vertexs;
		vector<double> crossAngle = input.crossAngles;
		int packagingType = algBodyMeasure->i_params.block.iPackagingType;
		double timeOutThreshold = algBodyMeasure->d_params.block.dTimeOutThreshold;
		if (timeOutThreshold > 999999 || timeOutThreshold <= 0)
			return 1;           // 算法超时参数输入有误

		// 开始计时
		DWORD startTime = GetTickCount64();

		// 计算BodySize
		auto computeDistance = [](Point2f p1, Point2f p2) {return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y)); };

		double bodySizeX = computeDistance(intersections[0], intersections[3]);
		double bodySizeY = computeDistance(intersections[2], intersections[3]);
		// 计算垂直度
		double standAngle = 90.0;
		double orthogonality = 0;
		for (int i = 0; i < 4; i++)
		{
			//计算垂直度
			double angleTmp;
			angleTmp = abs(crossAngle[i] - standAngle);
			if (angleTmp > orthogonality)
				orthogonality = angleTmp;
		}
		// 计算平行度
		double parallelism = 0;
		// 假设上、下边缘为平行状态，以上边缘为基准，只考虑左下角和右下角
		// 假设左、右边缘为平行状态，以左边缘为基准，只考虑右下角和右上角
		// 假设上、下边缘为平行状态，以下边缘为基准，只考虑左上角和右上角
		// 假设左、右边缘为平行状态，以右边缘为基准，只考虑左上角和左下角
		for (int i = 0; i < 4; i++)
		{
			double edgeLength;
			if (i % 2 == 0)
				edgeLength = bodySizeY;
			else
				edgeLength = bodySizeX;
			for (int j = i + 1; j < i + 3; j++)
			{
				int k = j % 4;
				double angleTmp = crossAngle[k];
				if (angleTmp > 90)
					angleTmp = 180 - angleTmp;
				//角度转换为弧度
				angleTmp = angleTmp * myPi / 180;
				double parallelismTmp = edgeLength * cos(angleTmp);
				if (parallelismTmp > parallelism)
					parallelism = parallelismTmp;
			}
		}
		double edgeStraightness = 0;
		if (packagingType == 1 && false) // QFN
		{
			// 计算封装边缘线性度
			HTuple hRowTmp, hColTmp;
			HObject hFitICRegion, hdetICRegion;
			GenEmptyRegion(&hdetICRegion);
			for (int i = 0; i < 4; i++)
			{
				hRowTmp.Append(intersections[i].y);
				hColTmp.Append(intersections[i].x);
			}
			//生成闭合轮廓
			hRowTmp.Append(intersections[0].y);
			hColTmp.Append(intersections[0].x);
			//生成区域
			GenRegionPolygonFilled(&hFitICRegion, hRowTmp, hColTmp);
			//计算区域差值
			HTuple hHomMat2D, hRowTmpTrans, hColTmpTrans, hDiffRegionArea, hDiffRegionRow, hDiffRegionCol, hDiffRegionNum;
			HObject hDetDiffStd, hDetDiffStdTrans, hDiffRegions, hDiffRegionUp, hDiffRegionLeft, hDiffRegionDown, hDiffRegionRight;
			Difference(hdetICRegion, hFitICRegion, &hDetDiffStd);
			VectorAngleToRigid(detTransPara.translationValue.x, detTransPara.translationValue.y, detTransPara.angle, detTransPara.translationValue.x, detTransPara.translationValue.y, 0, &hHomMat2D);
			AffineTransRegion(hDetDiffStd, &hDetDiffStdTrans, hHomMat2D, "nearest_neighbor");                 // 将区域转正
			AffineTransPixel(hHomMat2D, hRowTmp, hColTmp, &hRowTmpTrans, &hColTmpTrans);                      // 将顶点转正
			//对区域进行分组筛选
			Connection(hDetDiffStdTrans, &hDiffRegions);
			AreaCenter(hDiffRegions, &hDiffRegionArea, &hDiffRegionRow, &hDiffRegionCol);
			CountObj(hDiffRegions, &hDiffRegionNum);
			GenEmptyObj(&hDiffRegionUp);
			GenEmptyObj(&hDiffRegionLeft);
			GenEmptyObj(&hDiffRegionDown);
			GenEmptyObj(&hDiffRegionRight);
			for (int i = 0; i < hDiffRegionNum; i++)
			{
				if (hDiffRegionRow[i] < hRowTmpTrans[0])         //上
					ConcatObj(hDiffRegionUp, hDiffRegions[i], &hDiffRegionUp);
				else if (hDiffRegionCol[i] < hColTmpTrans[0])    //左
					ConcatObj(hDiffRegionLeft, hDiffRegions[i], &hDiffRegionLeft);
				else if (hDiffRegionRow[i] > hRowTmpTrans[2])    //下
					ConcatObj(hDiffRegionDown, hDiffRegions[i], &hDiffRegionDown);
				else if (hDiffRegionCol[i] > hColTmpTrans[2])    //右
					ConcatObj(hDiffRegionRight, hDiffRegions[i], &hDiffRegionRight);
			}
			//横向闭运算，计算上下边缘特征
			HObject hDiffRegionUpUnion, hDiffRegionLeftUnion, hDiffRegionDownUnion, hDiffRegionRightUnion, hDiffRegionUpTmp, hDiffRegionLeftTmp, hDiffRegionDownTmp, hDiffRegionRightTmp;
			HTuple hDiffRecUpRowLT, hDiffRecUpColLT, hDiffRecUpRowRD, hDiffRecUpColRD, hDiffRecLeftRowLT, hDiffRecLeftColLT, hDiffRecLeftRowRD, hDiffRecLeftColRD, hDiffRecDownRowLT, hDiffRecDownColLT, hDiffRecDownRowRD, hDiffRecDownColRD, hDiffRecRightRowLT, hDiffRecRightColLT, hDiffRecRightRowRD, hDiffRecRightColRD;
			Union1(hDiffRegionUp, &hDiffRegionUpUnion);
			ClosingRectangle1(hDiffRegionUpUnion, &hDiffRegionUpTmp, bodySizeX - 10, 1);
			SmallestRectangle1(hDiffRegionUpTmp, &hDiffRecUpRowLT, &hDiffRecUpColLT, &hDiffRecUpRowRD, &hDiffRecUpColRD);

			Union1(hDiffRegionDown, &hDiffRegionDownUnion);
			ClosingRectangle1(hDiffRegionDownUnion, &hDiffRegionDownTmp, bodySizeX - 10, 1);
			SmallestRectangle1(hDiffRegionDownTmp, &hDiffRecDownRowLT, &hDiffRecDownColLT, &hDiffRecDownRowRD, &hDiffRecDownColRD);
			double edgeStraightnessUp, edgeStraightnessLeft, edgeStraightnessDown, edgeStraightnessRight;
			edgeStraightnessUp = abs(hDiffRecUpRowRD.D() - hDiffRecUpRowLT.D());
			edgeStraightnessDown = abs(hDiffRecDownRowRD.D() - hDiffRecDownRowLT.D());
			//横向闭运算，计算左右边缘特征
			Union1(hDiffRegionLeft, &hDiffRegionLeftUnion);
			ClosingRectangle1(hDiffRegionLeftUnion, &hDiffRegionLeftTmp, 1, bodySizeY - 10);
			SmallestRectangle1(hDiffRegionLeftTmp, &hDiffRecLeftRowLT, &hDiffRecLeftColLT, &hDiffRecLeftRowRD, &hDiffRecLeftColRD);

			Union1(hDiffRegionRight, &hDiffRegionRightUnion);
			ClosingRectangle1(hDiffRegionRightUnion, &hDiffRegionRightTmp, 1, bodySizeY - 10);
			SmallestRectangle1(hDiffRegionRightTmp, &hDiffRecRightRowLT, &hDiffRecRightColLT, &hDiffRecRightRowRD, &hDiffRecRightColRD);
			edgeStraightnessLeft = abs(hDiffRecLeftColRD.D() - hDiffRecLeftColLT.D());
			edgeStraightnessRight = abs(hDiffRecRightColRD.D() - hDiffRecRightColLT.D());
			// 取最大值作为边缘线性度
			edgeStraightness = max(edgeStraightnessUp, edgeStraightnessDown);
			edgeStraightness = max(edgeStraightness, edgeStraightnessLeft);
			edgeStraightness = max(edgeStraightness, edgeStraightnessRight);
		}

		// 封装缺陷
		FlawInfoStruct flawTmp;

		vector<Point2f>  contourTmp(5);
		vector<vector<Point2f>> ICContour(1);
		for (int i = 0; i < 4; i++)
		{
			contourTmp[i] = intersections[i];
		}
		contourTmp[4] = intersections[0];
		ICContour[0] = contourTmp;

		flawTmp.FlawBasicInfo.flawWidth = bodySizeX;
		flawTmp.FlawBasicInfo.flawHeight = bodySizeY;
		flawTmp.FlawBasicInfo.flawArea = bodySizeX * bodySizeY;
		flawTmp.FlawBasicInfo.centerPt = intersections[4];
		flawTmp.FlawBasicInfo.pts = contourTmp;
		flawTmp.FlawBasicInfo.flawContours = ICContour;
		flawTmp.FlawBasicInfo.angle = atan2((intersections[0].y - intersections[3].y), (intersections[3].x - intersections[0].x));
		flawTmp.FlawBasicInfo.type = 0;

		flawTmp.extendParameters.d_params.block.bodySizeX = bodySizeX;
		flawTmp.extendParameters.d_params.block.bodySizeY = bodySizeY;
		flawTmp.extendParameters.d_params.block.bodyOrthogonality = orthogonality;
		flawTmp.extendParameters.d_params.block.bodyParallelism = parallelism;
		flawTmp.extendParameters.d_params.block.bodyEdgeStraightness = edgeStraightness;

		output.flawsData.push_back(flawTmp);
		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 2;                   // 算法超时
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------Packaging2dBodyMeasure--------------------------//
	//功能：封测底部2D-Body测量函数入口
	int Packaging2dBodyMeasure(const Packaging2dBodyMeasureInput& input, Packaging2dBodyMeasureOutput& output)
	{
		try
		{
			return realPackaging2dBodyMeasure(input, output);
		}
		catch (...)
		{
			return 2; //未知错误
		}
	}
}
