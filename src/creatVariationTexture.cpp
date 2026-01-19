#include <omp.h>
#include "alglibrary/zkhyProjectHuaNan/creatVariationTexture.h"
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
	int creatVariationmodel(HObject ho_image, HObject ho_creat_Roi, HObject* BImage,
		HTuple hv_angelstart, HTuple hv_angelend, HTuple hv_contrstH, HTuple hv_contrstL, HTuple contourNumber, HTuple hv_NumLevels,
		HTuple hv_minscore, HTuple hv_FileName, HTuple hv_imgID, HTuple hv_saveVarImgPath, HTuple hv_dilate, HTuple ncol, HTuple modelflag, HObject* DImage)
	{

		// Local iconic variables
		HObject  ho_ConnectedRegions;
		HObject  ho_MeanImage, ho_VarImage;

		// Local control variables
		HTuple  hv_Width, hv_Height, hv_Variationmodel;
		HTuple  hv_Number, hv_Index, hv_writfile;
		HTuple  hv_row, hv_clo, hv_Model, hv_ImageFiles;
		HTuple  hv_imgIndex;
		int FlgCol = 0;
		GetImageSize(ho_image, &hv_Width, &hv_Height);
		CreateVariationModel(hv_Width, hv_Height, "byte", "standard", &hv_Variationmodel);//创建模型
		if (modelflag < 0)
		{
			modelflag = 1;
		}

		Connection(ho_creat_Roi, &ho_ConnectedRegions);
		CountObj(ho_ConnectedRegions, &hv_Number);
		try {
			int end_val5 = hv_Number;
			HTuple step_val5 = 1;
			for (int hv_Index = 1; hv_Index < end_val5 + 1; hv_Index++)
			{
				HTuple Area, Row5, Column5, hv_ModelID, hv_Row, hv_Column, hv_Score, hv_Angle, hv_Indices, hv_Length, Area1, Row1, Column1;
				HObject SelectedRegion, ho_ImageReduced, ho_ObjectSelected, Region, ho_ModelContours;
				SelectObj(ho_ConnectedRegions, &ho_ObjectSelected, hv_Index);
				DilationCircle(ho_ObjectSelected, &ho_ObjectSelected, hv_dilate);
				ReduceDomain(ho_image, ho_ObjectSelected, &ho_ImageReduced);
				Threshold(ho_ImageReduced, &Region, hv_contrstL, hv_contrstH);
				//OpeningCircle(Region, &Region, 5);
				ClosingCircle(Region, &Region, 5);
				Connection(Region, &Region);
				AreaCenter(Region, &Area, &Row5, &Column5);
				TupleSortIndex(Area, &hv_Indices);
				//计算元组长度
				TupleLength(Area, &hv_Length);
				//从对象元组中选择对象。
				if (hv_Length < contourNumber)
				{
					contourNumber = hv_Length;
				}
				SelectObj(Region, &ho_ObjectSelected, HTuple(hv_Indices[hv_Length - contourNumber]) + 1);
				//计算面积
				AreaCenter(ho_ObjectSelected, &Area1, &Row1, &Column1);
				//借助形状特征选择区域，这里选取前5
				SelectShape(Region, &SelectedRegion, "area", "and", Area1 - 1,
					Area.TupleMax());
				DilationRectangle1(SelectedRegion, &SelectedRegion, 30, 30);
				Union1(SelectedRegion, &SelectedRegion);
				ReduceDomain(ho_image, SelectedRegion, &ho_ImageReduced);

				HObject SelectedFRegion, ho_ImageFReduced;
				DilationRectangle1(SelectedRegion, &SelectedFRegion, 500, 500);
				ReduceDomain(ho_image, SelectedFRegion, &ho_ImageFReduced);
				//ReduceDomain(ho_Image, ho_RegionDilation1, &ho_ImageReduced4);
				HObject b;
				CropDomain(ho_ImageReduced, &b);
				Mat a = HObject2Mat(b);
				CreateShapeModel(ho_ImageReduced, "auto", hv_angelstart.TupleRad(), hv_angelend.TupleRad(),
					"auto", "auto", "use_polarity", "auto", "auto",
					&hv_ModelID);
				GetShapeModelContours(&ho_ModelContours, (hv_ModelID), 1);
				FindShapeModel(ho_ImageFReduced, hv_ModelID, hv_angelstart.TupleRad(), hv_angelend.TupleRad(),
					hv_minscore, 1, 0.5, "least_squares", hv_NumLevels, 0,
					&hv_Row, &hv_Column, &hv_Angle, &hv_Score);
				HTuple Hom2D;
				HomMat2dIdentity(&Hom2D);
				VectorAngleToRigid(0, 0, 0, (hv_Row), (hv_Column), (hv_Angle), &Hom2D);
				AffineTransContourXld(ho_ModelContours, &ho_ModelContours, Hom2D);
				hv_row[hv_Index - 1] = hv_Row;
				hv_clo[hv_Index - 1] = hv_Column;
				hv_Model[hv_Index - 1] = hv_ModelID;
			}
			HTuple hv_ImageFiles1, hv_ImageFiles_tow;
			ListFiles(hv_FileName, (HTuple("files").Append("follow_links")), &hv_ImageFiles);
			TupleRegexpSelect(hv_ImageFiles, (HTuple(hv_imgID).Append("ignore_case")), &hv_ImageFiles1);//读取同一列的图像以ID来区分不同列
			int end_val17 = (hv_ImageFiles1.TupleLength()) - 1;
			if (end_val17 < 3)
			{
				return 10;
			}
			HObject	trainImage;
			//GenEmptyObj(&trainImage);
			HObject	tileImage;
			HTuple step_val17 = 1;
			int  trainIndex = 0;
#pragma omp parallel for schedule(static, 3)
			for (int hv_imgIndex = 0; hv_imgIndex < end_val17; hv_imgIndex++)
			{
				try {
					HTuple  hv_row1, hv_clo1, hv_HomMat2D;
					HObject  ho_ImageAffineTrans, ho_Image;
					ReadImage(&ho_Image, HTuple(hv_ImageFiles1[hv_imgIndex]));
					if (ncol == 1)
					{
						MirrorImage(ho_Image, &ho_Image, "row");
					}
					for (int Index = 1; Index < end_val5 + 1; Index++)
					{
						HObject ho_ObjectSelected, ho_RegionDilation1, ho_ImageReduced4, b;
						HTuple hv_Row4, hv_Column4, hv_Angle4, hv_Score4;
						SelectObj(ho_ConnectedRegions, &ho_ObjectSelected, Index);
						DilationCircle(ho_ObjectSelected, &ho_RegionDilation1, 400);
						ReduceDomain(ho_Image, ho_RegionDilation1, &ho_ImageReduced4);
						CropDomain(ho_ImageReduced4, &b);
						Mat a = HObject2Mat(b);
						FindShapeModel(ho_ImageReduced4, HTuple(hv_Model[Index - 1]), hv_angelstart.TupleRad(),
							hv_angelend.TupleRad(), hv_minscore, 1, 0.5, "least_squares", hv_NumLevels, 0, &hv_Row4,
							&hv_Column4, &hv_Angle4, &hv_Score4);
						hv_row1[Index - 1] = hv_Row4;
						hv_clo1[Index - 1] = hv_Column4;
						ho_ObjectSelected.Clear();
						ho_RegionDilation1.Clear();
						ho_ImageReduced4.Clear();
					}
					VectorToRigid(hv_row1, hv_clo1, hv_row, hv_clo, &hv_HomMat2D);
					AffineTransImage(ho_Image, &ho_ImageAffineTrans, hv_HomMat2D, "constant", "false");
					TrainVariationModel(ho_ImageAffineTrans, hv_Variationmodel);
					//GetVariationModel(&ho_MeanImage, &ho_VarImage, hv_Variationmodel);
					//ConcatObj(ho_ImageAffineTrans, trainImage, &trainImage);
					if (trainImage.IsInitialized() == 0)
					{
						CopyImage(ho_ImageAffineTrans, &trainImage);

					}
					else
					{
						AppendChannel(trainImage, ho_ImageAffineTrans, &trainImage);
					}
					ho_Image.Clear();
					ho_ImageAffineTrans.Clear();
					hv_HomMat2D.Clear();
				}

				catch (HalconCpp::HException& except)
				{
#pragma omp critical
					{
						trainIndex++;
					}
					continue;
				}

			}
			if (trainIndex > 2)
			{
				return 9;
			}
			// 执行当
			HTuple  Ran, n1, n2;
			HObject	RegionImage, RegionImage2, Paintregion, ho_ImageRNK, ho_ModleImage, ModleImage, BModleImage, DModleImage;
			GetVariationModel(&ho_MeanImage, &ho_VarImage, hv_Variationmodel);

			CountChannels(trainImage, &n1);
			if (modelflag == 1) {
				TupleCeil((end_val17 - trainIndex + 1) / 2, &Ran);
				RankN(trainImage, &ho_ImageRNK, Ran);
				ConcatObj(ho_MeanImage, ho_ImageRNK, &ho_ModleImage);
				TileImagesOffset(ho_ModleImage, &ModleImage, (HTuple(0).Append(0)), (HTuple(0).Append(0)),
					(HTuple(0).Append(0)), (HTuple(0).Append(0)), (HTuple(-1).Append(-1)), (HTuple(-1).Append(-1)),
					hv_Width, hv_Height);
				Threshold(ho_ModleImage, &Paintregion, 0, 1);
				PaintRegion(Paintregion, ModleImage, &BModleImage, 255, "fill");
				PaintRegion(Paintregion, ModleImage, &DModleImage, 0, "fill");
				(*BImage) = BModleImage;
				(*DImage) = DModleImage;
			}
			if (modelflag == 2)
			{
				HTuple Ran2;
				HObject BMeanImage, BMeanImageFul;
				Ran = (end_val17 - trainIndex + 1) / 3;
				if (Ran == 1)
				{
					Ran = (end_val17 - trainIndex + 1) / 2;
				}
				RankN(trainImage, &ho_ImageRNK, Ran);
				ConcatObj(ho_MeanImage, ho_ImageRNK, &ho_ModleImage);
				TileImagesOffset(ho_ModleImage, &(*DImage), (HTuple(0).Append(0)), (HTuple(0).Append(0)),
					(HTuple(0).Append(0)), (HTuple(0).Append(0)), (HTuple(-1).Append(-1)), (HTuple(-1).Append(-1)),
					hv_Width, hv_Height);
				Threshold((*DImage), &Paintregion, 0, 1);
				PaintRegion(Paintregion, (*DImage), &(*DImage), 0, "fill");

				ho_ModleImage.Clear();
				Ran2 = (end_val17 - trainIndex + 1) - Ran;
				RankN(trainImage, &BMeanImage, Ran2);
				ConcatObj(ho_MeanImage, BMeanImage, &ho_ModleImage);
				TileImagesOffset(ho_ModleImage, &(*BImage), (HTuple(0).Append(0)), (HTuple(0).Append(0)),
					(HTuple(0).Append(0)), (HTuple(0).Append(0)), (HTuple(-1).Append(-1)), (HTuple(-1).Append(-1)),
					hv_Width, hv_Height);
				PaintRegion(Paintregion, (*BImage), &(*BImage), 255, "fill");

			}
			if (modelflag == 0)
			{
				Threshold(ho_MeanImage, &Paintregion, 0, 1);
				PaintRegion(Paintregion, ho_MeanImage, &(*DImage), 0, "fill");
				PaintRegion(Paintregion, ho_MeanImage, &(*BImage), 255, "fill");

			}
			trainImage.Clear();
			ClearShapeModel(HTuple(hv_Model));
			ClearVariationModel(hv_Variationmodel);
			hv_Model.Clear();
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);

			ClearVariationModel(hv_Variationmodel);
			ClearShapeModel(hv_Model);
			return 5;
		}
		hv_writfile = ((const HTuple&)hv_ImageFiles)[(hv_ImageFiles.TupleLength()) - 1];
		String str3 = hv_writfile.S().Text();
		std::string prefix = str3.substr(0, str3.length() - 7);
		// 新的最后三位数字
		std::string newDigits = "666";
		// 拼接新的字符串
		std::string modifiedStr = prefix + newDigits;
		hv_saveVarImgPath = modifiedStr.c_str();
		HTuple a = hv_saveVarImgPath.TupleLength();
		try
		{
			if (FlgCol == 0)
			{
				//WriteImage(ho_MeanImage, "bmp", 0, hv_saveVarImgPath);
			}


		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			String name = hv_FileName.S().Text();
			std::string newname = "//666";
			std::string newname2 = name + newname;
			hv_saveVarImgPath = newname2.c_str();
			WriteImage(ho_MeanImage, "bmp", 0, hv_saveVarImgPath);
		}
		ClearVariationModel(hv_Variationmodel);
		return 0;
	}

	int realCreatVariationTexture(const  CreatVariationTextureInput& input, CreatVariationTextureOutput& output)
	{
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		Mat outimg;
		Mat src = input.srcImg;
		HObject		CreatRegionROI, CreatRegionROI1;//创建模板ROI和查找模板ROI.
		HTuple Number1, Number;//创建模板ROI个数，膨胀之后查找模板ROI个数。
		if (src.empty()) {
			return 1;
		}
		if (input.creatVarImgPath.empty())
		{
			return 3;
		}

		HObject  ModelImage, ho_modle_image, ho_creat_Roi, DImage;
		ModelImage = Mat2HObject(src);
		AlgParamCreatVariationTexture* alg = (AlgParamCreatVariationTexture*)&input.alg1;
		HTuple  hv_angelstart = alg->d_params.block.angleStart;
		HTuple  hv_angelend = alg->d_params.block.angleExtend;
		HTuple hv_contrstH = alg->i_params.block.contrstH;
		HTuple hv_contrstL = alg->i_params.block.contrstL;
		HTuple contourNumber = alg->i_params.block.contourNumber;
		HTuple hv_minscore = alg->d_params.block.minScore;
		HTuple hv_NumLevels = alg->i_params.block.NumLevels;
		HTuple hv_FileName = input.creatVarImgPath.c_str();
		HTuple hv_imgID = input.imgID.c_str();
		HTuple hv_saveVarImgPath = input.saveVarImgPath.c_str();
		HTuple modelflag = alg->i_params.block.modelflag;
		HTuple ncol = input.nCol;
		HTuple hv_dilate = alg->i_params.block.dilate;

		if (hv_dilate<0 || hv_minscore < 0 || hv_minscore>1 || hv_contrstL>hv_contrstH || hv_contrstL < 0 || hv_contrstH<0 || hv_angelstart>hv_angelend)
		{
			return 4;
		}
		try {

			GenEmptyObj(&CreatRegionROI);
			if (input.CreatRoi.IsInitialized())
			{
				HObject CreatRegionROIN = input.CreatRoi;
				Connection(CreatRegionROIN, &CreatRegionROIN);
				CountObj(CreatRegionROIN, &Number1);
			}
			if (input.CreatRoi.IsInitialized() && ((Number1 >= 2) || (input.posCreatROI.size() == 0))) {
				CreatRegionROI = input.CreatRoi;
				Connection(CreatRegionROI, &CreatRegionROI);

			}
			else if (input.posCreatROI.size() != 0) {

				GenRegionFromContoursInput in2;
				RegionsOutput out2;
				in2.vecContourPts = input.posCreatROI;
				int r = GenRegionFromContours(in2, out2);
				CreatRegionROI = out2.hRegion;
				Connection(CreatRegionROI, &CreatRegionROI);
				CountObj(CreatRegionROI, &Number1);
				//DilationCircle(CreatRegionROI, &CreatRegionROI1, hv_dilate);
				CountObj(CreatRegionROI, &Number);
				int a = input.posCreatROI.size();
				if (Number < a)
				{
					return 8;
				}
			}

			if (Number1 < 2) {
				return 2;
			}

		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			//throw except;
			return 5;
		}

		Mat OutModimage;
		int a = creatVariationmodel(ModelImage, CreatRegionROI, &ho_modle_image,
			hv_angelstart, hv_angelend, hv_contrstH, hv_contrstL, contourNumber, hv_NumLevels,
			hv_minscore, hv_FileName, hv_imgID, hv_saveVarImgPath, hv_dilate, ncol, modelflag, &DImage);
		if (a == 0) {

			output.shapeInfo1.findRegionROI = CreatRegionROI;
			OutModimage = HObject2Mat(DImage);
			output.shapeInfo1.BmodelImg = ho_modle_image;
			output.ouputimage = OutModimage;
			output.shapeInfo1.DmodelImg = DImage;
		}
		return a;
	}


}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------CreatVariationTexture--------------------------//
	//功能：创建差异化模型标准模板函数入口

	int creatVariationTexture(const CreatVariationTextureInput& input, CreatVariationTextureOutput& output)
	{
		try
		{
			return realCreatVariationTexture(input, output);
		}
		catch (HException hex)
		{
			string sInfo = hex.ErrorMessage().TextA();
			string sInfo2 = hex.ProcName().TextA();
			int sInfo3 = hex.ErrorCode();
			return 5;
		}
		catch (...)
		{
			return 5; //算法内部异常
		}
	}
}
