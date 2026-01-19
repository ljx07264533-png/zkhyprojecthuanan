#include "alglibrary/zkhyProjectHuaNan/waferImageProcess.h"
#include "alglibrary/alglibCuda.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::cuda;
using namespace alglib::ops::cuda::alg;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {

}

namespace alglib::ops::zkhyProHN::alg {
	//——————————————晶圆图像处理——————————————//
	//函数说明：通过解析运控的参数，对图像进行旋转、翻转、裁剪、存图等处理
	//WaferImageProcessInput &input;				            输入：待处理图、算法参数、运控的信息
	//WaferImageProcessOutput &output; 			            输出：处理后的图
	int waferImageProcess(WaferImageProcessInput& input, WaferImageProcessOutput& output)
	{
		try {
			cv::Mat mProcessImage = input.srcImage;
			HObject hProcessImage;

			//前置判断行列号是否正常，若是假图，否则直接返回错误码，后续算子不执行，达到加速的作用
			if (input.waferImageProcessInfo.nCol < 0 || input.waferImageProcessInfo.nRow < 0)
			{
				return 10;
			}


			if (!input.srcImage.empty() && input.srcImage.rows > 0)
				hProcessImage = Mat2HObjectDeepCopy(input.srcImage);
			else
				return 2;

			if (input.waferImageProcessInfo.Rotate.bEnable)
			{
				try
				{
					RotateImage(hProcessImage, &hProcessImage, input.waferImageProcessInfo.Rotate.sAngle, "constant");
				}
				catch (...)
				{
					return 3;
				}
			}

			if (input.waferImageProcessInfo.Mirror.bEnable)
			{
				try
				{
					if (input.waferImageProcessInfo.Mirror.sMirrorType == "Row")
						MirrorImage(hProcessImage, &hProcessImage, "row");
					else if (input.waferImageProcessInfo.Mirror.sMirrorType == "Column")
						MirrorImage(hProcessImage, &hProcessImage, "column");
					else
						return 4;
				}
				catch (...)
				{
					return 5;
				}
			}

			if (input.waferImageProcessInfo.ROIDomain.bEnable)
			{
				try
				{
					HObject hROIDomainRegion;
					GenRectangle1(&hROIDomainRegion, input.waferImageProcessInfo.ROIDomain.dRow1,
						input.waferImageProcessInfo.ROIDomain.dColumn1,
						input.waferImageProcessInfo.ROIDomain.dRow2,
						input.waferImageProcessInfo.ROIDomain.dColumn2);

					ReduceDomain(hProcessImage, hROIDomainRegion, &hProcessImage);
				}
				catch (...)
				{
					return 6;
				}
			}

			if (input.waferImageProcessInfo.ZoomSize.bEnable)
			{
				try
				{
					ZoomImageSize(hProcessImage, &hProcessImage, input.waferImageProcessInfo.ZoomSize.dWidth,
						input.waferImageProcessInfo.ZoomSize.dHeight, "constant");
				}
				catch (...)
				{
					return 7;
				}
			}

			if (input.waferImageProcessInfo.CutSize.bEnable)
			{
				try
				{
					HObject hROICutRegion;
					GenRectangle1(&hROICutRegion, input.waferImageProcessInfo.CutSize.dRow1,
						input.waferImageProcessInfo.CutSize.dColumn1,
						input.waferImageProcessInfo.CutSize.dRow2,
						input.waferImageProcessInfo.CutSize.dColumn2);

					ReduceDomain(hProcessImage, hROICutRegion, &hProcessImage);
					CropDomain(hProcessImage, &hProcessImage);
				}
				catch (...)
				{
					return 8;
				}
			}

			mProcessImage = HObject2Mat(hProcessImage);

			if (input.waferImageProcessInfo.ImageSave.bEnable)
			{
				try
				{
					size_t start = input.waferImageProcessInfo.sImageName.find('.'); // 找到逗号的位置，加上偏移量2
					string imageName = input.waferImageProcessInfo.sImageName.substr(0, start);
					string savePath = input.waferImageProcessInfo.ImageSave.sPath + "\\" + imageName + ".jpg";

					AlgParamWaferImageProcess* algPara = (AlgParamWaferImageProcess*)&input.algPara;


					if (algPara->i_params.block.saveImageType == 0)
					{
						cv::imwrite(savePath, mProcessImage, { cv::IMWRITE_JPEG_QUALITY, algPara->i_params.block.compressScale });
					}
					else if (algPara->i_params.block.saveImageType == 1)
					{
						HTuple hSavePath = savePath.c_str();
						WriteImage(hProcessImage, "jpg" + algPara->i_params.block.compressScale, 0, hSavePath);
					}
					else if (algPara->i_params.block.saveImageType == 2)
					{
						jpgGPUMatImrite(savePath, mProcessImage, algPara->i_params.block.compressScale);

					}
				}
				catch (...)
				{
					return 9;
				}
			}

			output.hOutImage = hProcessImage;
			output.mOutImage = mProcessImage;


			return 0;

		}
		catch (...)
		{
			return 10;
		}

	}

}
