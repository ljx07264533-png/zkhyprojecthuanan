
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_LOADPOINTSCLOUD_H
#define ZKHYPROJECTHUANAN_LOADPOINTSCLOUD_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------------------------加载点云数据-----------------------------------------------------------------//
	//算子input结构体、output结构体
	struct LoadPointsCloudInput
	{
		std::string loadPointsCloudFile = "";    //点云文件路径
	};

	struct LoadPointsCloudOutput {
		HalconCpp::HTuple pointCloudX;
		HalconCpp::HTuple pointCloudY;
		HalconCpp::HTuple pointCloudZ;
		std::vector<cv::Point3f> pointCloud;                 //点云数据
	};
	struct AlgErrMsgLoadPointsCloud
	{
		std::vector<std::string> msgs;
		AlgErrMsgLoadPointsCloud()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:加载点云文件失败");
		}
	};

	//! 接口

	namespace alg {
		
		//-----------------------------loadPointsCloud------------------------------------------//
		//函数说明：加载点云文件
		//LoadPointsCloudInput &input;				    输入：点云文件路径及文件名
		//LoadPointsCloudOutput &output:				输出：深度图和显示图
		_ALGLIBRARY_API int loadPointsCloud(LoadPointsCloudInput& input, LoadPointsCloudOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_LOADPOINTSCLOUD_H
