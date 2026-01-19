#include "alglibrary/zkhyProjectHuaNan/loadModel.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	int realLoadModel(const LoadModelInput& input, LoadModelOutput& output)
	{
		//SetSystem("border_shape_models", "true");
		//SetSystem("clip_region", "false");
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);

		if (input.loadPath1.save_modelRow == "" || input.loadPath1.saveModelIDpath == "" || input.loadPath1.FindRegionROI == "" || input.loadPath1.saveDModelImgPath == "")
			return 3;
		//模板图像row
		HTuple saveContourpath = input.loadPath1.save_modelRow.c_str();
		ReadTuple(saveContourpath, &output.shapeInfo1.hv_modelRow3);
		HTuple hv_modelRow = output.shapeInfo1.hv_modelRow3;
		//模板图像col
		HTuple saveContourpath1 = input.loadPath1.save_modelCol.c_str();
		ReadTuple(saveContourpath1, &output.shapeInfo1.hv_modelColumn3);
		HTuple hv_modelColumn = output.shapeInfo1.hv_modelColumn3;
		//模板句柄1
		HTuple SaveModelIDpath;
		std::string path = input.loadPath1.saveModelIDpath;
		int pos = path.find_last_of('.');
		if (pos != string::npos) { // 找到了"."，即文件后缀名
			path = path.substr(0, pos) + ".hdict"; // 替换为.hdict
		}
		SaveModelIDpath = path.c_str();
		ReadDict(SaveModelIDpath, HTuple(), HTuple(), &output.shapeInfo1.modelID);
		//模板句柄2

		/*HTuple SaveModelID2path = input.loadPath1.saveModelID2path.c_str();
		ReadShapeModel(SaveModelID2path, &output.shapeInfo1.modelID2);*/

		HTuple  SaveBModelImgPath = (input.loadPath1.saveBModelImgPath + ".bmp").c_str();
		ReadImage(&output.shapeInfo1.BmodelImg, SaveBModelImgPath);
		HTuple  SaveDModelImgPath = (input.loadPath1.saveDModelImgPath + ".bmp").c_str();
		ReadImage(&output.shapeInfo1.DmodelImg, SaveDModelImgPath);

		HTuple Save_Cregions1 = input.loadPath1.FindRegionROI.c_str();
		ReadRegion(&output.shapeInfo1.findRegionROI, Save_Cregions1);
		HObject Cregions = output.shapeInfo1.findRegionROI;
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------loadModel--------------------------//
    //功能：加载创建模型函数入口
	int loadModel(const LoadModelInput& input, LoadModelOutput& output)
	{
		try
		{
			return realLoadModel(input, output);
		}
		catch (...)
		{
			return 5; //算法内部异常
		}
	}
}
