
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_IMAGESCOMBINE_H
#define ZKHYPROJECTHUANAN_IMAGESCOMBINE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------ImagesCombine 图像组合 算子------------------------------------------//
	struct ImagesCombineInput
	{
		HalconCpp::HObject imageCurrent;               //当前图像
		HalconCpp::HObject imageAux;                   //辅助图像
		alglib::core::AlgrithmParams algPara;				//算法参数
	};
	struct ImagesCombineOutput
	{
		HalconCpp::HObject hImageCombined;            //组合结果，H图
		cv::Mat imageCombined;                 //组合结果，Mat图，显示用
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct IntParamsImagesCombine//int型算法参数，独立出来方便使用默认构造函数
	{
		int iCombineOperate = 0;           //组合操作方法
	};
	struct DoubleParamsImagesCombine//double型算法参数，独立出来方便使用默认构造函数
	{
		double dMultFactor = 1.0;                   //乘性系数
		double dAddValue = 0;                     //加性系数
		double dTimeOutThreshold = 3000;      //算法超时设定
	};
	struct AlgParamImagesCombine
	{
		union
		{
			IntParamsImagesCombine block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsImagesCombine block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgImagesCombine
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgImagesCombine()
		{
			msgs.push_back("iCombineOperate      $Int  $Combox   $0   $操作类型                      $默认为0，0-两图相加，1-两图相减，2-两图相乘，3-两图相与，4-两图相或 ￥加　￥减 ￥乘 ￥与 ￥或 $10");
			msgs.push_back("dMultFactor				$ Double $ D_SpinBox	$ 1.0	$ 乘性系数			$ 默认值1.0，范围：[-255,+255]。选择图像相加操作时推荐使用0.5，图像相减操作时推荐使用1.0，图像相乘时推荐使用0.005，其他操作不涉及该系数 $20");
			msgs.push_back("dAddValue				$ Double $ D_SpinBox	$ 0	$ 加性系数			$ 默认值0，范围：[-512,512]。选择图像相加操作时推荐使用0，图像相减操作时推荐使用128.0，图像相乘时推荐使用0，其他操作不涉及该系数 $30");
			msgs.push_back("dTimeOutThreshold      $Double  $D_Spinbox   $3000.0   $算法超时设定(单位：ms)                      $默认值3000.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常 $40");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgImagesCombine
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgImagesCombine() {
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgImagesCombine
	{
		std::vector<std::string> msgs;
		AlgErrMsgImagesCombine()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入当前图像为空");
			msgs.push_back("2:输入辅助图像为空");
			msgs.push_back("3:算法超时参数输入有误");
			msgs.push_back("4:乘性系数超出范围");
			msgs.push_back("5:加性系数超出范围");
			msgs.push_back("6:输入图像通道不一致");
			msgs.push_back("7:输入图像尺寸不一致");
			msgs.push_back("8:算法超时");
			msgs.push_back("9:未知错误");
		}
	};

	//! 接口

	namespace alg {
		
		//-----------------------------ImagesCombine----------------------------------------//
		//函数说明：对两幅输入图像进行相加、相减、与、或等操作
		//ImagesCombineInput &input;			输入：两幅需要进行操作的图像、算法参数
		//ImagesCombineOutput &output; 			输出：图像组合后的结果（Mat图和H图）
		_ALGLIBRARY_API int ImagesCombine(const ImagesCombineInput& input, ImagesCombineOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_IMAGESCOMBINE_H
