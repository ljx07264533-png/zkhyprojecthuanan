
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_QFNPAD2DDEFECTDET_H
#define ZKHYPROJECTHUANAN_QFNPAD2DDEFECTDET_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	
	//-----------------------------QFNPad2dDefectDet QFNPad2d缺陷特征 算子------------------------------------------//
	struct QFNPad2dDefectDetInput
	{
		HalconCpp::HObject srcImg;                            //原图
		alglib::core::CSYSTransPara layout2DetectTransPara;                  //坐标系
		std::vector<cv::Point2f>  vertexs;            //产品四个顶点和中心点信息
		QFNParam layoutInfo;                 //Layout信息
		alglib::core::AlgrithmParams algPara;					   //算法参数
	};
	struct QFNPad2dDefectDetOutput
	{
		std::vector<std::vector<cv::Point2f>> padAlignRoI;    //Pad Aligh生成的RoI
		std::vector<std::vector<cv::Point2f>> padAlignEdgePoints;  //Pad Align查找到的边缘点
		std::vector<std::vector<cv::Point2f>> padAlignResults;  //Pad Align的结果
		std::vector<std::vector<cv::Point2f>> padContour;  //Pad外接矩形
		HalconCpp::HObject padRegions;  //Pad外接矩形区域
		std::vector<alglib::core::FlawInfoStruct>  flawsData;               //缺陷信息
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct IntParamsQFNPad2dDefectDet//int型算法参数，独立出来方便使用默认构造函数
	{
		int isFitCircle = 0;            // Pad Align时是否拟合圆
		int iThresholdSegmPadLow = 100;    // 对Pad进行分割的低阈值
		int iThresholdSegmPadHigh = 255;    // 对Pad进行分割的高阈值
		int iDetectMode = 0;			// 检测模式：1-正常产品，2-黄金样本
	};
	struct DoubleParamsQFNPad2dDefectDet//double型算法参数，独立出来方便使用默认构造函数
	{
		double dEdgeSearchRateL = 0.6;    // Pad Align时ROI长边的系数
		double dEdgeSearchRateW = 0.6;     // Pad Align时ROI短边的系数
		double dThresholdPadAlign = 50;   // Pad Align时的阈值
		double dPadEdgePointNumMin = 3;  // Pad Align时查找到的边缘点最少数目
		double dTimeOutThreshold = 1000;               // 算法超时设定
	};
	struct AlgParamQFNPad2dDefectDet
	{
		union
		{
			IntParamsQFNPad2dDefectDet block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsQFNPad2dDefectDet block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgQFNPad2dDefectDet
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgQFNPad2dDefectDet()
		{
			msgs.push_back("isFitCircle      $Int  $Combox   $0   $是否拟合圆                      $Pad Align时是否拟合圆 ￥不拟合 ￥拟合 $10");
			msgs.push_back("iThresholdSegmPadLow               $Int  $I_Spinbox   $100     $Pad最小灰度阈值                      $默认值100，范围[0,255]。待分割Pad的最小灰度值 $20");
			msgs.push_back("iThresholdSegmPadHigh              $Int  $I_Spinbox   $255  $Pad最大灰度阈值                      $默认值255，范围[0,255]。待分割Pad的最大灰度值 $30");
			msgs.push_back("dEdgeSearchRateL      $Double  $D_Spinbox   $0.6   $搜索长度                      $默认值0.6，Pad Align时ROI长边的系数 $40");
			msgs.push_back("dEdgeSearchRateW      $Double  $D_Spinbox   $0.6   $搜索宽度                      $默认值0.6，Pad Align时ROI长边的系数 $50");
			msgs.push_back("dThresholdPadAlign      $Double  $D_Spinbox   $50   $边缘点梯度阈值                      $默认值50，Pad Align时的边缘点梯度阈值 $60");
			msgs.push_back("dPadEdgePointNumMin      $Double  $D_Spinbox   $3   $边缘点个数                      $默认值3，Pad Align时查找到的边缘点最少数目 $70");
			msgs.push_back("dTimeOutThreshold      $Double  $D_Spinbox   $3000.0   $算法超时设定(单位：ms)                      $默认值3000.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常 $80");
			msgs.push_back("iDetectMode      $Int  $Combox   $0   $检测模式                      $检测模式：1-正常产品，2-黄金样本 ￥正常产品 ￥黄金样本 $90");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgQFNPad2dDefectDet
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgQFNPad2dDefectDet() {
			// 增加BGA锡球2D检测算子的扩增特征信息
			exmsgs.push_back("padPitch          $Double	   $Pad间距	$Pad间距          $60   $CF ");
			exmsgs.push_back("padOffset          $Double	   $偏移	$Pad偏移          $61   $CF ");
			exmsgs.push_back("padConstrast          $Double	   $对比度	$Pad对比度          $62   $CF ");
			exmsgs.push_back("padLength          $Double	   $长度	$Pad长度          $63   $CF ");
			exmsgs.push_back("padWidth         $Double	   $宽度	$Pad宽度          $64   $CF ");
			exmsgs.push_back("padJLength          $Double	   $J.Length	$Pad顶部到IC边缘的距离          $65   $CF ");
			exmsgs.push_back("padTD          $Double	   $padTD	$IC上对称位置的Pad之间的距离          $66   $CF ");
			exmsgs.push_back("padOffsetLT          $Double	   $padOffsetLT	$Pad相对于IC左上角的偏移          $67   $CF ");
			exmsgs.push_back("padSpan          $Double	   $IC中心到Pad顶点的距离	$IC中心到Pad顶点的距离         $68   $CF ");
			exmsgs.push_back("cornerPadPitch         $Double	   $CornerPad间距	$CornerPad间距          $69   $CF ");
			exmsgs.push_back("cornerPadOffset          $Double	   $CornerPad偏移	$CornerPad偏移          $70   $CF ");
			exmsgs.push_back("cornerPadLength          $Double	   $CornerPad长度	$CornerPad长度          $71   $CF ");
			exmsgs.push_back("cornerPadWidth          $Double	   $CornerPad宽度	$CornerPad宽度          $72   $CF ");
			exmsgs.push_back("centerPadLength         $Double	   $CenterPad长度	$CenterPad长度         $73   $CF ");
			exmsgs.push_back("centerPadWidth          $Double	   $CenterPad宽度	$CenterPad宽度          $74   $CF ");
			exmsgs.push_back("centerPadOffset          $Double	   $CenterPad偏移	$CenterPad偏移          $75   $CF ");
			exmsgs.push_back("PadMissing          $Int	   $Pad缺失	$Pad不存在          $76   $CF ");
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgQFNPad2dDefectDet
	{
		std::vector<std::string> msgs;
		AlgErrMsgQFNPad2dDefectDet()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:算法超时参数输入有误");
			msgs.push_back("2:输入图像为空");
			msgs.push_back("3:算法超时");
			msgs.push_back("4:未知错误");
			msgs.push_back("5:输入产品顶点信息有误");
		}
	};

	//! 接口

	namespace alg {
		//-----------------------------QFNPad2dDefectDet------------------------------------------//
		//函数说明：计算QFN封装的pad特征
		//QFN2dDefectDetInput &input;				输入：待测图像、Layout到待测图的坐标变换矩阵、IC的顶点信息、Layout信息、算法参数
		//QFN2dDefectDetOutput &output; 			输出：待测图中Pad的2D缺陷特征、Pad的轮廓信息、Pad Align生成的ROI、Pad Align查找到的所有边缘点以及Pad Align最终的结果
		_ALGLIBRARY_API int QFNPad2dDefectDet(const QFNPad2dDefectDetInput& input, QFNPad2dDefectDetOutput& output);
	}


}

#endif	// ZKHYPROJECTHUANAN_QFNPAD2DDEFECTDET_H
