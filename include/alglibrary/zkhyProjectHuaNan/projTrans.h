
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_PROJTRANS_H
#define ZKHYPROJECTHUANAN_PROJTRANS_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------投影变换算子------------------------------------------//
	//Step1 算子input结构体、output结构体
	struct ProjTransInput
	{
		HalconCpp::HObject srcImg;                            //原图
		ProjTransMatInfo homMat2DInfo;     //在编写xml阶段，直接赋值为“投影变换函数”的输出，在软件启动之前，赋值为“加载投影变换函数”的输出
	};
	struct ProjTransOutput
	{
		HalconCpp::HObject dstImg; //变换后图像
		cv::Mat mdstImg;    //用于显示变换后图像
	};
	////Step3 算子参数描述
	struct AlgParaMsgProjTrans
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgProjTrans()
		{

		}
	};

	//Step4 异常返回值描述
	struct AlgErrMsgProjTrans
	{
		std::vector<std::string> msgs;
		AlgErrMsgProjTrans()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:投影变换转换失败");
			msgs.push_back("3:算法未知异常");
		}
	};

	//! 接口

	namespace alg {
		//------------------------------------------投影变换 projectiveTrans 20221115----------------------------------//
		//名称：  projectiveTrans
		//功能：  对输入图像进行投影变换
		//0-输入参数 1-输出参数
		//返回值：0-正常
		_ALGLIBRARY_API int projTrans(ProjTransInput& input, ProjTransOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_PROJTRANS_H
