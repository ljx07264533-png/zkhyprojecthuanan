#include "alglibrary/zkhyProjectHuaNan/creatTexture.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace zkhyPublicFuncHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;


namespace alglib::ops::zkhyProHN {
	void creatModle(HObject ho_Image, HObject* ho_ImagePart, HObject* ho_FindRegionROI,
		HObject* ho_Rectangle, HTuple contrstL, HTuple  contrstH, HTuple contourNumber, HTuple hv_AngleStart, HTuple hv_AngleExtend, HTuple hv_offset, HTuple hv_NumLevels,
		HTuple hv_Row1, HTuple hv_Column1, HTuple hv_Row2, HTuple hv_Column2, HObject CreatRegions, HTuple hv_MinScore,
		HTuple hv_NumMatches, HTuple hv_Scale, HTuple hv_Dilate, HTuple* hv_DictHandle, HTuple* row, HTuple* col,
		HTuple* hv_startRow, HObject* ho_ContoursAffineTrans1, HObject* ho_ContoursAffineTrans2)
	{

		// Local iconic variables
		HObject  ho_ImageReduced, ho_EmptyObject, ho_Contour, ho_ModelContours, ho_ModelContours1;
		HObject  ho_CreatRegionROI, ho_CreatRegionROI2, ho_ImageReduced2, ho_Contour1, ho_ImageReduced3;

		HTuple hv_ModelID1, hv_modelRow2, hv_modelColumn2, hv_Angle1;
		HTuple hv_ModelID2, hv_modelRow3, hv_modelColumn3, hv_Angle3;

		// Local control variables
		HTuple  hv_Width, hv_Height, hv_modelW, hv_modelH;
		HTuple  hv_num, hv_HomMat2D, hv_Score1, hv_Score3, hv_Savemodel, hv_HomMat2DIdentity1, hv_HomMat2DIdentity2, hv_HomMat2DScale2Ori1, hv_HomMat2DScale2Ori2;
		HTuple	hv_HomMat2DM1, hv_HomMat2DR, hv_HomMat2DM2, Number;
		vector<Point2f> TempContourRegion;

		GetImageSize(ho_Image, &hv_Width, &hv_Height);
		(*row).Clear();
		(*col).Clear();
		//	(*hv_ModelID).Clear();
		//创建Dict用来对模板进行保存
		CreateDict(&(*hv_DictHandle));
		if (hv_Row1 == 0)
		{
			GenRectangle1(&(*ho_Rectangle), 0, 0, hv_Height, hv_Width);
			(*ho_ImagePart) = ho_Image;
		}
		else {
			GenRectangle1(&(*ho_Rectangle), hv_Row1, hv_Column1, hv_Row2, hv_Column2);
			ReduceDomain(ho_Image, (*ho_Rectangle), &ho_ImageReduced);
			CropDomain(ho_ImageReduced, &(*ho_ImagePart));
		}

		GetImageSize((*ho_ImagePart), &hv_modelW, &hv_modelH);
		GenEmptyObj(&ho_EmptyObject);
		(*hv_startRow) = hv_Row1;

		hv_num = (hv_Row1.D() / hv_modelH.D());
		hv_num = hv_num.TupleCeil();
		//VectorAngleToRigid(0, 0, 0, -hv_Row1, 0, 0, &hv_HomMat2D);
		/*SelectObj(CreatRegions, &ho_CreatRegionROI, 1);
		DilationRectangle1(ho_CreatRegionROI, &ho_CreatRegionROI, hv_Dilate, hv_Dilate);
		HomMat2dIdentity(&hv_HomMat2DIdentity1);


		AffineTransRegion(ho_CreatRegionROI, &ho_CreatRegionROI, hv_HomMat2D, "nearest_neighbor");
		ReduceDomain((*ho_ImagePart), ho_CreatRegionROI, &ho_ImageReduced2);*/
		CountObj(CreatRegions, &Number);
		HObject TempContour;
		GenEmptyObj(&TempContour);
		if (Number >= 1) {
			for (int i = 1; i <= Number; i++)
			{
				SelectObj(CreatRegions, &ho_CreatRegionROI2, i);
				DilationRectangle1(ho_CreatRegionROI2, &ho_CreatRegionROI2, hv_Dilate, hv_Dilate);
				HTuple Area, Row5, Column5, hv_ModelID, hv_Row, hv_Column, hv_Score, hv_Angle, hv_Indices, hv_Length, Area1, Row1, Column1;
				HObject SelectedRegion, ho_ImageReduced, ho_ObjectSelected, Region, ho_ModelContours;
				//AffineTransRegion(ho_CreatRegionROI2, &ho_CreatRegionROI2, hv_HomMat2D, "nearest_neighbor");
				//AffineTransRegion(ho_CreatRegionROI2, &ho_CreatRegionROI2, hv_HomMat2DScale2Ori1, "nearest_neighbor");
				ReduceDomain((*ho_ImagePart), ho_CreatRegionROI2, &ho_ImageReduced3);
				//阈值分割，得到创建模型区域
				Threshold(ho_ImageReduced3, &Region, contrstL, contrstH);

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
				SelectShape(Region, &SelectedRegion, "area", "and", (Area1 - 1),
					Area.TupleMax());
				DilationRectangle1(SelectedRegion, &SelectedRegion, 10, 10);
				Union1(SelectedRegion, &SelectedRegion);
				ReduceDomain((*ho_ImagePart), SelectedRegion, &ho_ImageReduced3);
				Union2(TempContour, SelectedRegion, &TempContour);
				HObject b, SelectedFRegion, ho_ImageFReduced3;
				DilationRectangle1(SelectedRegion, &SelectedFRegion, 500, 500);
				ReduceDomain((*ho_ImagePart), SelectedFRegion, &ho_ImageFReduced3);
				CropDomain(ho_ImageReduced3, &b);
				Mat a = HObject2Mat(b);
				//AffineTransRegion(ho_CreatRegionROI2, &ho_CreatRegionROI2, hv_HomMat2DScale2Ori1, "nearest_neighbor");
				CreateShapeModel(ho_ImageReduced3, "auto", hv_AngleStart.TupleRad(), hv_AngleExtend.TupleRad(),
					"auto", "auto", "use_polarity", "auto", "auto", &(hv_ModelID2));
				GetShapeModelContours(&ho_ModelContours1, (hv_ModelID2), 1);
				FindShapeModel((ho_ImageFReduced3), (hv_ModelID2), hv_AngleStart.TupleRad(), hv_AngleExtend.TupleRad(),
					hv_MinScore, hv_NumMatches, 0, "least_squares", hv_NumLevels, 0, &(hv_modelRow3),
					&(hv_modelColumn3), &(hv_Angle3), &hv_Score1);
				VectorAngleToRigid(0, 0, 0, (hv_modelRow3), (hv_modelColumn3), (hv_Angle3),
					&hv_HomMat2DM2);
				AffineTransContourXld(ho_ModelContours1, &ho_ModelContours1, hv_HomMat2DM2);
				HomMat2dIdentity(&hv_HomMat2DIdentity2);
				HomMat2dScale(hv_HomMat2DIdentity2, 1 / hv_Scale, 1 / hv_Scale, 0, 0, &hv_HomMat2DScale2Ori2);//缩放检测
				//	AffineTransContourXld(ho_ModelContours1, &ho_ModelContours1, hv_HomMat2DScale2Ori2);//用于大图轮廓显示
				HTuple Number1, ContourR, ContourC;
				HObject TempContours1;
				CountObj(ho_ModelContours1, &Number1);
				/*for (int j = 1; j <=Number1; j++)
				{
					SelectObj(ho_ModelContours1, &TempContours1, j);
					GetContourXld(TempContours1, &ContourR, &ContourC);
				}*/																												  //创建模型2显示在原始图像中
				&(*row).Append((hv_modelRow3));
				&(*col).Append((hv_modelColumn3));
				//&(*hv_ModelID).Append((hv_ModelID2));
				SetDictTuple(*hv_DictHandle, i, (hv_ModelID2));
			}
			AffineTransRegion(TempContour, &TempContour, hv_HomMat2DScale2Ori2, "nearest_neighbor");
			*ho_ContoursAffineTrans1 = TempContour;
		}
		return;
	}

