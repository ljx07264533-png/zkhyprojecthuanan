#include "alglibrary/zkhyProjectHuaNan/inspectSegement.h"
#include "alglibrary/zkhyProjectHuaNan/segementVariation.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {
	int Insepect(HObject ho_Src, HObject ho_Partitioned, HObject ho_InspRegion, vector<HalconCpp::HObject>/*{eObjectVector,Dim=1}*/ hvec_imagemin,
		vector<HalconCpp::HObject>/*{eObjectVector,Dim=1}*/ hvec_imagemax, vector<HalconCpp::HObject>/*{eObjectVector,Dim=1}*/ hvec_Dregion,
		vector<HalconCpp::HObject>/*{eObjectVector,Dim=1}*/grid,
		HObject* ho_defectRegion, HTuple hv_NW, HTuple hv_NH, HTuple hv_Bthre, HTuple hv_Dthre,
		HTuple hv_mask, HTupleVector/*{eTupleVector,Dim=1}*/ hvec_HomDZM, HTupleVector/*{eTupleVector,Dim=1}*/ hvec_HomMZD, HTuple Dmask)
	{

		// Local iconic variables
		HObject  ho_ConnectedRegions, ho_duanlu1, ho_kailu1, ho_Ndefect1, ho_Grid_Defect1;
		HObject  ho_duanlu, ho_kailu, ho_Grid_Dfect, ho_ObjectsConcat;
		HObject   ho_ImageMin, ho_ImageMax, ho_EmptyObject, ho_RegionErosion1;
		HObject  ho_dregion, ho_ImageReduced2, ho_ImageReducedB, ho_ImageReducedD, ho_ImageSub2, ho_ImageSub3;
		HObject  ho_Region1, ho_Region2, ho_ConnectedRegions1, ho_RegionDilation2, ho_grid;
		HObject  ho_RegionIntersection, ho_ObjectSelected, ho_RegionAffineTrans, ho_Ndefect;
		HObject  ho_RegionDilation, ho_RegionOpening1, Paintregion, Paintregion2, Paint, ho_DfectRegion;
		// Local control variables
		HTuple  hv_Width, hv_Height, hv_Number, hv_Index, fArea, fC, fR, fArea1, fC1, fR1;
		HTuple  hv_HomDZM1, hv_HomMZD1, hv_Width1, hv_Height1, hv_Area;
		HTuple  hv_Row, hv_Column;
		HTuple  hv_fRow3, hv_fColumn3, hv_Area3, hv_Row3, hv_Column3;
		HTuple  hv_Area1, hv_Row1, hv_Column1, hv_Number1;
		HTuple   hv_Indices, hv_Indices1, hv_Indices2;
		HTuple  hv_Indices3, hv_Indices4, hv_Indices5, hv_Area2, hv_Indices6;
		HTuple  hv_Row2, hv_Column2, hv_In2, hv_Number2, hv_NN;
		HTuple  hv_NK, hv_ND, hv_NG, hv_Number3;
		int DNumber = 0;//缺陷数量

		GenEmptyObj(&ho_ConnectedRegions);
		GenEmptyObj(&ho_duanlu1);
		GenEmptyObj(&ho_kailu1);
		GenEmptyObj(&ho_Ndefect1);
		GenEmptyObj(&ho_Grid_Defect1);
		GetImageSize(ho_Src, &hv_Width, &hv_Height);
		SetSystem("border_shape_models", "true");
		//获取亮暗模板图像和区域
		CountObj(ho_Partitioned, &hv_Number);
		{
			HTuple end_val5 = hv_Number;
			HTuple step_val5 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val5, step_val5); hv_Index += step_val5)
			{
				int I = hv_Index;
				ho_ImageMin = hvec_imagemin[I - 1];
				ho_ImageMax = hvec_imagemax[I - 1];
				ho_dregion = hvec_Dregion[I - 1];
				size_t a = grid.size();
				//如果没有网格则检测区域就是全图，如果有网格，检测区域就是网格的补集。
				//网格的补集合，在加载哪里进行求取了。
				if (a > 1)
				{
					ho_grid = grid[I - 1];
				}
				else
				{
					GetDomain(ho_ImageMin, &ho_grid);
				}
				hv_HomDZM1 = hvec_HomDZM[hv_Index - 1].T();
				hv_HomMZD1 = hvec_HomMZD[hv_Index - 1].T();
				GetImageSize(ho_ImageMin, &hv_Width1, &hv_Height1);
				//affine_trans_image_size (Src, ImageReduced1, HomMat2D5, 'constant', Width1, Height1)
				AffineTransImageSize(ho_Src, &ho_ImageReduced2, hv_HomDZM1, "constant", hv_Width1,
					hv_Height1);
				GenEmptyObj(&Paint);
				ho_ImageReducedB = ho_ImageReduced2;
				ho_ImageReducedD = ho_ImageReduced2;
				Threshold(ho_ImageReduced2, &Paintregion, 0, 1);
				AreaCenter(Paintregion, &fArea, &fC, &fR);
				if (fArea != 0) {
					DilationRectangle1(Paintregion, &Paintregion, 2, 2);
					PaintRegion(Paintregion, ho_ImageReduced2, &ho_ImageReducedB, 0, "fill");
					PaintRegion(Paintregion, ho_ImageReduced2, &ho_ImageReducedD, 255, "fill");
				}
				Threshold(ho_ImageMax, &Paintregion2, 0, 1);
				AreaCenter(Paintregion2, &fArea1, &fC1, &fR1);
				if (fArea1 != 0) {
					DilationRectangle1(Paintregion2, &Paintregion2, 2, 2);
					PaintRegion(Paintregion2, ho_ImageMax, &ho_ImageMax, 255, "fill");
					PaintRegion(Paintregion2, ho_ImageMin, &ho_ImageMin, 0, "fill");
				}
				//精定位mark位移之后直接相减。
				SubImage(ho_ImageReducedB, ho_ImageMax, &ho_ImageSub2, 1, 0);
				SubImage(ho_ImageMin, ho_ImageReducedD, &ho_ImageSub3, 1, 0);
				Threshold(ho_ImageSub2, &ho_Region1, hv_Bthre, 255);
				Threshold(ho_ImageSub3, &ho_Region2, hv_Dthre, 255);
				Union2(ho_Region1, ho_Region2, &ho_Region1);
				Connection(ho_Region1, &ho_ConnectedRegions1);
				AreaCenter(ho_ConnectedRegions1, &hv_Area1, &hv_Row1, &hv_Column1);
				//缺陷和黑色区域膨胀之后的区域做交集
				if (0 != (int(hv_Area1 > 0)))
				{
					GenEmptyObj(&ho_EmptyObject);
					CountObj(ho_ConnectedRegions1, &hv_Number1);
					hv_Number += hv_Number1;
					if (0 != (int(hv_Number > 1000)))
					{
						// stop(...); only in hdevelop
						return 3;
					}
					//缺陷和黑色区域膨胀之后的区域做交集
					DilationRectangle1(ho_dregion, &ho_RegionDilation2, hv_mask, hv_mask);
					Intersection(ho_ConnectedRegions1, ho_RegionDilation2, &ho_RegionIntersection
					);
					//判断交集面积是否为0
					AreaCenter(ho_RegionIntersection, &hv_Area, &hv_Row, &hv_Column);
					TupleGreaterElem(hv_Area, 0, &hv_Indices);
					//有交集的也就是交集大于0的索引设为1，没有交集的索引设为0
					TupleFind(hv_Indices, 1, &hv_Indices1);
					TupleFind(hv_Indices, 0, &hv_Indices2);
					HTuple  len,squen;
					TupleLength(hv_Indices, &len);
					TupleGenSequence(1, len,1, &squen);
					//没有交集即为检测范围之外的类型3
					if (0 != (int(hv_Indices2 != -1)))
					{

						SelectObj(ho_ConnectedRegions1, &ho_Ndefect, hv_Indices2 + 1);
						Union1(ho_Ndefect, &ho_Ndefect);
						AffineTransRegion(ho_Ndefect, &ho_Ndefect, hv_HomMZD1, "nearest_neighbor");
						Union2(ho_Ndefect, ho_Ndefect1, &ho_Ndefect1);
					}
					//有交集的类型1和类型2，内缩之后仍然有交集的说明是区域内部的白色短路缺陷
					//剩余的是黑色断路缺陷
					if (0 != (int(hv_Indices1 != -1)))
					{
						HTuple Reduced;
						HObject ho_ObjectSelected1;
						TupleRemove(squen, hv_Indices2, &Reduced);
						SelectObj(ho_ConnectedRegions1, &ho_ObjectSelected1, Reduced);
						//对有交集的缺陷进行网格区域的判断，如果交集在网格区域的补集上则为真缺陷，进一步区分开路短路。
						//如果交集没有在网格区域的补集上则说明在网格区域内，为类型4.
						Intersection(ho_ObjectSelected1, ho_grid, &ho_DfectRegion);
						AreaCenter(ho_DfectRegion, &hv_Area3, &hv_Row3, &hv_Column3);
						TupleGreaterElem(hv_Area3, 0, &hv_Indices3);
						TupleFind(hv_Indices3, 1, &hv_Indices4);
						TupleFind(hv_Indices3, 0, &hv_Indices5);
						
						if (0 != (int(hv_Indices4 != -1)))
						{
							SelectObj(ho_ObjectSelected1, &ho_ObjectSelected, hv_Indices4 + 1);
							//对黑色区域进行一个小的腐蚀
							ErosionRectangle1(ho_dregion, &ho_RegionErosion1, 4, 4);
							//如果腐蚀之后仍然有交集的说明在黑色区域内部是白色的短路缺陷
							Intersection(ho_ObjectSelected, ho_RegionErosion1, &ho_duanlu);
							//用面积判断是否有交集
							AreaCenter(ho_duanlu, &hv_Area2, &hv_Row2, &hv_Column2);
							//面积大于0的索引被置为1也就是短路缺陷
							TupleGreaterElem(hv_Area2, 0, &hv_In2);
							//查找被置1的索引如果找不到则得到的值是-1
							TupleFind(hv_In2, 1, &hv_Indices6);
							//查找被置0的也就是没有交集的断路缺陷
							TupleFind(hv_In2, 0, &hv_Indices4);
							if (0 != (int(hv_Indices6 != -1)))
							{
								SelectObj(ho_ObjectSelected, &ho_duanlu, hv_Indices6 + 1);
								Union1(ho_duanlu, &ho_duanlu);
								AffineTransRegion(ho_duanlu, &ho_duanlu, hv_HomMZD1, "nearest_neighbor");
								Union2(ho_duanlu, ho_duanlu1, &ho_duanlu1);
								CountObj(ho_duanlu1, &hv_Number2);
							}
							if (0 != (int(hv_Indices4 != -1)))
							{
								SelectObj(ho_ObjectSelected, &ho_kailu, hv_Indices4 + 1);
								Union1(ho_kailu, &ho_kailu);
								AffineTransRegion(ho_kailu, &ho_kailu, hv_HomMZD1, "nearest_neighbor");
								Union2(ho_kailu, ho_kailu1, &ho_kailu1);
							}
						}
						if ((hv_Indices5 != -1))

						{
							if (hv_Indices3 == 0)
							{
								hv_Indices1 = hv_Indices1 + 1;
								SelectObj(ho_ConnectedRegions1, &ho_Grid_Dfect, hv_Indices1);
								Union1(ho_Grid_Dfect, &ho_Grid_Dfect);
								AffineTransRegion(ho_Grid_Dfect, &ho_Grid_Dfect, hv_HomMZD1, "nearest_neighbor");
								Union2(ho_Grid_Dfect, ho_Grid_Defect1, &ho_Grid_Defect1);
							}
							else
							{
								SelectObj(ho_ConnectedRegions1, &ho_Grid_Dfect, hv_Indices5+1);
								Union1(ho_Grid_Dfect, &ho_Grid_Dfect);
								AffineTransRegion(ho_Grid_Dfect, &ho_Grid_Dfect, hv_HomMZD1, "nearest_neighbor");
								Union2(ho_Grid_Dfect, ho_Grid_Defect1, &ho_Grid_Defect1);

							}
						}
					}
				}
			}
		}
		CountObj(ho_Ndefect1, &hv_NN);
		CountObj(ho_kailu1, &hv_NK);
		CountObj(ho_duanlu1, &hv_ND);
		CountObj(ho_Grid_Defect1, &hv_NG);
		HObject emptyRegon;
		GenEmptyRegion(&emptyRegon);
		if (0 != (int(hv_NK == 0)))
		{
			ho_kailu1 = emptyRegon;

		}
		if (0 != (int(hv_ND == 0)))
		{
			ho_duanlu1 = emptyRegon;
		}
		if (0 != (int(hv_NG == 0)))
		{
			ho_Grid_Defect1 = emptyRegon;
			//	GenRectangle1(&ho_Grid_Defect1, hv_Width / 2, hv_Width / 2, hv_Width / 2 + 1, hv_Width / 2 + 1);
		}
		if (0 != (int(hv_NN == 0)))
		{
			ho_Ndefect1 = emptyRegon;
			//GenRectangle1(&ho_Ndefect1, hv_Height / 2, hv_Width / 2, hv_Height / 2+1, hv_Width / 2+1);
		}
		ConcatObj(ho_kailu1, ho_duanlu1, &ho_ObjectsConcat);
		ConcatObj(ho_ObjectsConcat, ho_Ndefect1, &ho_ObjectsConcat);
		ConcatObj(ho_ObjectsConcat, ho_Grid_Defect1, &ho_ObjectsConcat);
		OpeningRectangle1(ho_ObjectsConcat, &ho_RegionOpening1, 2, 2);
		//图像灰度腐蚀膨胀之后检测的缺陷会变小，进行膨胀还原缺陷本身形态
		DilationRectangle1(ho_RegionOpening1, &ho_RegionDilation, Dmask - 2, Dmask - 2);
		//做一个简单的开运算去掉一些宽度为1的过检
		//和输入的检测区域进行交集获取最终缺陷
		(*ho_defectRegion) = ho_RegionDilation;
		return 0;
	}

	int realInspectSegement(const InspectSegementInput& input, InspectSegementOutput& output)
	{
		try {
			HObject ho_Src = input.Src;
			AlgParamInspectSegement* alg = (AlgParamInspectSegement*)&input.algPara;
			Mat a = HObject2Mat(ho_Src);
			if (a.cols == 0 || a.rows == 0)
			{
				return 1;

			}
			HObject InspRegion;
			HObject ho_markRegion = input.segment.ho_markRegion;
			HObject ho_Partitioned;//分区区域
			HTuple hv_DictHandle;//粗定位版轮廓
			HTuple hv_BMrow2;//粗定位row
			HTuple hv_BMclo2;//粗定位Col
			HTuple hv_BDictHandle;//分割小图模板罗阔
			HTuple hv_BMregion;//分割小图模板轮廓区域
			HTuple hv_col1;//分割小图col
			HTuple hv_row11;//分割小图row
			HTuple hv_col1Indices;//没有找到mark的小图索引
			vector<HObject> hv_Minimage, hv_Maximage, hvec_Dregion, grid;//小图
			HTupleVector  hvec_HomDZM(1), hvec_HomMZD(1);//小图仿射变换信息
			HTuple hv_HomMat2D21, hv_BTomMat2D;//大图仿射变换信息
			HTuple Disx;//拉伸信息
			HTuple Number;
			HObject	ho_defectRegion;
			HTuple Bthre = alg->i_params.block.Bthre;
			HTuple Dthre = alg->i_params.block.Dthre;
			
			HTuple maskSize = alg->i_params.block.maskSize;

			HTuple Dmask = input.segment.mask;
			ho_markRegion = input.segment.ho_markRegion;
			ho_Partitioned = input.segment.ho_Partitioned;
			hv_BDictHandle = input.segment.hv_BDictHandle;
			hv_BMclo2 = input.segment.hv_BMclo2;
			hv_BMregion = input.segment.hv_BMregion;
			hv_BMrow2 = input.segment.hv_BMrow2;
			hv_col1 = input.segment.hv_col1;

			hv_col1Indices = input.segment.hv_col1Indices;
			hv_Minimage = input.segment.hv_Minimage;
			hv_Maximage = input.segment.hv_Maximage;
			hvec_Dregion = input.segment.hvec_Dregion;
			grid = input.segment.grid;
			hv_row11 = input.segment.hv_row11;
			HTuple H_W;
			TupleLength(hv_row11,&H_W);
			HTuple NH = hv_row11[H_W - 1];
			HTuple NW = hv_col1[H_W - 1];
			if (NW>30)
			{
				NH = 3;
				NW = 3;
			}
			int Part = NH * NW;
			std::size_t  ParN = hv_Minimage.size();

			if (Part != int(ParN))
			{
				return 4;
			}
			hv_DictHandle = input.segment.hv_DictHandle;
			Disx = input.segment.Disx;
			GenEmptyObj(&InspRegion);
			//GenEmptyObj(&SFindtRegionROI);
			if (input.InspROI.size() != 0) {
				GenRegionFromContoursInput in2;
				RegionsOutput out2;
				in2.vecContourPts = input.InspROI;
				int r = GenRegionFromContours(in2, out2);
				InspRegion = out2.hRegion;
				Connection(InspRegion, &InspRegion);

				//AffineTransRegion(SFindtRegionROI, &ho_FindtRegionROI, hv_HomMat2DScale2Ori1, "nearest_neighbor");

				CountObj(InspRegion, &Number);
				int a = input.InspROI.size();
				if (a > Number[0])
				{
					return 9;
				}
			}
			else if (input.InspRegion.IsInitialized()) {

				InspRegion = input.InspRegion;
			}
			else {
				GetDomain(ho_Src, &InspRegion);
			}
			Position(ho_Src, ho_markRegion, ho_Partitioned, hv_DictHandle, hv_BMrow2, hv_BMclo2,
				hv_BDictHandle, hv_BMregion, hv_col1, hv_row11, hv_col1Indices, NW, NH, Disx,
				&hvec_HomDZM, &hvec_HomMZD, &hv_HomMat2D21, &hv_BTomMat2D);
			AffineTransImage(ho_Src, &ho_Src, hv_HomMat2D21, "constant", "false");

			int b = Insepect(ho_Src, ho_Partitioned, InspRegion, hv_Minimage, hv_Maximage,
				hvec_Dregion, grid, &ho_defectRegion, NW, NH, Bthre, Dthre, maskSize, hvec_HomDZM, hvec_HomMZD, Dmask);
			if (b == 0)
			{

				HObject Defect, Klu, Dlu, NDefect, Grid;
				HTuple	type, Number, lab;

				AffineTransRegion(ho_defectRegion, &ho_defectRegion, hv_BTomMat2D, "nearest_neighbor");
				Intersection(ho_defectRegion, InspRegion, &(ho_defectRegion));
				Defect = ho_defectRegion;
				vector<HObject> defectRegions;
				HObject tempRegion, ho_ObjectSelected;
				HTuple number, Earea, Erow, Ecol;
				HObject outRegion;
				GenEmptyObj(&outRegion);

				for (int i = 1; i <= 4; i++) {
					SelectObj(Defect, &tempRegion, i);
					AreaCenter(tempRegion, &Earea, &Erow, &Ecol);
					if (Earea == 0) {
						continue;
					}
					Connection(tempRegion, &tempRegion);
					CountObj(tempRegion, &number);
					for (int j = 1; j <= number; j++) {
						SelectObj(tempRegion, &ho_ObjectSelected, j);
						ConcatObj(outRegion, ho_ObjectSelected, &outRegion);
					}
					HTuple hv_Rows, hv_Columns, hv_Len1s;
					HTuple hv_Len2s, hv_Areas, hv_row1s, hv_col1s, hv_row2s;
					HTuple hv_col2s, hv_row3s, hv_col3s, hv_row4s, hv_col4s;
					inter_getRegionRect(tempRegion, &hv_Rows, &hv_Columns, &hv_Len1s, &hv_Len2s, &hv_Areas, &hv_row1s,
						&hv_col1s, &hv_row2s, &hv_col2s, &hv_row3s, &hv_col3s, &hv_row4s, &hv_col4s);

					for (int s = 0; s < hv_row1s.TupleLength(); s++)
					{

						FlawInfo flawinfo;
						flawinfo.labels = i;
						flawinfo.area = hv_Areas[s].D();
						flawinfo.rect2Len1 = 2 * (hv_Len1s[s].D() + 0.5);
						flawinfo.rect2Len2 = 2 * (hv_Len2s[s].D() + 0.5);
						flawinfo.d_params.block.area = hv_Areas[s].D();
						flawinfo.d_params.block.rect2Len1 = 2 * (hv_Len1s[s].D() + 0.5);
						flawinfo.d_params.block.rect2Len2 = 2 * (hv_Len2s[s].D() + 0.5);
						flawinfo.pos = Point2f((float)hv_Columns[s].D(), (float)hv_Rows[s].D());
						flawinfo.pts.push_back(Point2f((float)hv_col1s[s].D(), (float)hv_row1s[s].D()));
						flawinfo.pts.push_back(Point2f((float)hv_col2s[s].D(), (float)hv_row2s[s].D()));
						flawinfo.pts.push_back(Point2f((float)hv_col3s[s].D(), (float)hv_row3s[s].D()));
						flawinfo.pts.push_back(Point2f((float)hv_col4s[s].D(), (float)hv_row4s[s].D()));
						flawinfo.pts.push_back(Point2f((float)hv_col1s[s].D(), (float)hv_row1s[s].D()));
						output.flawinfo.push_back(flawinfo);
					}
				}
				output.ho_defectRegion = outRegion;
				CountObj(outRegion, &Number);
				int temp1 = output.flawinfo.size();
				int temp2 = Number.I();
				if (temp1 != temp2)
				{
					return 5;
				}
			}
			return b;
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
	int inspectSegement(const InspectSegementInput& input, InspectSegementOutput& output) {
		try
		{
			return realInspectSegement(input, output);
		}
		catch (...)
		{
			return 3; //算法内部异常
		}
	}
}
