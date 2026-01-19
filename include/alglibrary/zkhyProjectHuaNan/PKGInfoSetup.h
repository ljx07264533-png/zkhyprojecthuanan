
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_PKGINFOSETUP_H
#define ZKHYPROJECTHUANAN_PKGINFOSETUP_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	struct PKGInfoSetupInput
	{
		alglib::core::AlgrithmParams alg;               //算法参数
		int funID;                        //算子标识
		std::vector<cv::Point2f> inputPoints;  //软件输入中心点坐标，物理坐标
	};
	struct PKGInfoSetupOutput
	{
		cv::Mat showImg;     //排布后的显示图
		BGAParam BGAParam;//BGA参数设置
		QFNParam QFNParam;//QFN参数设置
		std::vector<cv::Point2f> inputPoints;  //中心点坐标，物理坐标
	};

	//! 接口

	namespace alg {
		_ALGLIBRARY_API	int PKGInfoSetup(const PKGInfoSetupInput& input, PKGInfoSetupOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_PKGINFOSETUP_H
