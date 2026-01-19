
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_LODSEGEMENT_H
#define ZKHYPROJECTHUANAN_LODSEGEMENT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------加载分割图片差异化模型 Start----------------//
	
	struct LodSegementInput
	{
		cv::String LodSegmentImgPath;
		alglib::core::AlgrithmParams algPara;					//算法参数

	};
	struct  LodSegementOutput
	{
		SegmentShap Segment;

	};

	struct IntParamsLodSegement//int型算法参数，独立出来方便使用默认构造函数
	{
		int mask;
		int Thre;
		int flg_Grd;
	};
	struct DoubleParamsLodSegement//double型算法参数，独立出来方便使用默认构造函数
	{

	};
	struct AlgParamLodSegement
	{
		union
		{
			IntParamsLodSegement block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsLodSegement block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};


	struct AlgParaMsgLodSegement
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgLodSegement()
		{
			msgs.push_back("mask	$Int			$I_Spinbox	$4  		    $模板图腐蚀膨胀大小		$模板图腐蚀膨胀大小	，范围[1,10]	$10");
			msgs.push_back("Thre	$Int			$I_Spinbox	$128  			$检测区域阈值		$检测区域阈值，范围[1,255]	$20");
			msgs.push_back("flg_Grd	$Int			$I_Spinbox	1  			    $是否开启网格检测   $检测区域阈值，范围[0,1]	$30");

		}
	};

	struct AlgErrMsgLodSegement
	{
		std::vector<std::string> msgs;
		AlgErrMsgLodSegement()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:异常");
			msgs.push_back("3:异常");
			msgs.push_back("4:异常");
			msgs.push_back("5:异常");
		}
	};

	//! 接口

	namespace alg {
		_ALGLIBRARY_API int lodSegement(const LodSegementInput& input, LodSegementOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_LODSEGEMENT_H
