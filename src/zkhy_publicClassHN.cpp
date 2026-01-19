//*********||************************************编写时间：2024年01月12日*******************************************||
//*********||************************************功能：华南算法库公共类 *********************************||
// 
// 该文件用于实现类中的部分函数
#include <numeric>
#include <string>
#include <vector>

#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"
#include "alglibrary/zkhyProjectHuaNan/cJSON.h"
#include "alglibrary/zkhyProjectHuaNan/utils.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace HalconCpp;
using namespace zkhyPublicClass;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;


// productInfoBase 函数实现
int productInfoBase::getProductInfo(char productInfoInput[])
{
	// 解析产品信息
	cJSON* root = cJSON_Parse(productInfoInput);
	if (root)
	{
		cJSON* pJson = cJSON_GetObjectItem(root, "ProductId");
		if (pJson)
			this->productID = to_string(pJson->valueint);

		pJson = cJSON_GetObjectItem(root, "Station");
		if (pJson)
			this->station = to_string(pJson->valueint);

		pJson = cJSON_GetObjectItem(root, "QRCode");
		if (pJson)
			this->QRCode = pJson->valuestring;
	}
	cJSON_Delete(root);
	return 0;
}

// productInfoReel 函数实现
int productInfoReel::getProductInfo(char productInfoInput[])
{
	// 解析产品信息
	cJSON* root = cJSON_Parse(productInfoInput);
	if (root)
	{
		cJSON* pJson = cJSON_GetObjectItem(root, "ProductId");
		if (pJson)
			this->productID = to_string(pJson->valueint);

		pJson = cJSON_GetObjectItem(root, "LotId");
		if (pJson)
			this->lotID = pJson->valuestring;

		pJson = cJSON_GetObjectItem(root, "Station");
		if (pJson)
			this->station = to_string(pJson->valueint);

		pJson = cJSON_GetObjectItem(root, "RollID");
		if (pJson)
			this->RollID = to_string(pJson->valueint);

		pJson = cJSON_GetObjectItem(root, "QRCode");
		if (pJson)
			this->QRCode = pJson->valuestring;
	}
	cJSON_Delete(root);

	return 0;
}

// productInfoMINILED 函数实现
int productInfoMINILED::getProductInfo(char productInfoInput[])
{
	// 获取产品ID 二维码
	cJSON* root = cJSON_Parse(productInfoInput);

	if (root)
	{
		cJSON* pJson = cJSON_GetObjectItem(root, "productId");
		if (pJson)
		{
			this->productID = to_string(pJson->valueint);
		}
		pJson = cJSON_GetObjectItem(root, "QRCode");
		if (pJson)
		{
			this->QRCode = pJson->valuestring;
		}
		pJson = cJSON_GetObjectItem(root, "Station");
		if (pJson)
		{
			this->station = to_string(pJson->valueint);
		}
		pJson = cJSON_GetObjectItem(root, "nRow");
		if (pJson)
		{
			this->row = to_string(pJson->valueint);
		}
		pJson = cJSON_GetObjectItem(root, "nCol");
		if (pJson)
		{
			this->col = to_string(pJson->valueint);
		}
		pJson = cJSON_GetObjectItem(root, "DataTime");
		if (pJson)
		{
			this->dataTime = pJson->valuestring;
		}
	}
	cJSON_Delete(root);
	return 0;
}

// productInfoWAFER函数实现
int productInfoWAFER::getProductInfo(char productInfoInput[])
{
	// 获取产品ID 二维码
	cJSON* root = cJSON_Parse(productInfoInput);

	if (root)
	{
		cJSON* pJson = cJSON_GetObjectItem(root, "nChannel");
		if (pJson)
		{
			this->channel = pJson->valueint;
		}
		pJson = cJSON_GetObjectItem(root, "ProductId");
		if (pJson)
		{
			this->ProductId = pJson->valueint;
		}
		pJson = cJSON_GetObjectItem(root, "nRow");
		if (pJson)
		{
			this->row = pJson->valueint;
		}
		pJson = cJSON_GetObjectItem(root, "nCol");
		if (pJson)
		{
			this->col = pJson->valueint;
		}
	}
	cJSON_Delete(root);
	return 0;
}