	int realCreatTexture(const  CreatTextureInput& input, CreatTextureOutput& output) {

		Mat subImg;
		HObject  ho_ModelImageSrc, ho_ModelImageSrc1;
		Mat srcImg = input.SrcImg;
		if (srcImg.rows == 0 || srcImg.cols == 0)
			return 1;

		//矩形裁剪模板图像	
		ho_ModelImageSrc1 = Mat2HObject(srcImg);

		HTuple  hv_HomMat2DIdentity, hv_HomMat2DScale2Ori, Number1, Number;//图像缩放

		HObject ho_region;//定义检测区域
		HObject ho_CreatRegionROI, CreatRegions, CreatRegions1;//定义ROI绘制区域
		AlgParamCreatTexture* alg = (AlgParamCreatTexture*)&input.alg;
		HTuple hv_AngleStart = alg->d_params.block.angleStart;//匹配起始角度
		HTuple hv_AngleExtend = alg->d_params.block.angleExtend;//匹配终止角度
		HTuple hv_MinScore = alg->d_params.block.minScore;//匹配置信度
		HTuple hv_NumMatches = alg->i_params.block.numMatches;//匹配个数
		HTuple hv_Scale = alg->d_params.block.scal;//缩放比例。
		HTuple contrstH = alg->d_params.block.contrstH;//模板图高阈值
		HTuple contrstL = alg->d_params.block.contrstL;//模板图低阈值
		HTuple contourNumber = alg->d_params.block.contourNumber;//模板图填充像素值


		HTuple hv_Dilate = alg->i_params.block.FindRoi;
		HTuple hv_posROIRows1 = HTuple();
		HTuple hv_posROICols1 = HTuple();
		HTuple hv_posROIRows2 = HTuple();
		HTuple hv_posROICols2 = HTuple();
		HTuple hv_posROIRows3 = HTuple();
		HTuple hv_posROICols3 = HTuple();

		HTuple hv_Row1 = 0 * hv_Scale;
		HTuple hv_Column1 = 0 * hv_Scale;
		HTuple hv_Row2 = (hv_Row1 + srcImg.rows * hv_Scale);
		HTuple hv_Column2 = (hv_Column1 + srcImg.cols * hv_Scale);
		HTuple hv_NumLevels = alg->i_params.block.NumLevels;
		HTuple hv_ShapeModelID, hv_Angle, hv_Savemodel;
		HObject  ho_Contour2, ho_CharRegionROI2, ho_ImageReduced2, ho_ImagePart, ho_FindRegionROI, ho_Rectangle;
		HObject ho_ContoursAffineTrans1, ho_ContoursAffineTrans2, Hregion, HRegionFillUp;
		Mat   ho_ImagePartTest;
		HTuple hv_ModelID1, hv_modelRow2, hv_modelColumn2, hv_startRow;

		HomMat2dIdentity(&hv_HomMat2DIdentity);

		HomMat2dScale(hv_HomMat2DIdentity, hv_Scale, hv_Scale, 0, 0, &hv_HomMat2DScale2Ori);//缩放检测
		//AffineTraneImage();
		//通过阈值处理对模板图上的脏污进行去除
		//对模板图进行缩放处理

		AffineTransImage(ho_ModelImageSrc1, &ho_ModelImageSrc1, hv_HomMat2DScale2Ori, "constant", "false");
		CropDomain(ho_ModelImageSrc1, &ho_ModelImageSrc1);
		HObject BmodelImg, DmodelImg;

		BmodelImg = input.shapeInfo.BmodelImg;
		DmodelImg = input.shapeInfo.DmodelImg;
		AffineTransImage(BmodelImg, &BmodelImg, hv_HomMat2DScale2Ori, "constant", "false");
		CropDomain(BmodelImg, &BmodelImg);
		AffineTransImage(DmodelImg, &DmodelImg, hv_HomMat2DScale2Ori, "constant", "false");
		CropDomain(DmodelImg, &DmodelImg);
		output.shapeInfo.BmodelImg = BmodelImg;//保存亮模板图像
		output.shapeInfo.DmodelImg = DmodelImg;//保存暗模板图像
		HTuple hv_HomMat2D;
		HTuple offset = 0 * hv_Scale;
		try {

			GenEmptyObj(&ho_region);

			if (input.posContour.size() != 0) {
				GenRegionFromContoursInput in1;
				RegionsOutput out1;
				in1.vecContourPts = input.posContour;
				int r = GenRegionFromContours(in1, out1);
				ho_region = out1.hRegion;
				AffineTransRegion(ho_region, &ho_region, hv_HomMat2DScale2Ori, "nearest_neighbor");

				VectorAngleToRigid(0, 0, 0, -hv_Row1, 0, 0, &hv_HomMat2D);
				AffineTransRegion(ho_region, &ho_region, hv_HomMat2D, "nearest_neighbor");

			}
			else {
				GetDomain(ho_ModelImageSrc1, &ho_region);

			}

		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}
		try {
			//绘制创建模板时绘制的ROI
			GenEmptyObj(&ho_CreatRegionROI);

			if (input.posROIContour.size() != 0) {
				GenRegionFromContoursInput in2;
				RegionsOutput out2;
				in2.vecContourPts = input.posROIContour;
				int r = GenRegionFromContours(in2, out2);
				ho_CreatRegionROI = out2.hRegion;
				AffineTransRegion(ho_CreatRegionROI, &ho_CreatRegionROI, hv_HomMat2DScale2Ori, "nearest_neighbor");
				//DilationRectangle1(ho_CreatRegionROI, &CreatRegions1, hv_Dilate, hv_Dilate);
				Connection(ho_CreatRegionROI, &CreatRegions1);
				Connection(ho_CreatRegionROI, &CreatRegions);
				CountObj(CreatRegions, &Number1);
				CountObj(CreatRegions1, &Number);
				int a = input.posROIContour.size();
				if (a > Number1)
				{
					return 8;
				}
			}
			else if (input.region.IsInitialized()) {

				ho_CreatRegionROI = input.region;
				AffineTransRegion(ho_CreatRegionROI, &ho_CreatRegionROI, hv_HomMat2DScale2Ori, "nearest_neighbor");

				//DilationRectangle1(ho_CreatRegionROI, &CreatRegions1, hv_Dilate, hv_Dilate);
				Connection(ho_CreatRegionROI, &CreatRegions1);
				Connection(ho_CreatRegionROI, &CreatRegions);
				CountObj(CreatRegions, &Number1);
				CountObj(CreatRegions1, &Number);
				if (Number1 > Number)
				{
					return 8;
				}
			}

			if (Number1 < 2)
			{
				return  3;
			}

		}

		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}

