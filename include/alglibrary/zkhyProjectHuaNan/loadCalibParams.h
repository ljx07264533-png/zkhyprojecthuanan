
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_LOADCALIBPARAMS_H
#define ZKHYPROJECTHUANAN_LOADCALIBPARAMS_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------加载参数------------------------------------------//
	//Step1算子input结构体、output结构体
	struct LoadCalibParamsInput
	{
		SaveCalibFilesPath loadPath;              //校正Map图存储路径
	};
	struct LoadCalibParamsOutput
	{
		CalibFilesInfo calibFilesInfo;            //输出校正Map图
	};
	struct AlgErrMsgLoadCalibParams
	{
		std::vector<std::string> msgs;
		AlgErrMsgLoadCalibParams()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:读取畸变校正Map图失败");
			msgs.push_back("2:读取透视变换Map图失败");
			msgs.push_back("3:算法未知异常");

		}
	};
	//! 接口

	namespace alg {
		
		//------------------------------------------加载参数 loadCalibParams 20221123-------------------------//
		//名称：  loadCalibParams
		//功能：  加载标定内参和外参
		//0-输入参数 1-输出参数
		_ALGLIBRARY_API int loadCalibParams(LoadCalibParamsInput& input, LoadCalibParamsOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_LOADCALIBPARAMS_H
