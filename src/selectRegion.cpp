#include "alglibrary/zkhyProjectHuaNan/selectRegion.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

	//通过前边的得到的区域和对应ID对区域进行选择，选择对应的区域进行输出。
	int realSelectRegion(const SelectRegionInput& input, SelectRegionOutput& output)
	{
		try
		{
			string ID = input.ID12;
			//output.img = input.img;
			if (ID.empty())
			{
				return 1;
			}
			vector<PanelRegion> pRegion = input.pRegion;
			if (pRegion.size() < 1)
			{
				return 2;
			}
			for (int i = 0; i < pRegion.size(); i++)
			{
				PanelRegion sRegion = pRegion[i];
				//Point2f topleft = sRegion.topleft;
				if ((sRegion.ID) == ID)
				{

					output.contours.push_back(sRegion.rPts);
					output.sRegion = sRegion.region;

				}
			}
		}
		catch (const std::exception&)
		{
			return 3;
		}
		return 0;
	}



}

namespace alglib::ops::zkhyProHN::alg {
	int selectRegion(const SelectRegionInput& input, SelectRegionOutput& output)
	{
		try
		{
			return realSelectRegion(input, output);
		}
		catch (...)
		{
			return 3; //算法内部异常
		}
	}
}
