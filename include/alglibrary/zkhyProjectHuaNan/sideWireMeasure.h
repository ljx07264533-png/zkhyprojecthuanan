
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: 侧面线宽线距测量（sideWireMeasure）算子，详细内容:辰显项目线宽线距测量算子。
***************************************************/
#ifndef ZKHYPROJECTHUANAN_SIDEWIREMEASURE_H
#define ZKHYPROJECTHUANAN_SIDEWIREMEASURE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体


	//————————————————————————sideWireMeasure 侧面线路测量算子 Begin————————————————//
	//Step1 输入结构体
	struct sideWireMeasureInput
	{
		HalconCpp::HObject inputImage;					//输入图像
		alglib::core::LineStruct baseLine;                //基准直线
		HalconCpp::HObject hDetRegion;                 //检测区域
		std::vector<cv::Point2f> detRegion;          //检测区域

		alglib::core::AlgrithmParams algPara;			    //算法参数
	};
	//Step2 输出结构体
	struct sideWireMeasureOutput
	{
		std::vector<std::vector<cv::Point2f>> wireEdges;		//线路边缘

		std::vector<wireMeasureInfo>	measureinfos;		//缺陷信息
	};
	//Step3 算法参数（int double）
	struct AlgParaSideWireMeasureInt       //int型算法参数，独立出来方便使用默认构造函数
	{
		int iLineOffsetDirection = 0;		//基准直线偏移方向，0-正向，1-反向。正向为向右或向上，反向为向左或向下
		int iLineHeight = 200;				//待测线路高度
		int iLineWidth = 50;				//待测线路宽度
		int iLineSpacing = 100;				//待测线路间距
		int iProductDirection = 0;			//产品方向，0-水平，1-垂直
		int iLineColor = 0;					//线路亮暗，0-暗，1-亮

		// 初始化参数
		AlgParaSideWireMeasureInt() :iLineOffsetDirection(0), iLineHeight(200), iLineWidth(50), iLineSpacing(100), iProductDirection(0), iLineColor(0) {}
	};
	struct AlgParaSideWireMeasureDouble    //double型算法参数，独立出来方便使用默认构造函数
	{
		double dSmoothFactor = 1;		//平滑系数，默认值1，范围[0，∞]
		double dEdgeStrength = 30;		//边缘强度，默认值30，范围[0，∞]
		double dLineWidthRange = 0.1;	//线宽变化范围，默认值0.1，范围[0，∞]
		double dLineSpacingRange = 0.1;	//线间距变化范围，默认值0.1，范围[0，∞]

		double dTimeOutThreshold = 500;		// 算法超时设定

		// 初始化参数
		AlgParaSideWireMeasureDouble() :dSmoothFactor(1), dEdgeStrength(30), dLineWidthRange(0.1), dLineSpacingRange(0.1), dTimeOutThreshold(500) {}
	};
	struct AlgParaSideWireMeasure
	{
		union
		{
			AlgParaSideWireMeasureInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaSideWireMeasureDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgSideWireMeasure
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgSideWireMeasure()
		{
			msgs.push_back("iLineOffsetDirection	$Int		$Combox	$0  	 $基准直线偏移方向		$基准直线偏移方向，0-正向，1-反向。正向为向右或向上，反向为向左或向下 ￥正向 ￥反向	$10");
			msgs.push_back("iLineHeight	$Int		$I_Spinbox	$200  	 $待测线路高度		$待测线路高度，默认值200，范围[1,∞]	$20");
			msgs.push_back("iLineWidth	$Int		$I_Spinbox	$50  	 $待测线路宽度		$待测线路宽度，默认值50，范围[1,∞]	$30");
			msgs.push_back("iLineSpacing $Int		$I_Spinbox	$100  	 $待测线路间距		$待测线路间距，默认值100，范围[1,∞]	$40");
			msgs.push_back("dSmoothFactor	$Double		$D_SpinBox	$1  	 $平滑系数		$平滑系数，默认值1，范围[0，∞]	$60");
			msgs.push_back("dEdgeStrength	$Double		$D_SpinBox	$30  	 $边缘强度		$边缘强度，默认值30，范围[0，∞]	$70");
			msgs.push_back("dTimeOutThreshold   $Double      $D_SpinBox	 $500.0  	 $算法超时设定(单位：ms)			 $默认值500.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常	$80");
			msgs.push_back("iProductDirection	$Int		$Combox	$0  	 $产品方向		$产品方向，0-水平，1-垂直 ￥水平 ￥垂直	$90");
			msgs.push_back("iLineColor	$Int		$Combox	$0  	 $线路亮暗		$线路亮暗，线路相较于周围背景的亮暗程度。0-暗，1-亮 ￥暗 ￥亮	$50");
			msgs.push_back("dLineWidthRange	$Double		$D_SpinBox	$0.1  	 $线宽变化范围		$线宽变化范围，默认值0.1，范围[0，∞]	$100");
			msgs.push_back("dLineSpacingRange	$Double		$D_SpinBox	$0.1  	 $线间距变化范围		$线间距变化范围，默认值0.1，范围[0，∞]	$110");

		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgSideWireMeasure
	{
		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgSideWireMeasure()
		{
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgSideWireMeasure
	{
		std::vector<std::string> msgs;
		AlgErrMsgSideWireMeasure()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:未知异常");
			msgs.push_back("2:算法超时");
			// 输入参数异常
			msgs.push_back("3:输入参数：基准直线偏移方向 异常");
			msgs.push_back("4:输入参数：待测线路高度 异常");
			msgs.push_back("5:输入参数：待测线路宽度 异常");
			msgs.push_back("6:输入参数：待测线路间距 异常");
			msgs.push_back("7:输入参数：线路亮暗 异常");
			msgs.push_back("8:输入参数：平滑系数 异常");
			msgs.push_back("9:输入参数：边缘强度 异常");
			msgs.push_back("10:输入参数：产品方向 异常");
			msgs.push_back("11:输入参数：线宽变化范围 异常");
			msgs.push_back("12:输入参数：线间距变化范围 异常");
			// 执行异常
			msgs.push_back("13:预处理操作异常");
			msgs.push_back("14:测量异常");
			msgs.push_back("15:后处理操作异常");
			msgs.push_back("16:输入图像异常");
		}
	};
	//————————————————————————sideWireMeasure 侧面线路测量算子 End————————————————//
	//! 接口

	namespace alg {
		
		//----------------------------- sideWireMeasure-----------------------------------------//
		//函数说明：侧面线路测量
		//SideWireMeasureInput &input;				输入：待测图像、基准直线
		//SideWireMeasureOutput &output; 			输出：侧面线路的线宽、线间距
		_ALGLIBRARY_API int sideWireMeasure(const sideWireMeasureInput& input, sideWireMeasureOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_SIDEWIREMEASURE_H
