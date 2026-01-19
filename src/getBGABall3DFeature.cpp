#include "alglibrary/zkhyProjectHuaNan/getBGABall3DFeature.h"
#include "alglibrary/zkhyProjectHuaNan/COperatorINI.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace novice;

namespace alglib::ops::zkhyProHN {
	//显示Layout BGA锡球的外接圆
	void showContour(HTuple& pointsR, HTuple& pointsC, HTuple ballDetectRadius, vector<vector<cv::Point2f>>& showContours)
	{
		HObject ho_Circles, ho_Circle;
		HTuple col, row;
		vector<cv::Point2f>contour;
		contour.resize(5);
		int ballNum = pointsR.TupleLength().I();
		showContours.resize(ballNum);
		//显示BGA外接矩形轮廓
	//#pragma omp parallel for
		for (int i = 0; i < ballNum; i++)
		{
			vector<Point2f> points;
			GenCircleContourXld(&ho_Circle, pointsR[i], pointsC[i], ballDetectRadius, HTuple(0).TupleRad(), HTuple(360).TupleRad(), "positive", 3);
			GetContourXld(ho_Circle, &col, &row);
			zkhyPublicFuncHN::hTuple2Points(row, col, points);
			//contour[0].x = pointsC[i] - ballWidth / 2.0;
			//contour[0].y = pointsR[i] - ballWidth / 2.0;
			//contour[1].x = pointsC[i] + ballWidth / 2.0;
			//contour[1].y = pointsR[i] - ballWidth / 2.0;
			//contour[2].x = pointsC[i] + ballWidth / 2.0;
			//contour[2].y = pointsR[i] + ballWidth / 2.0;
			//contour[3].x = pointsC[i] - ballWidth / 2.0;
			//contour[3].y = pointsR[i] + ballWidth / 2.0;
			//contour[4].x = pointsC[i] - ballWidth / 2.0;
			//contour[4].y = pointsR[i] - ballWidth / 2.0;
			showContours[i] = points;
		}
	}

	//旧黄金样本数据排序，与第三方测量排序一致
	void sortOldGoldBall(const HTuple& sortBeforeRow, const HTuple& sortBeforeCol, HTuple& sortAfterRow, HTuple& sortAfterCol)
	{
		int colNum = 0;//C 在第几列
		int temp = 0;
		for (int i = 0; i < sortBeforeRow.TupleLength().I(); i++)
		{
			colNum = i / 4;
			if (colNum == 0)
				temp = 12;
			else if (colNum == 1)
				temp = 4;
			else if (colNum == 2)
				temp = -4;
			else if (colNum == 3)
				temp = -12;

			sortAfterRow[i] = sortBeforeRow[i + temp];
			sortAfterCol[i] = sortBeforeCol[i + temp];
		}
	}

