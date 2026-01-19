/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_LASERETCHINGMEASUREMENT_H
#define ZKHYPROJECTHUANAN_LASERETCHINGMEASUREMENT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------激光刻蚀测量算子 BEGIN-----------------//

	struct LaseretchingmeasurementInput
	{
		HalconCpp::HObject inputImage;					//输入图像
		HalconCpp::HObject hDetRegion;                 //检测区域

		alglib::core::AlgrithmParams algPara;			    //算法参数
	};
	//Step2 输出结构体
	struct LaseretchingmeasurementOutput
	{
		std::vector<alglib::core::MeasureInfoStruct>	measureinfos;		//缺陷信息
	};
	//Step3 算法参数（int double）
	struct AlgParaLaseretchingmeasurementInt       //int型算法参数，独立出来方便使用默认构造函数
	{
		int iLineOffsetDirection = 0;		//基准直线偏移方向，0-正向，1-反向。正向为向右或向上，反向为向左或向下
		int iLineHeight = 200;				//待测线路高度

		int Upwarddistance = 100;           //测量上移量
		int Downwarddistance = 350;           //测量下移量


		// 初始化参数
		AlgParaLaseretchingmeasurementInt() :iLineOffsetDirection(0), iLineHeight(200), /*iLineWidth(50), iLineSpacing(100) ,*/ Upwarddistance(100), Downwarddistance(350) {}
	};
	struct AlgParaLaseretchingmeasurementDouble    //double型算法参数，独立出来方便使用默认构造函数
	{
		double dSmoothFactor = 1;		//平滑系数，默认值1，范围[0，∞]
		double dEdgeStrength = 30;		//边缘强度，默认值30，范围[0，∞]

		double dTimeOutThreshold = 500;		// 算法超时设定

		// 初始化参数
		AlgParaLaseretchingmeasurementDouble() :dSmoothFactor(1), dEdgeStrength(30), dTimeOutThreshold(500) {}
	};
	struct AlgParaLaseretchingmeasurement
	{
		union
		{
			AlgParaLaseretchingmeasurementInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaLaseretchingmeasurementDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgLaseretchingmeasurement
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgLaseretchingmeasurement()
		{
			msgs.push_back("iLineOffsetDirection	$Int		$Combox	$0  	 $基准直线偏移方向		$基准直线偏移方向，0-正向，1-反向。正向为向右或向上，反向为向左或向下 ￥正向 ￥反向	$10");
			msgs.push_back("iLineHeight	$Int		$I_Spinbox	$200  	 $待测线路范围		$待测线路范围，默认值200，范围[1,∞]	$20");

			msgs.push_back("Upwarddistance $Int		$I_Spinbox	$100  	 $测量上移量		$测量上移量，默认值100，范围[1,∞]	$50");
			msgs.push_back("Downwarddistance $Int		$I_Spinbox	$350  	 $测量下移量		$测量下移量，默认值350，范围[1,∞]	$60");
			msgs.push_back("dSmoothFactor	$Double		$D_SpinBox	$1  	 $平滑系数		$平滑系数，默认值1，范围[0，∞]	$70");
			msgs.push_back("dEdgeStrength	$Double		$D_SpinBox	$30  	 $边缘强度		$边缘强度，默认值30，范围[0，∞]	$80");
			msgs.push_back("dTimeOutThreshold   $Double      $D_SpinBox	 $500.0  	 $算法超时设定(单位：ms)			 $默认值500.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常	$90");

		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgLaseretchingmeasurement
	{
		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgLaseretchingmeasurement()
		{
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgLaseretchingmeasurement
	{
		std::vector<std::string> msgs;
		AlgErrMsgLaseretchingmeasurement()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:未知异常");
			msgs.push_back("2:算法超时");
			// 输入参数异常
			msgs.push_back("3:输入参数：基准直线偏移方向 异常");
			msgs.push_back("4:输入参数：待测线路高度 异常");
			msgs.push_back("5:输入参数：平滑系数 异常");
			msgs.push_back("6:输入参数：边缘强度 异常");
			// 执行异常
			msgs.push_back("7:测量异常");
			msgs.push_back("8:输入图像异常");
		}
	};
	//---------------------拟合直线算子 END-----------------//

	//! 接口

	namespace alg {

		//-----------------------------Laseretchingmeasurement-----------------------------//
		//函数说明：激光刻蚀测量算子
		//LaseretchingmeasurementInput &input;				输入：待测图像、待测区域
		//LaseretchingmeasurementOutput &output; 			输出：激光刻蚀深度
		_ALGLIBRARY_API int Laseretchingmeasurement(const LaseretchingmeasurementInput& input, LaseretchingmeasurementOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_LASERETCHINGMEASUREMENT_H
