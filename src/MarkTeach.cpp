#include <windows.h>
#include <numeric>
#include <random>

#include "alglibrary/zkhyProjectHuaNan/MarkTeach.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	// 根据传入的图像和区域创建形状匹配模型/差异化模型
	class createMarkModel : public zkhyPublicClass::AlgorithmBase
	{
	public:
		struct SegParams {
			int isBinarization;		// 是否二值化

			int thresholdFunc;		// 阈值化方法

			int lowThreshold;		// 阈值化的低阈值
			int highThreshold;		// 阈值化的高阈值
			int minCharArea;		// 字符最小面积
			int maxCharArea;		// 字符最大面积

			double openingRadius;	// 开运算半径，用于对分割区域进行平滑
			double dilationRadius;	// 建模时膨胀半径，不使用二值化时对区域进行膨胀

			// 使用初始化列表来初始化结构体中的变量，赋予默认值
			SegParams() : isBinarization(0), thresholdFunc(0), lowThreshold(100), highThreshold(255), minCharArea(30), maxCharArea(3000), openingRadius(1.5), dilationRadius(5) {}

		}seg_params;

		struct MatchParams {
			int angleStart;			// 字符旋转角度起始值
			int angleExtent;		// 字符旋转角度范围

			double markMinScore;	// mark最小置信度
			double charMinScore;	// 字符最小置信度

			HTuple hTargetProperty; // 目标亮暗
			HTuple hPyramidLevel;	// 匹配金字塔层数
			HTuple hMetric;			// 匹配性质
			HTuple hContrast;		// 对比度
			HTuple hMinContrast;	// 最小对比度
			HTuple hCreateModelDilation;	// 创建模板时膨胀的半径

			MatchParams() : angleStart(-20), angleExtent(20), markMinScore(0.3), charMinScore(0.6), hTargetProperty("dark"), hPyramidLevel("auto"), hMetric("ignore_local_polarity"), hContrast("auto"), hMinContrast("auto"), hCreateModelDilation(3) {}

		}match_params;

		struct OtherParams {

			double erosionRadius;	// 腐蚀半径，用于计算模板区域灰度值和背景灰度值

			HTuple hTeachModelHandle;	// 教学模板句柄

			HTuple hImgWidth, hImgHeight, hProductCenterX, hProductCenterY; // 图像宽高, 模板图中IC中心点


			OtherParams() : erosionRadius(1), hTeachModelHandle(HTuple()), hImgWidth(1024), hImgHeight(1024), hProductCenterX(512), hProductCenterY(512) {}

		}other_params;

		struct OutInfos {

			HObject hCharRegions;	// 字符区域
			HObject hLocRegion;	// mark区域

		}out_infos;

		createMarkModel() {}

		~createMarkModel() {}

		// 设置分割参数，采用分别传递每个变量const引用的方式
		void setSegParams(const int& isBinarization, const int& thresholdFunc, const int& lowThreshold, const int& highThreshold, const int& minCharArea, const int& maxCharArea, const double& openingRadius, const double& dilationRadius)
		{
			seg_params.isBinarization = isBinarization;
			seg_params.thresholdFunc = thresholdFunc;
			seg_params.lowThreshold = lowThreshold;
			seg_params.highThreshold = highThreshold;
			seg_params.minCharArea = minCharArea;
			seg_params.maxCharArea = maxCharArea;
			seg_params.openingRadius = openingRadius;
			seg_params.dilationRadius = dilationRadius;
		}

		// 设置匹配参数，采用传递对应变量const引用的方式 
		void setMatchParams(const int& angleStart, const int& angleExtent, const double& markMinScore, const double& charMinScore, const HTuple& hTargetProperty, const HTuple& hPyramidLevel, const HTuple& hMetric, const HTuple& hContrast, const HTuple& hMinContrast, const HTuple& hCreateModelDilation)
		{
			match_params.angleStart = angleStart;
			match_params.angleExtent = angleExtent;
			match_params.markMinScore = markMinScore;
			match_params.charMinScore = charMinScore;
			match_params.hTargetProperty = hTargetProperty;
			match_params.hPyramidLevel = hPyramidLevel;
			match_params.hMetric = hMetric;
			match_params.hContrast = hContrast;
			match_params.hMinContrast = hMinContrast;
			match_params.hCreateModelDilation = hCreateModelDilation;
		}

		// 设置其他参数，采用传递对应变量const引用的方式
		void setOtherParams(const double& erosionRadius, const HTuple& hTeachModelHandle, const HTuple& hImgWidth, const HTuple& hImgHeight, HTuple& hProductCenterX, HTuple& hProductCenterY)
		{
			other_params.erosionRadius = erosionRadius;
			other_params.hTeachModelHandle = hTeachModelHandle;
			other_params.hImgWidth = hImgWidth;
			other_params.hImgHeight = hImgHeight;
			other_params.hProductCenterX = hProductCenterX;
			other_params.hProductCenterY = hProductCenterY;
		}

		int creatMarkModelsBaseMethod(HObject& hDetImg, HObject& hDetROI);

	};

	int createMarkModel::creatMarkModelsBaseMethod(HObject& hDetImg, HObject& hDetROI)
	{
		//功能：创建检测模型，保存模板信息
		//		两次匹配，第一次匹配用于整体的定位，第二次匹配用于获取每个字符的精确位置
		//输入：hDetImg：检测图像；hDetROI：mark区域
		//输出：匹配后的区域，模板信息会保存在other_params.hTeachModelHandle中

		//*************************************** 创建mark模板 ******************************

		// 参数处理
		HTuple hTargetProperty = match_params.hTargetProperty;

		GenEmptyObj(&out_infos.hCharRegions);
		GenEmptyObj(&out_infos.hLocRegion);

		HObject hImageChars, hCharRegions, hImageLocate, hRegionSeg, hRegionUnion, hRegionClosing, hConnectedRegions, hSelectedRegions, hBinImage, hCharRegionsSelected;

		// 获得字符区域图像
		Union1(hDetROI, &hDetROI);
		ReduceDomain(hDetImg, hDetROI, &hImageChars);
		auto temp = HTuple(seg_params.lowThreshold);
		zkhyPublicFuncHN::thresholdSeg(hImageChars, hRegionSeg, temp, HTuple(seg_params.highThreshold), hTargetProperty, seg_params.thresholdFunc);
		//筛选字符
		ClosingCircle(hRegionSeg, &hRegionClosing, 1);
		Connection(hRegionClosing, &hConnectedRegions);
		SelectShape(hConnectedRegions, &hCharRegionsSelected, "area", "and", seg_params.minCharArea, seg_params.maxCharArea);

		SortRegion(hCharRegionsSelected, &hCharRegions, "character", "true", "row");     // 按行排序
		//根据这个region，内含每个字符的区域，在测试时，for循环把区域拿出来，再求个外接矩，在外接矩上加上偏移量进行匹配字符
		SetDictObject(hCharRegions, other_params.hTeachModelHandle, "teachCharRegion");

		// 字符个数
		HObject hLocROI;
		HTuple hCharNum, hLocMarkTmp, hLocMarkIndx;
		CountObj(hCharRegions, &hCharNum);
		GenEmptyObj(&hLocROI);
		hLocMarkIndx.Clear();
		int charNum = hCharNum.I();
		double locMarkProportion = 1;
		// 随机选取部分字符作为定位字符
		if (charNum > 1 && locMarkProportion < 1.0)
		{
			//设置随机数生成器
			random_device rd;
			mt19937 gen(rd());
			uniform_real_distribution<> dis(0, 1);

			for (int i = 0; i < charNum; ++i)
			{
				if (dis(rd) > locMarkProportion)
				{
					HObject hROITmp;
					SelectObj(hCharRegions, &hROITmp, i + 1);
					Union2(hLocROI, hROITmp, &hLocROI);
				}
			}

		}
		HTuple hLocMarkNum = 0;
		CountObj(hLocROI, &hLocMarkNum);
		if (hLocMarkNum.I() == 0)
		{
			hLocROI = hCharRegions;
		}

		//记录模板得中心坐标，后面会用到
		// @gxl 获得的是模板的左上角点
		HTuple hRowTmp1, hColTmp1, hRowTmp2, hColTmp2;
		SmallestRectangle1(hLocROI, &hRowTmp1, &hColTmp1, &hRowTmp2, &hColTmp2);


		// 准备创建Mark定位模型
		HObject hRegionDilation;
		HTuple hModelID, hRowTmp, hColTmp, hAngleTmp, hScoreTmp, hMatchNum;

		// 准备测试图像
		if (seg_params.isBinarization == 0)
		{    // 不使用二值化方法
			DilationCircle(hLocROI, &hRegionDilation, seg_params.dilationRadius);
			ReduceDomain(hDetImg, hRegionDilation, &hImageLocate);
		}
		else
		{    // 使用二值化方法
			RegionToBin(hLocROI, &hImageLocate, 255, 0, other_params.hImgWidth, other_params.hImgHeight);
		}
		// opencv 角度转弧度
		double angleStart = match_params.angleStart * myPi / 180.0;
		double angleExtent = match_params.angleExtent * myPi / 180.0;
		//创建匹配模型
		CreateShapeModel(hImageLocate, match_params.hPyramidLevel, angleStart, angleExtent, "auto", "auto", match_params.hMetric, match_params.hContrast, match_params.hMinContrast, &hModelID);

		//创建模板后再匹配一次
		FindShapeModel(hImageLocate, hModelID, angleStart, angleExtent,
			match_params.markMinScore, 1, 0.5, "least_squares", 0, 0.7, &hRowTmp, &hColTmp,
			&hAngleTmp, &hScoreTmp);
		hMatchNum = hScoreTmp.TupleLength();

		if (0 != (hMatchNum == 1))
		{
			SetDictTuple(other_params.hTeachModelHandle, "teachMarkShapeModel", hModelID);

			// 输出模板区域
			HObject hModelContourTmp, hModelContourTmpAffineTrans;
			HTuple hHomMat2DTmp;
			GetShapeModelContours(&hModelContourTmp, hModelID, 1);
			VectorAngleToRigid(0, 0, 0, hRowTmp, hColTmp, hAngleTmp, &hHomMat2DTmp);
			AffineTransContourXld(hModelContourTmp, &hModelContourTmpAffineTrans, hHomMat2DTmp);
			GenRegionContourXld(hModelContourTmpAffineTrans, &out_infos.hLocRegion, "filled");
		}
		else
		{
			ClearShapeModel(hModelID);
			return 1;// 创建mark模板失败
		}
		//ClearShapeModel(hModelID);

		//******************************************** 保存Mark定位相关的信息 ******************************

		HTuple hTeachMarkCenterDX, hTeachMarkCenterDY;
		hTeachMarkCenterDX = other_params.hProductCenterX - hColTmp;
		hTeachMarkCenterDY = other_params.hProductCenterY - hRowTmp;

		SetDictTuple(other_params.hTeachModelHandle, "teachBaseAngle", hAngleTmp);
		SetDictTuple(other_params.hTeachModelHandle, "mark_center_x", hColTmp);
		SetDictTuple(other_params.hTeachModelHandle, "mark_center_y", hRowTmp);
		SetDictTuple(other_params.hTeachModelHandle, "teach_mark_d_x", hTeachMarkCenterDX);
		SetDictTuple(other_params.hTeachModelHandle, "teach_mark_d_y", hTeachMarkCenterDY);

		//*********************************** 为每个字符创建模板 ******************************

		//******************************* 对每个字符创建匹配模型 **********
		HTuple hRowTmp3, hColTmp3, hRowTmp4, hColTmp4, hCharShapeModel;
		SmallestRectangle1(hCharRegions, &hRowTmp3, &hColTmp3, &hRowTmp4, &hColTmp4);
		hRowTmp3 -= match_params.hCreateModelDilation;
		hColTmp3 -= match_params.hCreateModelDilation;
		hRowTmp4 += match_params.hCreateModelDilation;
		hColTmp4 += match_params.hCreateModelDilation;
		hCharShapeModel = HTuple();
		SetDictTuple(other_params.hTeachModelHandle, "charShapeModelNum", hCharNum);
		{
			HTuple hRowTmp5, hColTmp5, hRowTmp6, hColTmp6;
			HObject hObjectSelected, hRectangle, hImgReduced;
			hModelID.Clear();
			int idx = 1;
			for (; idx <= charNum; ++idx)
			{
				SelectObj(hCharRegions, &hObjectSelected, idx);
				SmallestRectangle1(hObjectSelected, &hRowTmp5, &hColTmp5, &hRowTmp6, &hColTmp6);
				// 矩形尺寸
				GenRectangle1(&hRectangle, hRowTmp5 - 5, hColTmp5 - 5, hRowTmp6 + 10, hColTmp6 + 10);

				// 准备创建模型
				HObject hImageCharMatch = hDetImg;
				if (seg_params.isBinarization != 0)
				{    // 使用二值化方法
					RegionToBin(hObjectSelected, &hImageCharMatch, 255, 0, other_params.hImgWidth, other_params.hImgHeight);
				}
				else
				{    // 不使用二值化方法
					HObject hRegionTmp;
					DilationCircle(hObjectSelected, &hRegionTmp, seg_params.dilationRadius);
					ReduceDomain(hImageCharMatch, hRegionTmp, &hImageCharMatch);
				}
				ReduceDomain(hImageCharMatch, hRectangle, &hImgReduced);
				CropDomain(hImgReduced, &hImageCharMatch);

				CreateShapeModel(hImageCharMatch, match_params.hPyramidLevel, match_params.angleStart, match_params.angleExtent, "auto", "auto", match_params.hMetric, match_params.hContrast, match_params.hMinContrast, &hModelID);

				// @gxl 二值图建模，原图查找
				//ReduceDomain(hDetROI, hRectangle, &ho_ImageReduced);
				FindShapeModel(hImgReduced, hModelID, match_params.angleStart, match_params.angleExtent, match_params.charMinScore, 1, 0.5, "least_squares", 0, 0.7, &hRowTmp, &hColTmp,
					&hAngleTmp, &hScoreTmp);
				HTuple hIdxTmp = idx;
				// @gxl score
				if (0 != (hScoreTmp > match_params.charMinScore))
				{
					SetDictTuple(other_params.hTeachModelHandle, ("charShapeModel_" + to_string(idx)).c_str(), hModelID);

					// 存储区域
					HObject hModelContour, hModelRegion, hRegionTmp, hModelRegionTmp;
					HTuple hContourNum;
					GetShapeModelContours(&hModelContour, hModelID, 1);
					GenRegionContourXld(hModelContour, &hModelRegion, "filled");
					CountObj(hModelRegion, &hContourNum);
					GenEmptyRegion(&hRegionTmp);
					int contourNum = hContourNum.I();
					// 出现内部有空洞的区域，需要单独做处理，如0 4 6 8 9，拿到外面的轮廓
					if (contourNum > 1)
					{
						HObject hRegionTmp1;
						HTuple hAreaTmp, hIndexSorted;
						AreaCenter(hModelRegion, &hAreaTmp, nullptr, nullptr);
						TupleSortIndex(hAreaTmp, &hIndexSorted);
						SelectObj(hModelRegion, &hRegionTmp, hIndexSorted[contourNum - 1] + 1);

						int contourIdx = contourNum - 2;
						for (; contourIdx >= 0; --contourIdx)
						{
							SelectObj(hModelRegion, &hRegionTmp1, hIndexSorted[contourIdx] + 1);
							Difference(hRegionTmp, hRegionTmp1, &hRegionTmp);
						}
					}
					else
					{
						Union2(hRegionTmp, hModelRegion, &hRegionTmp);
					}

					// 平滑轮廓
					OpeningCircle(hRegionTmp, &hRegionTmp, seg_params.openingRadius);

					SetDictObject(hRegionTmp, other_params.hTeachModelHandle, ("charShapeModelRegion_" + to_string(idx)).c_str());

					// 记录模板字符对比度/灰度值
					HObject hCharForegroundRegionTmp, hCharBackgroundRegionTmp, hCharRectangleTmp;
					HTuple charRectRow1, charRectCol1, charRectRow2, charRectCol2, hCharForegroundMean, hCharBackgroundMean, hDeviation;
					SmallestRectangle1(hRegionTmp, &charRectRow1, &charRectCol1, &charRectRow2, &charRectCol2);
					GenRectangle1(&hCharRectangleTmp, charRectRow1, charRectCol1, charRectRow2, charRectCol2);

					hCharForegroundRegionTmp = hRegionTmp;
					Difference(hCharRectangleTmp, hCharForegroundRegionTmp, &hCharBackgroundRegionTmp);
					// 构建坐标变换关系，将区域映射回原图
					HTuple homMat2DTmp;
					VectorAngleToRigid(0, 0, 0, hRowTmp, hColTmp, hAngleTmp, &homMat2DTmp);
					AffineTransRegion(hCharForegroundRegionTmp, &hCharForegroundRegionTmp, homMat2DTmp, "nearest_neighbor");
					AffineTransRegion(hCharBackgroundRegionTmp, &hCharBackgroundRegionTmp, homMat2DTmp, "nearest_neighbor");
					// 腐蚀
					ErosionCircle(hCharForegroundRegionTmp, &hCharForegroundRegionTmp, other_params.erosionRadius);
					ErosionCircle(hCharBackgroundRegionTmp, &hCharBackgroundRegionTmp, other_params.erosionRadius);

					Intensity(hCharForegroundRegionTmp, hDetImg, &hCharForegroundMean, &hDeviation);
					Intensity(hCharBackgroundRegionTmp, hDetImg, &hCharBackgroundMean, &hDeviation);

					ConcatObj(out_infos.hCharRegions, hCharForegroundRegionTmp, &out_infos.hCharRegions);

					SetDictTuple(other_params.hTeachModelHandle, ("charContrast_" + to_string(idx)).c_str(), (hCharForegroundMean - hCharBackgroundMean).TupleAbs());
				}
				else
				{
					{
						int end = idx;
						for (int clearIdx = 1; clearIdx < end; ++clearIdx)
						{
							GetDictTuple(other_params.hTeachModelHandle, "charShapeModel_" + clearIdx, &hModelID);
							ClearShapeModel(hModelID);
						}
					}
					ClearShapeModel(hModelID);
					return 1;// 创建mark模板失败
				}
				//ClearShapeModel(hModelID);
			}
		}

		return 0;

	}

	// 计算仿射变换矩阵，适用于需要对图像进行旋转变换的场景，保留原始图像全部内容
	void computeRotateHomMat2D(const double& width, const double& height, const double& angle, double& newWidth, double& newHeight, HTuple& hHomMat2D)
	{
		newHeight = abs(width * cos(angle)) + abs(height * sin(angle));
		newWidth = abs(width * sin(angle)) + abs(height * cos(angle));

		VectorAngleToRigid(height / 2.0, width / 2.0, 0, newWidth / 2.0, newHeight / 2.0, angle, &hHomMat2D);
}

	inline int computeAngle(const Point2f& point1, const Point2f& point2, double& angle)
	{
#if 0
		if (point2.x > point1.x)           // 确保angle范围为(-pi/2,pi/2)
			angle = atan2((point1.y - point2.y), (point2.x - point1.x));
		else
			angle = atan2((point1.y - point2.y), (point1.x - point2.x));
#endif // 

		angle = atan((point1.y - point2.y) / (point1.x - point2.x));
		return 0;
	}

	int Contours2Region(vector<cv::Point2f>& input, HObject* ho_region)
	{
		GenEmptyObj(ho_region);
		//HObject _ho_region;
		//GenEmptyObj(&_ho_region);
		vector<cv::Point2f> inputPoints = input;
		int pointsNum = inputPoints.size();
		try {
			if (inputPoints[0].x != inputPoints[pointsNum - 1].x || inputPoints[0].y != inputPoints[pointsNum - 1].y)
			{
				inputPoints.emplace_back(inputPoints[0]);
			}
			zkhyPublicFuncHN::genRegionFromContour(inputPoints, ho_region);
		}
		catch (...)
		{
			inputPoints.clear();
			inputPoints.shrink_to_fit();
			return 2;
		}
		inputPoints.clear();
		inputPoints.shrink_to_fit();
		return 0;
	}

	//轮廓生成H区域,函数重载
	int Contours2Region(vector<vector<cv::Point2f>>& input, HObject* ho_region)
	{
		GenEmptyObj(ho_region);
		try {
			zkhyPublicFuncHN::genRegionFromContour(input, ho_region);
			//Union1(*ho_region, ho_region);
		}
		catch (...)
		{
			return 2;
		}
		return 0;
	}

	int computeMarkMargin(HObject& markRegion, const vector<Point2f>& ICContour, HTuple& hMarkMargin)
	{
		// 记录字符到产品边缘的距离
		HTuple hRow1Tmp, hCol1Tmp, hRow2Tmp, hCol2Tmp, hMarkMarginTop, hMarkMarginBottom, hMarkMarginLeft, hMarkMarginRight;

		SmallestRectangle1(markRegion, &hRow1Tmp, &hCol1Tmp, &hRow2Tmp, &hCol2Tmp);
		// 到产品边界的距离
		// 外接矩形到产品四条边的距离
		DistancePl(hRow1Tmp, hCol1Tmp, ICContour[0].y, ICContour[0].x, ICContour[3].y, ICContour[3].x, &hMarkMarginTop);
		DistancePl(hRow2Tmp, hCol2Tmp, ICContour[1].y, ICContour[1].x, ICContour[2].y, ICContour[2].x, &hMarkMarginBottom);
		DistancePl(hRow1Tmp, hCol1Tmp, ICContour[0].y, ICContour[0].x, ICContour[1].y, ICContour[1].x, &hMarkMarginLeft);
		DistancePl(hRow2Tmp, hCol2Tmp, ICContour[2].y, ICContour[2].x, ICContour[3].y, ICContour[3].x, &hMarkMarginRight);

		hMarkMargin.Clear();
		hMarkMargin[0] = hMarkMarginTop;
		hMarkMargin[1] = hMarkMarginBottom;
		hMarkMargin[2] = hMarkMarginLeft;
		hMarkMargin[3] = hMarkMarginRight;

		return 0;
	}


	// ================================================== Mark   ================================================ //
	int realMarkTeach(const MarkTeachInput& input, MarkTeachOutput& output)
	{
		// set environment
		SetHcppInterfaceStringEncodingIsUtf8(false);
		SetSystem("clip_region", "false");

		// 解析输入参数
		HObject hSrcImg = input.srcImg;
		std::vector<cv::Point2f> productEdges = input.edgeAlign;
		std::vector<std::vector<cv::Point2f>> markROIs = input.markRoi;
		string dictSavePath = input.dictSavePath;

		// 判断输入参数是否异常
		if (zkhyPublicFuncHN::isHObjectEmpty(hSrcImg))
			return 1;
		if (productEdges.size() == 0)
			return 5;
		if (markROIs.size() == 0)
			return 6;
		if (dictSavePath.empty())
			return 6;

		// 参数解析
		int lowThreshold, highThreshold, minCharArea, maxCharArea, angleStart, angleExtent, iThresholdFunc, iTargetProperty, isBinarization, iSpecDetectType, iPyramidLevel, iCharAngle;
		AlgParaMarkTeach* pParams = (AlgParaMarkTeach*)&input.algPara;
		lowThreshold = pParams->i_params.block.lowThreshold;
		highThreshold = pParams->i_params.block.highThreshold;
		minCharArea = pParams->i_params.block.minCharArea;
		maxCharArea = pParams->i_params.block.maxCharArea;
		angleStart = pParams->i_params.block.angleStart;
		angleExtent = pParams->i_params.block.angleExtent;

		iThresholdFunc = pParams->i_params.block.iThresholdFunc;
		iTargetProperty = pParams->i_params.block.iTargetProperty;
		isBinarization = pParams->i_params.block.isBinarization;
		iSpecDetectType = pParams->i_params.block.iSpecDetectType;
		iPyramidLevel = pParams->i_params.block.iPyramidLevel;
		iCharAngle = pParams->i_params.block.iAngle;

		double minMarkMatchScore, minCharMatchScore;
		minMarkMatchScore = pParams->d_params.block.minMarkMatchScore;
		minCharMatchScore = pParams->d_params.block.minCharMatchScore;

		// 将输入的轮廓转换为区域
		HObject hMarkROIs, hProductRegion;
		double productCenterX, productCenterY;
		GenEmptyObj(&hMarkROIs);
		GenEmptyObj(&hProductRegion);

		vector<vector<cv::Point2f>> markRois = input.markRoi;

		//mark ROI
		int _status = Contours2Region(markRois, &hMarkROIs);
		if (_status)
			return 6;

		// 根据edgeAlign输入，获取产品区域
		std::vector<cv::Point2f> productMarkPoint(4);
		if (productEdges.size() < 5)
			return 4;//edgeAlign输入数据异常
		productMarkPoint[0] = productEdges[0];
		productMarkPoint[1] = productEdges[1];
		productMarkPoint[2] = productEdges[2];
		productMarkPoint[3] = productEdges[3];
		productCenterX = productEdges[4].x;
		productCenterY = productEdges[4].y;
		_status = Contours2Region(productMarkPoint, &hProductRegion);
		if (_status)
			return 6;

		// 设置超参数
		bool isAdjustProduct = true;		// 是否将产品角度转为水平
		double dDilationRadius, dSmoothFactor, dCreateModelDilation, dOpeningRadius, dErosionRadius;
		dDilationRadius = 5;
		dSmoothFactor = 0.5;
		dCreateModelDilation = 3;
		dOpeningRadius = 2;		// 用于平滑轮廓
		dErosionRadius = 1;		// 腐蚀，用于计算对比度

		// 设置匹配相关参数
		HTuple hTargetProperty = "dark";
		if (iTargetProperty == 0)     // 动态阈值参数，'light' or 'dark'
			hTargetProperty = "light";

		HTuple hNumLevels, hMetric, hContrast, hMinContrast, hCreateModelDilation;
		if (iPyramidLevel == 0)
			hNumLevels = "auto";
		else
			hNumLevels = iPyramidLevel;
		hMetric = "ignore_local_polarity";
		hContrast = "auto";
		hMinContrast = "auto";


		// 预处理（PreProcess），为创建模型做好准备
		// 获得图像宽高
		HTuple hWidth, hHeight;
		GetImageSize(hSrcImg, &hWidth, &hHeight);
		double dCharAngleArc, dProductAngle;
		dCharAngleArc = iCharAngle * PI / 180.0;		// 转换为弧度
		dProductAngle = dCharAngleArc;

		if (isAdjustProduct)
		{
			// 计算产品旋转角度
			computeAngle(productMarkPoint[0], productMarkPoint[3], dProductAngle);
			dProductAngle = dCharAngleArc - dProductAngle;
		}

		// 生成坐标变换矩阵
		HTuple hProductAdjust, hProductAdjustInvert;
		double imgNewHeight, imgNewWidth;
		computeRotateHomMat2D(hWidth.D(), hHeight.D(), dProductAngle, imgNewHeight, imgNewWidth, hProductAdjust);
		HomMat2dInvert(hProductAdjust, &hProductAdjustInvert);
		// 将产品转正

		HObject hDetImg, hImgProduct;
		HTuple hProductCenterX, hProductCenterY;
		AffineTransImage(hSrcImg, &hDetImg, hProductAdjust, "bilinear", "true");
		// 将ROI进行旋转
		AffineTransRegion(hMarkROIs, &hMarkROIs, hProductAdjust, "nearest_neighbor");
		AffineTransRegion(hProductRegion, &hProductRegion, hProductAdjust, "nearest_neighbor");
		// 旋转产品中心点
		AffineTransPixel(hProductAdjust, productCenterY, productCenterX, &hProductCenterY, &hProductCenterX);

		// 将产品区域抠出
		ReduceDomain(hDetImg, hProductRegion, &hImgProduct);

		//对示教图做平滑，减少部分噪声
		SmoothImage(hImgProduct, &hDetImg, "deriche2", dSmoothFactor);

		// 获取待测图宽高
		GetImageSize(hDetImg, &hWidth, &hHeight);

		// 生成dict句柄
		HTuple hDictHandle;
		CreateDict(&hDictHandle);

		// 创建匹配模型
		createMarkModel markTeachModel;
		// 参数赋值
		markTeachModel.setSegParams(isBinarization, iThresholdFunc, lowThreshold, highThreshold, minCharArea, maxCharArea, dOpeningRadius, dDilationRadius);
		markTeachModel.setMatchParams(angleStart, angleExtent, minMarkMatchScore, minCharMatchScore, hTargetProperty, hNumLevels, hMetric, hContrast, hMinContrast, dCreateModelDilation);
		markTeachModel.setOtherParams(dErosionRadius, hDictHandle, hWidth, hHeight, hProductCenterX, hProductCenterY);
		// 执行建模
		markTeachModel.creatMarkModelsBaseMethod(hDetImg, hMarkROIs);

		// 记录字符到产品边缘的距离
		HTuple hMarkMargin;
		HObject hCharsRegion, hCharRegionUnion;
		// 将检测到的字符区域重新映射到图像上
		AffineTransRegion(markTeachModel.out_infos.hCharRegions, &hCharsRegion, hProductAdjustInvert, "nearest_neighbor");
		Union1(hCharsRegion, &hCharRegionUnion);

		computeMarkMargin(hCharRegionUnion, productMarkPoint, hMarkMargin);


		// 将需要的信息写入字典
		SetDictTuple(hDictHandle, "Width", hWidth);
		SetDictTuple(hDictHandle, "Height", hHeight);
		SetDictTuple(hDictHandle, "isAdjustProduct", isAdjustProduct);		// 是否将产品旋转至水平
		SetDictTuple(hDictHandle, "templateProductAngle", dCharAngleArc);			// 旋转角度
		SetDictObject(hMarkROIs, hDictHandle, "markDetROI");		// 检测ROI
		SetDictTuple(hDictHandle, "markMargin", hMarkMargin);

		try
		{	// 保存 hDict
			WriteDict(hDictHandle, dictSavePath.c_str(), HTuple(), HTuple());
		}
		catch (HException& except)
		{
			string errorMsg = except.ErrorMessage().Text();
			return 7;
		}

		// 输出轮廓
		zkhyPublicFuncHN::genContourFromRegion(hCharsRegion, output.charTeachCnts);

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------- MarkTeach start  ------------------------------------------//
	//函数说明：mark建模
	//MarkDetectInput &input;				输入：模板图像，建模保存路径，输入ROI，算法参数
	//MarkDetectOutput &output;				输出：模板图建模的每个字符轮廓
	int MarkTeach(const MarkTeachInput& input, MarkTeachOutput& output)
	{

		try
		{
			return realMarkTeach(input, output);
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 3; //未知错误
		}

	}
}
