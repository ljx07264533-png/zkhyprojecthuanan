
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_GENAREAFLATFIELDCORREFACTOR_H
#define ZKHYPROJECTHUANAN_GENAREAFLATFIELDCORREFACTOR_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//————————————————————————genAreaFlatFieldCorrFactor 生成面阵平场校正因子 Begin————————————————//
	//Step1 输入结构体
	struct GenAreaFlatFieldCorrFactorInput
	{
		HalconCpp::HObject srcImg;                         //平场校正板
		std::string path;                            //存储校正权重图像的路径 (
		alglib::core::AlgrithmParams algPara;					//算法参数
	};
	//Step2 输出结构体
	struct GenAreaFlatFieldCorrFactorOutput
	{
		HalconCpp::HObject weightImg;                         //平场校正权重图
	};
	//Step3 算法参数（int double）
	struct AlgParaGenAreaFlatFactorInt          //int型算法参数，独立出来方便使用默认构造函数
	{
		int filterW;         //滤波核尺寸
	};
	struct AlgParaGenAreaFlatFactor
	{
		union
		{
			AlgParaGenAreaFlatFactorInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			struct
			{
			}block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgGenAreaFlatFactor
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgGenAreaFlatFactor()
		{
			msgs.push_back("filterW	$Int       $I_Spinbox	 $128  	 $均值滤波核窗口		$平场校正均值滤波核窗口尺寸，尺寸越大，平场校正效果越明显				$1");
		}
	};
	//Step4 异常返回值描述
	struct AlgErrMsgGenAreaFlatFactor
	{
		std::vector<std::string> msgs;
		AlgErrMsgGenAreaFlatFactor()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:原图尺寸必须大于滤波核的两倍");
			msgs.push_back("3: 算法异常");
		}
	};
	//————————————————————————genAreaFlatFieldCorrFactor 生成面阵平场校正因子 End————————————————//

	//! 接口

	namespace alg {
		
		//——————————————生成面阵相机平场校正因子——————————————//
		//函数说明：通过输入的标定板进行面阵相机的平场校正因子的计算
		//GenAreaFlatFieldCorrFactorInput &input;		输入： 灰阶校正板、算法参数
		//SideAlignOutput &output; 			            输出： 平场校正权重因子
		_ALGLIBRARY_API int genAreaFlatFieldCorreFactor(const GenAreaFlatFieldCorrFactorInput& input, GenAreaFlatFieldCorrFactorOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_GENAREAFLATFIELDCORREFACTOR_H
