
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_SAVEPOINTSCLOUD_H
#define ZKHYPROJECTHUANAN_SAVEPOINTSCLOUD_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------------------------保存点云数据-----------------------------------------------------------------//
	//算子input结构体、output结构体
	struct SavePointsCloudInput
	{
		HalconCpp::HTuple pointCloudX;
		HalconCpp::HTuple pointCloudY;
		HalconCpp::HTuple pointCloudZ;
		std::vector<cv::Point3f> pointCloud;                 //点云数据
		std::string savePointsCloudFile = "";               //点云文件路径+文件名（加上后缀.ply）
	};

	struct SavePointsCloudOutput {

	};

	struct AlgErrMsgSavePointsCloud
	{
		std::vector<std::string> msgs;
		AlgErrMsgSavePointsCloud()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:保存点云文件失败");
		}
	};


	//! 接口

	namespace alg {
		
		//-----------------------------savePointsCloud------------------------------------------//
		//函数说明：保存点云文件
		//SavePointsCloudInput &input;				    输入：点云文件保存的路径及文件名
		//SavePointsCloudOutput &output:				输出：无
		_ALGLIBRARY_API int savePointsCloud(SavePointsCloudInput& input, SavePointsCloudOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_SAVEPOINTSCLOUD_H
