#include "alglibrary/zkhyProjectHuaNan/loadPointsCloud.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

	//-----------------------------realLoadPointsCloud------------------------------------------//
	//函数说明：加载点云文件
	//LoadPointsCloudInput &input;				    输入：点云文件路径及文件名
	//LoadPointsCloudOutput &output:				输出：深度图和显示图
	int realLoadPointsCloud(LoadPointsCloudInput& input, LoadPointsCloudOutput& output)
	{
		// Local iconic variables

		// Local control variables
		HTuple  hv_ObjectModel3D, hv_Status;
		HTuple  hv_point_coord_x1, hv_point_coord_y1, hv_point_coord_z1, hv_Index;

		const char* pImageName = input.loadPointsCloudFile.c_str();
		HTuple hv_path;
		hv_path = (HTuple)(pImageName);

		ReadObjectModel3d(hv_path, "mm", "convert_to_triangles",
			"true", &hv_ObjectModel3D, &hv_Status);
		//ReadObjectModel3d("C:/Users/yi.qiu/Desktop/8080.ply", "mm", "convert_to_triangles",
		//	"true", &hv_ObjectModel3D, &hv_Status);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_x", &hv_point_coord_x1);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_y", &hv_point_coord_y1);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_z", &hv_point_coord_z1);

		output.pointCloudX = hv_point_coord_x1;
		output.pointCloudY = hv_point_coord_y1;
		output.pointCloudZ = hv_point_coord_z1;

		vector<cv::Point3f> points;
		zkhyPublicFuncHN::hTuple2Points3D(hv_point_coord_x1, hv_point_coord_y1, hv_point_coord_z1, points);
		output.pointCloud = points;

		return 0;
	}



}

namespace alglib::ops::zkhyProHN::alg {
	int loadPointsCloud(LoadPointsCloudInput& input, LoadPointsCloudOutput& output)
	{
		try
		{
			return realLoadPointsCloud(input, output);
		}
		catch (...)
		{
			return 1; //加载文件失败
		}
	}
}
