#include "alglibrary/zkhyProjectHuaNan/getPKGInfoSetupItemList.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

}

namespace alglib::ops::zkhyProHN::alg {
	//-------------------------------------------------getPKGInfoSetupItemList---------------------------------------------//
	//名称：  getPKGInfoSetupItemList
	//功能：  获取封测处理模块算子列表（名称及标识）
	//返回值：0：成功
	//        1：获取封测处理模块算子列表为空
	//------------------------------------------------------------------------------------------------------------------------//
	int getPKGInfoSetupItemList(PKGInfoSetupItemV& itemV)
	{
		vector <PKGInfoSetupItem >().swap(itemV);
		PKGInfoSetupItem  item;

		item.name = "BGA";
		item.funID = 0;
		itemV.push_back(item);

		item.name = "QFN";
		item.funID = 1;
		itemV.push_back(item);

		return 0;
	}

}
