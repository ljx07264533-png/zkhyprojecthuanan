
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_LOADMARKTEACHINFO_H
#define ZKHYPROJECTHUANAN_LOADMARKTEACHINFO_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-------------------------------   Mark 加载模板数据结构   ---------------------------------------//
	struct LoadMarkTemplateInput
	{
		std::string path;					// 路径
	};
	struct LoadMarkTemplateOutput
	{
		HalconCpp::HTuple hv_teachDictHandle;		// Mark模板
	};//Step4 异常返回值描述
	struct AlgErrMsgLoadMarkTemplate
	{
		std::vector<std::string> msgs;
		AlgErrMsgLoadMarkTemplate()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:加载模型为空，请检查模型是否创建或检查路径");
			msgs.push_back("2:未知异常，请检查路径");
		}
	};

	//! 接口

	namespace alg {
		
		//函数说明：加载mark建模时保存的文件
		//string path:							输入：路径
		//HalconCpp::HTuple* hv_teachDictHandle:			输出：H字典
		_ALGLIBRARY_API int LoadMarkTeachInfo(LoadMarkTemplateInput input, LoadMarkTemplateOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_LOADMARKTEACHINFO_H
