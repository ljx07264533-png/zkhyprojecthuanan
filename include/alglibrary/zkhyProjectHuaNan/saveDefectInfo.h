
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_SAVEDEFECTINFO_H
#define ZKHYPROJECTHUANAN_SAVEDEFECTINFO_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//------与平台3.0共用的接口定义-------//
	struct saveDefectInfoInput
	{
		Param_Wafer_DefectSummaryAlg defectSummary;
		alglib::core::AlgrithmParams algPara;
	};
	struct saveDefectInfoOutput
	{
		std::vector<alglib::core::FlawInfo> flawsFilterInfo;//筛选后的缺陷信息
		HalconCpp::HObject defectRegion;   //筛选后的缺陷区域信息
	};

	struct AlgParaSaveDefectInfo
	{
		union
		{
			struct
			{
				int flagShow;
			}block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			struct
			{
				double a;
				double b;
				double c;
				double d;
				double e;
			}block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct   AlgParaMsgSaveDefectInfo
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgSaveDefectInfo()
		{
			msgs.push_back("flagShow          $Int      $CheckBox	 $0  	 $是否显示缺陷信息	     $运行过程中建议关闭，离线调试时可打开看筛选后的缺陷信息 	$10");
			msgs.push_back("a          $Double      $D_SpinBox	 $0  	 $是否显示缺陷信息	     $三阶系数 	$20");
			msgs.push_back("b          $Double      $D_SpinBox	 $0  	 $是否显示缺陷信息	     $二阶系数 	$30");
			msgs.push_back("c          $Double      $D_SpinBox	 $1  	 $是否显示缺陷信息	     $一次系数 	$40");
			msgs.push_back("d          $Double      $D_SpinBox	 $0  	 $是否显示缺陷信息	     $对数系数 	$50");
			msgs.push_back("e          $Double      $D_SpinBox	 $0  	 $是否显示缺陷信息	     $常数 	$60");
		}
	};

	struct AlgErrMsgSaveDefectInfo
	{
		std::vector<std::string> msgs;
		AlgErrMsgSaveDefectInfo()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:链接的筛选后缺陷个数大于筛选前的缺陷个数");
			msgs.push_back("2:未知异常");
			msgs.push_back("3:文件夹创建失败");
		}
	};
	//——————————————存储缺陷信息算子 0830 END-----------------//	


	//! 接口

	namespace alg {
		
		//----------------------------- saveDefectInfo-----------------------------------------//
		//函数说明：存储缺陷信息
		//saveDefectInfoInput &input;				输入：原始的和筛选后的缺陷信息
		//saveDefectInfoOutput &output; 			输出：筛选后的缺陷区域以及存储的hdict文件
		_ALGLIBRARY_API int saveDefectInfo(const saveDefectInfoInput& input, saveDefectInfoOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_SAVEDEFECTINFO_H
