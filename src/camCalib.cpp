#include "alglibrary/zkhyProjectHuaNan/camCalib.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	int realCamCalib(CamCalibInput& input, CamCalibOutput& output)
	{
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		AlgParamCamCalib* algPara = (AlgParamCamCalib*)&input.algPara;

		HTuple hv_xMarkDotNum = algPara->i_params.block.xMarkDotNum;
		HTuple hv_yMarkDotNum = algPara->i_params.block.yMarkDotNum;
		HTuple hv_distMarkDot = algPara->d_params.block.distMarkDot;
		HTuple hv_focus = algPara->d_params.block.focus;
		HTuple hv_diameterMarkDot = algPara->d_params.block.diameterMarkDot;
		HTuple hv_sensorSize = algPara->d_params.block.sensorSize;
		HTuple hv_pixelDist = algPara->d_params.block.pixelDist;

		HTuple hv_imgsPath, hv_ImageFiles, hv_Index, hv_endIndex, hv_stepIndex;
		HTuple hv_TmpCtrl_Errors, hv_CameraParameters, hv_NumCamParam;
		HObject ho_CalibImage, ho_ImageTemp;
		HTuple hv_imgHeight, hv_imgWith, hv_cx, hv_cy;
		//参数设置合理性判断
		if (hv_xMarkDotNum.I() <= 0)
		{
			hv_xMarkDotNum = 7;
		}
		if (hv_yMarkDotNum.I() <= 0)
		{
			hv_yMarkDotNum = 7;
		}
		if (hv_distMarkDot.D() <= 0)
		{
			hv_distMarkDot = 2;
		}
		if (hv_diameterMarkDot.D() <= 0)
		{
			hv_diameterMarkDot = 1;
		}
		if (hv_pixelDist.D() <= 0)
		{
			hv_pixelDist = 0.13;
		}
		//批量读入标定板图像文件夹图像
		try {
			hv_imgsPath = HTuple(input.imgsPath.c_str());
			ListFiles(hv_imgsPath, (HTuple("files").Append("follow_links")), &hv_ImageFiles);
			TupleRegexpSelect(hv_ImageFiles, (HTuple("\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima|hobj)$").Append("ignore_case")),
				&hv_ImageFiles);
			ReadImage(&ho_ImageTemp, HTuple(hv_ImageFiles[0]));
			GetImageSize(ho_ImageTemp, &hv_imgWith, &hv_imgHeight);
			hv_cx = hv_imgWith / 2.0;
			hv_cy = hv_imgHeight / 2.0;

		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			return 1;
		}

		HTuple hv_TmpCtrl_ReferenceIndex, hv_TmpCtrl_PlateDescription;
		HTuple hv_StartParameters, hv_CalibHandle;
		HTuple hv_TmpCtrl_FindCalObjParNames, hv_TmpCtrl_FindCalObjParValues;
		//参数初始化
		try
		{
			hv_TmpCtrl_ReferenceIndex = 1;
			hv_StartParameters.Clear();
			hv_StartParameters.Append(hv_focus / 1000.0);
			hv_StartParameters.Append(0);
			hv_StartParameters.Append(hv_sensorSize / 1000000.0);
			hv_StartParameters.Append(hv_sensorSize / 1000000.0);
			hv_StartParameters.Append(hv_cx);
			hv_StartParameters.Append(hv_cy);
			hv_StartParameters.Append(hv_imgWith);
			hv_StartParameters.Append(hv_imgHeight);
			//hv_StartParameters.Append(hv_focus / 1000.0);
			//hv_StartParameters.Append(0);
			//hv_StartParameters.Append(0);
			//hv_StartParameters.Append(0);
			//hv_StartParameters.Append(0);
			//hv_StartParameters.Append(0);
			//hv_StartParameters.Append(hv_sensorSize / 1000000.0);
			//hv_StartParameters.Append(hv_sensorSize / 1000000.0);
			//hv_StartParameters.Append(hv_cx);
			//hv_StartParameters.Append(hv_cy);
			//hv_StartParameters.Append(hv_imgWith);
			//hv_StartParameters.Append(hv_imgHeight);

			hv_TmpCtrl_FindCalObjParNames.Clear();
			hv_TmpCtrl_FindCalObjParNames[0] = "gap_tolerance";
			hv_TmpCtrl_FindCalObjParNames[1] = "alpha";
			hv_TmpCtrl_FindCalObjParNames[2] = "skip_find_caltab";
			hv_TmpCtrl_FindCalObjParValues.Clear();
			hv_TmpCtrl_FindCalObjParValues[0] = 1;
			hv_TmpCtrl_FindCalObjParValues[1] = 1;
			hv_TmpCtrl_FindCalObjParValues[2] = "false";

			GenCaltab(hv_xMarkDotNum, hv_yMarkDotNum, hv_distMarkDot / 1000.0, hv_diameterMarkDot / hv_distMarkDot, "caltab.descr", "caltab.ps");
			hv_TmpCtrl_PlateDescription = "caltab.descr";
			CreateCalibData("calibration_object", 1, 1, &hv_CalibHandle);
			//area_scan_division-普通畸变，area_scan_polynomial-高精度畸变
			SetCalibDataCamParam(hv_CalibHandle, 0, "area_scan_division", hv_StartParameters);
			//SetCalibDataCamParam(hv_CalibHandle, 0, "area_scan_polynomial", hv_StartParameters);
			SetCalibDataCalibObject(hv_CalibHandle, 0, hv_TmpCtrl_PlateDescription);
		}

		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			return 2;
		}

		//相机标定
		try
		{
			hv_endIndex = (hv_ImageFiles.TupleLength()) - 1;
			hv_stepIndex = 1;

			for (hv_Index = 0; hv_Index.Continue(hv_endIndex, hv_stepIndex); hv_Index += hv_stepIndex)
			{
				ReadImage(&ho_CalibImage, HTuple(hv_ImageFiles[hv_Index]));
				//FindCalibObject(ho_CalibImage, hv_CalibHandle, 0, 0, hv_Index, hv_TmpCtrl_FindCalObjParNames,hv_TmpCtrl_FindCalObjParValues);
				FindCalibObject(ho_CalibImage, hv_CalibHandle, 0, 0, hv_Index, HTuple(), HTuple());
			}

			CalibrateCameras(hv_CalibHandle, &hv_TmpCtrl_Errors);
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			return 3;
		}

		//保存镜头畸变校正Mat图
		HTuple hv_CamParamOut, hv_SaveLensDistortCalibMapPath;
		HObject ho_LensDistortCalibMap;
		try
		{
			output.calibError = hv_TmpCtrl_Errors.D();
			GetCalibData(hv_CalibHandle, "camera", 0, "params", &hv_CameraParameters);
			ChangeRadialDistortionCamPar("adaptive", hv_CameraParameters, 0, &hv_CamParamOut);
			GenRadialDistortionMap(&ho_LensDistortCalibMap, hv_CameraParameters, hv_CamParamOut, "bilinear");
			hv_SaveLensDistortCalibMapPath = input.savePath.saveLensDistortCalibMapPath.c_str();
			WriteImage(ho_LensDistortCalibMap, "tiff", 0, hv_SaveLensDistortCalibMapPath);
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			return 4;
		}

		//获取参考图
		HObject ho_srcImage;
		if (input.srcImg.IsInitialized()) {
			ho_srcImage = input.srcImg;
		}
		else
		{
			return 5;
		}
		HTuple hv_calibPlateThick = algPara->d_params.block.calibPlateThick;
		HTuple hv_WidthTemp, hv_HeightTemp;
		GetImageSize(ho_srcImage, &hv_WidthTemp, &hv_HeightTemp);
		HTuple hv_poseOffsetX = hv_WidthTemp * hv_pixelDist / 1000.0 / 2.0;
		HTuple hv_poseOffsetY = hv_HeightTemp * hv_pixelDist / 1000.0 / 2.0;

		//透视变换保存Map图
		HTuple hv_RCoord, hv_CCoord, hv_PoseForCalibrationPlate, hv_FinalPose, hv_SavePerspectTransMapPath;
		HObject ho_PerspectTransMap;
		try
		{
			//内参初始化
			SetCalibDataCamParam(hv_CalibHandle, 0, HTuple(), hv_CameraParameters);
			FindCalibObject(ho_srcImage, hv_CalibHandle, 0, 0, 1, HTuple(), HTuple());
			hv_poseOffsetX = -hv_poseOffsetX;
			hv_poseOffsetY = -hv_poseOffsetY;
			GetCalibDataObservPoints(hv_CalibHandle, 0, 0, 1, &hv_RCoord, &hv_CCoord, &hv_Index,
				&hv_PoseForCalibrationPlate);
			SetOriginPose(hv_PoseForCalibrationPlate, hv_poseOffsetX, hv_poseOffsetY, hv_calibPlateThick / 1000.0, &hv_FinalPose);

			GenImageToWorldPlaneMap(&ho_PerspectTransMap, hv_CameraParameters, hv_FinalPose, hv_WidthTemp, hv_HeightTemp,
				hv_WidthTemp, hv_HeightTemp, hv_pixelDist / 1000.0, "bilinear");
			hv_SavePerspectTransMapPath = input.savePath.savePerspectTransMapPath.c_str();
			WriteImage(ho_PerspectTransMap, "tiff", 0, hv_SavePerspectTransMapPath);

		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			return 6;
		}

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//------------------------------------------相机标定 camCalib 20221123----------------------------------//
	//名称：  camCalib
	//功能：  相机标定
	//0-输入参数 1-输出参数
	//返回值：0-正常
	int camCalib(CamCalibInput& input, CamCalibOutput& output)
	{
		try
		{
			return realCamCalib(input, output);
		}
		catch (...)
		{
			return 7; //算法未知异常
		}
	}
}
