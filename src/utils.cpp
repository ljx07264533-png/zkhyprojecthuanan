#include <Windows.h>
#include <direct.h>
#include <io.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <fstream>
#include <atomic>
#include "alglibrary/zkhyProjectHuaNan/utils.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace alglib::core;

namespace alglib::ops::zkhyProHN {
	bool convertLine2Contour(const LineStruct& line, vector<cv::Point2f>& contour)
	{
		contour.clear();
		contour.resize(3);
		contour[0] = line.pt1;
		contour[1] = line.pt2;
		contour[2] = line.pt1;
		return 0;
	}

	bool createDirectory(const string& path)
	{
		string pathTmp;
		string pathSub;

		int pathLen = path.size();
		pathSub.reserve(pathLen);

		for (auto i = path.begin(); i != path.end(); ++i)
		{
			const char c = *i;
			pathSub.push_back(c);
			if (c == '\\' || c == '/' || i == path.end() - 1)
			{
				pathTmp.append(pathSub);
				if (_access(pathTmp.c_str(), 0) != 0)
				{
					int ret = _mkdir(pathTmp.c_str());
					if (ret != 0)
						return false;
				}
				pathSub.clear();
			}
		}
		return true;
	}

	void image_recognition(HObject ho_imageGrop, HObject* ho_OutRegion, HTuple hv_char_height,
		HTuple hv_char_index, HTuple hv_Phi, HTuple hv_char_model, HTuple* hv_char)
	{

		// Local iconic variables
		HObject  ho_ObjectSelected, ho_ImageMean, ho_Region;
		HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_ImageCleared;
		HObject  ho_ImageResult, ho_RegionUnion;

		// Local control variables
		HTuple  hv_char_indexOut, hv_OCRHandle, hv_OCRHandle1;
		HTuple  hv_Number, hv_i, hv_Width, hv_Height, hv_UsedThreshold;
		HTuple  hv_Rows, hv_Columns, hv_judge, hv_Class, hv_Confidence;
		HTuple  hv_class, hv_HomMat2D;

		hv_char_indexOut = hv_char_index;
		(*hv_char) = "";

		//自己训练模型
		//read_ocr_class_mlp ('single_number_ocr.omc', OCRHandle)

		//系统自带模型
		ReadOcrClassMlp("Industrial_0-9_NoRej.omc", &hv_OCRHandle);
		ReadOcrClassMlp("Industrial_A-Z+_NoRej.omc", &hv_OCRHandle1);
		CountObj(ho_imageGrop, &hv_Number);
		GenEmptyObj(&(*ho_OutRegion));

		{
			HTuple end_val12 = hv_Number;
			HTuple step_val12 = 1;
			for (hv_i = 1; hv_i.Continue(end_val12, step_val12); hv_i += step_val12)
			{

				//读取图像，自适应二值化获取字符区域
				SelectObj(ho_imageGrop, &ho_ObjectSelected, hv_i);
				GetImageSize(ho_ObjectSelected, &hv_Width, &hv_Height);
				MeanImage(ho_ObjectSelected, &ho_ImageMean, 5, 5);

				//字符亮暗选择
				BinaryThreshold(ho_ImageMean, &ho_Region, "max_separability", hv_char_model, &hv_UsedThreshold);
				//去除因粘连二裁剪的横杆等干扰区域
				Connection(ho_Region, &ho_ConnectedRegions);
				SelectShapeStd(ho_ConnectedRegions, &ho_SelectedRegions, "max_area", 70);
				GetRegionPoints(ho_SelectedRegions, &hv_Rows, &hv_Columns);
				if (0 != (HTuple(((hv_Rows.TupleMax()) - (hv_Rows.TupleMin())) > (hv_char_height / 2)).TupleAnd(((hv_Columns.TupleMax()) - (hv_Columns.TupleMin())) > 5)))
				{
					//图像二值化
					GenImageProto(ho_ObjectSelected, &ho_ImageCleared, 255);
					PaintRegion(ho_Region, ho_ImageCleared, &ho_ImageResult, 0, "fill");

					//将字符region添加到数组
					Union1(ho_Region, &ho_RegionUnion);
					ConcatObj((*ho_OutRegion), ho_RegionUnion, &(*ho_OutRegion));

					//根据输入坐标判断待识别字符是否为字母
					hv_judge = hv_char_indexOut.TupleFind(hv_i);
					if (0 != (hv_judge > -1))
					{
						DoOcrSingleClassMlp(ho_Region, ho_ImageResult, hv_OCRHandle1, 2, &hv_Class,
							&hv_Confidence);
						//去除‘.’干扰
						if (0 != (HTuple(hv_Class[0]) == HTuple(".")))
						{
							hv_class = ((const HTuple&)hv_Class)[1];
						}
						else
						{
							hv_class = ((const HTuple&)hv_Class)[0];
						}
					}
					else
					{
						DoOcrSingleClassMlp(ho_Region, ho_ImageResult, hv_OCRHandle, 1, &hv_Class,
							&hv_Confidence);
						hv_class = hv_Class;
					}
					(*hv_char) += hv_class;
				}
				else
				{
					hv_char_indexOut += 1;
				}
			}
		}
		//字符区域旋转回原图
		VectorAngleToRigid(hv_Height / 2, hv_Width / 2, 0, hv_Height / 2, hv_Width / 2, hv_Phi,
			&hv_HomMat2D);
		AffineTransRegion((*ho_OutRegion), &(*ho_OutRegion), hv_HomMat2D, "nearest_neighbor");

		return;
	}

