
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_SAVEDETECTEDINFO_H
#define ZKHYPROJECTHUANAN_SAVEDETECTEDINFO_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------存储检测数据算子 BEGIN-----------------//

	struct saveDetectedInfoInput
	{
		void* infoStruct = NULL;		//待存储的信息

		char fileSavePath[MAX_PATH] = "";		//数据存储路径

		char productInfo[2048] = "";		//产品信息
		char environmentInfo[2048] = "";		//运行环境信息

		std::string inputInfo = "";		//待存储的信息

		alglib::core::AlgrithmParams algPara;		//算法参数

	};
	struct saveDetectedInfoOutput
	{

	};

	struct IntParamsSaveDetectedInfo//int型算法参数，独立出来方便使用默认构造函数
	{
		int iInfoType = 0;		// 存储的数据类型
		int iSaveModel = 0;		// 存储模式
	};
	struct DoubleParamsSaveDetectedInfo//double型算法参数，独立出来方便使用默认构造函数
	{
		double dTimeOutThreshold = 500;		// 算法超时设定
	};
	struct AlgParamSaveDetectedInfo
	{
		union
		{
			IntParamsSaveDetectedInfo block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsSaveDetectedInfo block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct   AlgParaMsgSaveDetectedInfo
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgSaveDetectedInfo()
		{
			msgs.push_back("iInfoType	$Int			$Combox	$0  				$存储的数据类型		$存储的数据类型，默认值0：￥二维码 ￥字符串	$10");
			msgs.push_back("iSaveModel	$Int			$Combox	$0  				$存储模式		$存储模式，默认值0：￥默认 ￥卷带包装机	$20");
			msgs.push_back("dTimeOutThreshold   $Double      $D_SpinBox	 $500.0  	 $算法超时设定(单位：ms)			 $默认值500.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常	$30");
		}
	};

	struct AlgErrMsgSaveDetectedInfo
	{
		std::vector<std::string> msgs;
		AlgErrMsgSaveDetectedInfo()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:算法超时");
			msgs.push_back("2:存储路径异常");
			msgs.push_back("3:文件打开失败");
			msgs.push_back("4:未知异常");
			msgs.push_back("5:输入数据为空");
		}
	};
	//---------------------存储检测数据算子 END-----------------//

	//! 接口

	namespace alg {
		
		//-----------------------------saveDetectedInfo-----------------------------//
		//函数说明：存储检测数据
		//saveDetectedInfoInput &input;				输入：待存储的数据，产品信息
		//saveDetectedInfoOutput &output; 			输出：
		_ALGLIBRARY_API int saveDetectedInfo(const saveDetectedInfoInput& input, saveDetectedInfoOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_SAVEDETECTEDINFO_H
