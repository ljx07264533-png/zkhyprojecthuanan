#include "alglibrary/zkhyProjectHuaNan/LoadMarkTeachInfo.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

}

namespace alglib::ops::zkhyProHN::alg {
	int LoadMarkTeachInfo(LoadMarkTemplateInput input, LoadMarkTemplateOutput& output)
	{

		try
		{
			HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
			SetSystem("clip_region", "false");
			ReadDict((HTuple)input.path.c_str(), HTuple(), HTuple(), &output.hv_teachDictHandle);
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 2; //Î´Öª´íÎó
		}

		HTuple  Key, GenParamValue;
		GetDictParam(output.hv_teachDictHandle, "keys", HTuple(), &GenParamValue);
		double keysLen = GenParamValue.TupleLength().D();
		if (keysLen < 1.0)
			return 1;

		return 0;

	}

}