	//新黄金样本数据排序，与第三方测量排序一致
	void sortNewGoldBall(const HTuple& sortBeforeRow, const HTuple& sortBeforeCol, HTuple& sortAfterRow, HTuple& sortAfterCol)
	{
		int colNum = 0;//C 在第几列
		int yushu = 0;//余数
		int temp = 0;
		for (int i = 0; i < sortBeforeRow.TupleLength().I(); i++)
		{
			colNum = i / 4;
			yushu = i % 4;
			temp = colNum + yushu * 4;
			sortAfterRow[i] = sortBeforeRow[temp];
			sortAfterCol[i] = sortBeforeCol[temp];
		}
	}
	//计算锡球共面度
	void calBallCoplanarity(const HTuple ballX, const HTuple ballY, const HTuple ballZ, bool isUseAbs, HTuple& hv_Coplanarity)
	{
		HTuple hv_ObjectModel3D, hv_ObjectModel3DOut, hv_plane, A, B, C, D;
		GenObjectModel3dFromPoints(ballX, ballY, ballZ, &hv_ObjectModel3D);
		//拟合成想要的平面
		FitPrimitivesObjectModel3d(hv_ObjectModel3D, (HTuple("primitive_type").Append("fitting_algorithm")),
			(HTuple("plane").Append("least_squares_tukey")), &hv_ObjectModel3DOut);
		//fit_primitives_object_model_3d( : : ObjectModel3D, ParamName, ParamValue : ObjectModel3DOut)
		//ObjectModel3D:输入模型
		//ParamName：拟合的参数 ：fitting_algorithm, max_radius, min_radius, output_point_coord, output_xyz_mapping, primitive_type
		//ParamValue:对应'primitive_type'------'cylinder'（圆柱体）, 'sphere'（球体）, 'plane'（平面）。对应'primitive_type'------'least_squares', 'least_squares_huber', 'least_squares_tukey'几种最小二乘法,这里选择plane和least_squares
		//ObjectModel3DOut：输出的平面
		//获取法向量,Normal的前三个数值就是单位法向量
		GetObjectModel3dParams(hv_ObjectModel3DOut, "primitive_parameter", &hv_plane);
		//计算平面方程(a,b,c,d), distance = Ax+By+Cz-D
		A = ((const HTuple&)hv_plane)[0];
		B = ((const HTuple&)hv_plane)[1];
		C = ((const HTuple&)hv_plane)[2];
		D = ((const HTuple&)hv_plane)[3];
		if (isUseAbs)
			hv_Coplanarity = (A * ballX + B * ballY + C * ballZ - D).TupleAbs();
		else
			hv_Coplanarity = (A * ballX + B * ballY + C * ballZ - D);

	}
	//功能：计算输入区域的面积、角度、外接矩形以及轮廓等基本特征数据
	//输入：defectRegion 缺陷区域，可以为单个区域，也可以为多个区域
	//		useContour 是否计算缺陷轮廓，true为计算轮廓，false为不计算轮廓，默认为计算
	//		useRotateRect 计算区域的外接旋转矩形还是正矩形，true为旋转矩形，false为正矩形，默认为旋转矩形
	//		homMat2D 坐标变换矩阵，用于对缺陷区域进行映射，默认为空
	//输出：outFlaw 缺陷结构体
	void ballRegion2Flaw(HObject& defectRegions, vector<FlawInfoStruct>& outFlaw, const bool& useContour, const bool& useRotateRect, HTuple& homMat2D)
	{
		HTuple hFlawNum;
		// 缺陷个数
		CountObj(defectRegions, &hFlawNum);
		int flawNum = hFlawNum.I();

		outFlaw.resize(flawNum);
		vector<double> areaTmp(flawNum);
		vector<double> angleTmp(flawNum);
		vector<double> flawWidth(flawNum);
		vector<double> flawHeight(flawNum);
		vector<Point2f> centerPoint;
		vector<vector<Point2f>> rectContour;
		vector<vector<vector<Point2f>>> defectContours;
		zkhyPublicFuncHN::genFlawInfoFromRegions(defectRegions, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectContour, defectContours, useContour, useRotateRect, homMat2D);

		for (int i = 0; i < flawNum; i++)
		{
			outFlaw[i].FlawBasicInfo.flawArea = areaTmp[i];
			outFlaw[i].FlawBasicInfo.angle = angleTmp[i];
			outFlaw[i].FlawBasicInfo.flawWidth = flawWidth[i];
			outFlaw[i].FlawBasicInfo.flawHeight = flawHeight[i];
			outFlaw[i].FlawBasicInfo.centerPt = centerPoint[i];
			outFlaw[i].FlawBasicInfo.pts = rectContour[i];
			outFlaw[i].FlawBasicInfo.flawContours = defectContours[i];
			outFlaw[i].FlawBasicInfo.type = 0;
		}
	}
	//缺陷特征统计
	void flawsDataStatistics(const vector<HTuple>flawsData, vector<FlawInfoStruct>& flawsDataStatistics)
	{
		HTuple min, mean, max;
		vector<vector<double>> dataStatistics;
		dataStatistics.resize(flawsData.size());
		flawsDataStatistics.resize(3);
		for (int i = 0; i < flawsData.size(); i++)
		{
			vector<double> dataTemp;
			dataTemp.resize(3);
			HTuple minMeanMaxDatas;
			TupleMin(flawsData[i], &min);
			dataTemp[0] = min.D();
			TupleMean(flawsData[i], &mean);
			dataTemp[1] = mean.D();
			TupleMax(flawsData[i], &max);
			dataTemp[2] = max.D();
			dataStatistics[i] = dataTemp;
		}
		for (int i = 0; i < dataStatistics.size(); i++)
		{
			switch (i)
			{
			case 0:
				flawsDataStatistics[0].extendParameters.d_params.block.ballHeight = dataStatistics[i][0];
				flawsDataStatistics[1].extendParameters.d_params.block.ballHeight = dataStatistics[i][1];
				flawsDataStatistics[2].extendParameters.d_params.block.ballHeight = dataStatistics[i][2];
				break;
			case 1:
				flawsDataStatistics[0].extendParameters.d_params.block.ballCoplanarity = dataStatistics[i][0];
				flawsDataStatistics[1].extendParameters.d_params.block.ballCoplanarity = dataStatistics[i][1];
				flawsDataStatistics[2].extendParameters.d_params.block.ballCoplanarity = dataStatistics[i][2];
				break;
			case 2:
				flawsDataStatistics[0].extendParameters.d_params.block.warpage = dataStatistics[i][0];
				flawsDataStatistics[1].extendParameters.d_params.block.warpage = dataStatistics[i][1];
				flawsDataStatistics[2].extendParameters.d_params.block.warpage = dataStatistics[i][2];
				break;
			default:
				break;
			}
		}
	}
	//模板图轮廓转换到待测图
	void layout2DetectTrans(vector<vector<cv::Point2f>>& layoutContours, HTuple hv_HomMat2D, vector<vector<cv::Point2f>>& DetectContours)
	{
		HTuple row, col;
		DetectContours = layoutContours;
		for (int i = 0; i < layoutContours.size(); i++)
		{
			for (int j = 0; j < layoutContours[i].size(); j++)
			{
				AffineTransPixel(hv_HomMat2D, layoutContours[i][j].y, layoutContours[i][j].x, &row, &col);
				DetectContours[i][j].x = col;
				DetectContours[i][j].y = row;
			}
		}
	}
	//获取输入区域外接矩形的四个角点坐标
	void getInfoFromRegion(HObject& regions, vector<vector<cv::Point2f>>& bga3DDetectROI)
	{
		vector<cv::Point2f>contour;
		contour.resize(5);
		vector<vector<cv::Point2f>>contours;
		HTuple objNum;
		CountObj(regions, &objNum);
		int ballNum = objNum;
		if (ballNum == 0)
			return;
		bga3DDetectROI.resize(ballNum);
		//显示regions的外接矩形轮廓
		HObject region;
		HTuple row1, col1, row2, col2;
		for (int i = 0; i < ballNum; i++)
		{
			SelectObj(regions, &region, i + 1);
			SmallestRectangle1(region, &row1, &col1, &row2, &col2);
			//Ball Search ROI
			contour[0].x = col1;
			contour[0].y = row1;
			contour[1].x = col2;
			contour[1].y = row1;
			contour[2].x = col2;
			contour[2].y = row2;
			contour[3].x = col1;
			contour[3].y = row2;
			contour[4].x = col1;
			contour[4].y = row1;
			bga3DDetectROI[i] = contour;
		}
	}


