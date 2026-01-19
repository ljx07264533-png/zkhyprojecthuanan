#include "alglibrary/zkhyProjectHuaNan/gatherLine.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/utils.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

	// 构造函数
	AlgGatherLine::AlgGatherLine()
	{
		// 算法参数
		isPreprocessing = 0;
		threshold = 128;
		edgePolarity = 0;
		edgeSelect = 0;
		instancesOutSideMeasureRegions = 0;
		iterationNum = -1;
		measureDis = 10;
		interpolation = 2;
		instancesNum = 1;
		isFixSeed = 0;
		smoothFactor = 0.5;
		edgeStrength = 30;
		ransacDis = 3;
		scoreMin = 0.7;
		searchWidthHalf = 5.0;
		searchHeightHalf = 20.0;

		hLineResult.Clear();
		hDetImgWidth.Clear();
		hDetImgHeight.Clear();
		hSearchLineStart.Clear();
		hSearchLineEnd.Clear();
		hEdgePointsRow.Clear();
		hEdgePointsCol.Clear();
		GenEmptyObj(&hDetImage);
		GenEmptyObj(&hMetrologyContours);

	}

	// 构造函数-重载
	AlgGatherLine::AlgGatherLine(const gatherLineInput& input)
	{
		AlgParamGatherLine* alg = (AlgParamGatherLine*)&input.algPara;

		// 算法参数
		isPreprocessing = alg->i_params.block.isPreprocessing;
		threshold = alg->i_params.block.iThreshold;
		edgePolarity = alg->i_params.block.iEdgePolarity;
		edgeSelect = alg->i_params.block.iEdgeSelect;
		instancesOutSideMeasureRegions = alg->i_params.block.iInstancesOutSideMeasureRegions;
		iterationNum = alg->i_params.block.iIterationNum;
		measureDis = alg->i_params.block.iMeasureDis;
		interpolation = alg->i_params.block.iInterpolation;
		instancesNum = alg->i_params.block.iInstancesNum;
		isFixSeed = alg->i_params.block.isFixSeed;
		smoothFactor = alg->d_params.block.dSmoothFactor;
		edgeStrength = alg->d_params.block.dEdgeStrength;
		ransacDis = alg->d_params.block.dRansacDis;
		scoreMin = alg->d_params.block.dScoreMin;

		searchWidthHalf = alg->i_params.block.iSearchWidth / 2.0;
		//searchHeightHalf = alg->i_params.block.iSearchHeight / 2.0;

		hLineResult.Clear();
		hDetImgWidth.Clear();
		hDetImgHeight.Clear();
		hSearchLineStart.Clear();
		hSearchLineEnd.Clear();
		hEdgePointsRow.Clear();
		hEdgePointsCol.Clear();
		GenEmptyObj(&hMetrologyContours);

		hDetImage = input.inputImg;
		searchROI = input.rectROI;

	}

	// 检查输入参数是否越界
	int AlgGatherLine::checkParams()
	{
		if (isPreprocessing < 0 || isPreprocessing>1) {
			return 3;
		}
		if (threshold < 0 || threshold > 255) {
			return 4;
		}
		if (edgePolarity < 0 || edgePolarity > 3) {
			return 5;
		}
		if (edgeSelect < 0 || edgeSelect > 2) {
			return 6;
		}
		if (searchWidthHalf < 0) {
			return 7;
		}
		if (instancesOutSideMeasureRegions < 0 || instancesOutSideMeasureRegions > 1) {
			return 9;
		}
		if (iterationNum < -1) {
			return 10;
		}
		if (measureDis < 0) {
			return 11;
		}
		if (interpolation < 0 || interpolation > 2) {
			return 12;
		}
		if (instancesNum < 0) {
			return 13;
		}
		if (isFixSeed < 0 || isFixSeed > 1) {
			return 14;
		}
		if (smoothFactor < 0) {
			return 15;
		}
		if (edgeStrength < 0) {
			return 16;
		}
		if (ransacDis < 0) {
			return 17;
		}
		if (scoreMin < 0 || scoreMin > 1) {
			return 18;
		}

		return 0;
	}

	// 预处理
	int AlgGatherLine::preProcessImage()
	{
		HObject hImageTmp;

		// 获取通道数
		HTuple hChannels;
		CountChannels(hDetImage, &hChannels);
		if (hChannels.I() == 3) {
			// 转换为灰度图
			Rgb1ToGray(hDetImage, &hImageTmp);
		}
		else {
			hImageTmp = hDetImage;
		}

		GetImageSize(hImageTmp, &hDetImgWidth, &hDetImgHeight);

		// 预处理，根据阈值对图像做阈值分割，并将结果转换为图像作为待测图
		if (isPreprocessing == 1) {
			HObject hSegRegion;

			Threshold(hImageTmp, &hSegRegion, threshold, 255);
			RegionToBin(hSegRegion, &hDetImage, 255, 0, hDetImgWidth, hDetImgHeight);
		}
		else {
			hDetImage = hImageTmp;
		}

		return 0;
	}

	// 解析输入ROI
	int AlgGatherLine::analyzeROI()
	{
		// 扫描方向为正，沿起始点到终点方向顺时针旋转90度为扫描方向, 与halcon中定义的扫描方向一致
		if (searchROI.direction == ROIDirection::Positive) {
			hSearchLineStart.Append(searchROI.pt1.y);
			hSearchLineStart.Append(searchROI.pt1.x);
			hSearchLineEnd.Append(searchROI.pt2.y);
			hSearchLineEnd.Append(searchROI.pt2.x);
		}
		else if (searchROI.direction == ROIDirection::Negative) {
			hSearchLineStart.Append(searchROI.pt2.y);
			hSearchLineStart.Append(searchROI.pt2.x);
			hSearchLineEnd.Append(searchROI.pt1.y);
			hSearchLineEnd.Append(searchROI.pt1.x);
		}
		else {
			return 20;
		}

		searchHeightHalf = searchROI.offset;

		if (searchWidthHalf < 0) {
			return 7;
		}

		return 0;
	}

	// 拟合直线
	int AlgGatherLine::fitLines()
	{
		HTuple hMetrologyHandle;
		// 创建测量对象
		CreateMetrologyModel(&hMetrologyHandle);

		// 设置测量图像尺寸
		SetMetrologyModelImageSize(hMetrologyHandle, hDetImgWidth, hDetImgHeight);

		// 设置测量参数
		// 选取的参数有 ['distance_threshold', 'instances_outside_measure_regions', 'max_num_iterations', 'measure_distance', 'measure_interpolation', 'measure_select', 'measure_transition', 'min_score', 'num_instances', 'rand_seed']
		HTuple hMetrologyObjName, hMetrologyObjValue;
		setMetrologyParams(hMetrologyObjName, hMetrologyObjValue);
		//SetMetrologyModelParam(hMetrologyHandle, hMetrologyObjName, hMetrologyObjValue);

		// 添加测量对象
		HTuple hMetrologyObjIndex;
		hMetrologyObjIndex.Clear();
		AddMetrologyObjectLineMeasure(hMetrologyHandle, hSearchLineStart[0], hSearchLineStart[1], hSearchLineEnd[0], hSearchLineEnd[1], searchHeightHalf, searchWidthHalf, smoothFactor, edgeStrength, hMetrologyObjName, hMetrologyObjValue, &hMetrologyObjIndex);

		ApplyMetrologyModel(hDetImage, hMetrologyHandle);

		// 可视化测量矩形
		HObject hFindEdges;
		GetMetrologyObjectMeasures(&hMetrologyContours, hMetrologyHandle, "all", "all", &hEdgePointsRow, &hEdgePointsCol);

		// 调试，NDEBUG在gcc和clang中生效，_DEBUG在vs中生效
#if not defined(NDEBUG) || defined(_DEBUG)
		// 可视化边缘点
		GenCrossContourXld(&hFindEdges, hEdgePointsRow, hEdgePointsCol, 6, 0.785398);
#endif

		// 获取测量结果
		GetMetrologyObjectResult(hMetrologyHandle, hMetrologyObjIndex, "all", "result_type", "all_param", &hLineResult);

		// 清理测量对象
		ClearMetrologyModel(hMetrologyHandle);

		return 0;
	}

	// 设置测量参数
	void AlgGatherLine::setMetrologyParams(HTuple& hMetrologyObjName, HTuple& hMetrologyObjValue)
	{
		// 选取的参数有 ['distance_threshold', 'instances_outside_measure_regions', 'max_num_iterations', 'measure_distance', 'measure_interpolation', 'measure_select', 'measure_transition', 'min_score', 'num_instances', 'rand_seed']
		hMetrologyObjName.Clear();
		hMetrologyObjValue.Clear();
		hMetrologyObjName[0] = "distance_threshold";
		hMetrologyObjName[1] = "instances_outside_measure_regions";
		hMetrologyObjName[2] = "max_num_iterations";
		hMetrologyObjName[3] = "measure_distance";
		hMetrologyObjName[4] = "measure_interpolation";
		hMetrologyObjName[5] = "measure_select";
		hMetrologyObjName[6] = "measure_transition";
		hMetrologyObjName[7] = "min_score";
		hMetrologyObjName[8] = "num_instances";
		hMetrologyObjName[9] = "rand_seed";

		hMetrologyObjValue[0] = ransacDis;
		if (instancesOutSideMeasureRegions == 1) {
			hMetrologyObjValue[1] = "true";
		}
		else {
			hMetrologyObjValue[1] = "false";
		}
		hMetrologyObjValue[2] = iterationNum;
		hMetrologyObjValue[3] = measureDis;
		if (interpolation == 0) {
			hMetrologyObjValue[4] = "bicubic";
		}
		else if (interpolation == 1) {
			hMetrologyObjValue[4] = "bilinear";
		}
		else {
			hMetrologyObjValue[4] = "nearest_neighbor";
		}

		HTuple hEdgeSelect, hEdgePolarity;
		getHedgeSelect(edgeSelect, hEdgeSelect);
		getHedgePolarity(edgePolarity, hEdgePolarity);

		hMetrologyObjValue[5] = hEdgeSelect;
		hMetrologyObjValue[6] = hEdgePolarity;
		hMetrologyObjValue[7] = scoreMin;
		hMetrologyObjValue[8] = instancesNum;
		if (isFixSeed == 0) {
			hMetrologyObjValue[9] = 0;
		}
		else {
			hMetrologyObjValue[9] = 202401311750;
		}
	}

	// 执行算法
	int AlgGatherLine::execute()
	{
		// 检查输入参数是否越界
		int ret = checkParams();
		if (ret != 0) {
			return ret;
		}

		// 预处理
		try {
			preProcessImage();
		}
		catch (HException& except) {
			string errorMsg = except.ErrorMessage().Text();
			LOG.log(errorMsg, alglib::core::err);
			return 21;
		}

		// 解析输入ROI
		ret = analyzeROI();
		if (ret != 0) {
			return ret;
		}

		// 拟合直线
		try {
			fitLines();
		}
		catch (HException& except) {
			string errorMsg = except.ErrorMessage().Text();
			LOG.log(errorMsg, alglib::core::err);
			return 22;
		}

		return 0;
	}

	//-----------------------------gatherLine 函数实现-----------------------------//
	int realGatherLine(const gatherLineInput& input, gatherLineOutput& output)
	{
		// 检查输入图像是否为空
		if (zkhyPublicFuncHN::isHObjectEmpty(input.inputImg)) {
			return 19;
		}
		// 解析参数
		AlgParamGatherLine* algParams = (AlgParamGatherLine*)&input.algPara;
		double timeOutThreshold = algParams->d_params.block.dTimeOutThreshold;

		// 拟合直线类
		AlgGatherLine cGatherLine(input);

		// 开始计时
		DWORD startTime = GetTickCount64();

		// 执行算法
		int ret = cGatherLine.execute();
		if (ret != 0) {
			return ret;
		}

		// 获取拟合直线结果
		HTuple hLineResult, hResPointsNum;
		cGatherLine.getLineResult(hLineResult);

		TupleLength(hLineResult, &hResPointsNum);

		if (hResPointsNum.I() == 4) {

			// 输出结果，将结果转换成对应的格式
			vector<cv::Point2f> lineEdgePoints(2);
			lineEdgePoints[0] = Point2f(hLineResult[1].D(), hLineResult[0].D());
			lineEdgePoints[1] = Point2f(hLineResult[3].D(), hLineResult[2].D());

			output.line.pt1 = lineEdgePoints[0];
			output.line.pt2 = lineEdgePoints[1];

			output.twoPtsLinePic.clear();
			output.twoPtsLinePic.push_back(lineEdgePoints);
		}
		// 未找到直线
		else if (hResPointsNum.I() == 0)
			return 23;

		// 获取测量矩形
		HObject hMetrologyContours;
		cGatherLine.getMetrologyContours(hMetrologyContours);

		zkhyPublicFuncHN::genContourFromHContour(hMetrologyContours, output.metrologyContours);
		// 获取边缘点
		HTuple hEdgePointsRow, hEdgePointsCol;
		cGatherLine.getEdgePoints(hEdgePointsRow, hEdgePointsCol);

		vector<cv::Point2f> edgePoints(hEdgePointsRow.Length());
		zkhyPublicFuncHN::hTuple2Points(hEdgePointsCol, hEdgePointsRow, edgePoints);

		output.edgePointsFind.clear();
		output.edgePointsFind.push_back(edgePoints);

		// 计算算法耗时
		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 2;                   // 算法超时

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//-----------------------------gatherLine 函数入口-----------------------------//
	int gatherLine(const gatherLineInput& input, gatherLineOutput& output)
	{
		try {
			int ret = realGatherLine(input, output);
			return ret;
		}
		catch (std::exception& msg) {	//错误信息输出到日志
			string errorMsg = msg.what();

			LOG.log(errorMsg, alglib::core::err);

			return 1;
		}
	}

}
