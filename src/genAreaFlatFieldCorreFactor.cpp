#include "alglibrary/zkhyProjectHuaNan/genAreaFlatFieldCorreFactor.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	void genAreaFlatFieldCorrFactor(HObject ho_CalibImg, HObject* ho_WeightImg, HTuple hv_filterW)
	{

		// Local iconic variables
		HObject  ho_Domain, ho_Partitioned, ho_SortedRegions;
		HObject  ho_ImgSet, ho_Image1, ho_Image2, ho_Image3, ho_ObjectSelected;
		HObject  ho_weight, ho_ImageConverted, ho_WeightImgSub, ho_ImageMin;
		HObject  ho_ObjectSelected1, ho_ObjectSelected2, ho_ObjectSelected3;

		// Local control variables
		HTuple  hv_Width, hv_Height, hv_Channels, hv_channel;
		HTuple  hv_Mean, hv_Deviation, hv_Area, hv_Row, hv_Column;
		HTuple  hv_Number, hv_indexRC, hv_Max1, hv_Index, hv_widthNum;
		HTuple  hv_heightNum, hv_Seconds1, hv_row, hv_col, hv_val;
		HTuple  hv_mean, hv_deviation;

		GetImageSize(ho_CalibImg, &hv_Width, &hv_Height);
		GetDomain(ho_CalibImg, &ho_Domain);
		//对盖板区域按照 滤波核尺寸进行划分区域
		PartitionRectangle(ho_Domain, &ho_Partitioned, hv_filterW, hv_filterW);
		//对区域进行排序 获取到区域的行和列信息
		SortRegion(ho_Partitioned, &ho_SortedRegions, "first_point", "true", "row");
		CountChannels(ho_CalibImg, &hv_Channels);
		//获取图像的通道数
		GenEmptyObj(&ho_ImgSet);
		GenEmptyObj(&(*ho_WeightImg));
		if (0 != (hv_Channels == 3))
		{
			Decompose3(ho_CalibImg, &ho_Image1, &ho_Image2, &ho_Image3);
			ConcatObj(ho_ImgSet, ho_Image1, &ho_ImgSet);
			ConcatObj(ho_ImgSet, ho_Image2, &ho_ImgSet);
			ConcatObj(ho_ImgSet, ho_Image3, &ho_ImgSet);
		}
		else
		{
			ConcatObj(ho_ImgSet, ho_CalibImg, &ho_ImgSet);
		}
		{
			HTuple end_val18 = hv_Channels;
			HTuple step_val18 = 1;
			for (hv_channel = 1; hv_channel.Continue(end_val18, step_val18); hv_channel += step_val18)
			{
				SelectObj(ho_ImgSet, &ho_ObjectSelected, hv_channel);
				Intensity(ho_SortedRegions, ho_ObjectSelected, &hv_Mean, &hv_Deviation);
				AreaCenter(ho_SortedRegions, &hv_Area, &hv_Row, &hv_Column);
				CountObj(ho_SortedRegions, &hv_Number);
				hv_indexRC = hv_Column - hv_Row;
				TupleMax(hv_indexRC, &hv_Max1);
				//获取到一行的区域个数
				TupleFindFirst(hv_indexRC, hv_Max1, &hv_Index);
				//获取一列的区域个数
				hv_widthNum = hv_Index + 1;
				hv_heightNum = hv_Number / hv_widthNum;
				GenImageConst(&ho_weight, "real", hv_widthNum, hv_heightNum);
				//dev_update_off();
				CountSeconds(&hv_Seconds1);
				{
					HTuple end_val33 = hv_heightNum - 1;
					HTuple step_val33 = 1;
					for (hv_row = 0; hv_row.Continue(end_val33, step_val33); hv_row += step_val33)
					{
						{
							HTuple end_val34 = hv_widthNum - 1;
							HTuple step_val34 = 1;
							for (hv_col = 0; hv_col.Continue(end_val34, step_val34); hv_col += step_val34)
							{
								hv_val = HTuple(hv_Mean[(hv_row * hv_widthNum) + hv_col]);
								SetGrayval(ho_weight, hv_row, hv_col, HTuple(hv_Mean[(hv_row * hv_widthNum) + hv_col]));
							}
						}
					}
				}
				//将原始的均值图进行上采样
				ZoomImageSize(ho_weight, &ho_ImageConverted, hv_Width, hv_Height, "constant");
				//Step2 将均值图进行上采样图像尺寸，并计算权重为min/Gi
				//生成一个权重图
				GenImageProto(ho_ObjectSelected, &ho_WeightImgSub, 1);
				ConvertImageType(ho_WeightImgSub, &ho_WeightImgSub, "real");
				//MinMaxGray(ho_Domain, ho_ImageConverted, 0, &hv_Min, &hv_Max, &hv_Range);
				Intensity(ho_Domain, ho_ImageConverted, &hv_mean, &hv_deviation);
				GenImageProto(ho_ImageConverted, &ho_ImageMin, hv_mean);
				//均值图像的最小值/每个灰度值 为权重
				DivImage(ho_ImageMin, ho_ImageConverted, &ho_WeightImgSub, 100, 0);
				ConvertImageType(ho_WeightImgSub, &ho_WeightImgSub, "byte");
				ConcatObj((*ho_WeightImg), ho_WeightImgSub, &(*ho_WeightImg));
			}
		}
		//对于彩图进行合并
		if (0 != (hv_Channels == 3))
		{
			SelectObj((*ho_WeightImg), &ho_ObjectSelected1, 1);
			SelectObj((*ho_WeightImg), &ho_ObjectSelected2, 2);
			SelectObj((*ho_WeightImg), &ho_ObjectSelected3, 3);
			Compose3(ho_ObjectSelected1, ho_ObjectSelected2, ho_ObjectSelected3, &(*ho_WeightImg)
			);
		}
		return;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//——————————————生成面阵相机平场校正因子——————————————//
  //函数说明：通过输入的标定板进行面阵相机的平场校正因子的计算
  //GenAreaFlatFieldCorrFactorInput &input;		输入： 灰阶校正板、算法参数
  //SideAlignOutput &output; 			            输出： 平场校正权重因子
	int genAreaFlatFieldCorreFactor(const GenAreaFlatFieldCorrFactorInput& input, GenAreaFlatFieldCorrFactorOutput& output)
	{
		AlgParaGenAreaFlatFactor* alg = (AlgParaGenAreaFlatFactor*)&input.algPara;
		int filterW = alg->i_params.block.filterW;
		if (!input.srcImg.IsInitialized())
			return 1;
		HTuple width, height;
		GetImageSize(input.srcImg, &width, &height);
		if (filterW > width.I() / 2 || filterW > height.I() / 2)
			return 2;
		try {
			genAreaFlatFieldCorrFactor(input.srcImg, &output.weightImg, filterW);
			HTuple path = input.path.c_str();
			WriteImage(output.weightImg, "bmp", 0, path);
		}
		catch (...)
		{
			return 3;
		}
		return 0;
	}

}