	int localCoordTrans2FullImageCoord(const Point2f& basePoint, const vector<Point2f>& oriContour, vector<Point2f>& outContour)
	{
		int pointsNum = oriContour.size();
		double basePointX = basePoint.x;
		double basePointY = basePoint.y;
		outContour.resize(pointsNum);
		for (int i = 0; i < pointsNum; ++i)
		{
			Point2f pointTmp;
			pointTmp.x = basePointX + oriContour[i].x;
			pointTmp.y = basePointY + oriContour[i].y;
			outContour[i] = pointTmp;
		}
		return 0;
	}

	int timeOutFusing(const DWORD startTime, const DWORD timeThreshold)
	{
		DWORD time = GetTickCount() - startTime;
		if (time > timeThreshold)
			return 1;
		else
			return 0;
	}

	std::string& std_string_format(std::string& _str, const char* _Format, ...) {
		std::string tmp;

		va_list marker = NULL;
		va_start(marker, _Format);

		size_t num_of_chars = _vscprintf(_Format, marker);

		if (num_of_chars > tmp.capacity()) {
			tmp.resize(num_of_chars + 1);
		}

		vsprintf_s((char*)tmp.data(), tmp.capacity(), _Format, marker);

		va_end(marker);

		_str = tmp.c_str();
		return _str;
	}

	void PrintMsg(std::string strInfo)
	{
		std::string strTime;
		SYSTEMTIME lpsystime;
		GetLocalTime(&lpsystime);
		std_string_format(strTime, "%d-%d-%d %d:%d:%d::", lpsystime.wYear, lpsystime.wMonth, lpsystime.wDay, lpsystime.wHour,
			lpsystime.wMinute, lpsystime.wSecond, lpsystime.wMilliseconds);

		std::string strTemp = strTime + " " + strInfo;
		OutputDebugStringA(strTemp.c_str());

	}

	int  GetRandom(int interval)
	{
		//  生成某个区间内的随机数,区间: [0-interval]
		int n = 10;   // 递推迭代次数
		int a = 29;  // 1-计算机字长之间的任意数
		int b = 5;
		int m = 1000000;  // 足够大的数
		int i;
		int res;
		static  int	RandomSeed = 100;

		for (i = 1; i <= n; i++)
			RandomSeed = (a * RandomSeed + b) % m;

		res = (int)((float)RandomSeed * interval / m);
		return res;
	}
	
	void getHedgePolarity(edgePolarity polarity, HTuple& hPolarity)
	{
		switch (polarity)
		{
		case edgePolarity::allPolarity:
			hPolarity = "all";
			break;
		case edgePolarity::positive:
			hPolarity = "positive";
			break;
		case edgePolarity::negative:
			hPolarity = "negative";
			break;
		case edgePolarity::uniform:
			hPolarity = "uniform";
			break;
		default:
			break;
		}
	}

	void getHedgePolarity(int polarity, HTuple& hPolarity)
	{
		getHedgePolarity(edgePolarity(polarity), hPolarity);
	}


	void getHedgeSelect(edgeSelect select, HTuple& hEdgeSelect)
	{
		switch (select)
		{
		case edgeSelect::allPoints:
			hEdgeSelect = "all";
			break;
		case edgeSelect::first:
			hEdgeSelect = "first";
			break;
		case edgeSelect::last:
			hEdgeSelect = "last";
			break;
		default:
			break;
		}
	}

	void getHedgeSelect(int select, HTuple& hSelect)
	{
		getHedgeSelect(edgeSelect(select), hSelect);
	}


}