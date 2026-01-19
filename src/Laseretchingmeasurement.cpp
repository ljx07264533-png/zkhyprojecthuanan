//20240425 新增激光刻蚀测量（Laseretchingmeasurement）算子。	   --edited by Wan MaoJia

#include "alglibrary/zkhyProjectHuaNan/Laseretchingmeasurement.h"
#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

//-------------------- realLaseretchingmeasurement Begin--------------------//
int realLaseretchingmeasurement(const LaseretchingmeasurementInput& input, LaseretchingmeasurementOutput& output);

//激光刻蚀测量算子算法类
class Laseretchingmeasurement : public zkhyPublicClass::AlgorithmBase
{
public:
	Laseretchingmeasurement();
	~Laseretchingmeasurement() {};
	Laseretchingmeasurement(const LaseretchingmeasurementInput& input);

	int checkParams();		// 检查输入参数是否越界

	int measureDistance();	// 测量距离
	int execute();			// 执行算法

	void getResult(LaseretchingmeasurementOutput& output);		// 获取测量结果

protected:
	HObject hDetImg, hDetRegion, hBaseLineContour;
	HTuple hImgWidth, hImgHeight, hHomMat2D, hMeasureRows, hMeasureCols, hMeasureAmplitudes, hMeasureDistances, hImgWidthTrans, hImgHeightTrans;

	int lineOffsetDirection, lineHeight, Upwarddistance, Downwarddistance;
	double smoothFactor, edgeStrength;

	LineStruct baseLine;
	LaseretchingmeasurementOutput measureResult;
};

// 默认构造函数
Laseretchingmeasurement::Laseretchingmeasurement()
{
	lineOffsetDirection = 0;
	lineHeight = 200;
	Upwarddistance = 100;
	Downwarddistance = 350;
	smoothFactor = 1;
	edgeStrength = 30;

	GenEmptyObj(&hDetImg);
	GenEmptyObj(&hDetRegion);
	GenEmptyObj(&hBaseLineContour);
	hImgWidth.Clear();
	hImgHeight.Clear();
	hHomMat2D.Clear();
	hMeasureRows.Clear();
	hMeasureCols.Clear();
	hMeasureAmplitudes.Clear();
	hMeasureDistances.Clear();
	hImgHeightTrans.Clear();
	hImgWidthTrans.Clear();

	baseLine.pt1 = cv::Point2f(0, 0);
	baseLine.pt2 = cv::Point2f(0, 0);
}

// 构造函数
Laseretchingmeasurement::Laseretchingmeasurement(const LaseretchingmeasurementInput& input)
{
	AlgParaLaseretchingmeasurement* algParams = (AlgParaLaseretchingmeasurement*)&input.algPara;

	lineOffsetDirection = algParams->i_params.block.iLineOffsetDirection;
	lineHeight = algParams->i_params.block.iLineHeight;
	Upwarddistance = algParams->i_params.block.Upwarddistance;
	Downwarddistance = algParams->i_params.block.Downwarddistance;
	smoothFactor = algParams->d_params.block.dSmoothFactor;
	edgeStrength = algParams->d_params.block.dEdgeStrength;

	hDetImg = input.inputImage;
	GetImageSize(hDetImg, &hImgWidth, &hImgHeight);

	hDetRegion = input.hDetRegion;


	GenEmptyObj(&hBaseLineContour);
	hHomMat2D.Clear();
	hMeasureRows.Clear();
	hMeasureCols.Clear();
	hMeasureAmplitudes.Clear();
	hMeasureDistances.Clear();
	hImgHeightTrans.Clear();
	hImgWidthTrans.Clear();
}

// 检查输入参数是否越界
int Laseretchingmeasurement::checkParams()
{
	if (lineOffsetDirection < 0 || lineOffsetDirection > 1)
	{
		return 3;
	}
	if (lineHeight < 0)
	{
		return 4;
	}

	if (smoothFactor < 0)
	{
		return 5;
	}
	if (edgeStrength < 0)
	{
		return 6;
	}

	return 0;
}

