
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_SELECTREGION_H
#define ZKHYPROJECTHUANAN_SELECTREGION_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//————————————————————————MuraSelectPanel mura Begin———————————
	struct SelectRegionInput
	{
		std::string               ID12;             //输入ID
		std::vector<PanelRegion>     pRegion;      //panel区域
	};
	struct SelectRegionOutput
	{
		HalconCpp::HObject     sRegion; //选择的panel区域
		std::vector<std::vector<cv::Point2f>> contours;//选择的panel布局轮廓显示
	};
	struct AlgErrMsgSelectMuraRegion
	{
		std::vector<std::string> msgs;
		AlgErrMsgSelectMuraRegion()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:ID输入为空");
			msgs.push_back("2:输入区域为空");
			msgs.push_back("3:未找到对应区域");
			msgs.push_back("4:未知异常");
		}
	};

	//! 接口

	namespace alg {
		_ALGLIBRARY_API int selectRegion(const SelectRegionInput& input, SelectRegionOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_SELECTREGION_H
