#include <iostream>
#include <math.h>
#include <direct.h>
#include <io.h>
#include <filesystem>

#include "alglibrary/zkhyProjectHuaNan/saveDefectInfo.h"
#include "alglibrary/zkhyProjectHuaNan/utils.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace cv;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	int realsaveDefectInfo(const saveDefectInfoInput& input, saveDefectInfoOutput& output)
	{
#ifdef DLP_DEBUG
		auto  timeD2HAll = cv::getTickCount();
#endif
		if (input.defectSummary.flaws.size() > input.defectSummary.flawsFilter.size())
			return 1;
		HTuple filterName = HTuple();
		//Step1-获取缺陷父类个数
		// Device->Host
#ifdef DLP_DEBUG
		auto timeD2H = cv::getTickCount();
#endif


		//int numdefect = 0;
		for (int i = 0; i < input.defectSummary.flawsFilter.size(); i++) {
			for (int j = 0; j < input.defectSummary.flawsFilter[i]->size(); j++) {
				HTuple index;
				TupleFind(filterName, input.defectSummary.flawsFilter[i]->at(j).szCheck_Flaw_Group, &index);
				//numdefect += 1;
				if (index.Length() == 0 || index == -1)   // 未添加过的缺陷类型
					filterName.Append(input.defectSummary.flawsFilter[i]->at(j).szCheck_Flaw_Group);
			}
		}

#ifdef DLP_DEBUG

		auto timeInferUsed = (cv::getTickCount() - timeD2H) * 1000 / cv::getTickFrequency();
		OutputDebugStringA(std::string("__waferINFO_TupleFind(filterName: " + std::string(std::to_string(timeInferUsed)) + "\n").c_str());
#endif
		int numFilter = filterName.Length();

		if (numFilter == 0)
			return 0;
		if (input.defectSummary.SdefectRegion.size() != input.defectSummary.flawsFilter.size())
			return 1;
		vector<HObject> defectRegion(numFilter);
		vector<vector<Hlong>> defectRegionTuple(numFilter);
		//目前存储3个特征：长、宽、面积尺寸特征
		vector<vector<HTuple>> feature(numFilter, vector<HTuple>(10));
		//Step2-统计缺陷区域信息
		HObject outputRegion;
		GenEmptyObj(&outputRegion);
		AlgParaSaveDefectInfo* alg = (AlgParaSaveDefectInfo*)&input.algPara;
		int flagShow = alg->i_params.block.flagShow;
		//新增系数

		double a = alg->d_params.block.a;
		double b = alg->d_params.block.b;
		double c = alg->d_params.block.c;
		double d = alg->d_params.block.d;
		double e = alg->d_params.block.e;
#ifdef DLP_DEBUG
		timeD2H = cv::getTickCount();
#endif
		// flaws 和 flawsFilter是算子级别，级联算子链接了几个获取缺陷特征算子，vector就有几个
		int numAlg = input.defectSummary.flawsFilter.size();
		vector<HTuple> hv_SurrogateTuple(numAlg);
		try {
			for (int i = 0; i < input.defectSummary.flawsFilter.size(); i++) {
				if (input.defectSummary.flawsFilter[i]->size() > 0) {
					//WriteObject(input.defectSummary.SdefectRegion[i], "D:/test");
					ObjToInteger(*input.defectSummary.SdefectRegion[i], 1, -1, &hv_SurrogateTuple[i]);
					/*HObject region;
					IntegerToObj(&region, hv_SurrogateTuple[i]);*/
				}


			}
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();

		}
		for (int i = 0; i < input.defectSummary.flawsFilter.size(); i++) {
			//HObject defectRegions = *input.defectSummary.SdefectRegion[i];
			for (int j = 0; j < input.defectSummary.flawsFilter[i]->size(); j++) {
				//获取缺陷父分类的索引
				int defectIndex = input.defectSummary.flawsFilter[i]->at(j).nResult_Index;  //缺陷索引
				// 功能：判断是否跟上一个缺陷的缺陷索引一致，如果是一致，将会跳过，实现一个获取缺陷特征算子输出的缺陷，流出唯一一个缺陷区域；
				if (j > 0)
				{
					if (defectIndex == input.defectSummary.flawsFilter[i]->at(j - 1).nResult_Index)
						continue;
				}

				HTuple indexT;
				TupleFind(filterName, input.defectSummary.flawsFilter[i]->at(j).szCheck_Flaw_Group, &indexT);
				int index = indexT.I();
				if (index == -1)
					return 2;
				/*if (!defectRegion[index].IsInitialized())
					GenEmptyObj(&defectRegion[index]);*/
					//生成缺陷区域
				HObject defectR;
				vector<vector<Point2f>> inputContours = input.defectSummary.flaws[i]->at(defectIndex).contours;
				//从缺陷区域中取出筛选后的缺陷区域
				//SelectObj(defectRegions, &defectR, defectIndex + 1);
				//zkhyPublicFuncHN::genRegionFromContour(inputContours, &defectR);
				double length, width, area;
				//新增获取范围内最大区域
				if (strcmp(input.defectSummary.flawsFilter[i]->at(j).szCheck_Flaw_Group, "particle") == 0) {
					/*HTuple region_features, new_len1, new_len2;
					SelectShapeStd(defectR, &defectR, "max_area", 70);
					RegionFeatures(defectR, ((HTuple("area").Append("width")).Append("height")), &region_features);

					length = max(region_features[1], region_features[2]);
					length = a * (length*length*length) + b * (length *length) + c * length + d * log(length) + e;
					width = min(region_features[1], region_features[2]);
					area = region_features[0];*/

				}
				else
				{
					//Union1(defectR, &defectR);
					length = input.defectSummary.flaws[i]->at(defectIndex).rect2Len1;
					width = input.defectSummary.flaws[i]->at(defectIndex).rect2Len2;
					area = input.defectSummary.flaws[i]->at(defectIndex).area;
				}
				//ConcatObj(defectRegion[index], defectR, &defectRegion[index]);
				//替换为：将第i个算子的 区域进行赋值
				//defectRegionTuple[index] = defectRegionTuple[index].TupleConcat(hv_SurrogateTuple[i][defectIndex]);
				defectRegionTuple[index].push_back(hv_SurrogateTuple[i][defectIndex]);
				Point2f pts = input.defectSummary.flaws[i]->at(defectIndex).pos;
				double angle = input.defectSummary.flaws[i]->at(defectIndex).d_params.block.rect2Phi;
				double grayMean = input.defectSummary.flaws[i]->at(defectIndex).d_params.block.grayMean;
				double grayMax = input.defectSummary.flaws[i]->at(defectIndex).d_params.block.graymax;
				double grayMin = input.defectSummary.flaws[i]->at(defectIndex).d_params.block.grayMin;
				double meangraydiff2 = input.defectSummary.flaws[i]->at(defectIndex).d_params.block.meangraydiff2;

				feature[index][0].Append(pts.x);
				feature[index][1].Append(pts.y);
				feature[index][2].Append(length);
				feature[index][3].Append(width);
				feature[index][4].Append(area);
				feature[index][5].Append(angle);
				feature[index][6].Append(grayMean);
				feature[index][7].Append(grayMax);
				feature[index][8].Append(grayMin);
				feature[index][9].Append(meangraydiff2);
				if (flagShow) {
					ConcatObj(outputRegion, defectR, &outputRegion);
					output.flawsFilterInfo.push_back(input.defectSummary.flaws[i]->at(defectIndex));
				}
			}
		}
		if (numFilter != defectRegion.size())
			return 1;
		for (int i = 0; i < numFilter; i++)
		{
			HTuple tTemp = HTuple(&defectRegionTuple[i][0], defectRegionTuple[i].size());
			IntegerToObj(&defectRegion[i], tTemp);
		}


#ifdef DLP_DEBUG
		timeInferUsed = (cv::getTickCount() - timeD2H) * 1000 / cv::getTickFrequency();
		OutputDebugStringA(std::string("__waferINFO_main: " + std::string(std::to_string(timeInferUsed)) + "\n").c_str());
#endif
#ifdef DLP_DEBUG
		timeD2H = cv::getTickCount();
#endif
		// Step3-创建区域字典 以及特征字典
		HTuple dictHandel, dictHande2, featureHanlde;
		CreateDict(&dictHandel);
		CreateDict(&dictHande2);
		CreateDict(&featureHanlde);
#ifdef DLP_DEBUG
		timeInferUsed = (cv::getTickCount() - timeD2H) * 1000 / cv::getTickFrequency();
		OutputDebugStringA(std::string("Time of creatdict:" + std::to_string(timeInferUsed) + "ms.").c_str());
#endif
#ifdef DLP_DEBUG
		timeD2H = cv::getTickCount();
#endif
		for (int i = 0; i < numFilter; i++) {
			//存储缺陷区域
			SetDictObject(defectRegion[i], dictHandel, filterName[i]);
			CreateDict(&featureHanlde);
			SetDictTuple(featureHanlde, "CenterPt_X", feature[i][0]);
			SetDictTuple(featureHanlde, "CenterPt_Y", feature[i][1]);
			SetDictTuple(featureHanlde, "Length", feature[i][2]);
			SetDictTuple(featureHanlde, "Width", feature[i][3]);
			SetDictTuple(featureHanlde, "Area", feature[i][4]);
			SetDictTuple(featureHanlde, "rect2Phi", feature[i][5]);
			SetDictTuple(featureHanlde, "grayMean", feature[i][6]);
			SetDictTuple(featureHanlde, "graymax", feature[i][7]);
			SetDictTuple(featureHanlde, "grayMin", feature[i][8]);
			SetDictTuple(featureHanlde, "meangraydiff2", feature[i][9]);
			SetDictTuple(dictHande2, filterName[i], featureHanlde);

		}
#ifdef DLP_DEBUG
		timeInferUsed = (cv::getTickCount() - timeD2H) * 1000 / cv::getTickFrequency();
		OutputDebugStringA(std::string("__waferINFO_featureHanlde: " + std::string(std::to_string(timeInferUsed)) + "\n").c_str());
#endif
		// Step4-存储区域字典
#ifdef DLP_DEBUG
		timeD2H = cv::getTickCount();
#endif
		char path[MAX_PATH], dirPath[MAX_PATH];
		int channel = input.defectSummary.waferInfo.nChannel;
		sprintf_s(dirPath, "%s/%02d/", input.defectSummary.waferInfo.szHdictPath, channel);
		std::filesystem::path path12(dirPath);
		if (!std::filesystem::exists(path12.parent_path()))
		{
			std::filesystem::create_directories(path12.parent_path());
		}
		/*
		* 旧的内容，训根、明辉、南南等写的，创建文件夹错误，改为上面的内容
		sprintf_s(dirPath, "%s/%02d/", input.defectSummary.waferInfo.szHdictPath, channel);
		int error = createDirectory(dirPath);
		if (error != 0)
			return 3;
		*/
		sprintf_s(path, "%s%02d%s%03d%s%03d", dirPath, channel, "_Row", input.defectSummary.waferInfo.nRow, "_Col", input.defectSummary.waferInfo.nCol);
		WriteDict(dictHandel, path, HTuple(), HTuple());
		// Step4-存储特征字典
		sprintf_s(path, "%s%02d%s%03d%s%03d%s", dirPath, channel, "_Row", input.defectSummary.waferInfo.nRow, "_Col", input.defectSummary.waferInfo.nCol, "_feature");

		WriteDict(dictHande2, path, HTuple(), HTuple());
		output.defectRegion = outputRegion;
#ifdef DLP_DEBUG
		timeInferUsed = (cv::getTickCount() - timeD2H) * 1000 / cv::getTickFrequency();
		OutputDebugStringA(std::string("__waferINFO_WriteDict: " + std::string(std::to_string(timeInferUsed)) + "\n").c_str());
#endif
#ifdef DLP_DEBUG

		timeInferUsed = (cv::getTickCount() - timeD2HAll) * 1000 / cv::getTickFrequency();
		OutputDebugStringA(std::string("__waferINFO_AllTime: " + std::string(std::to_string(timeInferUsed)) + "\n").c_str());
#endif
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------- saveDefectInfo-----------------------------------------//
	//函数说明：存储缺陷信息
	//saveDefectInfoInput &input;				输入：原始的缺陷信息
	//saveDefectInfoOutput &output; 			输出：筛选后的缺陷信息
	int saveDefectInfo(const saveDefectInfoInput& input, saveDefectInfoOutput& output)
	{

		try
		{
			return realsaveDefectInfo(input, output);
		}
		catch (...)
		{
			return 2; //算法未知异常
		}
		return 0;

	}
}
