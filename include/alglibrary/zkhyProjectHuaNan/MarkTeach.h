
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_MARKTEACH_H
#define ZKHYPROJECTHUANAN_MARKTEACH_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-------------------------------   Mark建模 算子 srart    ---------------------------------------//
	//Step1算子input结构体、output结构体
	struct MarkTeachInput
	{
		HalconCpp::HObject srcImg;										 // 原图
		std::vector<cv::Point2f> edgeAlign;					 // edge Align的结果输入
		std::string dictSavePath;								 // 建模保存的路径，保存的文件名为MarkTeachInfo
		std::vector<std::vector<cv::Point2f>> charRois;		 // 画字符的多个小框；
		std::vector<std::vector<cv::Point2f>> markRoi;					 // mark待检测的所有字符
		alglib::core::AlgrithmParams algPara;								 // 算法参数
	};
	struct MarkTeachOutput
	{
		std::vector<std::vector<cv::Point2f>> charTeachCnts;      //用于建模后每个字符的轮廓显示
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct AlgParaMarkTeachInputInt//int型算法参数，独立出来方便使用默认构造函数
	{
		int lowThreshold = 105;    // 阈值化的低高阈值
		int highThreshold = 255;
		int minCharArea = 30;     // 字符最小面积
		int maxCharArea = 3000;   // 字符最大面积

		int angleStart = -20;       // 建模选择角度
		int angleExtent = 20;

		int iThresholdFunc = 0;             // 阈值方法
		int iTargetProperty = 0;            // 目标亮暗
		int isBinarization = 0;             // 是否二值化
		int iSpecDetectType = 0;    // 检测场景， 0-封测， 1- 其它
		int iPyramidLevel = 3;	    // 匹配金字塔层数，默认为3，0为自动
		int iAngle = 0;             // 字符旋转角度
	};
	struct AlgParaMarkTeachInputDouble//double型算法参数，独立出来方便使用默认构造函数
	{
		double minMarkMatchScore = 0.3;    // mark最小匹配置信度
		double minCharMatchScore = 0.6;    // 字符最小匹配置信度
	};
	struct AlgParaMarkTeach
	{
		union
		{
			AlgParaMarkTeachInputInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaMarkTeachInputDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgMarkTeach
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgMarkTeach()
		{
			msgs.push_back("lowThreshold		$Int       $I_Spinbox	 $105  	 $字符低阈值				$字符低阈值				$10");
			msgs.push_back("highThreshold		$Int       $I_Spinbox	 $255  	 $字符高阈值				$字符高阈值				$20");
			msgs.push_back("minCharArea			$Int       $I_Spinbox	 $30  	 $字符最小面积				$字符最小面积			$30");
			msgs.push_back("maxCharArea			$Int       $I_Spinbox	 $3000 	 $字符最大面积				$字符最大面积			$40");
			msgs.push_back("angleStart			$Int       $I_Spinbox	 $-20  	 $字符建模起始角度			$字符建模起始角度		$50");
			msgs.push_back("angleExtent			$Int       $I_Spinbox	 $20  	 $字符角度范围				$字符角度范围			$60");
			msgs.push_back("minMarkMatchScore   $Double    $D_Spinbox   $0.3      $mark最小匹配置信度         $mark最小匹配置信度    $70");
			msgs.push_back("minCharMatchScore   $Double    $D_Spinbox   $0.6      $字符最小匹配置信度         $字符最小匹配置信度    $80");
			msgs.push_back("iThresholdFunc		$Int       $Combox	 $0  	 $阈值分割方法				$阈值分割方法，可选双阈值和大津法				$8");
			msgs.push_back("iTargetProperty		$Int       $Combox	 $0  	 $目标亮暗				$目标字符是相较于北京的亮暗程度				$25");
			msgs.push_back("isBinarization		$Int       $CheckBox	 $0  	 $是否二值化				$是否对图像进行二值化				$5");
			msgs.push_back("iSpecDetectType		$Int       $Combox	 $0  	 $检测类型				$检测场景， 0-封测， 1- 其它				$3");
			msgs.push_back("iPyramidLevel			$Int       $I_Spinbox	 $3  	 $金字塔层数				$匹配金字塔层数，默认为3，0为自动			$28");
			msgs.push_back("iAngle		$Int       $I_Spinbox	 $0  	 $字符旋转角度				$字符旋转角度				$90");
		}
	};
	//Step4 异常返回值描述
	struct AlgErrMsgMarkTeach
	{
		std::vector<std::string> msgs;
		AlgErrMsgMarkTeach()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:创建模板失败，请检查图像质量");
			msgs.push_back("3:算法未知异常");
			msgs.push_back("4:edgeAlign输入异常");
			msgs.push_back("5:字符Roi输入异常");
			msgs.push_back("6:MarkRoi输入异常");
			msgs.push_back("7:保存模型异常，请检查保存路径，只支持纯英文路径");
		}
	};
	//-------------------------------   Mark 建模 算子 end    ---------------------------------------//

	//! 接口

	namespace alg {
		
		//----------------------------- MarkTeach  ------------------------------------------//
		//函数说明：mark建模
		//MarkTeachInput &input;				输入：模板图像，建模保存路径，输入ROI，算法参数
		//MarkTeachOutput &output;				输出：模板图建模的每个字符轮廓
		_ALGLIBRARY_API int MarkTeach(const MarkTeachInput& input, MarkTeachOutput& output);
	}
	int Contours2Region(std::vector<cv::Point2f>& input, HalconCpp::HObject* ho_region);
	inline int computeAngle(const cv::Point2f& point1, const cv::Point2f& point2, double& angle);
	void computeRotateHomMat2D(const double& width, const double& height, const double& angle, double& newWidth, double& newHeight, HalconCpp::HTuple& hHomMat2D);
}

#endif	// ZKHYPROJECTHUANAN_MARKTEACH_H
