
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_GETPKGINFOSETUPITEMLIST_H
#define ZKHYPROJECTHUANAN_GETPKGINFOSETUPITEMLIST_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	struct PKGInfoSetupItem
	{
		std::string name;   //PKG检测类型的名称
		int funID = 0; //算子标识（BGA、QFN、BGA3D）
	};
	typedef std::vector<PKGInfoSetupItem> PKGInfoSetupItemV;

	//! 接口

	namespace alg {
		//-------------------------------------------------getPKGInfoSetupItemList---------------------------------------------//
		//名称：  getPKGInfoSetupItemList
		//功能：  获取PKG处理模块算子列表（名称及标识）
		//返回值：0：成功
		//        1：获取PKG处理模块算子列表为空
		//------------------------------------------------------------------------------------------------------------------------//
		_ALGLIBRARY_API int getPKGInfoSetupItemList(PKGInfoSetupItemV& itemV);
	}

}

#endif	// ZKHYPROJECTHUANAN_GETPKGINFOSETUPITEMLIST_H