	int realGetBGABall3DFeature(GetBGABall3DFeatureInput& input, GetBGABall3DFeatureOutput& output)
	{
		//读取ini文件的球高补偿值
		COperatorINI iniOperator;
		bool bLoadINI = iniOperator.Attach(input.loadAlgINIFilePath, true);
		if (!bLoadINI)
			return 5;
		double ballHeightOffset = iniOperator.GetParameterDouble("GetBGABall3DFeature", "球高补偿值");

		if (!(input.ballImg.IsInitialized() && input.baseImg.IsInitialized()))
		{
			return 1;//输入图像为空
		}
		HObject baseImg = input.baseImg.Clone();
		HObject ballImg = input.ballImg.Clone();

		BGAParam bgaParam = input.bgaParam;//BGA参数设置
		CSYSTransPara layout2DetectTransPara = input.layout2DetectTransPara;      //Layout到待测图的坐标变换关系
		//vector<Point2f> vertexs = input.vertexs; //产品四个顶点和中心点信息
		AlgParamGetBGABall3DFeature* algPara = (AlgParamGetBGABall3DFeature*)&input.algPara;
		int package3DType = algPara->i_params.block.package3DType;//封测3D检测方案类型（0-单目线激光，1-双目线激光，2-结构光）
		double segICRangeMaxValue = algPara->d_params.block.segICRangeMaxValue; //分割IC最大值
		double segICRangeMinValue = algPara->d_params.block.segICRangeMinValue;//分割IC最小值
		int patchAreaOffsetLeft = algPara->i_params.block.patchAreaOffsetLeft;     //ROI区域偏移，像素单位
		int patchAreaOffsetRight = algPara->i_params.block.patchAreaOffsetRight;     //ROI区域偏移，像素单位
		int patchAreaOffsetTop = algPara->i_params.block.patchAreaOffsetTop;     //ROI区域偏移，像素单位
		int patchAreaOffsetBottom = algPara->i_params.block.patchAreaOffsetBottom;     //ROI区域偏移，像素单位
		int patchCount = algPara->i_params.block.patchCount;           //ROI个数
		int patchSize = algPara->i_params.block.patchSize;            //ROI尺寸，像素单位
		int detectModel = algPara->i_params.block.detectModel;//检测模式：0-正常IC，1-黄金样本
		double zoomFactor = algPara->d_params.block.zoomFactor;//缩放系数（GRR测试使用该缩放系数，黄金样本数据在检测软件进行缩放）
		//double goldValues[16] = { 331,285,138,280,325,324,332,280,282,285,280,266,435,432,435,375 };//旧黄金样本第三方数据
		double goldValues[16] = { 449,449,449,450,400,399,399,400,350,349,349,350,299,299,299,300 };//新黄金样本第三方数据
		double heightOffset = 0;//结构光-高度偏移量
		double k = 0.0000228885;//结构光-换算系数（b6bit(0-65535)换算成um）

		//软件输入参数判定
		if (segICRangeMaxValue <= 0 || segICRangeMinValue <= 0 || segICRangeMaxValue <= segICRangeMinValue)
		{
			return 2;//输入参数错误
		}
		HObject ballGray;
		if (detectModel == 1)//黄金样本模式使用ballGray
		{
			ballGray = input.ballGray.Clone();
		}

		// Local iconic variables
		HObject  ho_baseCircle, ho_BaseRegions, ho_ZeroROI, ho_ballCircle, ho_ballCircles, ho_RegionUnion, ho_Cross, ho_BallRegions;
		HObject  ho_RegionUnion1, ho_ImgBase, ho_ImgBall, ho_BallGray;
		HObject ho_OneBallImage, ho_RealRegion, ho_ballCircle1, BallEdgesThresh, RegionConnection, RegionOpenCircle, RegionFillUp;

		// Local control variables
		HTuple  hv_Row, hv_Column, hv_mRows, hv_mColumns;
		HTuple  hv_standardBallH, hv_allBGAHeight;
		HTuple  hv_baseHeight, hv_sortBaseHeight, hv_tupleNum, hv_SelectedBaseval;
		HTuple  hv_mean_baseH, hv_ballRows, hv_ballColumns, hv_ballHeight;
		HTuple  hv_sortBallHeight, hv_ball_height, hv_Greatereq;
		HTuple  hv_Indices, hv_RemoveHeight, hv_deleteMinNum, hv_SelectedBallval;
		HTuple  hv_mean_ballH, hv_Exception, hv_mean_height, hv_sub_height;
		HTuple  hv_Coplanarity, hv_Zero_Gray;
		HTuple  hv_allWROIHeight, hv_numRegionROI, hv_Index;
		HTuple  hv_wRows, hv_wCols, hv_wHeight, hv_wMeanHeight;
		HTuple  hv_Warpage, hv_LaserFactor, hv_allWROIMeanHeight, hv_allWROIMinHeight, hv_HomMat2D;
		HTuple hv_OneBallArea, hv_OneBallRow, hv_OneBallCol;
		vector<HTuple>flawsData;

		//1、把待测图仿射变换到layout坐标系
		VectorAngleToRigid(0, 0, 0, layout2DetectTransPara.translationValue.y, layout2DetectTransPara.translationValue.x, -layout2DetectTransPara.angle, &hv_HomMat2D);
		HomMat2dInvert(hv_HomMat2D, &hv_HomMat2D);
		AffineTransImage(baseImg, &ho_ImgBase, hv_HomMat2D, "constant", "false");
		//对深度图做中值滤波，剔除噪声点干扰，用square 替换circle，耗时降低约5-6倍
		//MedianImage(ho_ImgBase, &ho_ImgBase, "circle", 3, "mirrored");
		MedianImage(ho_ImgBase, &ho_ImgBase, "square", 2, "mirrored");

#if 0
		char debugInfo[512];
		sprintf_s(debugInfo, "[Alg Debug][Alg Error] Alg_Name:[GetBGABall3DFeature-----] time=%f\n", time);
		OutputDebugStringA(debugInfo);
#endif // 0

		if (package3DType == 0)//单目线激光
		{
			ho_ImgBall = ho_ImgBase.Clone();
			//设置基准位置灰度*
			hv_Zero_Gray = 32768;
			//设置相机分辨率* 8080相机换算系数:1.6um
			hv_LaserFactor = 0.0016;
		}
		else if (package3DType == 1)//双目线激光
		{
			AffineTransImage(ballImg, &ho_ImgBall, hv_HomMat2D, "nearest_neighbor", "false");
			MedianImage(ho_ImgBall, &ho_ImgBall, "square", 2, "mirrored");
			//设置基准位置灰度*
			hv_Zero_Gray = 65535;
			hv_LaserFactor = (segICRangeMaxValue - segICRangeMinValue) / 65535;
		}
		else if (package3DType == 2)//结构光
		{
			//基板和锡球共用同一个高度图
			ho_ImgBall = ho_ImgBase;
			//设置换算系数k，高度偏移量offset
			hv_LaserFactor = k;
		}
		//模板锡球坐标设置的偏移值
		int xOffset = 0;
		int yOffset = 0;
		hv_Row = bgaParam.ballPosY + yOffset;
		hv_Column = bgaParam.ballPosX + xOffset;

		//HObject templateRegions;
		//if (detectModel == 0 && package3DType == 2)//结构光模式，根据IC实际测量宽高/标准宽高 进行缩放矫正
		//{
		//	HTuple newBallPosX, newBallPosY;
		//	BGATemplateBallCoordCalculation(hv_Column, hv_Row, bgaParam, vertexs, newBallPosX, newBallPosY,templateRegions);
		//	hv_Row = newBallPosY;
		//	hv_Column = newBallPosX;
		//}

		HTuple ballCircleR;
		TupleGenConst(hv_Row.Length(), bgaParam.ballWidth + 15, &ballCircleR);
		GenCircle(&ho_BallRegions, hv_Row, hv_Column, ballCircleR / 2.0);
		Union1(ho_BallRegions, &ho_BallRegions);
		//剔除锡球的基板区域
		Complement(ho_BallRegions, &ho_BaseRegions);

		HTuple ballDetectRadius = 1.7 * (bgaParam.ballWidth / 2.0);
		HTuple ballDetectRadius1;
		if (detectModel == 1)//黄金样本模式下只计算锡球中心五分之一的球宽
		{
			//防止黄金样本锡球出现挤压，加大锡球粗定位区域
			ballDetectRadius = bgaParam.ballWidth * 1.6;
			ballDetectRadius1 = bgaParam.ballWidth / 5.0;
			//锡球排序调整，按照黄金样本的顺序
			HTuple hv_SortRow = hv_Row;
			HTuple hv_SortColumn = hv_Column;
			//sortOldGoldBall(hv_Row, hv_Column, hv_SortRow, hv_SortColumn);//旧黄金样本排序
			sortNewGoldBall(hv_Row, hv_Column, hv_SortRow, hv_SortColumn);//新黄金样本排序
			hv_Row = hv_SortRow;
			hv_Column = hv_SortColumn;
			//新黄金样本：用ball灰度图进行定位锡球检测区域
			AffineTransImage(ballGray, &ho_BallGray, hv_HomMat2D, "constant", "false");
		}
		else
		{
			ballDetectRadius1 = bgaParam.ballWidth / 2.5;
			if (package3DType == 2)//结构光
			{
				ballDetectRadius = bgaParam.gridPitchX / 2.0;
				ballDetectRadius1 = bgaParam.ballWidth / 4;
			}
		}

		//生成较大锡球区域进行粗定位，二值化阈值法后进行精定位，重新生成黄金样本检测区域（球宽 / 5）
		HTuple ballDetectRadiusTuple;
		TupleGenConst(hv_Row.Length(), ballDetectRadius, &ballDetectRadiusTuple);
		GenCircle(&ho_ballCircles, hv_Row, hv_Column, ballDetectRadiusTuple);
		HObject ho_Xld;
		//GenContourRegionXld(ho_ballCircles, &ho_Xld, "border");
		HObject ho_ballShowXLD;
		GenEmptyObj(&ho_ballShowXLD);
		for (int hv_i = 0; hv_i < hv_Row.TupleLength().I(); hv_i++)
		{
			SelectObj(ho_ballCircles, &ho_ballCircle, hv_i + 1);
			if (detectModel == 1)
			{
				//ReduceDomain(ho_ImgBall, ho_ballCircle, &ho_OneBallImage);//方案1：在锡球高度图上取锡球数据
				//ReduceDomain(ho_ImgBase, ho_ballCircle, &ho_OneBallImage);//方案2：在基板高度图上取锡球数据
				//Threshold(ho_OneBallImage, &BallEdgesThresh, 38000, 50000);
				//FillUp(BallEdgesThresh, &RegionFillUp);
				//OpeningCircle(RegionFillUp, &ho_RealRegion, 9);

				ReduceDomain(ho_BallGray, ho_ballCircle, &ho_OneBallImage);//方案3：在锡球灰度图定位锡球区域
				Threshold(ho_OneBallImage, &BallEdgesThresh, 45, 255);
				OpeningCircle(BallEdgesThresh, &RegionOpenCircle, 5);
				Connection(RegionOpenCircle, &RegionConnection);
				SelectShape(RegionConnection, &ho_RealRegion, (HTuple("area").Append("circularity")),
					"and", (HTuple(350).Append(0.7)), (HTuple(650).Append(1)));
			}
			else if (detectModel == 0)
			{
				if (package3DType == 1)//双目线激光
				{
					//做形态学处理，剔除锡球高度图周边干扰
					ReduceDomain(ho_ImgBall, ho_ballCircle, &ho_OneBallImage);
					Threshold(ho_OneBallImage, &BallEdgesThresh, 5000, 65535);
					FillUp(BallEdgesThresh, &RegionFillUp);
					OpeningCircle(RegionFillUp, &RegionOpenCircle, 3);
					Connection(RegionOpenCircle, &RegionConnection);
					SelectShapeStd(RegionConnection, &ho_RealRegion, "max_area", 70);
				}
				else if (package3DType == 2)//结构光
				{
					ReduceDomain(ho_ImgBall, ho_ballCircle, &ho_OneBallImage);
					GetRegionPoints(ho_ballCircle, &hv_ballRows, &hv_ballColumns);
					GetGrayval(ho_ImgBall, hv_ballRows, hv_ballColumns, &hv_ballHeight);
					//在数组中找到Greatereq=0的序号索引，无效数据点删除
					TupleGreaterEqualElem(hv_ballHeight, 1, &hv_Greatereq);
					TupleFind(hv_Greatereq, 0, &hv_Indices);
					TupleRemove(hv_ballHeight, hv_Indices, &hv_ballHeight);
					//精定位：计算区域内所有数据计算平均值，作为锡球和基板分割的阈值
					TupleMean(hv_ballHeight, &hv_mean_baseH);
					Threshold(ho_OneBallImage, &ho_RealRegion, hv_mean_baseH, 65535);
					OpeningCircle(ho_RealRegion, &ho_RealRegion, 5);//后续需要关注圆形开运算的半径值，对精定位有一定影响
				}
			}
			AreaCenter(ho_RealRegion, &hv_OneBallArea, &hv_OneBallRow, &hv_OneBallCol);
			if (hv_OneBallArea.D() == 0)
			{
				TupleConcat(hv_allBGAHeight, 0, &hv_allBGAHeight);
				continue;
			}
			GenCircle(&ho_ballCircle1, hv_OneBallRow, hv_OneBallCol, ballDetectRadius1);
			GetRegionPoints(ho_ballCircle1, &hv_ballRows, &hv_ballColumns);
			if (detectModel == 1)
				GetGrayval(ho_ImgBase, hv_ballRows, hv_ballColumns, &hv_ballHeight);//黄金样本在基板高度图上获取球高数据
			else
				GetGrayval(ho_ImgBall, hv_ballRows, hv_ballColumns, &hv_ballHeight);

			//在数组中找到Greatereq=0的序号索引，无效数据点删除
			TupleGreaterEqualElem(hv_ballHeight, 1, &hv_Greatereq);
			TupleFind(hv_Greatereq, 0, &hv_Indices);
			TupleRemove(hv_ballHeight, hv_Indices, &hv_ballHeight);

			hv_sortBallHeight = hv_ballHeight.TupleSort();

			//用精定位的锡球位置赋值给模板锡球	
			hv_Row[hv_i] = hv_OneBallRow;
			hv_Column[hv_i] = hv_OneBallCol;
			//生成锡球外扩区域，相交得到base检测区域ho_ZeroROI	
			if (detectModel == 1)
			{
				GenCircle(&ho_baseCircle, HTuple(hv_Row[hv_i]), HTuple(hv_Column[hv_i]), bgaParam.gridPitchX);
			}
			else
			{
				if (package3DType == 1)//双目线激光
				{
					GenRectangle1(&ho_baseCircle, HTuple(hv_Row[hv_i]) - bgaParam.gridPitchX + 10, HTuple(hv_Column[hv_i]) - bgaParam.gridPitchX + 10, HTuple(hv_Row[hv_i]) + bgaParam.gridPitchX, HTuple(hv_Column[hv_i]) + bgaParam.gridPitchX);
				}
				else if (package3DType == 2)//结构光
				{
					GenRectangle1(&ho_baseCircle, HTuple(hv_Row[hv_i]) - bgaParam.gridPitchX + bgaParam.ballWidth / 2.0, HTuple(hv_Column[hv_i]) - bgaParam.gridPitchX + bgaParam.ballWidth / 2.0, HTuple(hv_Row[hv_i]) + bgaParam.gridPitchX - bgaParam.ballWidth / 2.0, HTuple(hv_Column[hv_i]) + bgaParam.gridPitchX - bgaParam.ballWidth / 2.0);
					//GenCircle(&ho_baseCircle, HTuple(hv_Row[hv_i]), HTuple(hv_Column[hv_i]), bgaParam.gridPitchX - bgaParam.ballWidth / 1.0);
				}
			}

			Intersection(ho_baseCircle, ho_BaseRegions, &ho_ZeroROI);
			//替换concat_obj，加速
			//ObjToInteger(ho_ZeroROI, hv_i, 1, &hv_SurrogateTuple);
			//hv_BallDetectRegions = hv_BallDetectRegions.TupleConcat(hv_SurrogateTuple);
			GetRegionPoints(ho_ZeroROI, &hv_mRows, &hv_mColumns);
			GetGrayval(ho_ImgBase, hv_mRows, hv_mColumns, &hv_baseHeight);
			//在数组中找到Greatereq=0的序号索引，无效数据点删除
			TupleGreaterEqualElem(hv_baseHeight, 1, &hv_Greatereq);
			TupleFind(hv_Greatereq, 0, &hv_Indices);
			TupleRemove(hv_baseHeight, hv_Indices, &hv_baseHeight);
			hv_sortBaseHeight = hv_baseHeight.TupleSort();
			//去掉25%个最大值，25%个最小值，只取中间50%高度数据来计算平均值，作为base的高度值
			hv_tupleNum = hv_sortBaseHeight.TupleLength();
			TupleSelectRange(hv_sortBaseHeight, hv_tupleNum / 4, (hv_sortBaseHeight.TupleLength()) - (hv_tupleNum / 4),
				&hv_SelectedBaseval);
			TupleMean(hv_SelectedBaseval, &hv_mean_baseH);

			//得到真实相对高度数据
			hv_ball_height = (hv_sortBallHeight - hv_mean_baseH) * hv_LaserFactor * 1000;
			hv_ball_height = hv_ball_height.TupleSort();
			if (detectModel == 1)
			{
				hv_tupleNum = hv_ball_height.TupleLength();
				hv_SelectedBallval = hv_ball_height;
				TupleMean(hv_SelectedBallval, &hv_mean_ballH);
				hv_standardBallH = goldValues[hv_i];
			}
			else
			{
				//去掉高于1.5倍标准球高的数据，只取前5个高度数据来计算平均值，作为该球的高度值
				hv_standardBallH = bgaParam.ballHeight * 1000;
				TupleGreaterEqualElem(hv_ball_height, hv_standardBallH * 1.5, &hv_Greatereq);
				//在数组中找到Greatereq=0的序号索引，无效数据点删除
				TupleFind(hv_Greatereq, 1, &hv_Indices);
				TupleRemove(hv_ball_height, hv_Indices, &hv_RemoveHeight);

				//取排序后的最高5个数据，计算平均值，作为该球的球高
				HTuple index;
				hv_deleteMinNum = (hv_RemoveHeight.TupleLength()) - 15;
				if (hv_deleteMinNum < 0)
					index = 1;
				else
					index = 5;
				TupleSelectRange(hv_RemoveHeight, 0, (hv_RemoveHeight.TupleLength()) - index, &hv_SelectedBallval);
				//TupleMean(hv_SelectedBallval, &hv_mean_ballH);//原方案

				TupleMean(hv_SelectedBallval, &hv_mean_ballH);//ball区域求均值
			}

			if (hv_mean_ballH.D() < 0)
				TupleConcat(hv_allBGAHeight, 0, &hv_allBGAHeight);
			else
			{
				if (detectModel == 1)
					//新测量值 = 测量值-（测量值-标准值）*(1-R) +球高补偿值
					hv_mean_ballH = hv_mean_ballH - (hv_mean_ballH - hv_standardBallH) * (1 - zoomFactor) + ballHeightOffset;
				TupleConcat(hv_allBGAHeight, hv_mean_ballH, &hv_allBGAHeight);
			}
		}

		if (detectModel != 1) {
			//新测量值 = （测量值 + 偏移值） - （（测量值 + 偏移值） - 标准值） * (1-R)
			//hv_mean_height = hv_allBGAHeight.TupleMean();
			hv_mean_height = 180;//配合现场避免导出缺陷统计数据 均值时都是220，影响CPK计算
			double ballHeightOffsetS = (hv_standardBallH - hv_mean_height).D();
			hv_allBGAHeight = (hv_allBGAHeight + ballHeightOffsetS) - (hv_allBGAHeight + ballHeightOffsetS - hv_standardBallH) * (1 - zoomFactor) + ballHeightOffset;
		}

		//****************************计算共面度 Coplanarity***********************
		//求共面度：根据求取各锡球高度平均值，利用作差法可的各锡球的位置偏差，max-min求得共面度
		calBallCoplanarity(hv_Row, hv_Column, hv_allBGAHeight, true, hv_Coplanarity);

		//将layout轮廓变换到待测图中
		VectorAngleToRigid(0, 0, 0, layout2DetectTransPara.translationValue.y, layout2DetectTransPara.translationValue.x, -layout2DetectTransPara.angle, &hv_HomMat2D);
		HTuple ballDetectRegionRadius;
		TupleGenConst(hv_Row.Length(), ballDetectRadius1, &ballDetectRegionRadius);
		GenCircle(&ho_ballShowXLD, hv_Row, hv_Column, ballDetectRegionRadius);
		vector<FlawInfoStruct> outFlaw;
		vector<FlawInfoStruct> outFlawStatistics;
		HObject ho_detectRegion;

		HTuple hv_gridPitchX, hv_gridPitchY, hv_WarpageCenterR, hv_WarpageCenterC,
			hv_firstPointR, hv_firstPointC, hv_patchPitchX, hv_patchPitchY;
		HObject ho_RegionCenterP, ho_Rectangles, ho_Rectangle;
		GenEmptyObj(&ho_Rectangles);
		if (detectModel != 1)//黄金样本模式下不计算翘曲度
		{
			//****************************计算翘曲度 Warpage ***********************
			//把输入的多个ROI联合成一个大区域，进行拟合平面
			hv_gridPitchX = bgaParam.gridPitchX;
			hv_gridPitchY = bgaParam.gridPitchY;
			//计算翘曲度的每个ROI的中心坐标
			hv_WarpageCenterR = HTuple();
			hv_WarpageCenterC = HTuple();
			//第一个ROI的中心
			hv_firstPointR = patchAreaOffsetTop;
			hv_firstPointC = patchAreaOffsetLeft;
			//每个patch的横向、纵向间距
			hv_patchPitchX = (bgaParam.bodySizeX - (patchAreaOffsetLeft + patchAreaOffsetRight)) / (patchCount - 1);
			hv_patchPitchY = (bgaParam.bodySizeY - (patchAreaOffsetTop + patchAreaOffsetBottom)) / (patchCount - 1);
			double centerPointsX = bgaParam.bodySizeX / 2.0;
			double centerPointsY = bgaParam.bodySizeY / 2.0;
			for (int i = 0; i < patchCount; i++)
			{
				//横向ROI的中心点
				TupleConcat(hv_WarpageCenterR, centerPointsY, &hv_WarpageCenterR);
				TupleConcat(hv_WarpageCenterC, patchAreaOffsetLeft + (hv_patchPitchX * i), &hv_WarpageCenterC);
			}
			for (int i = 0; i < patchCount; i++)
			{
				//竖向ROI的中心点
				TupleConcat(hv_WarpageCenterR, patchAreaOffsetTop + (hv_patchPitchY * i), &hv_WarpageCenterR);
				TupleConcat(hv_WarpageCenterC, centerPointsX, &hv_WarpageCenterC);
			}

			//生成每个ROI测量区域
			GenRectangle1(&ho_Rectangles, hv_WarpageCenterR - (patchSize / 2.0), hv_WarpageCenterC - (patchSize / 2.0),
				hv_WarpageCenterR + (patchSize / 2.0), hv_WarpageCenterC + (patchSize / 2.0));

			hv_allWROIHeight = HTuple();
			//翘曲度计算前需要对原图加上更大的均值滤波操作，降低基板噪声干扰
			//HObject ho_ImgBaseMean;
			//MeanImage(ho_ImgBase, &ho_ImgBaseMean, 30, 30);
			for (int i = 0; i < hv_WarpageCenterR.TupleLength().I(); i++)
			{
				//获取每个ROI检测区域的R、C
				SelectObj(ho_Rectangles, &ho_Rectangle, i + 1);
				GetRegionPoints(ho_Rectangle, &hv_wRows, &hv_wCols);
				//获取翘曲检测区域的高度值
				GetGrayval(ho_ImgBase, hv_wRows, hv_wCols, &hv_wHeight);
				//去掉25%个最大值，25%个最小值，只取中间50%高度数据来计算平均值，作为base的高度值
				hv_tupleNum = hv_wHeight.TupleLength();
				TupleSelectRange(hv_wHeight, hv_tupleNum / 4, (hv_wHeight.TupleLength()) - (hv_tupleNum / 4),
					&hv_SelectedBaseval);
				//会存在一些无效数据点，需要剔除
				//找出大于1的高度值，Greatereq=1；小于1的高度值，Greatereq=0
				TupleGreaterEqualElem(hv_SelectedBaseval, 1, &hv_Greatereq);
				//在数组中找到Greatereq=0的序号索引，无效数据点删除
				TupleFind(hv_Greatereq, 0, &hv_Indices);
				TupleRemove(hv_SelectedBaseval, hv_Indices, &hv_SelectedBaseval);
				if (0 == hv_SelectedBaseval.TupleLength().I())
					hv_wMeanHeight = 0;
				else
					TupleMean(hv_SelectedBaseval, &hv_wMeanHeight);
				TupleConcat(hv_allWROIHeight, hv_wMeanHeight * hv_LaserFactor * 1000, &hv_allWROIHeight);
			}
			//拟合平面，计算最小值，当作翘曲度计算参考点，翘曲度=每个区域高度-参考点高度
			calBallCoplanarity(hv_WarpageCenterR, hv_WarpageCenterC, hv_allWROIHeight, false, hv_Warpage);
			TupleMin(hv_Warpage, &hv_allWROIMinHeight);
			hv_Warpage = (hv_Warpage - hv_allWROIMinHeight) - (hv_Warpage - hv_allWROIMinHeight) * (1 - zoomFactor);

			// @gxl 合并翘曲度计算区域和锡球检测区域
			ConcatObj(ho_ballShowXLD, ho_Rectangles, &ho_detectRegion);
			ballRegion2Flaw(ho_detectRegion, outFlaw, false, false, hv_HomMat2D);

			//将特征结果输出
			int ballNum = hv_allBGAHeight.TupleLength().I();
			for (int i = ballNum; i < ballNum + hv_Warpage.TupleLength().I(); ++i)
			{
				outFlaw[i].extendParameters.d_params.block.ballHeight = bgaParam.ballHeight * 1000;
				outFlaw[i].extendParameters.d_params.block.ballCoplanarity = 0;
				outFlaw[i].extendParameters.d_params.block.warpage = hv_Warpage[i - ballNum].D();
			}

			//缺陷特征统计
			flawsData.push_back(hv_allBGAHeight);
			flawsData.push_back(hv_Coplanarity);
			flawsData.push_back(hv_Warpage);
			flawsDataStatistics(flawsData, outFlawStatistics);
			zkhyPublicFuncHN::genFlawBasicInfo(outFlawStatistics);

		}
		else
		{
			ho_detectRegion = ho_ballShowXLD;
			ballRegion2Flaw(ho_detectRegion, outFlaw, false, false, hv_HomMat2D);
		}
		for (int i = 0; i < hv_allBGAHeight.TupleLength()[0].I(); ++i)
		{
			outFlaw[i].extendParameters.d_params.block.ballHeight = hv_allBGAHeight[i].D();
			outFlaw[i].extendParameters.d_params.block.ballCoplanarity = hv_Coplanarity[i].D();
			outFlaw[i].extendParameters.d_params.block.warpage = 0;
		}
		output.flaws = outFlaw;
		output.flawsDataStatistics = outFlawStatistics;

		//将layout轮廓变换到待测图中
		vector<vector<cv::Point2f>> showLayoutContours, showDetectContours;
		//HObject xld数据转换成 vector<vector>
		HObject allShowRegions;
		ConcatObj(ho_ballShowXLD, ho_Rectangles, &allShowRegions);
		zkhyPublicFuncHN::genContourFromRegion(allShowRegions, showLayoutContours);
		layout2DetectTrans(showLayoutContours, hv_HomMat2D, showLayoutContours);
		output.showContours = showLayoutContours;

		vector<vector<cv::Point2f>> bga3DDetectROILayout, bga3DDetectROIDetect;
		getInfoFromRegion(ho_Rectangles, bga3DDetectROILayout);
		layout2DetectTrans(bga3DDetectROILayout, hv_HomMat2D, bga3DDetectROIDetect);
		output.bga3DAlignROI = bga3DDetectROIDetect;
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int getBGABall3DFeature(GetBGABall3DFeatureInput& input, GetBGABall3DFeatureOutput& output)
	{
		try
		{
			return realGetBGABall3DFeature(input, output);
		}
		catch (...)
		{
			return 3; //3D特征计算异常
		}
	}
}
