
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_LOADPROJTRANSMAT_H
#define ZKHYPROJECTHUANAN_LOADPROJTRANSMAT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------加载投影变换矩阵------------------------------------------//
	//Step1 算子input结构体、output结构体
	struct LoadProjTransMatInput {
		SaveProjTransMatPath loadPath;        //在创建投影变换函数的路径输入
	};
	struct LoadProjTransMatOutput {
		ProjTransMatInfo transHomMat2DInfo;   //创建的投影变换矩阵
	};
	struct AlgErrMsgLoadProjTransMat
	{
		std::vector<std::string> msgs;
		AlgErrMsgLoadProjTransMat()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:加载文件失败");
			msgs.push_back("2:算法未知异常");
		}
	};

	//! 接口

	namespace alg {
		
		//------------------------------------------投影变换 loadProjectiveTransHomMat2D 20221115----------------------------------//
		//名称：  loadProjectiveTransHomMat2D
		//功能：  加载投影变换矩阵
		//0-输入参数 1-输出参数
		//返回值：0-正常
		_ALGLIBRARY_API int loadProjTransMat(const LoadProjTransMatInput& input, LoadProjTransMatOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_LOADPROJTRANSMAT_H
