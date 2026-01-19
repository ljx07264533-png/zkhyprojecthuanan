#include "alglibrary/zkhyProjectHuaNan/QFNInfoSetup.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/BGAInfoSetup.h"
#include "alglibrary/zkhyProjectHuaNan/alglibmisc.h"
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
	int realQFNInfoSetup(const QFNSetupInput& input, QFNSetupOutput& output)
	{
		AlgParamQFNSetup* inputAlgParam = (AlgParamQFNSetup*)&input.alg;
		double bodySizeX = inputAlgParam->d_params.block.bodySizeX;    //封测尺寸x 长度
		double bodySizeY = inputAlgParam->d_params.block.bodySizeY;    //封测尺寸y 宽度
		double bodySizeZ = inputAlgParam->d_params.block.bodySizeZ;    //封测尺寸z 厚度                      
		int gridNumX = inputAlgParam->i_params.block.gridNumX;        //行数
		int gridNumY = inputAlgParam->i_params.block.gridNumY;        //列数  
		double gridOffsetX = inputAlgParam->d_params.block.gridOffsetX;  //网格中心与IC中心的x距离
		double gridOffsetY = inputAlgParam->d_params.block.gridOffsetY;  //网格中心与IC中心的y距离
		double gridPitchX = inputAlgParam->d_params.block.gridPitchX;   //网格x方向间距
		double gridPitchY = inputAlgParam->d_params.block.gridPitchY;   //网格y方向间距				
		double padWidth = inputAlgParam->d_params.block.padWidth;     //pad宽度
		double padLenght = inputAlgParam->d_params.block.padLenght;    //pad长度
		double padHeight = inputAlgParam->d_params.block.padHeight;    //pad厚度
		double distToTipX = inputAlgParam->d_params.block.distToTipX;   //到顶部x距离
		double distToTipY = inputAlgParam->d_params.block.distToTipY;   //到顶部y距离
		double leftUpCornerX = inputAlgParam->d_params.block.leftUpCornerX;   //左上角点x距离
		double leftUpCornerY = inputAlgParam->d_params.block.leftUpCornerY;   //左上角点y距离
		double centerPadWidth = inputAlgParam->d_params.block.centerPadWidth;   //中间pad的宽度
		double centerPadLenght = inputAlgParam->d_params.block.centerPadLenght;   //中间pad的长度
		double centerPadToLeft = inputAlgParam->d_params.block.centerPadToLeft;//中间pad中心到左边距离
		double centerPadToUp = inputAlgParam->d_params.block.centerPadToUp;//中间pad中心到上边距离
		double mmOfPixelX = inputAlgParam->d_params.block.mmOfPixelX;    //像素当量 mm/pixel
		double mmOfPixelY = inputAlgParam->d_params.block.mmOfPixelY;    //像素当量 mm/pixel
		vector<cv::Point2f> inputPoints = input.inputPoints;  //软件输入中心点坐标

		//软件输入参数判定
		if (bodySizeX <= 0 || bodySizeY <= 0 || bodySizeZ <= 0 || gridNumX <= 0 || gridNumY <= 0 || gridOffsetX < 0 || gridOffsetY < 0 ||
			gridPitchX <= 0 || gridPitchY <= 0 || padWidth <= 0 || padLenght <= 0 || padHeight <= 0 || distToTipX < 0 || distToTipY < 0 ||
			leftUpCornerX < 0 || leftUpCornerY < 0 ||
			centerPadWidth <= 0 || centerPadLenght <= 0 || centerPadToLeft <= 0 || centerPadToUp <= 0 || mmOfPixelX <= 0 || mmOfPixelY <= 0)
		{
			return 1;//输入参数错误
		}
		if (inputPoints.size() < 1)
		{
			return 2;//输入中心点个数为0
		}

		if (false)
		{
			bodySizeX = 8;    //封测尺寸x 长度
			bodySizeY = 8;    //封测尺寸y 宽度
			bodySizeZ = 1;    //封测尺寸z 厚度  
			gridNumX = 14;        //行数
			gridNumY = 14;        //列数 
			gridOffsetX = 0;  //网格中心偏移x距离
			gridOffsetY = 0;  //网格中心偏移y距离
			gridPitchX = 0.5;   //横向间距
			gridPitchY = 0.5;   //纵向间距
			padWidth = 0.28;     //pad宽度
			padLenght = 0.49;    //pad长度
			padHeight = 0.02;    //pad厚度
			distToTipX = 0;   //到顶部x距离
			distToTipY = 0;   //到顶部y距离
			centerPadLenght = 5.2;//中间pad的长度
			centerPadWidth = 5.2;//中间pad的宽度
			centerPadToLeft = 4;//中间pad中心到左边距离
			centerPadToUp = 4;//中间pad中心到上边距离
			leftUpCornerX = 0.64;   //左上角点x距离
			leftUpCornerY = 0.57;   //左上角点y距离
			mmOfPixelX = 0.014;    //x方向像素当量 mm/pixel
			mmOfPixelY = 0.014;    //y方向像素当量 mm/pixel
			//vector<cv::Point2f> inputPoints = input.inputPoints;  //软件输入中心点坐标
		}


		//1、计算上部分起始点第一个pad的中心点坐标
		double startPointXUp = leftUpCornerX + gridOffsetX;
		double startPointYUp = padLenght / 2.0 + distToTipY + gridOffsetY;
		//2、计算下部分起始点第一个pad的坐标
		double startPointXDown = leftUpCornerX + gridOffsetX;
		double startPointYDown = bodySizeY - padLenght / 2.0 - distToTipY + gridOffsetY;
		//3、计算左部分起始点第一个pad的坐标
		double startPointXLeft = padLenght / 2.0 + distToTipX + gridOffsetX;
		double startPointYLeft = leftUpCornerY + gridOffsetY;
		//4、计算右部分起始点第一个pad的坐标
		double startPointXRight = bodySizeX - padLenght / 2.0 - distToTipX + gridOffsetX;
		double startPointYRight = leftUpCornerY + gridOffsetY;

		//5、计算每一组pad的中心点坐标（分为 上下、左右）
		//vector<vector<cv::Point2f>> pos;//中心坐标点
		//HTuple pointRowsUorD, pointColsUorD, pointRowsLorR, pointColsLorR;
		vector<vector<PKGInfo>> posInfo;//中心坐标点+标志位
		vector<PKGInfo> firstRowPoints;
		vector<PKGInfo> secondRowPoints;
		PKGInfo onePKG;
		HTuple pointRows, pointCols;
		double pointRowX, pointRowY;
		//#pragma omp parallel for
		for (int j = 0; j < gridNumX; j++)
		{
			//上部分
			pointRowY = startPointYUp;
			pointRowX = startPointXUp + gridPitchX * j;
			onePKG.pos = cv::Point2f(pointRowX, pointRowY);
			onePKG.existFlag = 1;
			firstRowPoints.emplace_back(onePKG);
			//TupleConcat(pointRowsUorD, pointRowY, &pointRowsUorD);
			//TupleConcat(pointColsUorD, pointRowX, &pointColsUorD);
			//下部分
			pointRowY = startPointYDown;
			pointRowX = startPointXDown + gridPitchX * j;
			onePKG.pos = cv::Point2f(pointRowX, pointRowY);
			onePKG.existFlag = 1;
			secondRowPoints.emplace_back(onePKG);
			//TupleConcat(pointRowsUorD, pointRowY, &pointRowsUorD);
			//TupleConcat(pointColsUorD, pointRowX, &pointColsUorD);
		}
		posInfo.emplace_back(firstRowPoints);
		posInfo.emplace_back(secondRowPoints);

		firstRowPoints.clear();
		secondRowPoints.clear();
		double pointColX, pointColY;
		//#pragma omp parallel for
		for (int j = 0; j < gridNumY; j++)
		{
			//左部分
			pointRowX = startPointXLeft;
			pointRowY = startPointYLeft + gridPitchY * j;
			onePKG.pos = cv::Point2f(pointRowX, pointRowY);
			onePKG.existFlag = 1;
			firstRowPoints.emplace_back(onePKG);
			//TupleConcat(pointRowsLorR, pointRowY, &pointRowsLorR);
			//TupleConcat(pointColsLorR, pointRowX, &pointColsLorR);
			//右部分
			pointRowX = startPointXRight;
			pointRowY = startPointYRight + gridPitchY * j;
			onePKG.pos = cv::Point2f(pointRowX, pointRowY);
			onePKG.existFlag = 1;
			secondRowPoints.emplace_back(onePKG);
			//TupleConcat(pointRowsLorR, pointRowY, &pointRowsLorR);
			//TupleConcat(pointColsLorR, pointRowX, &pointColsLorR);
		}
		posInfo.emplace_back(firstRowPoints);
		posInfo.emplace_back(secondRowPoints);

		//6、软件输入的坐标与算法排布的坐标进行比对，找到pad缺失索引，给出标志位（KD-Tree方法）	
		kdtree kdTreerootDetectBall;
		pointKdTreeBuild3D(inputPoints, kdTreerootDetectBall);  // 建立KD树
		HTuple missPointRowUorD = HTuple(), missPointColUorD = HTuple();
		HTuple missPointRowLorR = HTuple(), missPointColLorR = HTuple();
		HTuple existPointRowUorD = HTuple(), existPointColUorD = HTuple();
		HTuple existPointRowLorR = HTuple(), existPointtColLorR = HTuple();
		//HTuple existPointR = HTuple(), existPointC = HTuple();
		int index = 0;
		for (int i = 0; i < posInfo.size(); i++)
		{
			for (int j = 0; j < posInfo[i].size(); j++)
			{
				Point2f nearestpoint, detectPoint;
				double disTmp = 0;
				detectPoint.x = posInfo[i][j].pos.x;
				detectPoint.y = posInfo[i][j].pos.y;
				//计算球距、最近点的坐标
				searchNearestNew3D(kdTreerootDetectBall, detectPoint, nearestpoint, disTmp, 1, 1);
				posInfo[i][j].pos.x = detectPoint.x / mmOfPixelX;
				posInfo[i][j].pos.y = detectPoint.y / mmOfPixelY;
				if (disTmp > padWidth * 0.01)
				{
					posInfo[i][j].existFlag = 0;
					if (i == 0 || i == 1)
					{
						TupleConcat(missPointRowUorD, posInfo[i][j].pos.y, &missPointRowUorD);
						TupleConcat(missPointColUorD, posInfo[i][j].pos.x, &missPointColUorD);
					}
					else
					{
						TupleConcat(missPointRowLorR, posInfo[i][j].pos.y, &missPointRowLorR);
						TupleConcat(missPointColLorR, posInfo[i][j].pos.x, &missPointColLorR);
					}
				}
				else
				{
					if (i == 0 || i == 1)
					{
						TupleConcat(existPointRowUorD, posInfo[i][j].pos.y, &existPointRowUorD);
						TupleConcat(existPointColUorD, posInfo[i][j].pos.x, &existPointColUorD);
					}
					else
					{
						TupleConcat(existPointRowLorR, posInfo[i][j].pos.y, &existPointRowLorR);
						TupleConcat(existPointtColLorR, posInfo[i][j].pos.x, &existPointtColLorR);
					}

				}
			}
		}
		output.QFNParam.posInfo = posInfo;
		//6、计算中心pad的坐标	
		double centerPadX = centerPadToLeft / mmOfPixelX;
		double centerPadY = centerPadToUp / mmOfPixelY;
		output.QFNParam.centerPadPoint = cv::Point2f(centerPadX, centerPadY);

		//计算图像像素的大小
		int allPixelsX = (int)(bodySizeX / mmOfPixelX);
		int allPixelsY = (int)(bodySizeY / mmOfPixelY);
		//7、创建一张类型为'byte'的图像，用于软件显示使用
		HObject ho_BigImageConstByte, ho_RegionUorD, ho_RegionLorR, ho_RegionDilUorD, ho_RegionDilLorR,
			ho_RegionDil, ho_BallImage, ho_Rectangle, ho_RegionCenter, ho_RegionDilCenter;
		GenImageConst(&ho_BigImageConstByte, "byte", allPixelsX, allPixelsY);
		GenRectangle1(&ho_Rectangle, 0, 0, allPixelsY, allPixelsX);
		PaintRegion(ho_Rectangle, ho_BigImageConstByte, &ho_BallImage, 100, "fill");//背景颜色填充
		//中心点构建空pad区域
		GenRegionPoints(&ho_RegionUorD, missPointRowUorD, missPointColUorD);
		DilationRectangle1(ho_RegionUorD, &ho_RegionDilUorD, padWidth / mmOfPixelX, padLenght / mmOfPixelX);
		GenRegionPoints(&ho_RegionLorR, missPointRowLorR, missPointColLorR);
		DilationRectangle1(ho_RegionLorR, &ho_RegionDilLorR, padLenght / mmOfPixelX, padWidth / mmOfPixelX);
		Union2(ho_RegionDilUorD, ho_RegionDilLorR, &ho_RegionDil);
		PaintRegion(ho_RegionDil, ho_BallImage, &ho_BallImage, 50, "fill");//空Pad 颜色填充
		//中心点构建全部pad区域
		GenRegionPoints(&ho_RegionUorD, existPointRowUorD, existPointColUorD);
		DilationRectangle1(ho_RegionUorD, &ho_RegionDilUorD, padWidth / mmOfPixelX, padLenght / mmOfPixelY);
		GenRegionPoints(&ho_RegionLorR, existPointRowLorR, existPointtColLorR);
		DilationRectangle1(ho_RegionLorR, &ho_RegionDilLorR, padLenght / mmOfPixelX, padWidth / mmOfPixelY);
		Union2(ho_RegionDilUorD, ho_RegionDilLorR, &ho_RegionDil);
		GenRegionPoints(&ho_RegionCenter, centerPadY, centerPadX);
		DilationRectangle1(ho_RegionCenter, &ho_RegionDilCenter, centerPadWidth / mmOfPixelX, centerPadLenght / mmOfPixelY);
		Union2(ho_RegionDil, ho_RegionDilCenter, &ho_RegionDil);
		PaintRegion(ho_RegionDil, ho_BallImage, &ho_BallImage, 255, "fill");//Pad 颜色填充
		//WriteImage(ho_BallImage, "bmp", 0, "C:\\Users\\yi.qiu\\Desktop\\QFNtest.bmp");

		//8、输出参数
		output.showImg = HObject2Mat(ho_BallImage).clone();
		output.QFNParam.bodySizeX = bodySizeX / mmOfPixelX;
		output.QFNParam.bodySizeY = bodySizeY / mmOfPixelY;
		output.QFNParam.bodySizeZ = bodySizeZ;
		output.QFNParam.gridNumX = gridNumX;
		output.QFNParam.gridNumY = gridNumY;
		output.QFNParam.gridOffsetX = gridOffsetX / mmOfPixelX;
		output.QFNParam.gridOffsetY = gridOffsetY / mmOfPixelY;
		output.QFNParam.gridPitchX = gridPitchX / mmOfPixelX;
		output.QFNParam.gridPitchY = gridPitchY / mmOfPixelY;
		output.QFNParam.padWidth = padWidth / mmOfPixelX;
		output.QFNParam.padLenght = padLenght / mmOfPixelX;
		output.QFNParam.padHeight = padHeight;
		output.QFNParam.distToTipX = distToTipX / mmOfPixelX;
		output.QFNParam.distToTipY = distToTipX / mmOfPixelY;
		output.QFNParam.leftUpCornerX = leftUpCornerX / mmOfPixelX;
		output.QFNParam.leftUpCornerY = leftUpCornerY / mmOfPixelY;
		output.QFNParam.centerPadWidth = centerPadWidth / mmOfPixelX;
		output.QFNParam.centerPadLenght = centerPadLenght / mmOfPixelY;
		output.QFNParam.centerPadToLeft = centerPadToLeft / mmOfPixelX;
		output.QFNParam.centerPadToUp = centerPadToUp / mmOfPixelY;
		output.QFNParam.mmOfPixelX = mmOfPixelX;
		output.QFNParam.mmOfPixelY = mmOfPixelY;
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int QFNInfoSetup(const QFNSetupInput& input, QFNSetupOutput& output)
	{
		try
		{
			return realQFNInfoSetup(input, output);
		}
		catch (...)
		{
			return 3; //PKG计算异常
		}
	}
}