		//HTuple hv_PreprocessFlag = modelParaInput.peprocessFlag;

		try {

			creatModle(ho_ModelImageSrc1, &ho_ImagePart, &ho_FindRegionROI,
				&ho_Rectangle, contrstL, contrstH, contourNumber, hv_AngleStart, hv_AngleExtend, offset, hv_NumLevels,
				hv_Row1, hv_Column1, hv_Row2, hv_Column2, CreatRegions, hv_MinScore,
				hv_NumMatches, hv_Scale, hv_Dilate, &hv_ModelID1, &hv_modelRow2, &hv_modelColumn2,
				&hv_startRow, &ho_ContoursAffineTrans1, &ho_ContoursAffineTrans2);
			HTuple hv_modelColumn;

			output.ContoursAffineTrans1 = ho_ContoursAffineTrans1;
			output.ContoursAffineTrans2 = ho_ContoursAffineTrans2;
			output.shapeInfo.cregions1 = ho_region;
			//hv_modelColumn = hv_modelColumn2;
			output.shapeInfo.hv_modelColumn3 = hv_modelColumn2;
			output.shapeInfo.hv_modelRow3 = hv_modelRow2;
			output.shapeInfo.modelID = hv_ModelID1;//保存创建好的模型1句柄			
			output.shapeInfo.rectangle = ho_Rectangle;//模板图的ROI轮廓
			output.shapeInfo.findRegionROI = CreatRegions1;//查找匹配模板的ROI轮廓
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}

