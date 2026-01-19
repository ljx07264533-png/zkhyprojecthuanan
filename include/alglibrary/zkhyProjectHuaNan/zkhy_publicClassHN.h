//*********||************************************编写时间：2024年01月12日*******************************************||
//*********||************************************功能：华南算法库公共类 *********************************||

#ifndef ZKHYPROJECTHUANAN_PUBLICCLASSHN_H
#define ZKHYPROJECTHUANAN_PUBLICCLASSHN_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"

namespace zkhyPublicClass {
	//*************************** 获取产品信息类 用于解析平台3.0的产品信息数据 ***************************//
	class productInfoBase		//产品信息
	{
	public:
		std::string productID;
		std::string station;
		std::string QRCode;

		// 初始化
		productInfoBase()
		{
			this->productID = "";
			this->station = "";
			this->QRCode = "";
		}

		virtual ~productInfoBase() {}

		virtual int getProductInfo(char productInfoInput[]);  //解析产品信息

	};

	class productInfoReel : public productInfoBase		//卷带机产品信息
	{
	public:
		std::string RollID;
		std::string lotID;

		// 初始化
		productInfoReel()
		{
			this->RollID = "";
			this->lotID = "";
		}

		virtual ~productInfoReel() {}

		virtual int getProductInfo(char productInfoInput[]) override;  //解析产品信息

	};

	class productInfoMINILED :public productInfoBase		//MINILED产品信息
	{
	public:
		std::string col;
		std::string row;
		std::string dataTime;

		productInfoMINILED()
		{
			col = "";
			row = "";
			dataTime = "";
		}

		virtual ~productInfoMINILED() {}

		virtual int getProductInfo(char productInfoInput[]) override;  //解析产品信息

	};

	class productInfoWAFER :public productInfoBase		//晶圆产品信息
	{
	public:
		int col;
		int row;
		int ProductId;
		int channel;

		productInfoWAFER()
		{
			col = 0;
			row = 0;
			channel = 0;
			ProductId = 0;
		}

		virtual ~productInfoWAFER() {}

		// 派生类中实现该函数
		virtual int getProductInfo(char productInfoInput[]) override;  //解析产品信息

	};

	class enviromentInfoBase
	{
	public:
		std::string productID;
		std::string timeStamp;

		enviromentInfoBase()
		{
			productID = "";
			timeStamp = "";
		}

		virtual ~enviromentInfoBase() {}

		// 派生类中自行实现该函数
		virtual int getEnvironmentInfo(char environmentInfoInput[]);
	};

	// 算法类
	class AlgorithmBase
	{
	public:
		AlgorithmBase() {}
		virtual ~AlgorithmBase() {}
	};

	// 基于Halcon measurePos的测量类
	class measure1D : public AlgorithmBase
	{
	public:
		~measure1D() {
			CloseMeasure(hMeasureRectangle);
		};

		void setMeasureRect(const double& rowStart, const double& colStart, const double& rowEnd, const double& colEnd, const double& rectHeight);

		void setMeasureParams(const double& smoothFactor, const double& edgeStrength, const alglib::ops::zkhyProHN::edgePolarity& polarity, const alglib::ops::zkhyProHN::edgeSelect& select);

		void genMeasureHandle(const HalconCpp::HObject& hImage);

		void transMeasureHandle(const double& row, const double& col);

		int rect2MeasureRect(alglib::core::RectangleROI inputROI);		// 利用扫描矩形生成测量矩形

		int execute(const HalconCpp::HObject& hImage);	// 执行测量

		int getResult(HalconCpp::HTuple& measureRows, HalconCpp::HTuple& measureCols, HalconCpp::HTuple& measureAmplitudes, HalconCpp::HTuple& measureDistances);	// 获取测量结果

		int getRes2Disp(HalconCpp::HObject& hMeasureRectDisp, HalconCpp::HObject& hMeasureResDsip);	  // 获取测量结果，用于显示

	protected:
		HalconCpp::HTuple hRectCenterRow, hRectCenterCol, hRectPhi, hRectLength1, hRectLength2, hMeasureRows, hMeasureCols, hMeasureAmplitudes, hMeasureDistances, hPolarity, hSelect, hMeasureRectangle;
		double smoothFactor, edgeStrength;
	};
}

#endif //ZKHYPROJECTHUANAN_PUBLICCLASSHN_H