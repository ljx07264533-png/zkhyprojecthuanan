
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_AUTOMARK_H
#define ZKHYPROJECTHUANAN_AUTOMARK_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------自动绘制定位mark算子 Start----------------//
	struct autoMarkInput
	{
		HalconCpp::HObject Himg;//查找mark的图像源
		HalconCpp::HObject Hregion;//查找mark的区域
		std::vector< std::vector<cv::Point2f>> posFindROI;
		alglib::core::AlgrithmParams algPara;					//算法参数

	};
	struct  autoMarkOutput
	{
		HalconCpp::HObject SubMarkRegion;
		HalconCpp::HObject Maybemark;
	};

	struct IntParamsAutoMark//int型算法参数，独立出来方便使用默认构造函数
	{
		int MinArea;//备选mark面积最小值
		int MaxArea;//备选mark面积最大值		
	};
	struct DoubleParamsAutoMark//double型算法参数，独立出来方便使用默认构造函数
	{
		double  hv_angleStart;
		double  hv_angleExtent;
	};
	struct AlgParamAutoMark
	{
		union
		{
			IntParamsAutoMark block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsAutoMark block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};


	struct AlgParaMsgAutoMark
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgAutoMark()
		{
			msgs.push_back("MinArea	$Int			$I_Spinbox	$8000  				$mark最小面积		$查找mark的最小面积，范围[1,999999999]	$10");
			msgs.push_back("MaxArea	$Int			$I_Spinbox	$20000  			$mark最大面积		$查找mark的最小面积，范围[1,999999999]	$20");

		}
	};

	struct AlgErrMsgAutoMark
	{
		std::vector<std::string> msgs;
		AlgErrMsgAutoMark()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:异常");
			msgs.push_back("2:异常");
			msgs.push_back("3:异常");
			msgs.push_back("4:异常");
			msgs.push_back("5:异常");

		}
	};
	//---------------------自动绘制定位mark算子 End----------------//

	//! 接口

	namespace alg {
		_ALGLIBRARY_API int autoMark(const autoMarkInput& input, autoMarkOutput& output);
	}
	void FindAutoMark(HalconCpp::HObject ho_hImage, HalconCpp::HObject ho_hRegion, HalconCpp::HObject* ho_hMarkRegions, HalconCpp::HObject* Maybemark,
		HalconCpp::HTuple hv_UniqueScope, HalconCpp::HTuple hv_Accuracy, HalconCpp::HTuple MinArea, HalconCpp::HTuple MaxArea, HalconCpp::HTuple hv_MarkPosType,
		HalconCpp::HTuple* hv_tMarkChannels, HalconCpp::HTuple* hv_tMarkFinded);
}

#endif	// ZKHYPROJECTHUANAN_AUTOMARK_H
