
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_TARGETAFFINETRANS_H
#define ZKHYPROJECTHUANAN_TARGETAFFINETRANS_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------仿射变换算子 BEGIN-----------------//
	struct targetAffineTransInput
	{
		cv::Mat inputImg;		// 输入图像
		HalconCpp::HObject inputTarget;		// 输入目标，可以是图像，区域或轮廓
		alglib::core::LineStruct inputLine;		// 输入直线
		alglib::core::CSYSTransPara coordSys;		// 参考坐标系
		alglib::core::AlgrithmParams algPara;		// 算法参数
	};
	struct targetAffineTransOutput
	{
		cv::Mat outputImg;		// 输出图像
		HalconCpp::HObject outputTarget;		// 输出目标，可以是图像，区域或轮廓
		alglib::core::LineStruct outputLine;		// 输出直线
	};

	struct IntParamsTargetAffineTrans//int型算法参数，独立出来方便使用默认构造函数
	{
		int isTransV2H = 0;		// 是否将垂直状态的目标转换为水平状态

		int iTransMode = 0;		// 仿射变换模式，0-默认模式，1-辰显项目

		// 初始化算法参数
		IntParamsTargetAffineTrans() : isTransV2H(0), iTransMode(0) {}
	};
	struct DoubleParamsTargetAffineTrans//double型算法参数，独立出来方便使用默认构造函数
	{

		double dTimeOutThreshold = 1000;		// 算法超时设定

		// 初始化算法参数
		DoubleParamsTargetAffineTrans() : dTimeOutThreshold(100) {}
	};
	struct AlgParamTargetAffineTrans
	{
		union
		{
			IntParamsTargetAffineTrans block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsTargetAffineTrans block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct   AlgParaMsgTargetAffineTrans
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgTargetAffineTrans()
		{
			msgs.push_back("isTransV2H	$Int			$Checkbox	$0			$是否将垂直状态的目标转换为水平状态	$默认值0，范围[0,1]。勾选后，将垂直状态的目标转换为水平状态	$10");
			msgs.push_back("dTimeOutThreshold	$Double			$D_Spinbox	$500.0			$算法超时设定(单位：ms)	$默认值500.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常	$30");
			msgs.push_back("iTransMode	$Int			$Combox	$0			$仿射变换模式	$仿射变换模式，用于兼容不同需求。0-默认模式，1-辰显项目	$40");
		}
	};

	struct AlgErrMsgTargetAffineTrans
	{
		std::vector<std::string> msgs;
		AlgErrMsgTargetAffineTrans()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:未知异常");
			msgs.push_back("2:算法超时");
			msgs.push_back("3:根据直线计算坐标变换关系失败");
			msgs.push_back("4:无法建立变换矩阵");
			msgs.push_back("5:图像变换失败");
			msgs.push_back("6:H对象变换失败");
			msgs.push_back("7:直线变换失败");
		}
	};
	//---------------------仿射变换算子 END-----------------//
	//! 接口

	namespace alg {
		
		// @brief 仿射变换算子，对输入的图片、区域、直线进行放射变换
		// @param input 待变换的图像，区域、轮廓等
		// @param output 变换后的图像，区域、轮廓等
		_ALGLIBRARY_API int targetAffineTrans(targetAffineTransInput& input, targetAffineTransOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_TARGETAFFINETRANS_H
