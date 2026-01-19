/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_UTILS_H
#define ZKHYPROJECTHUANAN_UTILS_H

#include <Windows.h>
#include <halconcpp/HalconCpp.h>

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;

namespace alglib::ops::zkhyProHN {
	bool convertLine2Contour(const LineStruct& line, vector<cv::Point2f>& contour);
	bool createDirectory(const string& path);
	void image_recognition(HObject ho_imageGrop, HObject* ho_OutRegion, HTuple hv_char_height,
		HTuple hv_char_index, HTuple hv_Phi, HTuple hv_char_model, HTuple* hv_char);
	int localCoordTrans2FullImageCoord(const Point2f& basePoint, const vector<Point2f>& oriContour, vector<Point2f>& outContour);
	int timeOutFusing(const DWORD startTime, const DWORD timeThreshold);
	std::string& std_string_format(std::string& _str, const char* _Format, ...);
	void PrintMsg(std::string strInfo);
	int  GetRandom(int interval);

	// 将输入的边缘点极性转换为Halcon的边缘点极性
	void getHedgePolarity(edgePolarity polarity, HalconCpp::HTuple& hPolarity);
	void getHedgePolarity(int polarity, HalconCpp::HTuple& hPolarity);

	// 将输入的边缘点选择转换为Halcon的边缘点选择
	void getHedgeSelect(edgeSelect select, HalconCpp::HTuple& hSelect);
	void getHedgeSelect(int select, HalconCpp::HTuple& hSelect);

}

#endif // ZKHYPROJECTHUANAN_UTILS_H