
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_CREATVARIATIONTEXTURE_H
#define ZKHYPROJECTHUANAN_CREATVARIATIONTEXTURE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//----------------------------------------------创建差异化模型（标准模板图）-------------------------------------//

	struct  CreatVariationTextureInput
	{
		cv::Mat srcImg;                  //将该图用于创建模型绘制ROI。
		std::vector<std::vector<cv::Point2f>>  posCreatROI; //绘制创建模型时的ROI。
		alglib::core::AlgrithmParams alg1;           //算法参数
		std::string  creatVarImgPath = "";  //创建差异化模型的图片文件夹。
		HalconCpp::HObject  CreatRoi;          //创建模型时的H轮廓。
		cv::String imgID;              //图像ID(提供的路径下 只需要筛选出对应ID的图)
		cv::String saveVarImgPath;    //保存模板图路径
		int nCol;				//miniled列号
		int nColCount;			//miniled总列号
	};
	struct CreatVariationTextureOutput
	{
		ShapeModelInfo1 shapeInfo1;
		cv::Mat ouputimage;
	};
	struct AlgParamCreatVariationTextureInt                           //int型算法参数，独立出来方便使用默认构造函数
	{
		int dilate;                      //区域膨胀大小（查找模型时使用）
		int contrstH;					//创建和查找时的高对比度阈值
		int contrstL;				   //创建和查找时的高对比度阈值
		int contourNumber;			//创建模型时使用的轮廓个数
		int NumLevels;					//金字塔层数
		int modelflag;				//模型训练方式分为三种，1.中值模型，2.亮暗模型，3、均值模型
	};

	struct AlgParamCreatVariationTextureDouble                       //double型算法参数，独立出来方便使用默认构造函数
	{
		double  angleStart = -10;			//形状匹配模型：样本可能的旋转起始角度
		double  angleExtend = 20.0;			//形状匹配模型：样本可能的旋转角度范围                      //是否输出Mat图    0 不输出Mat图 1 输出Mat图
		double  minScore = 0.4;				//形状匹配模型：匹配置信度。
		double  scal = 1;					//缩放比例

	};

	struct AlgParamCreatVariationTexture
	{
		union
		{
			AlgParamCreatVariationTextureInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParamCreatVariationTextureDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgErrorMsgCreatVariationTexture
	{
		std::vector<std::string> msgs;
		AlgErrorMsgCreatVariationTexture()
		{
			msgs.push_back("0: 成功");
			msgs.push_back("1: 载入图像为空");
			msgs.push_back("2: 输入ROI区域小于两个");
			msgs.push_back("3: 未传入训练图路径");
			msgs.push_back("4: 参数设置不合理");
			msgs.push_back("5: 未知错误");
			msgs.push_back("6: 算法超时");
			msgs.push_back("7: 图像ID输入有误");
			msgs.push_back("8: 创建或查找ROI有交集，注意调整膨胀参数");
			msgs.push_back("9: 训练失败图像大于两个");
		}
	};

	struct AlgParamMsgCreatVariationTexture
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型 Int  Double
	//控件类型 D_SpinBox I_Spinbox Combox CheckBox
		//                 “算子类型 0-正常参数 1-界面勾选特征 $参数标识（只看不用）   $数据类型  $控件       $默认值  $参数名称               $参数意义  ￥下拉框的定义 $在界面显示顺序，不同算子的显示顺序是独立的 互补影响”
		std::vector<std::string> msgs;
		AlgParamMsgCreatVariationTexture()
		{
			msgs.push_back("dilate          $Int    $I_Spinbox       $150     $膨胀运算                            $查找模型匹配时ROI的膨胀像素默认值350一般比mark大一些。     $10");
			msgs.push_back("angleStart      $Double $D_Spinbox       $-10     $起始旋转角度(单位：角度值)          $默认值-20。创建模板的起始角度                       $20");
			msgs.push_back("angleEnd        $Double $D_Spinbox       $20      $终止旋转角度(单位：角度值)          $默认值20。创建模板的终止角度，在内部用是弧度 角度范围为终止-起始角度 $30");
			msgs.push_back("minScore        $Double $D_Spinbox       $0.4     $匹配置信度                          $匹配模型 $40");
			msgs.push_back("scal            $Do uble $D_Spinbox      $1       $缩放比例                            $图像缩放比例    $50");
			msgs.push_back("contrstH        $Int    $I_Spinbox       $15      $高对比度                            $默认值15，范围：[0,255]。创建模板时的高对比度大小。 $60");
			msgs.push_back("contrstL        $Int    $I_Spinbox       $15      $低对比度                            $默认值15，范围：[0,255]。创建模板时的低对比度大小。 $70");
			msgs.push_back("contourNumber        $Int    $I_Spinbox       $1      $匹配轮廓个数                            $默认值1，范围：[0,10]。创建模板时轮廓个数。 $80");
			msgs.push_back("NumLevels       $Int   $I_Spinbox        $4    $金字塔层数             $默认值4，范围：[0,10]。基于模板创建匹配时的金字塔层数。 $90");
			msgs.push_back("modelflag       $Int    $Combox	         $1		 $模型类型			     $模型类型：￥均值模型￥中值模型￥亮暗模型。	$100");
		}
		//----------------------------------------------基于模板图异常查找的缺陷检测算子(end)-------------------------------------//
	};

	//! 接口

	namespace alg {
		_ALGLIBRARY_API int creatVariationTexture(const CreatVariationTextureInput& input, CreatVariationTextureOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_CREATVARIATIONTEXTURE_H
