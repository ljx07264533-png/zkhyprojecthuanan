
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_GATHERLINE_H
#define ZKHYPROJECTHUANAN_GATHERLINE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"
#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------拟合直线算子 BEGIN-----------------//

	struct gatherLineInput
	{
		HalconCpp::HObject inputImg;		//输入图像
		alglib::core::RectangleROI rectROI;		//感兴趣区域

		alglib::core::AlgrithmParams algPara;		//算法参数
	};
	struct gatherLineOutput
	{
		alglib::core::LineStruct line;		//直线结构
		std::vector<std::vector<cv::Point2f>> twoPtsLinePic;    //两点直线轮廓坐标（直线的起始点和终止点）
		std::vector<std::vector<cv::Point2f>> edgePointsFind;    //原始轮廓点集
		std::vector<std::vector<cv::Point2f>> metrologyContours;    //
	};

	struct IntParamsGatherLine//int型算法参数，独立出来方便使用默认构造函数
	{
		int isPreprocessing = 0;		//是否进行二值化，0-无，1-二值化
		int iThreshold = 128;				//二值化阈值，默认值128，范围[0,255]
		int iEdgePolarity = 0;			//边缘点属性，0-全部，1-黑到白，2-白到黑，3-Uniform
		int iEdgeSelect = 0;			//边缘点选择，0-全部，1-第一个点，2-最后一个点
		int iSearchWidth = 10;			//搜索宽度，默认值5，范围[0,∞]
		int iInstancesOutSideMeasureRegions = 0;		//是否使用测量区域外的结果，默认值0，范围[0, 1]
		int iIterationNum = -1;			//迭代次数，默认值-1，不限制迭代次数，范围[-1,∞]
		int iMeasureDis = 10;			//测量区域间隔，默认值10，范围[1,∞]
		int iInterpolation = 2;			//插值方式，0-双三次，1-双线性，2-最近邻
		int iInstancesNum = 1;			//查找直线数，默认值1，范围[1,∞]
		int isFixSeed = 0;				//是否固定随机种子，默认值0，范围[0,1]

		// 初始化算法参数
		IntParamsGatherLine() : isPreprocessing(0), iThreshold(128), iEdgePolarity(0), iEdgeSelect(0), iSearchWidth(10), iInstancesOutSideMeasureRegions(0), iIterationNum(-1), iMeasureDis(10), iInterpolation(2), iInstancesNum(1), isFixSeed(0) {}
	};
	struct DoubleParamsGatherLine//double型算法参数，独立出来方便使用默认构造函数
	{
		double dSmoothFactor = 1;		//平滑系数，默认值1，范围[0，∞]
		double dEdgeStrength = 30;		//边缘强度，默认值30，范围[0，∞]
		double dRansacDis = 3;			//Ransac距离，默认值3，范围[0，∞]
		double dScoreMin = 0.7;			//最小分数，默认值0.7，范围[0，1]
		double dTimeOutThreshold = 1000;		// 算法超时设定

		// 初始化算法参数
		DoubleParamsGatherLine() : dSmoothFactor(1), dEdgeStrength(30), dRansacDis(3), dScoreMin(0.7), dTimeOutThreshold(1000) {}
	};
	struct AlgParamGatherLine
	{
		union
		{
			IntParamsGatherLine block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsGatherLine block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct   AlgParaMsgGatherLine
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgGatherLine()
		{
			msgs.push_back("isPreprocessing	$Int			$CheckBox	$0			$是否进行二值化	$是否进行二值化，默认值0：￥否 ￥是	$10");
			msgs.push_back("iThreshold	$Int			$I_Spinbox	$128			$二值化阈值	$二值化阈值，默认值128，范围[0,255]	$20");
			msgs.push_back("iEdgePolarity	$Int			$Combox	$0			$边缘点属性	$边缘点属性，默认值0：￥全部 ￥黑到白 ￥白到黑 ￥Uniform	$30");
			msgs.push_back("iEdgeSelect	$Int			$Combox	$0			$边缘点选择	$边缘点选择，默认值0：￥全部 ￥第一个点 ￥最后一个点	$40");
			msgs.push_back("iSearchWidth	$Int			$I_Spinbox	$10			$搜索宽度	$搜索宽度，默认值10，范围[0,∞]	$50");
			msgs.push_back("iInstancesOutSideMeasureRegions	$Int			$Combox	$0			$是否使用测量区域外的结果	$是否使用测量区域外的结果，默认值为否：￥false ￥true	$70");
			msgs.push_back("iIterationNum	$Int			$I_Spinbox	$-1			$迭代次数	$迭代次数，默认值-1，不限制迭代次数，范围[-1,∞]	$80");
			msgs.push_back("iMeasureDis	$Int			$I_Spinbox	$10			$测量区域间隔	$测量区域间隔，默认值10，范围[1,∞]	$90");
			msgs.push_back("iInterpolation	$Int			$Combox	$2			$插值方式	$插值方式，默认为最近邻方式：￥双三次 ￥双线性 ￥最近邻	$100");
			msgs.push_back("iInstancesNum	$Int			$I_Spinbox	$1			$查找直线数	$直线数，默认值1，范围[1,∞]	$110");
			msgs.push_back("isFixSeed	$Int			$Combox	$0			$是否固定随机种子	$是否固定随机种子，默认值为否：￥false ￥true	$120");
			msgs.push_back("dSmoothFactor	$Double			$D_Spinbox	$1			$平滑系数	$平滑系数，默认值1，范围[0，∞]	$130");
			msgs.push_back("dEdgeStrength	$Double			$D_Spinbox	$30			$边缘强度	$边缘梯度值，默认值30，范围[0，∞]	$140");
			msgs.push_back("dRansacDis	$Double			$D_Spinbox	$3			$Ransac距离	$Ransac距离，默认值3，范围[0，∞]	$150");
			msgs.push_back("dScoreMin	$Double			$D_Spinbox	$0.7			$最小分数	$最小分数，默认值0.7，范围[0，1]	$160");
			msgs.push_back("dTimeOutThreshold	$Double			$D_Spinbox	$1000.0			$算法超时设定(单位：ms)	$默认值1000.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常	$170");
		}
	};

	struct AlgErrMsgGatherLine
	{
		std::vector<std::string> msgs;
		AlgErrMsgGatherLine()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:未知异常");
			msgs.push_back("2:算法超时");
			// 输入参数异常
			msgs.push_back("3:输入参数：是否进行二值化 异常");
			msgs.push_back("4:输入参数：二值化阈值 异常");
			msgs.push_back("5:输入参数：边缘点属性 异常");
			msgs.push_back("6:输入参数：边缘点选择 异常");
			msgs.push_back("7:输入参数：搜索宽度 异常");
			msgs.push_back("8:输入参数：搜索高度 异常");	// 参数已移除，不会再出现该错误码
			msgs.push_back("9:输入参数：是否使用测量区域外的结果 异常");
			msgs.push_back("10:输入参数：迭代次数 异常");
			msgs.push_back("11:输入参数：测量区域间隔 异常");
			msgs.push_back("12:输入参数：插值方式 异常");
			msgs.push_back("13:输入参数：查找直线数 异常");
			msgs.push_back("14:输入参数：是否固定随机种子 异常");
			msgs.push_back("15:输入参数：平滑系数 异常");
			msgs.push_back("16:输入参数：边缘强度 异常");
			msgs.push_back("17:输入参数：Ransac距离 异常");
			msgs.push_back("18:输入参数：最小分数 异常");
			msgs.push_back("19:输入图像异常");
			msgs.push_back("20:输入ROI异常");
			msgs.push_back("21:预处理操作异常");
			msgs.push_back("22:拟合直线异常");
			msgs.push_back("23:未能找到直线，请检查参数");

		}
	};
	//---------------------拟合直线算子 END-----------------//
	
	// 拟合直线类
	class AlgGatherLine : public zkhyPublicClass::AlgorithmBase
	{
	public:
		AlgGatherLine();
		~AlgGatherLine() {}

		AlgGatherLine(const gatherLineInput& input);

		int checkParams();		// 检查输入参数是否越界

		int preProcessImage();	// 预处理
		int analyzeROI();		// 解析输入ROI
		int fitLines();			// 拟合直线

		void setMetrologyParams(HalconCpp::HTuple& hMetrologyObjName, HalconCpp::HTuple& hMetrologyObjValue);	 // 设置测量参数
		int execute();			// 执行算法

		void getLineResult(HalconCpp::HTuple& hResultTuple)		// 获取拟合直线结果
		{
			hResultTuple = hLineResult;
		}
		void getMetrologyContours(HalconCpp::HObject& hContours)	// 获取测量轮廓
		{
			hContours = hMetrologyContours;
		}
		void getEdgePoints(HalconCpp::HTuple& hRow, HalconCpp::HTuple& hCol)	// 获取边缘点
		{
			hRow = hEdgePointsRow;
			hCol = hEdgePointsCol;
		}

	protected:
		// 算法参数
		int isPreprocessing, threshold, edgePolarity, edgeSelect, instancesOutSideMeasureRegions, iterationNum, measureDis, interpolation, instancesNum, isFixSeed;
		double smoothFactor, edgeStrength, ransacDis, scoreMin, searchWidthHalf, searchHeightHalf;

		// 输入输出及中间变量
		HalconCpp::HTuple hLineResult, hDetImgWidth, hDetImgHeight, hSearchLineStart, hSearchLineEnd, hEdgePointsRow, hEdgePointsCol;
		HalconCpp::HObject hDetImage, hMetrologyContours;

		alglib::core::RectangleROI searchROI;
	};

	//! 接口

	namespace alg {
		
		//-----------------------------gatherLine-----------------------------//
		//函数说明：拟合直线
		//gatherLineInput &input;				输入：待测图像，测量ROI，测量参数
		//gatherLineOutput &output; 			输出：
		_ALGLIBRARY_API int gatherLine(const gatherLineInput& input, gatherLineOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_GATHERLINE_H
