#include "alglibrary/zkhyProjectHuaNan/savePointsCloud.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	//-----------------------------realLoadPointsCloud------------------------------------------//
	//函数说明：保存点云文件
	//SavePointsCloudInput &input;				    输入：点云文件保存的路径及文件名
	//SavePointsCloudOutput &output:				输出：无
	int realSavePointsCloud(SavePointsCloudInput& input, SavePointsCloudOutput& output)
	{
		// Local control variables
		HTuple x, y, z;
		//points3D2HTuple(input.pointCloud, x, y, z);	
		HTuple  hv_ObjectModel3D;
		//根据输入点云坐标生成点云3D模型
		x = input.pointCloudX;
		y = input.pointCloudY;
		z = input.pointCloudZ;
		GenObjectModel3dFromPoints(x, y, z, &hv_ObjectModel3D);

		const char* pImageName = input.savePointsCloudFile.c_str();
		HTuple hv_path;
		hv_path = (HTuple)(pImageName);
		//保存点云数据	
		WriteObjectModel3d(hv_ObjectModel3D, "ply", hv_path, HTuple(), HTuple());

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int savePointsCloud(SavePointsCloudInput& input, SavePointsCloudOutput& output)
	{
		try
		{
			return realSavePointsCloud(input, output);
		}
		catch (...)
		{
			return 1; //保存文件失败
		}
	}
}
