
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_BGABALL2DDEFECTDET_H
#define ZKHYPROJECTHUANAN_BGABALL2DDEFECTDET_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------BGABall2dDefectDet BGA锡球2D缺陷特征 算子------------------------------------------//
	struct BGABall2dDefectDetInput
	{
		HalconCpp::HObject srcImg;                            //原图
		alglib::core::CSYSTransPara layout2DetectTransPara;                  //坐标系
		std::vector<cv::Point2f>  vertexs;            //产品四个顶点和中心点信息
		BGAParam layoutInfo;                 //Layout信息
		alglib::core::AlgrithmParams algPara;					   //算法参数
	};
	struct BGABall2dDefectDetOutput
	{
		std::vector <std::vector<cv::Point2f>> ballContour; //检测到的锡球轮廓
		HalconCpp::HObject ballRegions;  //锡球区域
		std::vector<alglib::core::FlawInfoStruct>  flawsData;               //缺陷信息
		HalconCpp::HObject ballTemplateRegions;		// 锡球模板区域
		std::vector<alglib::core::FlawInfoStruct>  flawStatic;			   //缺陷信息统计
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct IntParamsBGABall2dDefectDet//int型算法参数，独立出来方便使用默认构造函数
	{
		int iQualityMethod = 0;                       //锡球质量计算方法，0-Normal，1-Pattern Matching
		int iSamplingStep = 10;                         //计算锡球质量时的采样步长
		int iThresholdSegBallLow = 200;         //分割锡球时使用的低阈值
		int iThresholdSegBallHigh = 255;         //分割锡球时使用的高阈值
		int iBallPitchFunc = 0;                  //锡球间距计算方法，0-锡球到其相邻锡球距离与标准值偏差最大的距离，1-锡球到其右侧和下方锡球的距离
		int isNormalizeBallWidth = 0;            //是否对锡球宽度进行归一化，0-不归一化，1-归一化
		int iBallRegionMode = 1;				// 输出锡球区域的方式：0: 使用阈值分割获取锡球区域，1：使用2D测量获取锡球区域
		int iBallDetFunc = 3;					// 锡球检测方法：0: 使用阈值分割获取锡球区域，1：使用阈值分割进行粗定位，随后再进行亚像素分割，2：使用2D测量获取锡球区域，3：使用阈值分割进行粗定位，然后使用2D测量获取锡球区域
	};
	struct DoubleParamsBGABall2dDefectDet//double型算法参数，独立出来方便使用默认构造函数
	{
		double dInnerCircle = 0.6;              //采用Normal方法计算球质量时的内圆半径系数
		double dOuterCircle = 1.2;              //采用Normal方法计算球质量时的外圆半径系数
		double dThresholdMeasure = 100;         //采用Normal方法计算球质量时的边缘点梯度阈值
		double dBallAreaMin = 0;               //锡球面积最小值
		double dBallAreaMax = 99999;              //锡球面积最大值
		double dTimeOutThreshold = 1000;               // 算法超时设定
		double dBallSquashInner = 0.4;			// 计算锡球挤压度时的内圆半径系数
	};
	struct AlgParamBGABall2dDefectDet
	{
		union
		{
			IntParamsBGABall2dDefectDet block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsBGABall2dDefectDet block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgBGABall2dDefectDet
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgBGABall2dDefectDet()
		{
			msgs.push_back("iQualityMethod      $Int  $Combox   $0   $球质量计算方法                      $锡球质量计算方法 ￥Normal ￥Pattern Matching $10");
			msgs.push_back("iSamplingStep      $Int  $I_Spinbox   $10   $采样步长                      $默认值10，范围[0,360]，查找锡球和计算球质量时的采样步长，步长越小用的采样点越多 $20");
			msgs.push_back("iThresholdSegBallLow               $Int  $I_Spinbox   $100     $锡球最小灰度阈值                      $默认值100，范围（0,255]。待分割锡球的最小灰度值 $30");
			msgs.push_back("iThresholdSegBallHigh              $Int  $I_Spinbox   $255  $锡球最大灰度阈值                      $默认值255，范围[0,255]。待分割锡球的最大灰度值 $40");
			msgs.push_back("dInnerCircle      $Double  $D_Spinbox   $0.7   $内圆半径系数                      $默认值0.7，查找锡球时的内圆半径 = 锡球标准半径 * 内圆半径系数  $50");
			msgs.push_back("dOuterCircle      $Double  $D_Spinbox   $1.2   $外圆半径系数                      $默认值1.2，查找锡球时的外圆半径 = 锡球标准半径 * 外圆半径系数  $60");
			msgs.push_back("dThresholdMeasure      $Double  $D_Spinbox   $50   $边缘点梯度阈值                      $默认值50，锡球边界的最小梯度值（沿半径方向） $70");
			msgs.push_back("dBallAreaMin      $Double  $D_Spinbox   $0   $锡球面积最小值                      $默认值0，锡球区域的面积最小值 $80");
			msgs.push_back("dBallAreaMax      $Double  $D_Spinbox   $99999   $锡球面积最大值                      $默认值99999，锡球区域的面积最大值 $90");
			msgs.push_back("dTimeOutThreshold      $Double  $D_Spinbox   $1000.0   $算法超时设定(单位：ms)                      $默认值1000.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常 $100");
			msgs.push_back("iBallPitchFunc      $Int  $Combox   $0   $球距计算方法                      $锡球间距计算方法，0-相邻锡球偏差最大距离，锡球到其相邻锡球距离与标准值偏差最大的距离，缺陷检测时使用；1-相邻锡球距离，锡球到其右侧和下方锡球的距离，黄金样本测试时使用 ￥缺陷检测 ￥黄金样本 $16");
			msgs.push_back("isNormalizeBallWidth      $Int  $Combox   $0   $球宽归一化                     $是否对锡球宽度进行归一化，0-不归一化，1-归一化 ￥不归一化 ￥归一化 $13");
			msgs.push_back("iBallRegionMode      $Int  $Combox   $0   $锡球区域获取方法                     $输出锡球区域的方式：0: 使用阈值获取锡球区域，1：使用测量获取锡球区域 ￥阈值分割区域 ￥测量圆区域 $14");
			msgs.push_back("iBallDetFunc      $Int  $Combox   $0   $锡球检测方法                     $锡球检测方法：0: 使用阈值分割获取锡球区域，1：使用阈值分割进行粗定位，随后再进行亚像素分割，2：使用2D测量获取锡球区域，3：使用阈值分割进行粗定位，然后使用2D测量获取锡球区域 ￥阈值分割 ￥阈值分割+亚像素 ￥测量圆 ￥阈值分割+测量圆 $15");
			msgs.push_back("dBallSquashInner      $Double  $D_Spinbox   $0.4   $球挤压度内圆半径系数                      $默认值0.4，计算锡球挤压度时的内圆半径 = 锡球标准半径 * 内圆半径系数  $17");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgBGABall2dDefectDet
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgBGABall2dDefectDet() {
			// 增加BGA锡球2D检测算子的扩增特征信息
			exmsgs.push_back("ballWidth          $Double	   $球宽	$锡球的宽度          $30   $CF ");
			exmsgs.push_back("ballXPitch          $Double	   $球距X方向	$相邻锡球X方向的距离          $31   $CF ");
			exmsgs.push_back("ballYPitch          $Double	   $球距Y方向	$相邻锡球Y方向的距离          $32   $CF ");
			exmsgs.push_back("ballXOffset          $Double	   $球x方向偏移	$锡球X方向的偏移距离          $33   $CF ");
			exmsgs.push_back("ballYOffset         $Double	   $球Y方向偏移	$锡球Y方向的偏移距离          $34   $CF ");
			exmsgs.push_back("ballROffset          $Double	   $球心偏移	$锡球球心的偏移距离          $35   $CF ");
			exmsgs.push_back("ballSquash          $Double	   $球挤压度	$锡球的扁平程度          $36   $CF ");
			exmsgs.push_back("ballQuality          $Double	   $球质量	$锡球质量          $37   $CF ");
			exmsgs.push_back("ballContrast          $Double	   $球对比度	$锡球的对比度          $38   $CF ");
			exmsgs.push_back("ballMissing          $Double	   $球缺失	$锡球是否缺失          $39   $CF ");
			exmsgs.push_back("ballGridOffsetX          $Double	   $整体球X偏移	$锡球整体在X方向上的偏移          $40   $CF ");
			exmsgs.push_back("ballGridOffsetY          $Double	   $整体球Y偏移	$锡球整体在Y方向上的偏移          $41   $CF ");
		}
	};
	//Step5 异常返回值描述 锡球特征检测错误信息结构体
	struct AlgErrMsgBGABall2dDefectDet
	{
		std::vector<std::string> msgs;
		AlgErrMsgBGABall2dDefectDet()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:算法超时参数输入有误");
			msgs.push_back("2:Layout坐标信息与锡球行列数不匹配");
			msgs.push_back("3:不支持的球质量计算方法");
			msgs.push_back("4:算法超时");
			msgs.push_back("5:未知错误");
			msgs.push_back("6:输入图像为空");
			msgs.push_back("7:输入产品顶点信息有误");
			msgs.push_back("8:球质量计算出错");
			msgs.push_back("9:不支持的球距计算方法");
		}
	};


	//! 接口

	namespace alg {
		
		//-----------------------------BGABall2dDefectDet------------------------------------------//
		//函数说明：计算BAG锡球特征（如是否有缺失、锡球在X轴和Y轴上的位置偏移、相邻锡球之间的最小距离、球质量以及球对比度）
		//BGABallDefectDetInput &input;				输入：待测图像、Layout到待测图的坐标变换矩阵、IC的顶点信息、Layout信息、算法参数
		//BGABallDefectDetOutput &output; 			输出：待测图中锡球的2D缺陷特征、锡球的轮廓
		_ALGLIBRARY_API int BGABall2dDefectDet(const BGABall2dDefectDetInput& input, BGABall2dDefectDetOutput& output);
	}
	class BGADetInfo
	{
	public:
		cv::Point2f templateBallCenter;
		cv::Point2f detBallCenter;
		double area;
		double radius;
		double radiusNorm;
		double pitchX;
		double pitchY;
		double offsetX;
		double offsetY;
		double offsetXY;
		double contrast;
		double squash;
		double quality;
		int isExist;
		bool isMissing;

		BGADetInfo() {
			cv::Point2f pointTmp;
			pointTmp.x = 0;
			pointTmp.y = 0;
			templateBallCenter = pointTmp;
			detBallCenter = pointTmp;
			area = -9999;
			radius = -9999;
			radiusNorm = -9999;
			pitchX = -9999;
			pitchY = -9999;
			offsetX = -9999;
			offsetY = -9999;
			offsetXY = -9999;
			contrast = -9999;
			squash = -9999;
			quality = -9999;
			isExist = 0;
			isMissing = false;
		}
	};

	class existedTarget {
	public:
		int row;
		int col;

		existedTarget() {
			row = 0;
			col = 0;
		}
	};

	struct myLine
	{
		cv::Point startPoint;
		cv::Point endPoint;
	};
	int normWithStdValue(const std::vector<double>& oriValue, const std::vector<double>& stdValue, std::vector<double>& normValue, const double scaleCoef = 1.0);
	//----------------------------------------------packingDetInputTrans--------------------------------//
	//功能：对封测产品的输入数据进行坐标变换
	// 输入：srcImg 待测图像
	//      transPara 坐标变换信息
	//      vertexs 产品的四个顶点及其中心点
	//      coordOffset 坐标偏移量，变换后的图像相对于（0，0）点的偏移量
	// 输出：imgTrans 坐标变换后的图像
	//      layout2DetectTrans 坐标变换矩阵
	//      vertexsTrans 坐标变换后的产品顶点信息
	int packingDetInputTrans(HalconCpp::HObject srcImg, alglib::core::CSYSTransPara transPara, std::vector<cv::Point2f> vertexs, double coordOffset, HalconCpp::HObject& imgTrans, HalconCpp::HTuple& layout2DetectTrans, std::vector<cv::Point2f>& vertexsTrans);


}

#endif	// ZKHYPROJECTHUANAN_BGABALL2DDEFECTDET_H
