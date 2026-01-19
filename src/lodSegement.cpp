#include "alglibrary/zkhyProjectHuaNan/lodSegement.h"
#include "alglibrary/zkhyProjectHuaNan/segementVariation.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

	int lodfile(HObject* ho_markRegion, HObject* ho_Partitioned, vector<HObject>/*{eObjectVector,Dim=1}*/* hvec_imagemin,
		vector<HObject>/*{eObjectVector,Dim=1}*/* hvec_imagemax, vector<HObject>/*{eObjectVector,Dim=1}*/* hvec_Dregion,
		vector<HObject>/*{eObjectVector,Dim=1}*/* grid,
		HTuple hv_file, HTuple hv_mask, HTuple hv_Dthre, int flg_Grd, HTuple* hv_DictHandle, HTuple* hv_BMrow2,
		HTuple* hv_BMclo2, HTuple* hv_BDictHandle, HTuple* hv_BMregion, HTuple* hv_col1,
		HTuple* hv_row11, HTuple* hv_col1Indices, HTuple* Disx)
	{

		// Local iconic variables
		HObject  ho_ImageMin, ho_Region, ho_ImageMax;
		// Local control variables
		HTuple  hv_Minimage, hv_Maximage, hv_MinParamValue;
		HTuple  hv_MaxParamValue, hv_Length, hv_Index, hv_Index2, hv_Length2;
		HTuple  hv_Width, hv_Height, hv_w, hv_h, hv_Number;
		HTuple  hv_NumConnected, hv_NumHoles, Index3;
		HTuple  hv_Greater, hv_Sequence, hv_Reduced;
		HTuple  hv_Index1;
		HTuple  hv_Indices;

		HObject  ho_ObjectSelected, ho_ImageReduced1, ho_Region1, ho_ImageMin2, ho_RegionFillUp;
		HObject  ho_ConnectedRegions, ho_ObjectSelected1, ho_ImageReduced, ho_EmptyObject, ho_Domain, ho_RegionDifference;
		//整图的放射变换信息，轮廓模板，轮廓区域
		ReadRegion(&(*ho_markRegion), hv_file + "FRegion");
		ReadDict(hv_file + "FDHandle", HTuple(), HTuple(), &(*hv_DictHandle));
		ReadTuple(hv_file + "Frow", &(*hv_BMrow2));
		ReadTuple(hv_file + "Fclo", &(*hv_BMclo2));
		ReadTuple(hv_file + "FDisx", &(*Disx));
		//小图的仿射变换等信息
		//小图的模板轮廓信息
		GenEmptyObj(&ho_EmptyObject);
		ReadDict(hv_file + "SHModel", HTuple(), HTuple(), &(*hv_BDictHandle));
		//小图的轮廓区域
		ReadDict(hv_file + "SMregion", HTuple(), HTuple(), &(*hv_BMregion));
		//小图的放射变换信息和分割信息
		ReadRegion(&(*ho_Partitioned), hv_file + "PRegion");
		ReadTuple(hv_file + "Scol", &(*hv_col1));
		ReadTuple(hv_file + "Srow", &(*hv_row11));
		ReadTuple(hv_file + "CIndice", &(*hv_col1Indices));
		HTuple	CLength,RLength,Comper, Comper1, hv_row22;
		TupleLength(*hv_row11, &RLength);
		TupleSelect(*hv_row11, RLength - 1,&Comper);
		TupleAbs(Comper, &Comper);
		TupleCeil(Comper,&Comper1);
		if (Comper== Comper1)
		{
			TupleRemove(*hv_row11, RLength-1,&hv_row22);
		}
		TupleLength(*hv_col1Indices, &CLength);
		int i = 0;
		HTuple Select;
		for (i = 0; i < CLength - 1; i++)
		{
			TupleSelect(*hv_col1Indices, i, &Select);
			(*hv_col1)[Select] = -1;
		}
		if (*hv_col1Indices != -1)
		{
			NNinter_COPY_2((hv_row22), &(hv_row22), &(*hv_col1Indices));
			hv_row22[RLength - 1] = (*hv_row11)[RLength - 1];
			(*hv_row11) = hv_row22;
		}		
		//模板图像，以及黑色检测区域
		ReadDict(hv_file + "Minimg", HTuple(), HTuple(), &hv_Minimage);
		ReadDict(hv_file + "Maximg", HTuple(), HTuple(), &hv_Maximage);
		GetDictParam(hv_Minimage, "keys", HTuple(), &hv_MinParamValue);
		GetDictParam(hv_Maximage, "keys", HTuple(), &hv_MaxParamValue);
		TupleLength(hv_MaxParamValue, &hv_Length);
		{
			HTuple end_val21 = hv_Length;
			HTuple step_val21 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val21, step_val21); hv_Index += step_val21)
			{
				GetDictObject(&ho_ImageMin, hv_Minimage, HTuple(hv_MinParamValue[hv_Index - 1]));
				GrayErosionRect(ho_ImageMin, &ho_ImageMin, hv_mask, hv_mask);
				Threshold(ho_ImageMin, &ho_Region, 0, hv_Dthre);
				hvec_Dregion->push_back(ho_Region);
				hvec_imagemin->push_back(ho_ImageMin);
				GetDictObject(&ho_ImageMax, hv_Maximage, HTuple(hv_MaxParamValue[hv_Index - 1]));
				GrayDilationRect(ho_ImageMax, &ho_ImageMax, hv_mask, hv_mask);
				hvec_imagemax->push_back(ho_ImageMax);
			}
			if (flg_Grd == 1)
			{
				{
					HTuple end_val39 = hv_Length;
					HTuple step_val39 = 1;
					for (hv_Index1 = 1; hv_Index1.Continue(end_val39, step_val39); hv_Index1 += step_val39)
					{
						GetDictObject(&ho_ImageMin2, hv_Minimage, HTuple(hv_MinParamValue[hv_Index1 - 1]));
						Threshold(ho_ImageMin2, &ho_Region1, hv_Dthre, 255);
						Connection(ho_Region1, &ho_ConnectedRegions);
						ConnectAndHoles(ho_ConnectedRegions, &hv_NumConnected, &hv_NumHoles);
						TupleLength(hv_NumHoles, &hv_Length);
						TupleGreaterElem(hv_NumHoles, 10, &hv_Greater);
						TupleFind(hv_Greater, 1, &hv_Indices);
						hv_Reduced = hv_Indices + 1;
						GetDomain(ho_ImageMin2, &ho_Domain);
						if (0 != (int(hv_Reduced > 0)))
						{
							SelectObj(ho_ConnectedRegions, &ho_ObjectSelected1, hv_Reduced);
							FillUp(ho_ObjectSelected1, &ho_RegionFillUp);
							Union1(ho_RegionFillUp, &ho_ObjectSelected1);
							ErosionRectangle1(ho_ObjectSelected1, &ho_ObjectSelected1, 10, 10);
							Difference(ho_Domain, ho_ObjectSelected1, &ho_RegionDifference);
							ConcatObj(ho_RegionDifference, ho_EmptyObject, &ho_EmptyObject);
							grid->push_back(ho_RegionDifference);
						}
						else
						{
							//GetDomain(ho_ImageMin2, &ho_Domain);
							//ConcatObj(ho_Domain, ho_EmptyObject, &ho_EmptyObject);
							grid->push_back(ho_Domain);
						}
					}
				}
			}
		}
		return 0;
	}
	int realLodSegement(const LodSegementInput& input, LodSegementOutput& output) {
		try {
			AlgParamLodSegement* alg = (AlgParamLodSegement*)&input.algPara;
			HTuple hv_file = input.LodSegmentImgPath.c_str();
			HObject ho_markRegion;//粗定位mark
			HObject ho_Partitioned;//分区区域
			HTuple hv_DictHandle;//粗定位版轮廓
			HTuple hv_BMrow2;//粗定位row
			HTuple hv_BMclo2;//粗定位Col
			HTuple hv_BDictHandle;//分割小图模板罗阔
			HTuple hv_BMregion;//分割小图模板轮廓区域
			HTuple hv_col1;//分割小图col
			HTuple hv_row11;//分割小图row
			HTuple hv_col1Indices;//没有找到mark的小图索引
			//HObjectVector hv_Minimage, hv_Maximage;//小图
			HTuple Disx;//模板图粗定位mark的X方向距离
			vector<HObject>  hv_Minimage, hv_Maximage, hvec_Dregion, grid;

			/*int a=lodfile(&ho_markRegion, &ho_Partitioned, hv_file, &hv_DictHandle, &hv_BMrow2,
				&hv_BMclo2, &hv_BDictHandle, &hv_BMregion, &hv_col1, &hv_row11, &hv_col1Indices,
				&hv_Minimage, &hv_Maximage);*/
			HTuple mask = alg->i_params.block.mask;
			HTuple Thre = alg->i_params.block.Thre;
			int flg_Grd = alg->i_params.block.flg_Grd;
			int a = lodfile(&ho_markRegion, &ho_Partitioned, &hv_Minimage, &hv_Maximage, &hvec_Dregion, &grid,
				hv_file, mask, Thre, flg_Grd, &hv_DictHandle, &hv_BMrow2, &hv_BMclo2, &hv_BDictHandle,
				&hv_BMregion, &hv_col1, &hv_row11, &hv_col1Indices, &Disx);

			output.Segment.ho_markRegion = ho_markRegion;
			output.Segment.ho_Partitioned = ho_Partitioned;
			output.Segment.hv_BDictHandle = hv_BDictHandle;
			output.Segment.hv_BMclo2 = hv_BMclo2;
			output.Segment.hv_BMregion = hv_BMregion;
			output.Segment.hv_BMrow2 = hv_BMrow2;
			output.Segment.hv_col1 = hv_col1;
			output.Segment.hv_col1Indices = hv_col1Indices;
			output.Segment.hv_DictHandle = hv_DictHandle;
			output.Segment.hv_Minimage = hv_Minimage;
			output.Segment.hv_Maximage = hv_Maximage;
			output.Segment.hv_row11 = hv_row11;
			output.Segment.hvec_Dregion = hvec_Dregion;
			output.Segment.mask = mask;
			output.Segment.Disx = Disx;
			output.Segment.grid = grid;
			return a;
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int lodSegement(const LodSegementInput& input, LodSegementOutput& output) {
		try
		{
			return realLodSegement(input, output);
		}
		catch (...)
		{
			return 3; //算法内部异常
		}
	}
}
