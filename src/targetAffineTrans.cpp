//20240411 1. 仿射变换算子，对输入的图片、区域、直线进行放射变换(待完善)。 -edited by Guo Xulong
//20240421 1. 仿射变换（targetAffineTrans）算子：完善根据输入直线对图像、区域、直线以及轮廓进行变换的功能。 -edited by Guo Xulong
#include "alglibrary/zkhyProjectHuaNan/targetAffineTrans.h"
#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/alglibMisc/HObject2Mat.h"
#include "alglibrary/alglibMisc/Mat2HObjectColorSpeedUp.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

	// 定义类
	class AffineTransTarget :zkhyPublicClass::AlgorithmBase
	{
	public:
		// 构造函数，初始化变量
		AffineTransTarget() {}

		// 析构函数
		~AffineTransTarget() {}

		// @brief 设置变换参数
		// @param transCoord 坐标系变换参数
		int setTransPara(const CSYSTransPara& transCoord, const int& transMode, const bool isTransV2H = false);
		// @brief 设置变换参数（重载，根据直线计算）
		// @param line 直线
		int setTransPara(const LineStruct& line, const int& transMode, const bool isTransV2H = false);
		// @brief 设置图像大小
		// @param imgInput 输入图像
		int setImgSize(const cv::Mat& imgInput);
		// @brief 设置图像大小
		// @param hInput 输入H对象
		int setImgSize(const HObject& hInput);
		// @brief 设置图像大小，重载，不以图像中心为旋转中心
		int setImgSize();


		// @brief 构建Opencv 仿射变换矩阵
		int createTransMatrixCV();
		// @brief 构建Halcon 仿射变换矩阵
		int createTransMatrixHalcon();

		// @brief 图像变换
		// @param imgInput 输入图像
		// @param imgOutput 输出图像
		int transImageCV(const cv::Mat& imgInput, cv::Mat& imgOutput);
		// @brief H对象变换
		// @param hInput 输入H对象
		// @param hOutput 输出H对象
		int transHObject(const HObject& hInput, HObject& hOutput);
		// @brief 直线变换
		// @param lineInput 输入直线
		// @param lineOutput 输出直线
		int transLine(const LineStruct& lineInput, LineStruct& lineOutput);

	private:
		CSYSTransPara transPara;		// 变换参数
		cv::Matx32d transMatrix;		// 变换矩阵
		HTuple hTransMatrix;			// Halcon变换矩阵

		int imgWidth, imgHeight;		// 图像大小
	};

	// 函数实现
	// 设置变换参数
	int AffineTransTarget::setTransPara(const CSYSTransPara& transCoord, const int& transMode, const bool isTransV2H)
	{
		transPara = transCoord;
		return 0;
	}

	// 设置变换参数（重载，根据直线计算）
	int AffineTransTarget::setTransPara(const LineStruct& line, const int& transMode, const bool isTransV2H)
	{
#if 0 
		// 使用直线构建仿射变换矩阵，已直线中心点为基准，将目标旋转至水平
		CSYSTransPara locStart, locEnd;
		// 计算角度
		float angle = atan2(line.pt2.y - line.pt1.y, line.pt2.x - line.pt1.x);
		// 计算中心点
		float centerX = (line.pt1.x + line.pt2.x) / 2;
		float centerY = (line.pt1.y + line.pt2.y) / 2;

		locStart.angle = angle;
		locStart.translationValue.x = centerX;
		locStart.translationValue.y = centerY;

		locEnd.angle = 0;
		locEnd.translationValue.x = centerX;
		locEnd.translationValue.y = centerY;

		ComputeTemplate2SampleCSYSparaInput transParaInput;
		transParaInput.sampleImgCSYSpara = locStart;
		transParaInput.templateImgCSYSpara = locEnd;
		int rlt = constructCSYS::computeTemplate2SampleCSYSpara(transParaInput, transPara);

		if (rlt != 0)
			return 1;		// 根据直线计算坐标变换关系失败
#endif

		LineStruct lineTmp;

		if (line.pt1.y > line.pt2.y)
		{
			lineTmp.pt1 = line.pt2;
			lineTmp.pt2 = line.pt1;
		}
		else
		{
			lineTmp.pt1 = line.pt1;
			lineTmp.pt2 = line.pt2;
		}


		double angle2zero = 0;
		if (isTransV2H)
			angle2zero = PI;

		//double lineAngle = (atan2(lineTmp.pt2.y - lineTmp.pt1.y, lineTmp.pt2.x - lineTmp.pt1.x) - angle2zero);
		double lineAngle = (atan((lineTmp.pt2.y - lineTmp.pt1.y) / (lineTmp.pt2.x - lineTmp.pt1.x)) - angle2zero);

		// 计算中心点
		float centerX1, centerY1, centerX2, centerY2;
		switch (transMode) {
		case 0:
		{
			if (imgWidth > 0 && imgHeight > 0)
			{
				centerX1 = imgWidth / 2;
				centerY1 = imgHeight / 2;
				centerX2 = centerY1;
				centerY2 = centerX1;
			}
			else
			{	// 未输入图像，以直线中心为旋转中心
				centerX1 = (lineTmp.pt1.x + lineTmp.pt2.x) / 2;
				centerY1 = (lineTmp.pt1.y + lineTmp.pt2.y) / 2;
				centerX2 = centerY1;
				centerY2 = centerX1;
			}
			break;
		}
		case 1:
		{
			if (isTransV2H == 0)
			{
				centerX1 = imgWidth / 2;
				centerY1 = (lineTmp.pt1.y + lineTmp.pt2.y) / 2;
				centerX2 = imgWidth / 2;
				centerY2 = imgHeight / 2;

			}
			else
			{	// TODO 需要测试
				centerX1 = (lineTmp.pt1.x + lineTmp.pt2.x) / 2;
				centerY1 = imgHeight / 2;
				centerX2 = imgHeight / 2;
				centerY2 = imgWidth / 2;
			}
			break;
		}
		default:
			break;
		}


		// 注意旋转角度
		VectorAngleToRigid(centerX1, centerY1, 0, centerX2, centerY2, lineAngle, &hTransMatrix);


		return 0;
	}

	// 设置图像大小
	int AffineTransTarget::setImgSize(const cv::Mat& imgInput)
	{
		imgWidth = imgInput.cols;
		imgHeight = imgInput.rows;
		return 0;
	}

	// 设置图像大小
	int AffineTransTarget::setImgSize(const HObject& hInput)
	{
		HTuple hvWidth, hvHeight;
		GetImageSize(hInput, &hvWidth, &hvHeight);
		imgWidth = hvWidth[0].D();
		imgHeight = hvHeight[0].D();
		return 0;
	}

	// 设置图像大小，重载，不以图像中心为旋转中心
	int AffineTransTarget::setImgSize()
	{
		imgWidth = -1024;
		imgHeight = -1024;
		return 0;
	}

	// 构建Opencv 仿射变换矩阵
	int AffineTransTarget::createTransMatrixCV()
	{
		//transMatrix = cv::getAffineTransform(cv::Point2f(0, 0), transPara.translationValue);
		return 0;
	}

	// 构建Halcon 仿射变换矩阵
	int AffineTransTarget::createTransMatrixHalcon()
	{
		return 0;
	}

	// 图像变换
	int AffineTransTarget::transImageCV(const cv::Mat& imgInput, cv::Mat& imgOutput)
	{
		cv::Mat imgTmp = imgInput.clone();
		std::vector<cv::Mat> channels;
		HObject hImgTmp, hImgOutput;
		hImgTmp = alglib::ops::misc::alg::Mat2HObjectColorSpeedUp(imgTmp, channels);
		transHObject(hImgTmp, hImgOutput);

		imgOutput = alglib::ops::misc::alg::HObject2Mat(hImgOutput).clone();

		return 0;
	}

	// H对象变换
	int AffineTransTarget::transHObject(const HObject& hInput, HObject& hOutput)
	{
		HTuple hClass;
		hClass = hInput.GetObjClass();
		if (hClass[0] == "image") {
			// 图像
			AffineTransImage(hInput, &hOutput, hTransMatrix, "bilinear", "false");
		}
		else if (hClass[0] == "region") {
			// 区域
			AffineTransRegion(hInput, &hOutput, hTransMatrix, "bilinear");
		}
		else if (hClass[0] == "xld")
		{
			// XLD轮廓
			AffineTransContourXld(hInput, &hOutput, hTransMatrix);
		}

		return 0;
	}

	// 直线变换
	int AffineTransTarget::transLine(const LineStruct& lineInput, LineStruct& lineOutput)
	{
		HTuple hLineRow, hLineCol, hLineRowTrans, hLineColTrans;
		hLineRow.Clear();
		hLineCol.Clear();
		hLineRowTrans.Clear();
		hLineColTrans.Clear();
		hLineRow.Append(lineInput.pt1.y);
		hLineRow.Append(lineInput.pt2.y);
		hLineCol.Append(lineInput.pt1.x);
		hLineCol.Append(lineInput.pt2.x);

		//AffineTransPoint2d(hTransMatrix, hLineCol, hLineRow, &hLineColTrans, &hLineRowTrans);
		AffineTransPoint2d(hTransMatrix, hLineRow, hLineCol, &hLineRowTrans, &hLineColTrans);

		lineOutput.pt1.x = hLineColTrans[0].D();
		lineOutput.pt1.y = hLineRowTrans[0].D();
		lineOutput.pt2.x = hLineColTrans[1].D();
		lineOutput.pt2.y = hLineRowTrans[1].D();

		return 0;
	}



	int realtargetAffineTrans(targetAffineTransInput& input, targetAffineTransOutput& output)
	{
		Mat img = input.inputImg;
		HObject hTarget = input.inputTarget;
		LineStruct line = input.inputLine;
		CSYSTransPara transPara = input.coordSys;

		AlgParamTargetAffineTrans* alg = (AlgParamTargetAffineTrans*)&input.algPara;
		int transMode = alg->i_params.block.iTransMode;
		double timeOutThreshold = alg->d_params.block.dTimeOutThreshold;

		// 开始计时
		std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

		int rlt = 0;
		bool isLineEmpty = true;
		bool isTransV2H = false;
		if (alg->i_params.block.isTransV2H == 1)
			isTransV2H = true;

		// 创建仿射变换类
		AffineTransTarget targetTrans;

		// 判断是否输入的有图像，设置图像大小
		if (!img.empty())
		{
			targetTrans.setImgSize(img);
		}
		else if (!zkhyPublicFuncHN::isHObjectEmpty(hTarget))
		{
			HTuple hClass;
			hClass = hTarget.GetObjClass();
			if (hClass[0] == "image") {
				targetTrans.setImgSize(hTarget);
			}
		}
		else
			targetTrans.setImgSize();
		// 输入坐标变换参数不为空时使用坐标变换参数构建仿射变换矩阵
		if (transPara.translationValue.x != 0 || transPara.translationValue.y != 0 || transPara.angle != 0)
		{
			targetTrans.setTransPara(transPara, transMode);
		}
		// 输入直线不为空时使用直线构建仿射变换矩阵
		else if (line.pt1.x != 0 || line.pt1.y != 0 || line.pt2.x != 0 || line.pt2.y != 0)
		{
			isLineEmpty = false;
			rlt = targetTrans.setTransPara(line, transMode);
			if (rlt != 0)
				return 3;		// 根据直线计算坐标变换关系失败
		}
		else
		{
			// 无法建立变换矩阵
			return 4;
		}

		// 输入图像不为空，对图像进行变换
		if (!img.empty())
		{
			rlt = targetTrans.transImageCV(img, output.outputImg);
			if (rlt != 0)
				return 5;		// 图像变换失败
		}
		if (!zkhyPublicFuncHN::isHObjectEmpty(hTarget))
		{
			rlt = targetTrans.transHObject(hTarget, output.outputTarget);
			if (rlt != 0)
				return 6;		// H对象变换失败
		}
		if (isLineEmpty == false)
		{
			rlt = targetTrans.transLine(line, output.outputLine);
			if (rlt != 0)
				return 7;		// 直线变换失败
		}

		// 计算算法耗时
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		std::chrono::duration<double> timeUsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);

		if (timeUsed.count() > timeOutThreshold)
		{
			return 2;
		}

		return 0;
	}
}

namespace alglib::ops::zkhyProHN::alg {
	//——————————————仿射变换算子——————————————//
	int targetAffineTrans(targetAffineTransInput& input, targetAffineTransOutput& output)
	{
		int rlt = 0;
		try
		{
			rlt = realtargetAffineTrans(input, output);
		}
		catch (exception e)
		{
			string errorMsg = e.what();

			LOG.log(errorMsg, alglib::core::LEVEL::err);
			return 1;
		}
		return 0;
	}
}
