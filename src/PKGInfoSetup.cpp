#include "alglibrary/zkhyProjectHuaNan/PKGInfoSetup.h"
#include "alglibrary/zkhyProjectHuaNan/BGAInfoSetup.h"
#include "alglibrary/zkhyProjectHuaNan/QFNInfoSetup.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace cv;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

}

namespace alglib::ops::zkhyProHN::alg {

	//-----------------------------PKGInformationSetup------------------------------------------//
	//功能：PKG信息参数设置,生成BGA、QFN等型号的排布
	int PKGInfoSetup(const PKGInfoSetupInput& PKGinput, PKGInfoSetupOutput& PKGoutput)
	{
		//调用封测模块算子
		if (PKGinput.funID == 0) {
			BGASetupInput BGAInput;
			BGASetupOutput BGAOutput;
			BGAInput.alg = PKGinput.alg;
			BGAInput.inputPoints = PKGinput.inputPoints;
			int ret0 = BGAInfoSetup(BGAInput, BGAOutput);
			if (ret0 != 0)
				return PKGinput.funID * 100 + ret0;
			PKGoutput.showImg = BGAOutput.showImg;
			PKGoutput.BGAParam = BGAOutput.BGAParam;
			PKGoutput.inputPoints = BGAOutput.inputPoints;
		}
		else if (PKGinput.funID == 1) {
			QFNSetupInput QFNInput;
			QFNSetupOutput QFNOutput;
			QFNInput.alg = PKGinput.alg;
			QFNInput.inputPoints = PKGinput.inputPoints;
			int ret0 = QFNInfoSetup(QFNInput, QFNOutput);
			if (ret0 != 0)
				return PKGinput.funID * 100 + ret0;
			PKGoutput.showImg = QFNOutput.showImg;
			PKGoutput.QFNParam = QFNOutput.QFNParam;
			PKGoutput.inputPoints = QFNOutput.inputPoints;
		}
		else
			return 9999;
		return 0;
	}

}
