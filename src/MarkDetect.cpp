#include "alglibrary/zkhyProjectHuaNan/MarkDetect.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/MarkTeach.h"
#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"

#define LOG alglib::core::LoggerManager::getInstance()


using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	class markDetect : public zkhyPublicClass::AlgorithmBase
	{
	public:

	};


	void testRegionsToDefectStruct(HObject& defectRegions, vector<FlawInfoStruct>& outFlaw, const bool& useContour, const bool& useRotateRect,const HTuple& homMat2D)
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



	// 重载，显示的轮廓为传入的contourRegion轮廓，其他缺陷信息则由defectRegion生成，用于显示轮廓不为缺陷真实轮廓的情况
	void markDetRegionToDefectStruct(HObject& defectRegion, HObject& contourRegion, FlawInfoStruct& outFlaw, const bool& useContour, const bool& useRotateRect,const HTuple& homMat2D)
	{
		double areaTmp, angleTmp, flawWidth, flawHeight;
		Point2f centerPoint;
		vector<Point2f> rectContour;
		vector<vector<Point2f>> defectContours;
		zkhyPublicFuncHN::genFlawInfoFromRegion(defectRegion, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectContour, defectContours, false, useRotateRect, homMat2D);
		outFlaw.FlawBasicInfo.flawArea = areaTmp;
		outFlaw.FlawBasicInfo.angle = angleTmp;
		outFlaw.FlawBasicInfo.flawWidth = flawWidth;
		outFlaw.FlawBasicInfo.flawHeight = flawHeight;
		outFlaw.FlawBasicInfo.centerPt = centerPoint;
		outFlaw.FlawBasicInfo.pts = rectContour;
		zkhyPublicFuncHN::genFlawInfoFromRegion(contourRegion, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectContour, defectContours, useContour, useRotateRect, homMat2D);
		outFlaw.FlawBasicInfo.flawContours = defectContours;
		outFlaw.FlawBasicInfo.type = 0;
	}


	void simpleOCR(HObject ho_imageGrop, HTuple hv_char_height,
		HTuple hv_char_index, HTuple hv_char_model, const int OCRClassifier, HTuple* hv_char)
	{

		// Local iconic variables
		HObject  ho_ObjectSelected, ho_ImageMean, ho_Region;
		HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_ImageCleared;
		HObject  ho_ImageResult, ho_RegionUnion;

		// Local control variables
		HTuple  hv_char_indexOut, hv_OCRHandle, hv_OCRHandle1;
		HTuple  hv_Number, hv_i, hv_Width, hv_Height, hv_UsedThreshold;
		HTuple  hv_Rows, hv_Columns, hv_judge, hv_Class, hv_Confidence;
		HTuple  hv_class, hv_HomMat2D;

		hv_char_indexOut = hv_char_index;
		(*hv_char) = "";

		//自己训练模型
		//read_ocr_class_mlp ('single_number_ocr.omc', OCRHandle)
		CountObj(ho_imageGrop, &hv_Number);

		HTuple hOCRHandleCNNNumbers, hOCRHandleCNNLetters;
		if (OCRClassifier == 0)
		{
			//系统自带模型
			ReadOcrClassMlp("Industrial_0-9_NoRej.omc", &hv_OCRHandle);
			ReadOcrClassMlp("Industrial_A-Z+_NoRej.omc", &hv_OCRHandle1);
		}
		else if (OCRClassifier == 1)
		{
			ReadOcrClassCnn("Universal_0-9_NoRej.occ", &hOCRHandleCNNNumbers);
			ReadOcrClassCnn("Universal_A-Z+_NoRej.occ", &hOCRHandleCNNLetters);
		}

		{
			HTuple end_val12 = hv_Number;
			HTuple step_val12 = 1;
			for (hv_i = 1; hv_i.Continue(end_val12, step_val12); hv_i += step_val12)
			{

				//读取图像，自适应二值化获取字符区域
				SelectObj(ho_imageGrop, &ho_ObjectSelected, hv_i);
				GetImageSize(ho_ObjectSelected, &hv_Width, &hv_Height);
				MeanImage(ho_ObjectSelected, &ho_ImageMean, 5, 5);

				//字符亮暗选择
				BinaryThreshold(ho_ImageMean, &ho_Region, "max_separability", hv_char_model, &hv_UsedThreshold);

				//图像二值化
				GenImageProto(ho_ObjectSelected, &ho_ImageCleared, 255);
				PaintRegion(ho_Region, ho_ImageCleared, &ho_ImageResult, 0, "fill");

				//根据输入坐标判断待识别字符是否为字母
				hv_judge = hv_char_indexOut.TupleFind(hv_i);
				if (0 != (hv_judge > -1))
				{
					if (OCRClassifier == 0)
						DoOcrSingleClassMlp(ho_Region, ho_ImageResult, hv_OCRHandle1, 2, &hv_Class,
							&hv_Confidence);
					else if (OCRClassifier == 1)
						DoOcrSingleClassCnn(ho_Region, ho_ImageResult, hOCRHandleCNNLetters, 2, &hv_Class,
							&hv_Confidence);
					//去除‘.’干扰
					if (0 != (HTuple(hv_Class[0]) == HTuple(".")))
					{
						hv_class = ((const HTuple&)hv_Class)[1];
					}
					else
					{
						hv_class = ((const HTuple&)hv_Class)[0];
					}
				}
				else
				{
					if (OCRClassifier == 0)
						DoOcrSingleClassMlp(ho_Region, ho_ImageResult, hv_OCRHandle, 1, &hv_Class,
							&hv_Confidence);
					else if (OCRClassifier == 1)
						DoOcrSingleClassCnn(ho_Region, ho_ImageResult, hOCRHandleCNNNumbers, 1, &hv_Class,
							&hv_Confidence);

					hv_class = hv_Class;
				}
				(*hv_char) += hv_class;
			}
		}

		return;
	}



	// 计算输入向量舍弃部分较大或较小值后的平均值
	int computePartialDataMean(const vector<double>& inputData, const double& ignoreRatio, double& resMean)
	{
		int dataSize = inputData.size();
		if (dataSize == 0)
			resMean = 0;
		else if (dataSize == 1)
			resMean = inputData[0];
		else
		{
			int indexBegin = floor(dataSize * ignoreRatio);
			int indexEnd = ceil(dataSize * (1 - ignoreRatio));
			// 对 inputData进行排序
			vector<double> inputDataTmp = inputData;
			sort(inputDataTmp.begin(), inputDataTmp.end());
			// 计算输入数据 从 indexBegin 到 indexEnd 的和
			double sumTmp = 0;
			for (int i = indexBegin; i < indexEnd; ++i)
				sumTmp += inputDataTmp[i];
			// 计算输入数据 从 indexBegin 到 indexEnd 的平均值
			resMean = sumTmp / (indexEnd - indexBegin + Inf);
		}
		return 0;
	}


	// MarkFeatureStatic Mark特征值统计
	int markFeatureStatic(const vector<FlawInfoStruct>& inputFlaws, vector<FlawInfoStruct>& staticRes)
	{
		// 统计Mark缺陷特征值
		int flawNum = inputFlaws.size();

		// 需要统计的特征值
		vector<double> markCharOffset(flawNum), markCharConstrast(flawNum), markCharDistance(flawNum), markCharBlobSize(flawNum), markCharUnderPrintArea(flawNum), markCharMorePrintArea(flawNum), markCharUnderPrintRate(flawNum), markCharMorePrintRate(flawNum), markCharMatchRate(flawNum), markCharOffsetX(flawNum), markCharOffsetY(flawNum), markCharOffsetR(flawNum);
		if (flawNum > 0)
		{
			for (int i = 0; i < flawNum; ++i)
			{
				markCharOffset[i] = inputFlaws[i].extendParameters.d_params.block.markCharOffset;
				markCharConstrast[i] = inputFlaws[i].extendParameters.d_params.block.markCharContrast;
				markCharDistance[i] = inputFlaws[i].extendParameters.d_params.block.markCharDistance;
				markCharBlobSize[i] = inputFlaws[i].extendParameters.d_params.block.markCharBlobSize;
				markCharUnderPrintArea[i] = inputFlaws[i].extendParameters.d_params.block.markCharUnderPrintArea;
				markCharMorePrintArea[i] = inputFlaws[i].extendParameters.d_params.block.markCharMorePrintArea;
				markCharUnderPrintRate[i] = inputFlaws[i].extendParameters.d_params.block.markCharUnderPrintRate;
				markCharMorePrintRate[i] = inputFlaws[i].extendParameters.d_params.block.markCharMorePrintRate;
				markCharMatchRate[i] = inputFlaws[i].extendParameters.d_params.block.markCharMatchRate;
				markCharOffsetX[i] = inputFlaws[i].extendParameters.d_params.block.markCharOffsetX;
				markCharOffsetY[i] = inputFlaws[i].extendParameters.d_params.block.markCharOffsetY;
				markCharOffsetR[i] = inputFlaws[i].extendParameters.d_params.block.markCharOffsetR;
			}
		}

		// 计算统计值
		vector<double> markCharOffsetStatic, markCharConstrastStatic, markCharDistanceStatic, markCharBlobSizeStatic, markCharUnderPrintAreaStatic, markCharMorePrintAreaStatic, markCharUnderPrintRateStatic, markCharMorePrintRateStatic, markCharMatchRateStatic, markCharOffsetXStatic, markCharOffsetYStatic, markCharOffsetRStatic;

		zkhyPublicFuncHN::numericalStatic(markCharOffset, markCharOffsetStatic);
		zkhyPublicFuncHN::numericalStatic(markCharConstrast, markCharConstrastStatic);
		zkhyPublicFuncHN::numericalStatic(markCharDistance, markCharDistanceStatic);
		zkhyPublicFuncHN::numericalStatic(markCharBlobSize, markCharBlobSizeStatic);
		zkhyPublicFuncHN::numericalStatic(markCharUnderPrintArea, markCharUnderPrintAreaStatic);
		zkhyPublicFuncHN::numericalStatic(markCharMorePrintArea, markCharMorePrintAreaStatic);
		zkhyPublicFuncHN::numericalStatic(markCharUnderPrintRate, markCharUnderPrintRateStatic);
		zkhyPublicFuncHN::numericalStatic(markCharMorePrintRate, markCharMorePrintRateStatic);
		zkhyPublicFuncHN::numericalStatic(markCharMatchRate, markCharMatchRateStatic);
		zkhyPublicFuncHN::numericalStatic(markCharOffsetX, markCharOffsetXStatic);
		zkhyPublicFuncHN::numericalStatic(markCharOffsetY, markCharOffsetYStatic);
		zkhyPublicFuncHN::numericalStatic(markCharOffsetR, markCharOffsetRStatic);

		// 将统计值写入输出
		int staticNum = staticRes.size();
		for (int i = 0; i < staticNum; ++i)
		{
			staticRes[i].extendParameters.d_params.block.markCharOffset = markCharOffsetStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharContrast = markCharConstrastStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharDistance = markCharDistanceStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharBlobSize = markCharBlobSizeStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharUnderPrintArea = markCharUnderPrintAreaStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharMorePrintArea = markCharMorePrintAreaStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharUnderPrintRate = markCharUnderPrintRateStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharMorePrintRate = markCharMorePrintRateStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharMatchRate = markCharMatchRateStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharOffsetX = markCharOffsetXStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharOffsetY = markCharOffsetYStatic[i];
			staticRes[i].extendParameters.d_params.block.markCharOffsetR = markCharOffsetRStatic[i];
		}
		return 0;
	}


	// 根据输入的坐标点集计算相邻点之间的X方向距离
	int computePointsDisX(const vector<cv::Point2f>& pointsCenter, vector<double>& resDis)
	{
		double disLeft = 0; //记录左侧距离
		int pointsNum = pointsCenter.size();
		// 遍历输入的点集
		for (int i = 0; i < pointsNum; ++i)
		{
			double disTmp = 0;
			// 计算当前点到其右侧相邻点的距离
			if (i + 1 < pointsNum)
				disTmp = pointsCenter[i + 1].x - pointsCenter[i].x;
			else
				disTmp = 0;

			// 每行最后一个点时计算得到的disTmp < 0，不需要特殊处理
			// 比较当前距离与左侧距离，取较大的值
			double disRes = disTmp;
			if (disTmp < disLeft)
				disRes = disLeft;

			// 记录结果
			resDis[i] = disRes;

			// 更新左侧距离
			disLeft = disTmp;
		}
		return 0;
	}


	//--------------------------------------markDetRegionToDefectStruct--------------------------------------------------//
	void markDetRegionToDefectStruct(HObject& defectRegion, FlawInfoStruct& outFlaw, const bool& useContour, const bool& useRotateRect, const HTuple& homMat2D)
	{
		double areaTmp, angleTmp, flawWidth, flawHeight;
		Point2f centerPoint;
		vector<Point2f> rectContour;
		vector<vector<Point2f>> defectContours;
		zkhyPublicFuncHN::genFlawInfoFromRegion(defectRegion, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectContour, defectContours, useContour, useRotateRect, homMat2D);
		outFlaw.FlawBasicInfo.flawArea = areaTmp;
		outFlaw.FlawBasicInfo.angle = angleTmp;
		outFlaw.FlawBasicInfo.flawWidth = flawWidth;
		outFlaw.FlawBasicInfo.flawHeight = flawHeight;
		outFlaw.FlawBasicInfo.centerPt = centerPoint;
		outFlaw.FlawBasicInfo.pts = rectContour;
		outFlaw.FlawBasicInfo.flawContours = defectContours;
		outFlaw.FlawBasicInfo.type = 0;
	}


	int realMarkDetect(const MarkDetectInput& input, MarkDetectOutput& output)
	{
#pragma region
		// Local iconic variables
		HObject  ho_testImg, ho_MarkRegion, ho_ModelContours;
		HObject  ho_ContoursAffinTrans, ho_Region1, ho_RegionUnion1;
		HObject  ho_MarkRegionAffineTrans, ho_smoothImage, ho_Region;
		HObject  ho_RegionClosing, ho_ConnectedRegions, ho_SelectedRegions;
		HObject  ho_RegionUnion, ho_Rectangle, ho_newMarkRegion;
		HObject  ho_ObjectSelected, ho_matchCharRectangle, ho_ImageReduced;
		HObject  ho_charModelContours, ho_ContoursAffineTrans, ho_Region3;
		HObject  ho_ObjectSelected1, ho_ObjectSelected2, ho_RegionIntersection;
		HObject  ho_maxOffsetRectangle, ho_oneRectangle, ho_oneCharForegroundRegion;
		HObject  ho_oneCharBackgroundRegion, ho_RegionUnion2, ho_testMarkRegion;
		HObject  ho_RegionIntersection1, ho_lessRegionDifference;
		HObject  ho_moreRegionDifference, ho_BlobRegion, ho_RegionErosion;

		// Local control variables
		HTuple  hv_low_threshold, hv_high_threshold, hv_min_char_area;
		HTuple  hv_minMarkScore, hv_minCharScore, hv_char_search_offset_x;
		HTuple	hv_minInvertProductArea;
		HTuple  hv_char_search_offset_y, hv_outDataDict, hv_teachDictHandle;
		HTuple  hv_teachImageWidth, hv_teachImageHeight, hv_MarkModelID;
		HTuple  hv_charShapeModelIDs, hv__tmp, hv_charShapeModelNum;
		HTuple  hv_idx, hv_dict_shapeModel_name, hv__shapeModel;
		HTuple  hv_NumLevels, hv_AngleStart, hv_AngleExtent, hv_AngleStep;
		HTuple  hv_ScaleMin, hv_ScaleMax, hv_ScaleStep, hv_Metric;
		HTuple  hv_MinContrast, hv_Row, hv_Column, hv_Angle, hv_Score;
		HTuple  hv_matchNum, hv_degAngle, hv_HomMat2D, hv_Width;
		HTuple  hv_Height, hv_productCenterX, hv_productCenterY;
		HTuple  hv_Row1, hv_Column1, hv_Phi, hv_Length1, hv_Length2;
		HTuple  hv_test_mark_center_x, hv_test_mark_center_y, hv_test_mark_d_x;
		HTuple  hv_test_mark_d_y, hv_teach_mark_d_x, hv_teach_mark_d_y;
		HTuple  hv_Prod_x, hv_Prod_y, hv_mark_position, hv_mark_angle;
		HTuple  hv_char_offset_xy, hv_Row11, hv_Column11, hv_Row2;
		HTuple  hv_Column2, hv_org_template_left_up_x, hv_org_template_left_up_y;
		HTuple  hv_offset_x, hv_offset_y, hv_new_HomMat2D, hv_Row12;
		HTuple  hv_Column12, hv_Row21, hv_Column21, hv_new_char_Row1;
		HTuple  hv_new_char_Column1, hv_new_char_Row2, hv_new_char_Column2;
		HTuple  hv_templateCharNum, hv_NumLevels1, hv_AngleStart1;
		HTuple  hv_AngleExtent1, hv_AngleStep1, hv_ScaleMin1, hv_ScaleMax1;
		HTuple  hv_ScaleStep1, hv_Metric1, hv_MinContrast1, hv_charIdx;
		HTuple  hv_Row9, hv_Column9, hv_Angle1, hv_Score1, hv_Row10;
		HTuple  hv_Column10, hv_HomMat2D3, hv_Number, hv_Index;
		HTuple  hv_Area1, hv_Row3, hv_Column3, hv_Area, hv_oneCharCenterOffset_x;
		HTuple  hv_oneCharCenterOffset_y, hv_Sqrt, hv_Row13, hv_Column13;
		HTuple  hv_Row22, hv_Column22, hv_mark_Margin_top, hv_mark_Margin_bottom;
		HTuple  hv_mark_Margin_left, hv_mark_Margin_right, hv_charMaxOffset;
		HTuple  hv_maxOffsetIndex, hv_constant_list, hv_charI, hv_onecharForegroundMean;
		HTuple  hv_Deviation, hv_onecharBackgroundMean, hv_cosntantMin;
		HTuple  hv_cosntantMax, hv_matchArea, hv_Row8, hv_Column8;
		HTuple  hv_newMarkArea, hv_Row5, hv_Column5, hv_matchRate;
		HTuple  hv_lessArea, hv_Row6, hv_Column6, hv_moreArea, hv_Row7;
		HTuple  hv_Column7, hv_underPrint, hv_morePrint;
		// 输出相关参数
		HTuple angleOffset, positionOffset, hv_charMinConstant;

#pragma endregion

		if (input.srcImg.IsInitialized() == false)
			return 1;

		//HTuple savePath = tempPath.c_str();
		//if (input.inTeachDictHandle == HTuple())
			//int len1 = 999;
		//int len = input.inTeachDictHandle.TupleLength().I();
		//WriteDict(input.inTeachDictHandle, "E:\\enPathTemp\\0313test", HTuple(), HTuple());

		// 外部检测参数输入 赋值
		AlgParaMarkDetect* pParams = (AlgParaMarkDetect*)&input.algPara;
		hv_low_threshold = pParams->i_params.block.lowThreshold;
		hv_high_threshold = pParams->i_params.block.highThreshold;
		hv_min_char_area = pParams->d_params.block.dCharArea;
		hv_char_search_offset_x = pParams->i_params.block.charSearchOffsetX / 2.0;
		hv_char_search_offset_y = pParams->i_params.block.charSearchOffsetY / 2.0;

		hv_minInvertProductArea = pParams->i_params.block.minInvertProductArea;
		hv_minMarkScore = pParams->d_params.block.minMarkMatchScore;
		hv_minCharScore = pParams->d_params.block.minCharMatchScore;

		int iThresholdFunc = pParams->i_params.block.iThresholdFunc;
		int iTargetProperty = pParams->i_params.block.iTargetProperty;
		int isBinarization = pParams->i_params.block.isBinarization;
		double dSmoothFactor = pParams->d_params.block.dSmoothFactor;
		double dOpeningRadius = pParams->d_params.block.dOpeningRadius;
		double dCharWidth = pParams->d_params.block.dCharWidth;
		double dCharHeight = pParams->d_params.block.dCharHeight;
		int isUseOCR = pParams->i_params.block.isUseOCR;
		int iOCRClassifier = pParams->i_params.block.iOCRClassifier;

		// OCR
		vector<int> OCRCharIndex = input.OCRCharIndex;

		HTuple targetProperty = "dark";
		if (iTargetProperty == 0)     // 动态阈值参数，'light' or 'dark'
			targetProperty = "light";

		HObject hSrcImg = input.srcImg;

		HTuple  hv_Key, hv_GenParamValue;
		try {
			GetDictParam(input.inTeachDictHandle, "keys", HTuple(), &hv_GenParamValue);
			double keysLen = hv_GenParamValue.TupleLength().D();
			if (keysLen < 1.0)
				return 2;
		}
		catch (HalconCpp::HException& HDevExpDefaultException) {
			return 2;
		}

		// 根据edgeAlign输入，把小的产品图抠出来， edgealign输出的5个点顺序为 左上 - 左下 - 右下 - 右上 - 中心
		std::vector<cv::Point2f> productMarkPoint;
		if (input.edgeAlign.size() < 5)
			return 3;//edgeAlign输入数据异常
		productMarkPoint.emplace_back(input.edgeAlign[0]);
		productMarkPoint.emplace_back(input.edgeAlign[1]);
		productMarkPoint.emplace_back(input.edgeAlign[2]);
		productMarkPoint.emplace_back(input.edgeAlign[3]);
		hv_productCenterX = input.edgeAlign[4].x;
		hv_productCenterY = input.edgeAlign[4].y;

		// 对输入区域进行坐标变换，需要处理反料检测ROI和检测ROI
		HTuple hv_transX, hv_transY, hv_transAngle, hv_HomMat2DInputRegion;
		hv_transX = input.template2DetectTransPara.translationValue.x;
		hv_transY = input.template2DetectTransPara.translationValue.y;
		hv_transAngle = -input.template2DetectTransPara.angle;
		VectorAngleToRigid(0, 0, 0, hv_transY, hv_transX, hv_transAngle, &hv_HomMat2DInputRegion);
		// 传入区域为空时不会返回异常值，而是返回一个空区域
		// 需要对异常进行处理
		HObject ho_productRegion, ho_invertDetectRegion, ho_detectRoi;
		bool isInvertDetectRegion = true;
		bool isDetectRoi = true;
		int ret = 0;
		if (ret)
			return 4;
		// Debug
		/*std::vector<cv::Point2f> invertRoi;
		invertRoi = input.invertRoi;
		if (input.invertRoi.size() <= 0)
		{
			invertRoi.emplace_back(cv::Point2f(258, 176));
			invertRoi.emplace_back(cv::Point2f(370, 180));
			invertRoi.emplace_back(cv::Point2f(366, 288));
			invertRoi.emplace_back(cv::Point2f(272, 281));
			invertRoi.emplace_back(cv::Point2f(258, 176));
		}*/
		if (input.invertRoi.size() <= 0)
		{
			isInvertDetectRegion = false;
		}
		else
		{
			vector<cv::Point2f> invertRoi = input.invertRoi;
			ret = Contours2Region(invertRoi, &ho_invertDetectRegion);
			AffineTransRegion(ho_invertDetectRegion, &ho_invertDetectRegion, hv_HomMat2DInputRegion, "nearest_neighbor");

			// 清理vector
			invertRoi.clear();
			invertRoi.shrink_to_fit();
			if (ret)
				return 4;
		}

		//test 
#if 0
		std::vector<cv::Point2f> detectRoi;
		detectRoi = input.detectRoi;
		HObject ho_product;
		if (detectRoi.size() <= 0)
		{
			detectRoi.emplace_back(cv::Point2f(227.44, 131.62));
			detectRoi.emplace_back(cv::Point2f(225.52, 1221.72));
			detectRoi.emplace_back(cv::Point2f(951.65, 1223.33));
			detectRoi.emplace_back(cv::Point2f(953.42, 133.85));
			detectRoi.emplace_back(cv::Point2f(227.44, 131.62));
			ret = Contours2Region(detectRoi, &ho_detectRoi);
			AffineTransRegion(ho_detectRoi, &ho_detectRoi, hv_HomMat2DInputRegion, "nearest_neighbor");
			Contours2Region(productMarkPoint, &ho_product);
		}
#endif

#if 1
		isDetectRoi = false;
		ret = Contours2Region(productMarkPoint, &ho_productRegion);
		ho_detectRoi = ho_productRegion;
		if (ret)
			return 4;

		if (input.detectRoi.size() > 0)
		{
			vector<cv::Point2f> detectRoi = input.detectRoi;
			ret = Contours2Region(detectRoi, &ho_detectRoi);
			AffineTransRegion(ho_detectRoi, &ho_detectRoi, hv_HomMat2DInputRegion, "nearest_neighbor");

			// 清理vector
			detectRoi.clear();
			detectRoi.shrink_to_fit();

			if (ret)
				return 4;
		}
#endif

		// test
#if 0
		ConvertedROICoordinateInput testInput;
		testInput.typeROI = ROIType::ROI_ContourPtsROI;
		ContourPtsROI* inputRoi = new ContourPtsROI();
		inputRoi->contourPts = detectRoi;
		inputRoi->offset = 0;
		testInput.ROIStruct = inputRoi;
		testInput.templateImg2SampleImgCSYSpara = input.template2DetectTransPara;

		ConvertedROICoordinateRlt testOutput;
		testOutput.ROIStruct = new ContourPtsROI();
		constructCSYS::convertedROICoordinate(testInput, testOutput);

		ContourPtsROI outRoi = *(ContourPtsROI*)testOutput.ROIStruct;
		vector<Point2f> outContour = outRoi.contourPts;
		HObject testROI;
		ret = Contours2Region(outContour, &testROI);
#endif

		//isInvertDetectRegion = false;
		//double dSmoothFactor = 0.5;
		// 矩形腐蚀宽度和高度
		double dDilationWidth = 10.0;
		double dDilationHeight = 10.0;
		//double dOpeningRadius = 3;
		double dErosionRadius = 1;

		HObject hEmpty, hEmptyRegion;
		HTuple hIsEuqal;
		GenEmptyObj(&hEmpty);
		GenEmptyRegion(&hEmptyRegion);

		GetImageSize(input.srcImg, &hv_Width, &hv_Height);
		CreateDict(&hv_outDataDict);
		DilationRectangle1(ho_detectRoi, &ho_detectRoi, dDilationWidth, dDilationHeight);
		//************************************  参数读取 *******************

		//SmoothImage(ho_testImg, &ho_testImg, "gauss", dSmoothFactor);
		hv_teachDictHandle = input.inTeachDictHandle;

		//读取教学图的好的字符区域
		HTuple hv_invertMarkModelID, GenParamName, Key, GenParamValue, hIsAdjustProduct, hCharAngleArc, hCharContrast, hMarkMargin;
		hCharContrast.Clear();
		HObject ho_charShapeModelRegions;
		GenEmptyObj(&ho_charShapeModelRegions);
		try
		{
			HTuple hv_dict_shapeModelRegion_name, hDictKeyName, hCharContrastTmp;
			HObject ho_charShapeModelRegionTmp;
			GetDictParam(hv_teachDictHandle, "keys", HTuple(), &GenParamValue);
			GetDictObject(&ho_MarkRegion, hv_teachDictHandle, "teachCharRegion");

			GetDictTuple(hv_teachDictHandle, "isAdjustProduct", &hIsAdjustProduct);
			GetDictTuple(hv_teachDictHandle, "templateProductAngle", &hCharAngleArc);

			//Mark匹配模型
			GetDictTuple(hv_teachDictHandle, "teachMarkShapeModel", &hv_MarkModelID);
			GetShapeModelContours(&ho_ModelContours, hv_MarkModelID, 1);

			// Mark到边缘的距离
			GetDictTuple(hv_teachDictHandle, "markMargin", &hMarkMargin);

			hv_charShapeModelIDs = HTuple();
			hv__tmp = HTuple();
			GetDictTuple(hv_teachDictHandle, "charShapeModelNum", &hv_charShapeModelNum);
			{
				HTuple end_val43 = hv_charShapeModelNum;
				HTuple step_val43 = 1;
				for (hv_idx = 1; hv_idx.Continue(end_val43, step_val43); hv_idx += step_val43)
				{
					hDictKeyName = "charContrast_" + hv_idx;
					GetDictTuple(hv_teachDictHandle, hDictKeyName, &hCharContrastTmp);
					hCharContrast.Append(hCharContrastTmp);

					hv_dict_shapeModel_name = "charShapeModel_" + hv_idx;
					GetDictTuple(hv_teachDictHandle, hv_dict_shapeModel_name, &hv__shapeModel);
					hv_charShapeModelIDs = hv_charShapeModelIDs.TupleConcat(hv__shapeModel);
					hv_dict_shapeModelRegion_name = "charShapeModelRegion_" + hv_idx;
					GetDictObject(&ho_charShapeModelRegionTmp, hv_teachDictHandle, hv_dict_shapeModelRegion_name);
					ConcatObj(ho_charShapeModelRegions, ho_charShapeModelRegionTmp, &ho_charShapeModelRegions);
				}
			}
		}
		catch (...)
		{
			return 5;
		}
		// 参数读取完毕 


		// 是否将产品转正
		int isAdjustProduct = hIsAdjustProduct.I();
		double productAngle = hCharAngleArc.D();

		if (isAdjustProduct != 0)
		{
			// 计算产品旋转角度
			computeAngle(input.edgeAlign[0], input.edgeAlign[3], productAngle);
			productAngle = (hCharAngleArc.D() - productAngle);
		}
		// 生成坐标变换矩阵

		HTuple hProductAdjust, hDet2OriImg;
		double imgNewHeight, imgNewWidth;
		computeRotateHomMat2D(hv_Width.D(), hv_Height.D(), productAngle, imgNewHeight, imgNewWidth, hProductAdjust);
		HomMat2dInvert(hProductAdjust, &hDet2OriImg);
		// 将产品转正
		SetSystem("clip_region", "false");

		//AffineTransImageSize(hImgSrc, &ho_Image, hProductAdjust, "nearest_neighbor", hv_Height, hv_Width);

		AffineTransImage(hSrcImg, &ho_testImg, hProductAdjust, "bilinear", "true");

		AffineTransRegion(ho_detectRoi, &ho_detectRoi, hProductAdjust, "nearest_neighbor");
		ReduceDomain(ho_testImg, ho_detectRoi, &ho_testImg);
		AffineTransPixel(hProductAdjust, hv_productCenterY, hv_productCenterX, &hv_productCenterY, &hv_productCenterX);

		// 对IC顶点进行旋转
		// productMarkPoint
		// 将产品的四个顶点信息映射到Layout上
		std::vector<cv::Point2f> productMarkPointTrans;
		productMarkPointTrans.resize(4);
		for (int i = 0; i < 4; i++)
		{
			HTuple hPointTmpY, hPointTmpX;
			Point2f pointTmp;
			AffineTransPixel(hProductAdjust, productMarkPoint[i].y, productMarkPoint[i].x, &hPointTmpY, &hPointTmpX);
			pointTmp.x = hPointTmpX;
			pointTmp.y = hPointTmpY;
			productMarkPointTrans[i] = pointTmp;
		}

		// ************************  阈值化方法提取字符区域   **************
		SmoothImage(ho_testImg, &ho_smoothImage, "deriche2", dSmoothFactor);
		//ho_smoothImage = ho_testImg;
		// Threshold(ho_smoothImage, &ho_Region, hv_low_threshold, hv_high_threshold);
		//thresholdSeg(ho_testImg, ho_Region, hv_low_threshold, hv_high_threshold, targetProperty, iThresholdFunc);
		//ClosingCircle(ho_Region, &ho_RegionClosing, 1);
		//Connection(ho_RegionClosing, &ho_ConnectedRegions);
		//只做了一次 字符区域 筛选
		//SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", hv_min_char_area, 999999999);
		//Union1(ho_SelectedRegions, &ho_RegionUnion);


		//*********************************  放反料检测   *******************************
		//坐标系转换
		HTuple hv_invertArea, hv_Row4, hv_Column4;
		//HTuple hv_transX, hv_transY, hv_transAngle, hv_invertHomMat2D;
		HObject ho_invertRegionAffineTrans, ho_invertRegion, ho_emptyRegion;

		//markDetRegionToDefectStruct(ho_invertRegion, invertDefect);
		/*else {
			invertDefect.extendParameters.d_params.block.markIsInvertProduct = 0;
			output.flawinfos.emplace_back(invertDefect);
		}*/

		//***************************************** 第一项：mark position ****************************************
		//主要思想：用基准的模板姿态偏移向量减去待测图的姿态偏移向量，得到一个字符相对于产品图像中心的偏移
		// @gxl Teach时查找到字符区域后存储位姿信息
		// @gxl 准备创建模型
		HObject ho_ImageMarkTest;
		// 不使用二值化方法
		//ho_ImageMarkTest = ho_smoothImage;
		// 裁剪出待检测区域，并记录左上角坐标，后续便于还原
		CropDomain(ho_smoothImage, &ho_ImageMarkTest);
		HTuple hv_productRowLT, hv_productColLT, hv_productRowRB, hv_productColRB;
		SmallestRectangle1(ho_detectRoi, &hv_productRowLT, &hv_productColLT, &hv_productRowRB, &hv_productColRB);
		if (isBinarization != 0)
		{   // 使用二值化方法
			// 准备测试图像
			HObject ho_RegionTmp, ho_ConnectedRegionTmp, ho_SelectedRegionTmp, ho_UnionedRegionTmp;
			zkhyPublicFuncHN::thresholdSeg(ho_ImageMarkTest, ho_RegionTmp, hv_low_threshold, hv_high_threshold, targetProperty, iThresholdFunc);
			//筛选字符
			ClosingCircle(ho_RegionTmp, &ho_RegionTmp, 1);
			Connection(ho_RegionTmp, &ho_ConnectedRegionTmp);
			SelectShape(ho_ConnectedRegionTmp, &ho_SelectedRegionTmp, "area", "and", hv_min_char_area, 999999999);
			Union1(ho_SelectedRegionTmp, &ho_UnionedRegionTmp);
			RegionToBin(ho_UnionedRegionTmp, &ho_ImageMarkTest, 255, 0, hv_Width, hv_Height);
		}
		GetShapeModelParams(hv_MarkModelID, &hv_NumLevels, &hv_AngleStart, &hv_AngleExtent,
			&hv_AngleStep, &hv_ScaleMin, &hv_ScaleMax, &hv_ScaleStep, &hv_Metric, &hv_MinContrast);
		FindShapeModel(ho_ImageMarkTest, hv_MarkModelID, hv_AngleStart.TupleRad(), hv_AngleExtent.TupleRad(),
			hv_minMarkScore, 1, 0.5, "least_squares", 0, 0.9, &hv_Row, &hv_Column, &hv_Angle,
			&hv_Score);
		HObject ho_ModelRegionTmp;
		GetShapeModelContours(&ho_ModelRegionTmp, hv_MarkModelID, 1);
		//可视化匹配后的效果
		hv_matchNum = hv_Angle.TupleLength();
		hv_degAngle = hv_Angle.TupleDeg();
		if ((hv_matchNum.I() == 1) && (hv_Score.D() > hv_minMarkScore.D()))
		{
			/*if (isAdjustProduct == 0)
			//得到对应匹配目标的旋转矩阵
				VectorAngleToRigid(0, 0, 0, hv_Row, hv_Column, hv_Angle, &hv_HomMat2D);
			else
				VectorAngleToRigid(0, 0, 0, hv_Row, hv_Column, (hv_Angle.D() + productAngle), &hv_HomMat2D);*/
			VectorAngleToRigid(0, 0, 0, hv_Row, hv_Column, hv_Angle, &hv_HomMat2D);
			AffineTransContourXld(ho_ModelContours, &ho_ContoursAffinTrans, hv_HomMat2D);
			//xld转换成region
			GenRegionContourXld(ho_ContoursAffinTrans, &ho_Region1, "filled");
			Union1(ho_Region1, &ho_RegionUnion1);
		}
		else
		{   // mark定位匹配失败
			FlawInfoStruct wholeDefect;
			wholeDefect.extendParameters.d_params.block.markCharMissing = 1;
			wholeDefect.extendParameters.d_params.block.markAngleOffset = 0;
			wholeDefect.extendParameters.d_params.block.markPositionOffset = 0;
			if (isInvertDetectRegion)
			{
				HObject hRegionTmp;
				AffineTransRegion(ho_invertRegion, &hRegionTmp, hDet2OriImg, "nearest_neighbor");
				output.ho_markCorrectionRegions = hRegionTmp;  //@gxl 反料检测结果不为空
				markDetRegionToDefectStruct(ho_invertRegion, wholeDefect);
			}
			else
			{
				HObject hRegionTmp;
				AffineTransRegion(ho_detectRoi, &hRegionTmp, hDet2OriImg, "nearest_neighbor");
				output.ho_markCorrectionRegions = hRegionTmp;            //给出IC区域，使后续算子能继续执行，且软件能裁剪到合适的缺陷小图
				markDetRegionToDefectStruct(ho_detectRoi, wholeDefect);
			}
			output.flawinfos.emplace_back(wholeDefect);
			return 0;
		}

		//SmallestRectangle2(ho_RegionUnion1, &hv_Row1, &hv_Column1, &hv_Phi, &hv_Length1, &hv_Length2);
		hv_test_mark_center_x = hv_Column + hv_productColLT;
		hv_test_mark_center_y = hv_Row + hv_productRowLT;

		hv_test_mark_d_x = hv_productCenterX - hv_test_mark_center_x;
		hv_test_mark_d_y = hv_productCenterY - hv_test_mark_center_y;
		GetDictTuple(hv_teachDictHandle, "teach_mark_d_x", &hv_teach_mark_d_x);
		GetDictTuple(hv_teachDictHandle, "teach_mark_d_y", &hv_teach_mark_d_y);
		// @gxl 是否取绝对值
		TupleMult(hv_test_mark_d_x - hv_teach_mark_d_x, hv_test_mark_d_x - hv_teach_mark_d_x, &hv_Prod_x);
		TupleMult(hv_test_mark_d_y - hv_teach_mark_d_y, hv_test_mark_d_y - hv_teach_mark_d_y, &hv_Prod_y);
		//第一项输出
		TupleSqrt(hv_Prod_x + hv_Prod_y, &hv_mark_position);
		positionOffset = (float)hv_mark_position.D();

		//***************************************** 第二项：mark angle ****************************************
		//主要思想：用教学图中建模的基准角度，和测试图片上匹配的角度相减
		//第二项输出
		// @gxl 角度可能存在问题
		GetDictTuple(hv_teachDictHandle, "teachBaseAngle", &hv_mark_angle);
		//angleOffset = (float)hv_mark_angle.D() - (float)hv_Angle.D();
		// @gxl Halcon 匹配获得的角度是会被限定在创建模型时设置的角度范围，该角度表示待测图中目标与模板图中目标的相对旋转角度
		// 弧度转角度

		auto radian2Angle = [](double radian) {return (radian / myPi * 180); };
		if (isAdjustProduct == 0)
			angleOffset = (radian2Angle(hv_Angle.D()) - radian2Angle(input.template2DetectTransPara.angle));
		else
			angleOffset = radian2Angle(hv_Angle.D());


		//***************************************** 第三&四项：mark margin , char offset ****************************************
		//主要思想：1.先把字符区域阈值化检测出来，
		//2.把模板的字符region进行校正生成 AffineRegion ，再用校正后的 AffineRegion 做外接矩，并外扩，抠图 匹配，生成新的 NerMarkRegion；
		//3.对NerMarkRegion进行求margin；
		//4.在匹配时会有中心坐标，并求校正后的 AffineRegion 每个字符的中心，两个中心相减就是offset

		hv_char_offset_xy = HTuple();
		//新建坐标系 ， 对模板的region 进行转换,粗定位
		SmallestRectangle1(ho_RegionUnion1, &hv_Row11, &hv_Column11, &hv_Row2, &hv_Column2);

		// @gxl 使用定位mark中心坐标计算偏移
		HTuple hv_mark_center_x, hv_mark_center_y;
		GetDictTuple(hv_teachDictHandle, "mark_center_x", &hv_mark_center_x);
		GetDictTuple(hv_teachDictHandle, "mark_center_y", &hv_mark_center_y);
		/*hv_offset_x = hv_Column - hv_mark_center_x;
		hv_offset_y = hv_Row - hv_mark_center_y;
		HTuple hv_mark_center_x, hv_mark_center_y;
		hv_offset_x = hv_teach_mark_d_x - hv_test_mark_d_x;
		hv_offset_y = hv_teach_mark_d_y - hv_test_mark_d_y;*/

		VectorAngleToRigid(hv_mark_center_y, hv_mark_center_x, 0, hv_test_mark_center_y, hv_test_mark_center_x, hv_Angle, &hv_new_HomMat2D);
		//VectorAngleToRigid(0, 0, 0, (hv_test_mark_center_y - hv_mark_center_y), (hv_test_mark_center_x - hv_mark_center_x), -hv_Angle, &hv_new_HomMat2D);
		AffineTransRegion(ho_MarkRegion, &ho_MarkRegionAffineTrans, hv_new_HomMat2D, "nearest_neighbor");


		//获取模板图每个字符的矩形框，并外扩一定的offset
		// @gxl 图像发生旋转的话，字符区域可能存在问题
		// TODO 字符区域顺序会发生混乱
		SmallestRectangle1(ho_MarkRegionAffineTrans, &hv_Row12, &hv_Column12, &hv_Row21, &hv_Column21);
		TupleSub(hv_Row12, hv_char_search_offset_y, &hv_new_char_Row1);
		TupleSub(hv_Column12, hv_char_search_offset_x, &hv_new_char_Column1);
		TupleAdd(hv_Row21, hv_char_search_offset_y, &hv_new_char_Row2);
		TupleAdd(hv_Column21, hv_char_search_offset_x, &hv_new_char_Column2);
		GenRectangle1(&ho_Rectangle, hv_new_char_Row1, hv_new_char_Column1, hv_new_char_Row2,
			hv_new_char_Column2);

		//逐个字符匹配模型
		CountObj(ho_MarkRegionAffineTrans, &hv_templateCharNum);
		GenEmptyRegion(&ho_newMarkRegion);
		GenEmptyObj(&ho_RegionUnion);
		GetShapeModelParams(HTuple(hv_charShapeModelIDs[0]), &hv_NumLevels1, &hv_AngleStart1,
			&hv_AngleExtent1, &hv_AngleStep1, &hv_ScaleMin1, &hv_ScaleMax1, &hv_ScaleStep1,
			&hv_Metric1, &hv_MinContrast1);

		HObject ho_TemplateMarkRegion, ho_DetCharRegion;
		GenEmptyObj(&ho_TemplateMarkRegion);
		GenEmptyObj(&ho_DetCharRegion);

		HTuple hv_imgTestHeight, hv_imgTestWidth;
		GetImageSize(ho_testImg, &hv_imgTestWidth, &hv_imgTestHeight);

		HTuple hv_matchRate_list, hv_underPrint_list, hv_morePrint_list, hv_charRow, hv_charCol, hv_char_offset_x, hv_char_offset_y, hv_lessArea_list, hv_moreArea_list, hv_blobSize_list;
		HObject ho_charTemplate, ho_lessOpened, ho_moreOpened, ho_lessMoreRegion;
		hv_constant_list = HTuple();
		hv_matchRate_list = HTuple();
		hv_underPrint_list = HTuple();
		hv_morePrint_list = HTuple();
		hv_blobSize_list = HTuple();
		hv_lessArea_list = HTuple();
		hv_moreArea_list = HTuple();

		vector<bool> charMissingFlag(hv_templateCharNum.I(), false);
		// lambda表达式 处理字符缺失的情况
		auto charMissingFunc = [](vector<bool>& charMissingFlag, const int& indx, HTuple& hConstantList, HTuple& hMatchRateList, HTuple& hUnderPrintList, HTuple& hMorePrintList, HTuple& hBlobSizeList, HTuple& hCharOffsetX, HTuple& hCharOffsetY, HTuple& hCharOffsetXY, HTuple& hLessAreaList, HTuple& hMoreAreaList, HObject& charRegion, HObject& hDetCharRegions) {
			charMissingFlag[indx] = true;
			hConstantList.Append(0);
			hMatchRateList.Append(0);
			hUnderPrintList.Append(0);
			hMorePrintList.Append(0);
			hBlobSizeList.Append(0);
			hCharOffsetX.Append(0);
			hCharOffsetY.Append(0);
			hCharOffsetXY.Append(0);
			hLessAreaList.Append(0);
			hMoreAreaList.Append(0);
			ConcatObj(hDetCharRegions, charRegion, &hDetCharRegions);
			};

		// OCR
		HObject hSingleCharImage;
		GenEmptyObj(&hSingleCharImage);

		vector<cv::Point2f> detCharCenter(hv_templateCharNum.I());     // 记录检测到的字符中心坐标
		{
			HTuple end_val130 = hv_templateCharNum - 1;
			HTuple step_val130 = 1;
			for (hv_charIdx = 0; hv_charIdx.Continue(end_val130, step_val130); hv_charIdx += step_val130)
			{
				SelectObj(ho_MarkRegionAffineTrans, &ho_ObjectSelected, hv_charIdx + 1);
				GenRectangle1(&ho_matchCharRectangle, HTuple(hv_new_char_Row1[hv_charIdx]), HTuple(hv_new_char_Column1[hv_charIdx]),
					HTuple(hv_new_char_Row2[hv_charIdx]), HTuple(hv_new_char_Column2[hv_charIdx]));
				// 矩形框超出图像大小时跳过本次循环
				if (hv_new_char_Row2[hv_charIdx].D() > hv_productRowRB || hv_new_char_Column2[hv_charIdx].D() > hv_productColRB || hv_new_char_Row1[hv_charIdx].D() < hv_productRowLT || hv_new_char_Column1[hv_charIdx].D() < hv_productColLT)
				{
					continue;
				}
				ReduceDomain(ho_testImg, ho_matchCharRectangle, &ho_ImageReduced);
				// 将字符区域裁剪为小图，记录每个小图左上角坐标，后续进行还原
				// @gxl 准备匹配使用的图片
				// 不使用二值化方法
				HObject ho_ImageCharTest;
				CropDomain(ho_ImageReduced, &ho_ImageCharTest);

				if (isBinarization != 0)
				{   // 使用二值化方法
					// 准备测试图像
					HObject ho_RegionTmp, ho_ConnectedRegionTmp, ho_SelectedRegionTmp, ho_UnionedRegionTmp;
					zkhyPublicFuncHN::thresholdSeg(ho_ImageCharTest, ho_RegionTmp, hv_low_threshold, hv_high_threshold, targetProperty, iThresholdFunc);
					//筛选字符
					//OpeningCircle(ho_RegionTmp, &ho_RegionTmp, dOpeningRadius);
					ClosingCircle(ho_RegionTmp, &ho_RegionTmp, dOpeningRadius);
					Connection(ho_RegionTmp, &ho_ConnectedRegionTmp);
					SelectShape(ho_ConnectedRegionTmp, &ho_SelectedRegionTmp, "area", "and", hv_min_char_area, 999999999);
					HTuple hAreaTmp, hRowTmp, hCloTmp;
					AreaCenter(ho_SelectedRegionTmp, &hAreaTmp, &hRowTmp, &hCloTmp);
					//SelectShapeStd(ho_ConnectedRegionTmp, &ho_SelectedRegionTmp, "max_area", 70);
					Union1(ho_SelectedRegionTmp, &ho_UnionedRegionTmp);
					RegionToBin(ho_UnionedRegionTmp, &ho_ImageCharTest, 255, 0, hv_Width, hv_Height);
				}
				// 在待测区域内查找目标

				FindShapeModel(ho_ImageCharTest, HTuple(hv_charShapeModelIDs[hv_charIdx]), hv_AngleStart1.TupleRad(),
					hv_AngleExtent1.TupleRad(), hv_minCharScore, 1, 0.6, "least_squares", 0, 0.7, &hv_Row9,
					&hv_Column9, &hv_Angle1, &hv_Score1);
				// Debug
				GetShapeModelContours(&ho_charModelContours, HTuple(hv_charShapeModelIDs[hv_charIdx]), 1);
				if (0 != (hv_Score1 < hv_minCharScore))
				{
					//匹配为空,字符缺失
					charMissingFunc(charMissingFlag, hv_charIdx, hv_constant_list, hv_matchRate_list, hv_underPrint_list, hv_morePrint_list, hv_blobSize_list, hv_char_offset_x, hv_char_offset_y, hv_char_offset_xy, hv_lessArea_list, hv_moreArea_list, ho_matchCharRectangle, ho_DetCharRegion);
					continue;
				}
				// 将坐标还原到大图 
				hv_Row9 = hv_Row9 + hv_new_char_Row1[hv_charIdx];
				hv_Column9 = hv_Column9 + hv_new_char_Column1[hv_charIdx];

				//OCR
				ConcatObj(hSingleCharImage, ho_ImageCharTest, &hSingleCharImage);

				// @gxl, 将模板区域旋转至对应位置
				SelectObj(ho_charShapeModelRegions, &ho_Region3, hv_charIdx + 1);
				VectorAngleToRigid(0, 0, 0, hv_Row9, hv_Column9, hv_Angle1, &hv_HomMat2D3);
				AffineTransRegion(ho_Region3, &ho_charTemplate, hv_HomMat2D3, "nearest_neighbor");
				ConcatObj(ho_newMarkRegion, ho_charTemplate, &ho_newMarkRegion);
#if 0
				if (isAdjustProduct != 0)
				{	// 将区域变换至待测图
					AffineTransRegion(ho_charTemplate, &ho_ObjectSelected1, hDet2OriImg, "nearest_neighbor");
					ConcatObj(ho_TemplateMarkRegion, ho_ObjectSelected1, &ho_TemplateMarkRegion);
				}
				else
				{
					ConcatObj(ho_TemplateMarkRegion, ho_charTemplate, &ho_TemplateMarkRegion);
				}
#endif
				// 将区域变换至待测图
				AffineTransRegion(ho_charTemplate, &ho_ObjectSelected1, hDet2OriImg, "nearest_neighbor");
				ConcatObj(ho_TemplateMarkRegion, ho_ObjectSelected1, &ho_TemplateMarkRegion);

				// 依据匹配到的位置和字符大小生成矩形区域，并在该区域内重新进行分割获得精确的字符区域
				HTuple charWidthStdHalf, charHighStdHalf;
				charWidthStdHalf = (hv_new_char_Row2[hv_charIdx] - hv_new_char_Row1[hv_charIdx]) / 2.0;
				charHighStdHalf = (hv_new_char_Column2[hv_charIdx] - hv_new_char_Column1[hv_charIdx]) / 2.0;
				//GenRectangle1(&ho_oneRectangle, hv_Row9 - charWidthStdHalf, hv_Column9 - charHighStdHalf, hv_Row9 + charWidthStdHalf, hv_Column9 + charHighStdHalf);
				// 默认情况下矩形长边方向与矩形角度一致
				GenRectangle2(&ho_oneRectangle, hv_Row9, hv_Column9, hv_Angle1, charHighStdHalf, charWidthStdHalf);
				HObject ho_ImageReduced2, ho_RegionTmp2, ho_ConnectedRegionTmp2, ho_SelectedRegionTmp2, ho_UnionedRegionTmp2;
				HTuple hSurrogateDetRegion;
				ReduceDomain(ho_testImg, ho_oneRectangle, &ho_ImageReduced2);
				zkhyPublicFuncHN::thresholdSeg(ho_ImageReduced2, ho_RegionTmp2, hv_low_threshold, hv_high_threshold, targetProperty, iThresholdFunc);

				//筛选字符
				TestEqualObj(ho_RegionTmp2, hEmpty, &hIsEuqal);
				if (hIsEuqal.I() == 1)
				{
					// 分割后面积不满足条件，判断为字符缺失
					// //匹配为空,字符缺失
					charMissingFunc(charMissingFlag, hv_charIdx, hv_constant_list, hv_matchRate_list, hv_underPrint_list, hv_morePrint_list, hv_blobSize_list, hv_char_offset_x, hv_char_offset_y, hv_char_offset_xy, hv_lessArea_list, hv_moreArea_list, ho_charTemplate, ho_DetCharRegion);
					continue;
				}
				ClosingCircle(ho_RegionTmp2, &ho_RegionTmp2, dOpeningRadius);
				Connection(ho_RegionTmp2, &ho_ConnectedRegionTmp2);

				// 高度筛选
				SelectShape(ho_ConnectedRegionTmp2, &ho_SelectedRegionTmp2, "height", "and", dCharHeight, 999999999);
				TestEqualObj(ho_SelectedRegionTmp2, hEmpty, &hIsEuqal);
				if (hIsEuqal.I() == 1)
				{
					// 分割后面积不满足条件，判断为字符缺失
					charMissingFunc(charMissingFlag, hv_charIdx, hv_constant_list, hv_matchRate_list, hv_underPrint_list, hv_morePrint_list, hv_blobSize_list, hv_char_offset_x, hv_char_offset_y, hv_char_offset_xy, hv_lessArea_list, hv_moreArea_list, ho_charTemplate, ho_DetCharRegion);
					continue;
				}

				// 宽度筛选
				SelectShape(ho_SelectedRegionTmp2, &ho_SelectedRegionTmp2, "width", "and", dCharWidth, 999999999);
				TestEqualObj(ho_SelectedRegionTmp2, hEmpty, &hIsEuqal);
				if (hIsEuqal.I() == 1)
				{
					// 分割后面积不满足条件，判断为字符缺失
					charMissingFunc(charMissingFlag, hv_charIdx, hv_constant_list, hv_matchRate_list, hv_underPrint_list, hv_morePrint_list, hv_blobSize_list, hv_char_offset_x, hv_char_offset_y, hv_char_offset_xy, hv_lessArea_list, hv_moreArea_list, ho_charTemplate, ho_DetCharRegion);
					continue;
				}

				// 面积筛选
				SelectShape(ho_SelectedRegionTmp2, &ho_SelectedRegionTmp2, "area", "and", hv_min_char_area, 999999999);
				//SelectShapeStd(ho_ConnectedRegionTmp, &ho_SelectedRegionTmp, "max_area", 70);
				TestEqualObj(ho_SelectedRegionTmp2, hEmpty, &hIsEuqal);
				if (hIsEuqal.I() == 1)
				{
					// 分割后面积不满足条件，判断为字符缺失
					charMissingFunc(charMissingFlag, hv_charIdx, hv_constant_list, hv_matchRate_list, hv_underPrint_list, hv_morePrint_list, hv_blobSize_list, hv_char_offset_x, hv_char_offset_y, hv_char_offset_xy, hv_lessArea_list, hv_moreArea_list, ho_charTemplate, ho_DetCharRegion);
					continue;
				}
				// 保留最大面积
				SelectShapeStd(ho_SelectedRegionTmp2, &ho_SelectedRegionTmp2, "max_area", 70);

				Union1(ho_SelectedRegionTmp2, &ho_UnionedRegionTmp2);
				ConcatObj(ho_DetCharRegion, ho_UnionedRegionTmp2, &ho_DetCharRegion);


				//第四项 字符偏移 计算
				HTuple hv_rectRowTmp1, hv_rectRowTmp2, hv_rectColTmp1, hv_rectColTmp2, charCenterRow, charCenterCol;
				SmallestRectangle1(ho_UnionedRegionTmp2, &hv_rectRowTmp1, &hv_rectColTmp1, &hv_rectRowTmp2, &hv_rectColTmp2);
				charCenterRow = (hv_rectRowTmp1 + hv_rectRowTmp2) / 2.0;
				charCenterCol = (hv_rectColTmp1 + hv_rectColTmp2) / 2.0;
				//AreaCenter(ho_ObjectSelected1, &hv_Area1, &hv_Row3, &hv_Column3);
				//AreaCenter(ho_ObjectSelected, &hv_Area, &hv_Row10, &hv_Column10);
				hv_oneCharCenterOffset_y = charCenterRow - ((hv_new_char_Row1[hv_charIdx] + hv_new_char_Row2[hv_charIdx]) / 2.0);
				hv_oneCharCenterOffset_x = charCenterCol - ((hv_new_char_Column2[hv_charIdx] + hv_new_char_Column1[hv_charIdx]) / 2.0);
				TupleSqrt((hv_oneCharCenterOffset_x * hv_oneCharCenterOffset_x) + (hv_oneCharCenterOffset_y * hv_oneCharCenterOffset_y),
					&hv_Sqrt);
				TupleConcat(hv_char_offset_x, hv_oneCharCenterOffset_x, &hv_char_offset_x);
				TupleConcat(hv_char_offset_y, hv_oneCharCenterOffset_y, &hv_char_offset_y);
				TupleConcat(hv_char_offset_xy, hv_Sqrt, &hv_char_offset_xy);
				//记录字符中心坐标
				detCharCenter[hv_charIdx.I()].x = charCenterCol;
				detCharCenter[hv_charIdx.I()].y = charCenterRow;

				// @gxl 使用精定位
				// 第五项： char Contrast  主要思想：1.根据外接矩，抠下图片，再根据抠下字符区域，和背景区域,计算对应图像的平均值；
				// 第六项：  Matching Rate 主要思想：1.计算匹配上的区域的比例，就是求交集，进行计算比例
				// 第七项： Under Print, Over Print 主要思想：1.少打印=模板-测试，过打印=测试-模板；

				//第五项 对比度 计算
				//Intersection(ho_oneRectangle, ho_DetCharRegion, &ho_oneCharForegroundRegion);
				OpeningCircle(ho_UnionedRegionTmp2, &ho_oneCharForegroundRegion, dOpeningRadius);
				Difference(ho_oneRectangle, ho_oneCharForegroundRegion, &ho_oneCharBackgroundRegion);
				// 腐蚀，减少误差
				ErosionCircle(ho_oneCharForegroundRegion, &ho_oneCharForegroundRegion, dErosionRadius);
				ErosionCircle(ho_oneCharBackgroundRegion, &ho_oneCharBackgroundRegion, dErosionRadius);
				Intensity(ho_oneCharForegroundRegion, ho_testImg, &hv_onecharForegroundMean, &hv_Deviation);
				Intensity(ho_oneCharBackgroundRegion, ho_testImg, &hv_onecharBackgroundMean, &hv_Deviation);
				//hv_constant_list[hv_charI] = (hv_onecharForegroundMean - hv_onecharBackgroundMean).TupleAbs();
				//TupleConcat(hv_constant_list, (hv_onecharForegroundMean - hv_onecharBackgroundMean).TupleAbs()/(255 + Inf) * 100, &hv_constant_list);  // 转换为百分数
				TupleConcat(hv_constant_list, (hv_onecharForegroundMean - hv_onecharBackgroundMean).TupleAbs() / (hCharContrast[hv_charIdx] + Inf) * 100, &hv_constant_list);  // 转换为百分数
				//constant_list[charI - 1] := (onecharForegroundMean - onecharBackgroundMean) /  (onecharForegroundMean + onecharBackgroundMean)

				// @gxl 区域有孔洞时可能存在多个contour
				//Intersection(ho_newMarkRegion, ho_oneRectangle, &ho_charTemplate);
				//GetRegionContour(ho_charTemplate, &hv_charRow, &hv_charCol);				// 单个字符的轮廓，需要汇总到最后
				AreaCenter(ho_charTemplate, &hv_newMarkArea, &hv_Row5, &hv_Column5);

				//第七项计算
				// @gxl 开运算 替换 腐蚀
				//少打印
				Difference(ho_charTemplate, ho_oneCharForegroundRegion, &ho_lessRegionDifference);
				OpeningCircle(ho_lessRegionDifference, &ho_lessOpened, dOpeningRadius);
				AreaCenter(ho_lessOpened, &hv_lessArea, &hv_Row6, &hv_Column6);
				//多打印
				Difference(ho_oneCharForegroundRegion, ho_charTemplate, &ho_moreRegionDifference);
				OpeningCircle(ho_moreRegionDifference, &ho_moreOpened, dOpeningRadius);
				AreaCenter(ho_moreOpened, &hv_moreArea, &hv_Row7, &hv_Column7);
				//输出比例
				// @gxl 区域为空时需要进行异常处理
				//hv_underPrint = 1.0 - (hv_lessArea / (hv_newMarkArea + Inf));
				//hv_morePrint = 1.0 + (hv_moreArea / (hv_newMarkArea + Inf));

				// 0901 更改计算方式
				hv_underPrint = hv_lessArea / (hv_newMarkArea + Inf);
				hv_morePrint = hv_moreArea / (hv_newMarkArea + Inf);

				TupleConcat(hv_underPrint_list, hv_underPrint, &hv_underPrint_list);
				TupleConcat(hv_morePrint_list, hv_morePrint, &hv_morePrint_list);
				TupleConcat(hv_lessArea_list, hv_lessArea, &hv_lessArea_list);
				TupleConcat(hv_moreArea_list, hv_moreArea, &hv_moreArea_list);
				// BlobSize
				HTuple hv_blobSize;
				hv_blobSize = hv_lessArea + hv_moreArea;
				TupleConcat(hv_blobSize_list, hv_blobSize, &hv_blobSize_list);
				//hv_underPrint_list[hv_charI] = hv_underPrint;
				//hv_morePrint_list[hv_charI] = hv_morePrint;

				//第六项 匹配率 计算
				hv_matchRate = 1 - ((hv_lessArea + hv_moreArea) / (hv_newMarkArea + Inf));
				TupleConcat(hv_matchRate_list, hv_matchRate, &hv_matchRate_list);
				//hv_matchRate_list[hv_charI] = hv_matchRate;

			}

		}

		//交集，找出字符
		HObject hMarkRegion;
		Union1(ho_DetCharRegion, &hMarkRegion);
		//Intersection(ho_newMarkRegion, ho_Rectangle, &ho_RegionIntersection);
		SmallestRectangle1(hMarkRegion, &hv_Row13, &hv_Column13, &hv_Row22, &hv_Column22);
		// 到产品边界的距离
		// 外接矩形到产品四条边的距离
		DistancePl(hv_Row13, hv_Column13, productMarkPointTrans[0].y, productMarkPointTrans[0].x, productMarkPointTrans[3].y, productMarkPointTrans[3].x, &hv_mark_Margin_top);
		DistancePl(hv_Row22, hv_Column22, productMarkPointTrans[1].y, productMarkPointTrans[1].x, productMarkPointTrans[2].y, productMarkPointTrans[2].x, &hv_mark_Margin_bottom);
		DistancePl(hv_Row13, hv_Column13, productMarkPointTrans[0].y, productMarkPointTrans[0].x, productMarkPointTrans[1].y, productMarkPointTrans[1].x, &hv_mark_Margin_left);
		DistancePl(hv_Row22, hv_Column22, productMarkPointTrans[2].y, productMarkPointTrans[2].x, productMarkPointTrans[3].y, productMarkPointTrans[3].x, &hv_mark_Margin_right);

		// 计算字符最大的偏移量
		TupleMax(hv_char_offset_xy, &hv_charMaxOffset);
		TupleFind(hv_char_offset_xy, hv_charMaxOffset, &hv_maxOffsetIndex);
		HTuple hFindCharNum;
		CountObj(ho_DetCharRegion, &hFindCharNum);

		// 字符间距计算
		vector<double> charDistance(hv_templateCharNum.I());
		computePointsDisX(detCharCenter, charDistance);

		if (isAdjustProduct != 0)
		{	// 将区域变换至待测图
			AffineTransRegion(ho_DetCharRegion, &ho_DetCharRegion, hDet2OriImg, "nearest_neighbor");
		}

		TupleMin(hv_constant_list, &hv_charMinConstant);
		HTuple hMarkContrast;
		TupleMean(hv_constant_list, &hMarkContrast);
		//***********************************   生成缺陷数据   ****************************
		{
			HTuple end_val216 = hFindCharNum - 1;
			HTuple step_val216 = 1;
			for (hv_charI = 0; hv_charI.Continue(end_val216, step_val216); hv_charI += step_val216)
			{
				FlawInfoStruct oneDefect;
				SelectObj(ho_DetCharRegion, &ho_ObjectSelected, hv_charI + 1);
				//SelectObj(ho_TemplateMarkRegion, &ho_charTemplate, hv_charI + 1);
				HObject ho_charRegion;
				ho_charRegion = ho_ObjectSelected;
				/*if (isAdjustProduct != 0)   // ho_oneCharForegroundRegion
				{	// 将区域变换至待测图
					AffineTransRegion(ho_ObjectSelected, &ho_charRegion, hDet2OriImg, "nearest_neighbor");
				}*/
				markDetRegionToDefectStruct(ho_charRegion, oneDefect, true, false);

				oneDefect.FlawBasicInfo.type = 0;
				//if (oneDefect.FlawBasicInfo.flawArea == 0)
					//continue;
				// 对每一个少多打印缺陷的扩展特征 进行赋值
				oneDefect.extendParameters.d_params.block.markCharUnderPrintArea = (float)hv_lessArea_list[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markCharMorePrintArea = (float)hv_moreArea_list[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markCharUnderPrintRate = (float)hv_underPrint_list[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markCharMorePrintRate = (float)hv_morePrint_list[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markCharMatchRate = (float)hv_matchRate_list[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markAngleOffset = (float)angleOffset.D();
				oneDefect.extendParameters.d_params.block.markPositionOffset = (float)positionOffset.D();
				oneDefect.extendParameters.d_params.block.markMarginTop = (float)hMarkMargin[0].D();
				oneDefect.extendParameters.d_params.block.markMarginBottom = (float)hMarkMargin[1].D();
				oneDefect.extendParameters.d_params.block.markMarginLeft = (float)hMarkMargin[2].D();
				oneDefect.extendParameters.d_params.block.markMarginRight = (float)hMarkMargin[3].D();
				oneDefect.extendParameters.d_params.block.markCharOffset = (float)hv_charMaxOffset.D();
				oneDefect.extendParameters.d_params.block.markCharContrast = ((float)hv_constant_list[hv_charI].D() > 100) ? (100 - Inf) : (float)hv_constant_list[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markCharOffsetX = (float)hv_char_offset_x[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markCharOffsetY = (float)hv_char_offset_y[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markCharOffsetR = (float)hv_char_offset_xy[hv_charI].D();
				oneDefect.extendParameters.d_params.block.markContrast = (float)hMarkContrast.D();
				oneDefect.extendParameters.d_params.block.markCharBlobSize = (float)hv_blobSize_list.D();
				oneDefect.extendParameters.d_params.block.markCharDistance = (float)charDistance[hv_charI.I()];
				oneDefect.extendParameters.d_params.block.markIsInvertProduct = 0;
				oneDefect.extendParameters.d_params.block.markCharMissing = (charMissingFlag[hv_charI.I()] == true) ? 1 : 0;
				output.flawinfos.emplace_back(oneDefect);
			}
		}

		// 计算mark缺陷特征统计值
		output.flawStatic.resize(3);
		//zkhyPublicFuncHN::genFlawBasicInfo(output.flawStatic);
		markFeatureStatic(output.flawinfos, output.flawStatic);

		HObject ho_markRegions, ho_DetMarkRegion;
		if (isInvertDetectRegion)
		{
			Union2(ho_DetCharRegion, ho_invertRegion, &ho_markRegions);
			output.ho_markCorrectionRegions = ho_markRegions;                // 输出校正后的mark模板字符
		}
		else
			output.ho_markCorrectionRegions = ho_DetCharRegion;
		//output.ho_markCorrectionRegions = ho_newMarkRegion;		// 输出校正后的mark模板字符

		// 计算字符平均间距
		double charDisMean;
		//double charDisMean = accumulate(charDistance.begin(), charDistance.end(), 0.0) / charDistance.size();

		// 排除部分异常值
		computePartialDataMean(charDistance, 0.1, charDisMean);

		// 最后一个是整体的字符，特征包含常规的和定制的整体特征；
		FlawInfoStruct wholeDefect;
		Union1(ho_DetCharRegion, &ho_DetMarkRegion);
		markDetRegionToDefectStruct(ho_DetMarkRegion, wholeDefect);
		wholeDefect.FlawBasicInfo.type = 0;
		wholeDefect.extendParameters.d_params.block.markCharUnderPrintArea = Inf;
		wholeDefect.extendParameters.d_params.block.markCharMorePrintArea = Inf;
		wholeDefect.extendParameters.d_params.block.markCharUnderPrintRate = Inf;
		wholeDefect.extendParameters.d_params.block.markCharMorePrintRate = Inf;
		wholeDefect.extendParameters.d_params.block.markCharMatchRate = 1.0;
		wholeDefect.extendParameters.d_params.block.markAngleOffset = (float)angleOffset.D();
		wholeDefect.extendParameters.d_params.block.markPositionOffset = (float)positionOffset.D();
		wholeDefect.extendParameters.d_params.block.markMarginTop = (float)hv_mark_Margin_top.D();
		wholeDefect.extendParameters.d_params.block.markMarginBottom = (float)hv_mark_Margin_bottom.D();
		wholeDefect.extendParameters.d_params.block.markMarginLeft = (float)hv_mark_Margin_left.D();
		wholeDefect.extendParameters.d_params.block.markMarginRight = (float)hv_mark_Margin_right.D();
		wholeDefect.extendParameters.d_params.block.markCharOffset = (float)hv_charMaxOffset.D();
		wholeDefect.extendParameters.d_params.block.markCharOffsetX = 0;
		wholeDefect.extendParameters.d_params.block.markCharOffsetY = 0;
		wholeDefect.extendParameters.d_params.block.markCharOffsetR = 0;
		wholeDefect.extendParameters.d_params.block.markCharContrast = 100 - Inf;  //TODO 这个值感觉没什么用
		wholeDefect.extendParameters.d_params.block.markCharBlobSize = 0;
		wholeDefect.extendParameters.d_params.block.markContrast = 100 - Inf;
		wholeDefect.extendParameters.d_params.block.markIsInvertProduct = 0;
		wholeDefect.extendParameters.d_params.block.markCharDistance = charDisMean;
		wholeDefect.extendParameters.d_params.block.markCharMissing = 0;
#if 0
		if (hFindCharNum.I() != hv_templateCharNum.I())
			wholeDefect.extendParameters.d_params.block.markCharMissing = 1;
		else
			wholeDefect.extendParameters.d_params.block.markCharMissing = 0;
#endif
		output.flawinfos.emplace_back(wholeDefect);

		// 填充Mark整体特征的统计值
		for (int i = 0; i < 3; ++i)
		{
			output.flawStatic[i].extendParameters.d_params.block.markAngleOffset = (float)angleOffset.D();
			output.flawStatic[i].extendParameters.d_params.block.markPositionOffset = (float)positionOffset.D();
			output.flawStatic[i].extendParameters.d_params.block.markMarginTop = (float)hv_mark_Margin_top.D();
			output.flawStatic[i].extendParameters.d_params.block.markMarginBottom = (float)hv_mark_Margin_bottom.D();
			output.flawStatic[i].extendParameters.d_params.block.markMarginLeft = (float)hv_mark_Margin_left.D();
			output.flawStatic[i].extendParameters.d_params.block.markMarginRight = (float)hv_mark_Margin_right.D();
			output.flawStatic[i].extendParameters.d_params.block.markCharOffset = (float)hv_charMaxOffset.D();
		}


		// 将模板轮廓压入输出
		zkhyPublicFuncHN::genContourFromRegion(ho_TemplateMarkRegion, output.templateCharContour);

		// OCR
		if (isUseOCR == 1)
		{
			HTuple hCharIndex, hCharsOCR, hChar;
			hCharIndex = HTuple();
			hCharsOCR = HTuple();

			for (int i = 0; i < OCRCharIndex.size(); ++i)
			{
				hCharIndex.Append(OCRCharIndex[i]);
			}

			HObject hCharRegionOCRTmp;
			//image_recognition(hSingleCharImage, &hCharRegionOCRTmp, dCharHeight, hCharIndex, 0, targetProperty, &hChar);
			simpleOCR(hSingleCharImage, dCharHeight, hCharIndex, targetProperty, iOCRClassifier, &hChar);
			hCharsOCR.Append(hChar);
			// OCR 结果
			output.OCRResult = hCharsOCR.S();
		}
		else
		{
			output.OCRResult = "";
		}

#if 0
		// monitor the number of shape module
		// The number of mark module or char modules are not changed when running.
		time_t t = time(0);
		char tmp[64];
		struct tm ptm;
		localtime_s(&ptm, &t);
		sprintf_s(tmp, "%d%d%d", ptm.tm_year + 1900, ptm.tm_mon + 1, ptm.tm_mday);

		String fileName = "./AlgMarkDetectDebug_" + String(tmp) + ".txt";

		// 输出执行状态以及输入参数
		ofstream fout(fileName, ios::app);
		HTuple hMarkLocNum, hCharNum;

		string exeInfo = "Mark定位模型数量：" + to_string(hv_MarkModelID.Length()) + "    " + "字符模型数量：" + to_string(hv_charShapeModelIDs.Length()) + "\n";

		fout << exeInfo;
		fout.close();
#endif

#if 0
		// clear shape model. This Operate will lead to error for the next detection
		for (int i = 0; i < hv_templateCharNum - 1; ++i)
		{
			ClearShapeModel(hv_charShapeModelIDs[i]);
		}
		ClearShapeModel(hv_MarkModelID);
#endif
#if 0
		vector<FlawInfoStruct> testDefect;
		testRegionsToDefectStruct(ho_DetCharRegion, testDefect, true, false);
		output.flawinfos = testDefect;
#endif

		// 清理vector
		productMarkPointTrans.clear();
		detCharCenter.clear();

		productMarkPointTrans.shrink_to_fit();
		detCharCenter.shrink_to_fit();

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int MarkDetect(const MarkDetectInput& input, MarkDetectOutput& output)
	{
		try
		{
			return realMarkDetect(input, output);
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 7; //未知错误
		}
	}
}
