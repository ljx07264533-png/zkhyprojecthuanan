
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_MARKDETECT_H
#define ZKHYPROJECTHUANAN_MARKDETECT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体


	//-------------------------------   Mark 检测 算子 srart    -------------------//
	//Step1算子input结构体、output结构体
	struct MarkDetectInput
	{
		HalconCpp::HObject srcImg;										//原图
		alglib::core::CSYSTransPara template2DetectTransPara;				// edgeAlign输入的坐标系
		std::vector<cv::Point2f> edgeAlign;						// edge Align的结果输入,5个点
		std::vector<cv::Point2f> invertRoi;						// 反料检测区域
		std::vector<cv::Point2f> detectRoi;							// 检测区域
		HalconCpp::HTuple inTeachDictHandle;							// 建模时保存的字典
		std::vector<int> OCRCharIndex;							// OCR识别中字母的索引	
		alglib::core::AlgrithmParams algPara;							    //算法参数
	};
	struct MarkDetectOutput
	{
		HalconCpp::HObject ho_markCorrectionRegions;				// mark校正后的模板字符区域
		std::vector<std::vector<cv::Point2f>> templateCharContour;	// mark模板字符轮廓
		std::vector<alglib::core::FlawInfoStruct>	flawinfos;		//缺陷所有信息
		std::vector<alglib::core::FlawInfoStruct>	flawStatic;		//缺陷统计信息
		std::string OCRResult;		// OCR 识别结果
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct AlgParaMarkDetectInputInt//int型算法参数，独立出来方便使用默认构造函数
	{
		int lowThreshold = 105;				// 阈值化的低阈值
		int highThreshold = 255;			// 阈值化的高阈值
		int minInvertProductArea = 100;		// 反料最小面积阈值
		int charSearchOffsetX = 30;			// 字符搜索X方向外扩长度
		int charSearchOffsetY = 20;			// 字符搜索Y方向外扩长度
		int iThresholdFunc = 0;             // 阈值方法
		int iTargetProperty = 0;            // 目标亮暗
		int isBinarization = 0;             // 是否二值化
		int isUseOCR = 0;					// 是否使用OCR
		int iOCRClassifier = 0;				// OCR分类器，0-mlp，1-cnn
	};
	struct AlgParaMarkDetectInputDouble//double型算法参数，独立出来方便使用默认构造函数
	{
		double minMarkMatchScore = 0.3;    // mark最小匹配置信度
		double minCharMatchScore = 0.6;    // 字符最小匹配置信度
		double dSmoothFactor = 0.5;	       // 平滑因子
		double dCharWidth = 20.0;          // 字符宽度
		double dCharHeight = 20.0;         // 字符高度
		double dCharArea = 30.0;		   // 筛选面积小的噪声，保留字符
		double dOpeningRadius = 3;             // 开运算半径
	};
	struct AlgParaMarkDetect
	{
		union
		{
			AlgParaMarkDetectInputInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaMarkDetectInputDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgMarkDetect
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgMarkDetect()
		{
			msgs.push_back("lowThreshold		$Int       $I_Spinbox	 $105  	 $字符低阈值				$字符低阈值				$10");
			msgs.push_back("highThreshold		$Int       $I_Spinbox	 $255  	 $字符高阈值				$字符高阈值				$20");
			msgs.push_back("minInvertProductArea   $Int    $I_Spinbox   $100     $反料判断最小面积	        $反料判断最小面积    $51");
			msgs.push_back("charSearchOffsetX	$Int       $I_Spinbox	 $30  	 $字符搜索X方向外扩长度		$字符搜索X方向外扩长度	$40");
			msgs.push_back("charSearchOffsetY	$Int       $I_Spinbox	 $20  	 $字符搜索Y方向外扩长度		$字符搜索Y方向外扩长度	$50");
			msgs.push_back("iThresholdFunc		$Int       $Combox	 $0  	 $阈值分割方法				$阈值分割方法，可选双阈值和大津法				$8");
			msgs.push_back("iTargetProperty		$Int       $Combox	 $0  	 $目标亮暗				$目标字符是相较于背景的亮暗程度				$25");
			msgs.push_back("isBinarization		$Int       $CheckBox	 $0  	 $是否二值化				$是否对图像进行二值化				$5");
			msgs.push_back("iAngle		$Int       $I_Spinbox	 $0  	 $字符旋转角度				$字符旋转角度				$80");
			msgs.push_back("minMarkMatchScore  $Double    $D_Spinbox   $0.3      $mark最小匹配置信度         $mark最小匹配置信度    $60");
			msgs.push_back("minCharMatchScore  $Double    $D_Spinbox   $0.6      $字符最小匹配置信度         $字符最小匹配置信度    $70");
			msgs.push_back("dSmoothFactor			$double       $D_Spinbox	 $30.0  	 $平滑因子				$图像平滑滤波因子				$29");
			msgs.push_back("dCharWidth  $Double    $D_Spinbox   $20.0      $字符宽度         $字符最小宽度    $31");
			msgs.push_back("dCharHeight  $Double    $D_Spinbox   $20.0      $字符高度         $字符最小高度    $32");
			msgs.push_back("dCharArea			$double       $D_Spinbox	 $30.0  	 $字符面积				$字符面积最小值				$30");
			msgs.push_back("dOpeningRadius  $Double    $D_Spinbox   $3      $开运算半径         $字符边缘开运算半径    $75");
			msgs.push_back("isUseOCR		$Int       $CheckBox	 $0  	 $是否使用OCR				$是否使用OCR，0-禁用，1-启用			$80");
			msgs.push_back("iOCRClassifier		$Int       $Combox	 $0  	 $OCR分类器				$OCR分类器，￥MLP，￥CNN				$85");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgMarkDefectDet
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgMarkDefectDet()
		{
			// 封测Mark特征
			exmsgs.push_back("markPositionOffset          $Double	   $mark姿态偏移			$mark姿态偏移				$110   $OF ");
			exmsgs.push_back("markAngleOffset			  $Double	   $mark角度偏移			$mark角度偏移				$111   $OF ");
			exmsgs.push_back("markMarginTop			  $Double	   $mark到产品上边缘距离	$mark到产品上边缘距离       $112   $OF ");
			exmsgs.push_back("markMarginBottom        $Double	   $mark到产品下边缘距离	$mark到产品下边缘距离       $113   $OF ");
			exmsgs.push_back("markMarginLeft          $Double	   $mark到产品左边缘距离	$mark到产品左边缘距离       $114   $OF ");
			exmsgs.push_back("markMarginRight         $Double	   $mark到产品右边缘距离	$mark到产品右边缘距离       $115   $OF ");
			exmsgs.push_back("markCharOffset          $Double	   $单个字符偏移			$单个字符偏移				$116   $OF ");
			exmsgs.push_back("markCharContrast        $Double	   $单个字符对比度			$单个字符对比度				$117   $OF ");
			exmsgs.push_back("markCharUnderPrintArea  $Double	   $单个字符少打印面积		$单个字符少打印面积         $119   $OF ");
			exmsgs.push_back("markCharMorePrintArea   $Double	   $单个字符多打印面积		$单个字符多打印面积         $120   $OF ");
			exmsgs.push_back("markCharUnderPrintRate  $Double	   $单个字符少打印比例		$单个字符少打印比例         $121   $OF ");
			exmsgs.push_back("markCharMorePrintRate   $Double	   $单个字符多打印比例		$单个字符多打印比例         $122   $OF ");
			exmsgs.push_back("markCharMatchRate       $Double	   $单个字符匹配率			$单个字符匹配率				$118   $OF ");
			exmsgs.push_back("markIsInvertProduct	  $Double	   $放料是否异常			$放料是否异常				$130   $OF ");
			exmsgs.push_back("markCharMissing		    $Double	   $字符漏印			    $字符漏印				    $140   $OF");
			exmsgs.push_back("markCharOffsetX   $Double	   $单个字符X方向偏移量		$单个字符X方向偏移量         $141   $OF");
			exmsgs.push_back("markCharOffsetY  $Double	   $单个字符Y方向偏移量		$单个字符Y方向偏移量         $142   $OF");
			exmsgs.push_back("markCharOffsetR   $Double	   $单个字符偏移量		$单个字符偏移量         $143   $OF");
			exmsgs.push_back("markContrast		$Double	   $字符整体对比度			$字符整体对比度				$144   $OF");
			exmsgs.push_back("markCharDistance		$Double	   $字符间距			$字符间距				$145   $OF");
			exmsgs.push_back("markCharBlobSize		$Double	   $Blob面积			$每个字符的blob（多打印和少打印）面积				$146   $OF");
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgMarkDetect
	{
		std::vector<std::string> msgs;
		AlgErrMsgMarkDetect()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:Mark模型输入异常");
			msgs.push_back("3:edgeAlign输入异常");
			msgs.push_back("4:输入区域解析异常");
			msgs.push_back("5:Mark模型解析异常");
			msgs.push_back("6:Mark匹配失败");
			msgs.push_back("7:算法未知异常");
			msgs.push_back("8:反料检测区域异常");
		}
	};
	//-------------------------------   Mark 检测 算子 end    ---------------------------------------//

	//! 接口

	namespace alg {
		
		//----------------------------- MarkDetect  ------------------------------------------//
		//函数说明：mark异常检测
		//MarkDetectInput &input;				输入：j检测输入参数
		//MarkDetectOutput &output;				输出：模板图建模的每个字符轮廓
		_ALGLIBRARY_API int MarkDetect(const MarkDetectInput& input, MarkDetectOutput& output);
	}
	//--------------------------------------markDetRegionToDefectStruct--------------------------------------------------//
	//功能：计算输入区域的面积、角度、外接矩形以及轮廓等基本特征数据
	//输入：defectRegion 缺陷区域，输入为单个缺陷的区域
	//		   useContour 是否计算缺陷轮廓，true为计算轮廓，false为不计算轮廓，默认为计算
	//		   useRotateRect 计算区域的外接旋转矩形还是正矩形，true为旋转矩形，false为正矩形，默认为旋转矩形
	//		   homMat2D 坐标变换矩阵，用于对缺陷区域进行映射，默认为空
	//输出：outFlaw 缺陷结构体
	void markDetRegionToDefectStruct(HalconCpp::HObject& defectRegion, alglib::core::FlawInfoStruct& outFlaw, const bool& useContour = true, const bool& useRotateRect = true,const HalconCpp::HTuple& homMat2D = HalconCpp::HTuple());
	// 重载，显示的轮廓为传入的contourRegion轮廓，其他缺陷信息则由defectRegion生成，用于显示轮廓不为缺陷真实轮廓的情况
	void markDetRegionToDefectStruct(HalconCpp::HObject& defectRegion, HalconCpp::HObject& contourRegion, alglib::core::FlawInfoStruct& outFlaw, const bool& useContour = true, const bool& useRotateRect = true, const HalconCpp::HTuple& homMat2D = HalconCpp::HTuple());
	//--------------------------------------testRegionsToDefectStruct--------------------------------------------------//
	//功能：计算输入区域的面积、角度、外接矩形以及轮廓等基本特征数据
	//输入：defectRegion 缺陷区域，输入单个或者多个缺陷区域
	//		   useContour 是否计算缺陷轮廓，true为计算轮廓，false为不计算轮廓，默认为计算
	//		   useRotateRect 计算区域的外接旋转矩形还是正矩形，true为旋转矩形，false为正矩形，默认为旋转矩形
	//		   homMat2D 坐标变换矩阵，用于对缺陷区域进行映射，默认为空
	//输出：outFlaw 缺陷结构体
	void testRegionsToDefectStruct(HalconCpp::HObject& defectRegions, std::vector<alglib::core::FlawInfoStruct>& outFlaw, const bool& useContour = true, const bool& useRotateRect = true, const HalconCpp::HTuple& homMat2D = HalconCpp::HTuple());
	//--------------------------------------computePointsDisX--------------------------------------------------//


}

#endif	// ZKHYPROJECTHUANAN_MARKDETECT_H