// 测量距离
int Laseretchingmeasurement::measureDistance()
{
	// 计算扫描矩形
	// 计算测量矩形起点和终点
	// 列坐标为检测区域的左右两端或者图像宽度

	double offsetDistance = lineHeight * 0.5;

	if (lineOffsetDirection == 0)
	{
		offsetDistance = -offsetDistance;
	}

	HObject ConnectedRegions;
	HTuple Area, Row, Column;

	Connection(hDetRegion, &ConnectedRegions);
	AreaCenter(ConnectedRegions, &Area, &Row, &Column);

	zkhyPublicClass::measure1D measureTool;

	measureTool.setMeasureParams(smoothFactor, edgeStrength, edgePolarity::allPolarity, edgeSelect::allPoints);
	measureTool.setMeasureRect(Row[0] - Upwarddistance, Column[0], Row[0] + Downwarddistance, Column[0], lineHeight);
	//单次生成句柄
	double radius;
	radius = (Downwarddistance - Upwarddistance) / 2;
	measureTool.genMeasureHandle(hDetImg);

	for (int i = 0; i < Row.TupleLength(); i++)
	{
		//移动句柄，代替重复创建
		measureTool.transMeasureHandle(Row[i] + radius, Column[i]);

		measureTool.execute(hDetImg);

		measureTool.getResult(hMeasureRows, hMeasureCols, hMeasureAmplitudes, hMeasureDistances);

		// 结果存储
		// 申请空间
		MeasureInfoStruct measureInfo;
		HTuple hRow1, hCol1, hRow2, hCol2;
		hRow1 = hMeasureRows[0];
		hCol1 = hMeasureCols[0] - lineHeight * 0.32;
		hRow2 = hMeasureRows[0];
		hCol2 = hMeasureCols[0] + lineHeight * 0.32;
		std::vector<cv::Point2f> edge(4);
		edge[0].x = hCol1.D();
		edge[0].y = hRow1.D();
		edge[1].x = hCol2.D();
		edge[1].y = hRow2.D();
		edge[2] = edge[1];
		edge[3] = edge[0];
		measureInfo.basicInfo.contours.push_back(edge);
		hRow1 = hMeasureRows[1];
		hCol1 = hMeasureCols[1] - lineHeight * 0.32;
		hRow2 = hMeasureRows[1];
		hCol2 = hMeasureCols[1] + lineHeight * 0.32;
		edge[0].x = hCol1.D();
		edge[0].y = hRow1.D();
		edge[1].x = hCol2.D();
		edge[1].y = hRow2.D();
		edge[2] = edge[1];
		edge[3] = edge[0];
		measureInfo.basicInfo.contours.push_back(edge);
		measureInfo.basicInfo.height = (hMeasureRows[1] - hMeasureRows[0]).TupleAbs();
		measureResult.measureinfos.push_back(measureInfo);

		// 调试，NDEBUG在gcc和clang中生效，_DEBUG在vs中生效
#if not defined(NDEBUG) || defined(_DEBUG)
		HObject hMeasureRectDisp, hMeasureOutDisp;
		measureTool.getRes2Disp(hMeasureRectDisp, hMeasureOutDisp);
		Sleep(5);
#endif
	};
	return 0;
}

// 执行算法
int Laseretchingmeasurement::execute()
{
	// 检查输入参数是否越界
	int ret = checkParams();
	if (ret != 0)
	{
		return ret;
	}


	// 测量距离
	try
	{
		measureDistance();
	}
	catch (HException& except)
	{
		std::string errorMsg = except.ErrorMessage().Text();
		LOG.log(errorMsg, alglib::core::err);

		return 7; //测量距离异常
	}

	return 0;
}


// 获取测量结果
void Laseretchingmeasurement::getResult(LaseretchingmeasurementOutput& output)
{
	output = measureResult;
}


//激光刻蚀测量算法实现
int realLaseretchingmeasurement(const LaseretchingmeasurementInput& input, LaseretchingmeasurementOutput& output)
{
	if (zkhyPublicFuncHN::isHObjectEmpty(input.inputImage))
		return 8;

	// 解析参数
	AlgParaLaseretchingmeasurement* algParams = (AlgParaLaseretchingmeasurement*)&input.algPara;
	double timeOutThreshold = algParams->d_params.block.dTimeOutThreshold;

	// 开始计时
	DWORD startTime = GetTickCount64();

	Laseretchingmeasurement cLaseretchingmeasurement(input);
	int ret = cLaseretchingmeasurement.execute();
	if (ret != 0)
		return ret;

	cLaseretchingmeasurement.getResult(output);

	// 计算算法耗时
	DWORD costTime = GetTickCount64() - startTime;
	if (costTime > timeOutThreshold)
		return 2;                   // 算法超时

	return 0;
};


//----------------------------- Laseretchingmeasurement 调用接口-----------------------------//
namespace alglib::ops::zkhyProHN::alg {
	int Laseretchingmeasurement(const LaseretchingmeasurementInput& input, LaseretchingmeasurementOutput& output)
	{
		try
		{
			return realLaseretchingmeasurement(input, output);
		}
		catch (...)
		{
			return 1; //算法未知异常
		}
		return 0;
	}
}

//-------------------- Laseretchingmeasurement End--------------------//