// enviromentInfoBase 函数实现
int enviromentInfoBase::getEnvironmentInfo(char environmentInfoInput[])
{
	// 获取产品ID 二维码
	cJSON* root = cJSON_Parse(environmentInfoInput);

	if (root)
	{
		cJSON* pJson = cJSON_GetObjectItem(root, "Product_ID");
		if (pJson)
		{
			this->productID = to_string(pJson->valueint);
		}

		pJson = cJSON_GetObjectItem(root, "TimeStamp");
		if (pJson)
		{
			this->timeStamp = pJson->valuestring;
		}
	}
	cJSON_Delete(root);

	return 0;
}

// measure1D函数实现

// 设置测量矩形
void measure1D::setMeasureRect(const double& rowStart, const double& colStart, const double& rowEnd, const double& colEnd, const double& rectHeight)
{
	// 设置测量矩形
	HTuple hRectDr, hRectDc;

	hRectCenterRow = (rowStart + rowEnd) * 0.5;
	hRectCenterCol = (colStart + colEnd) * 0.5;

	hRectDr = rowEnd - rowStart;
	hRectDc = colEnd - colStart;
	hRectPhi = atan2(hRectDr.D(), hRectDc.D());

	HTuple test = hRectDr.TupleAtan2(hRectDc);

	hRectLength1 = (hRectDr * hRectDr + hRectDc * hRectDc).TupleSqrt() * 0.5;
	hRectLength2 = rectHeight * 0.5;

}

// 设置测量参数
void measure1D::setMeasureParams(const double& smoothFactor, const double& edgeStrength, const edgePolarity& polarity, const edgeSelect& select)
{
	// 设置测量参数
	this->smoothFactor = smoothFactor;
	this->edgeStrength = edgeStrength;
	getHedgePolarity(polarity, hPolarity);
	getHedgeSelect(select, hSelect);
}

// 利用扫描矩形生成测量矩形
int measure1D::rect2MeasureRect(RectangleROI inputROI)
{
	// 解析输入ROI

	return 0;
}

void measure1D::genMeasureHandle(const HObject& hImage)
{

	// 获取输入图像尺寸
	HTuple hImgWidth, hImgHeight;
	GetImageSize(hImage, &hImgWidth, &hImgHeight);
	// 构建扫描矩形
	GenMeasureRectangle2(hRectCenterRow, hRectCenterCol, hRectPhi, hRectLength1, hRectLength2, hImgWidth, hImgHeight, "nearest_neighbor", &hMeasureRectangle);
}


void measure1D::transMeasureHandle(const double& row, const double& col)
{
	TranslateMeasure(hMeasureRectangle, row, col);
}



// 执行测量
int measure1D::execute(const HObject& hImage)
{

	try
	{
		if (smoothFactor > 0.5 * hRectLength1)
			smoothFactor = 0.4;

		// 执行测量
		MeasurePos(hImage, hMeasureRectangle, smoothFactor, edgeStrength, hPolarity, hSelect, &hMeasureRows, &hMeasureCols, &hMeasureAmplitudes, &hMeasureDistances);
	}
	catch (HException& except)
	{
		std::string errorMsg = except.ErrorMessage().Text();

		LOG.log(errorMsg, alglib::core::err);

		return 1;
	}

	LOG.log("#measure1D#执行成功。", alglib::core::err);

	return 0;
}

// 获取测量结果
int measure1D::getResult(HTuple& measureRows, HTuple& measureCols, HTuple& measureAmplitudes, HTuple& measureDistances)
{
	measureRows = hMeasureRows;
	measureCols = hMeasureCols;
	measureAmplitudes = hMeasureAmplitudes;
	measureDistances = hMeasureDistances;

	return 0;
}

// 获取测量结果用于显示
int measure1D::getRes2Disp(HObject& hMeasureRectDisp, HObject& hMeasureResDsip)
{
	GenRectangle2(&hMeasureRectDisp, hRectCenterRow, hRectCenterCol, hRectPhi, hRectLength1, hRectLength2);

	// 设置显示矩形的角度
	HTuple hRectPhiDisp, hRectDispLen1, hRectDispLen2;
	int pointsNum = hMeasureRows.Length();
	double phi = hRectPhi.D() - PI / 2;
	for (int i = 0; i < pointsNum; i++)
	{
		hRectPhiDisp.Append(phi);
		hRectDispLen1.Append(hRectLength2);
		hRectDispLen2.Append(1);
	}

	GenRectangle2(&hMeasureResDsip, hMeasureRows, hMeasureCols, hRectPhiDisp, hRectDispLen1, hRectDispLen2);

	return 0;
}