		HTuple hv_saveModelID_path = input.modelImgsPath1.saveModelIDpath.c_str();
		HTuple	hv_file_exists_flag;

		//存储模板图像
		SetHcppInterfaceStringEncodingIsUtf8(false);
		HTuple saveBImgpath = input.modelImgsPath1.saveBModelImgPath.c_str();
		HTuple saveDImgpath = input.modelImgsPath1.saveDModelImgPath.c_str();
		//HTuple saveImgpath = "D:/model";
		if (saveBImgpath != "" || saveDImgpath != "") {
			try {
				//WriteImage(modelImg, 'bmp', 0, "D:/model");
				WriteImage(BmodelImg, "bmp", 0, saveBImgpath);
				WriteImage(DmodelImg, "bmp", 0, saveDImgpath);
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				HTuple hv_Exception;
				except.ToHTuple(&hv_Exception);
				return 5;
			}
		}
		//存储模板矩形轮廓
		HTuple saveContourpath = input.modelImgsPath1.rectangle.c_str();
		if (saveContourpath != "") {
			WriteRegion(ho_Rectangle, saveContourpath);

		}
		//存储匹配轮廓modelID 
		if (input.modelImgsPath1.saveModelIDpath != "") {
			HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);

			HTuple hv_save_path;

			std::string path = input.modelImgsPath1.saveModelIDpath;
			int pos = path.find_last_of('.');
			if (pos != string::npos) { // 找到了"."，即文件后缀名
				path = path.substr(0, pos) + ".hdict"; // 替换为.hdict
			}
			hv_save_path = path.c_str();
			try {
				WriteDict(hv_ModelID1, hv_save_path, HTuple(), HTuple());
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				HTuple hv_Exception;
				except.ToHTuple(&hv_Exception);
				return 5;
			}
		}

		HTuple modelCol = input.modelImgsPath1.save_modelCol.c_str();
		if (input.modelImgsPath1.save_modelRow != "") {
			try {
				WriteTuple(output.shapeInfo.hv_modelColumn3, modelCol);
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				HTuple hv_Exception;
				except.ToHTuple(&hv_Exception);
				return 5;
			}
		}
		//}
		//存储放射变换ROW
		HTuple modelRow = input.modelImgsPath1.save_modelRow.c_str();
		if (input.modelImgsPath1.save_modelRow != "") {
			try {
				WriteTuple(output.shapeInfo.hv_modelRow3, modelRow);
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				HTuple hv_Exception;
				except.ToHTuple(&hv_Exception);
				return 5;
			}
		}
		//存储查找模型ROI
		HTuple save_FindROI = input.modelImgsPath1.FindRegionROI.c_str();
		if (input.modelImgsPath1.FindRegionROI != "") {

			WriteRegion(CreatRegions1, save_FindROI);

		}
		//模板图矩形ROI
		HTuple save_Rectangle = input.modelImgsPath1.rectangle.c_str();
		if (input.modelImgsPath1.rectangle != "") {

			//	WriteRegion(output.shapeInfo.rectangle, save_Rectangle);
		}
		// 存储检测ROI区域
		HTuple  save_Cregions1 = input.modelImgsPath1.cregions.c_str();
		if (input.modelImgsPath1.cregions != "") {
			try {
				//	WriteRegion(ho_region, save_Cregions1);
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				HTuple hv_Exception;
				except.ToHTuple(&hv_Exception);
				return 5;
			}
		}
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------creatTexture--------------------------//
	//功能：创建模板函数入口
	int creatTexture(const  CreatTextureInput& input, CreatTextureOutput& output) {

		try
		{
			return realCreatTexture(input, output);
		}
		catch (...)
		{
			return 5; //算法内部异常
		}
	}
}
