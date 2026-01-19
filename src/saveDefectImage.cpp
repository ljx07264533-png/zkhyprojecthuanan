#include <direct.h>
#include <io.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <fstream>
#include <atomic>

#include "alglibrary/zkhyProjectHuaNan/saveDefectImage.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"
#include "alglibrary/zkhyProjectHuaNan/utils.h"
#include "alglibrary/zkhyProjectHuaNan/cJSON.h"

#define LOG alglib::core::LoggerManager::getInstance()
#define Max_Char_Num 128
// 显示文字用的字体
const char* fontDisp = "Arial";
std::atomic<bool> stopFlag(false);

using namespace cv;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

	struct SavePath {
		string imageLabled;
		string imageUnlabled;
		string imageFull;

		// 初始化
		SavePath()
		{
			imageLabled = "";
			imageUnlabled = "";
			imageFull = "";
		}
	};

	struct MyFlawInfo {
		int index;
		string fileName[2];		// fileName[0] 带标注的图像 fileName[1] 不带标注的图像 名称为空时不存图
		string defectName;
		string fatherClass;
		string childClass;
		double length;
		double width;
		double area;
		double confidence;
		Point2f defectCenter;
		vector<vector<Point2f>> defectContours;
		vector<Point2f> outRect;
		SavePath fileSavePath;

		// 初始化
		MyFlawInfo()
		{
			index = 0;
			fileName[0] = "";
			fileName[1] = "";
			defectName = "";
			fatherClass = "";
			childClass = "";
			length = 0;
			width = 0;
			area = 0;
			confidence = 0;
			defectCenter = Point2f(0, 0);
			defectContours.clear();
			outRect.clear();
		}
	};

	// 存储缺陷小图时使用
	struct imgGroup {
		Mat colorImg;
		Mat grayImg;
	};

	struct imgWithName {
		string name;
		Mat	img;
		int imgType;	// 图像格式
	};

	class imgQueue
	{
	public:
		imgQueue() {}
		~imgQueue() {}

		int push(const imgWithName& img);
		int pop(imgWithName& img);
		bool isEmpty() { return imgQueue_.empty(); }
	private:
		std::queue<imgWithName> imgQueue_;
		std::mutex mtx_;
		std::condition_variable cv_;
	};


	// 需要新增 新产品线的 productInfo 和 environmentInfo 时需要在 "zkhy_publicClassHN.h" 中新增新的派生类

	int realSaveDefectImage(const saveDefectImageInput& input, saveDefectImageOutput& output);

	// 保存缺陷小图
	int writeDefectImages(const Mat& inputImage, const int cropedWidth, const int cropedHeight, const int lineWidth, const int charSize, const int boundingDist, const int flawNum, vector <MyFlawInfo>& flaws, Mat& imageLabled, const vector<int>& writeImageParams, const int isShowSize, const int isShowArea, const int isShowScore, const int maxSaveImgNum);

	// 重载
	int writeDefectImages(const imgGroup& inputImgs, const int cropedWidth, const int cropedHeight, const int lineWidth, const int charSize, const int boundingDist, const int flawNum, vector <MyFlawInfo>& flaws, Mat& imageLabled, const vector<int>& writeImageParams, const int isShowSize, const int isShowArea, const int isShowScore, const int maxSaveImgNum);

	// 计算缺陷的最小外接矩形
	int computeBoundingRect(const vector<vector<Point2f>>& contours, Rect& rect, vector<vector<Point>>& contoursForDraw);

	// 获取缺陷信息
	int getFlawInfos(const vector <FlawInfo>& flawsInfo, vector <MyFlawInfo>& myFlawsInfo);
	// 获取缺陷信息
	int getFlawInfos(const vector <FlawInfoStruct>& flawsInfoStruct, vector <MyFlawInfo>& myFlawsInfo);

	// 将输入图像转换为RGB图像
	int convertImage2RGB(const Mat& inputImage, Mat& outputImage);


	// 缺陷数据写入 json
	int writeFlawInfo2Json(const string& fileSaved, const string& fileNameBase, const string& imageSavePath,vector<MyFlawInfo> flawInfos, char*& jsonString);

	// 输入时间戳 解析出 年月日 时分秒  输入形如 2023-12-20-14-37-01-765
	int analysisTimeStamp(const string& timeStamp, string& year, string& month, string& day, string& hour, string& minute, string& second);

	// 解析构建文件名，文件存储路径
	int setFlawsInfo(char productInfo[], char environmentInfo[], const int saveModel, const int flawNum, const int isStoreSeparately, const string& description, const string& jsonSavePath, const string& imageSavePath, string& fileNameBase, string& jsonFilePath, vector <MyFlawInfo>& myFlawsInfo, zkhyPublicClass::productInfoBase& productInfobase);

	// 存储文件名，文件存储路径
	int setFlawsSavedName(const int saveModel, const int flawNum, const int isStoreSeparately, const string& fileName, const string& savePath, vector <MyFlawInfo>& myFlawsInfo, set<string>& savePathAll);

	// 将输入图像 转换成 灰度/彩色，并将两张图存放在 imgGroup 中
	int genImageGroup(const Mat& inputImg, imgGroup& outputImgs);

	int writeAllImg(imgQueue& queue);

	int defectImages2Queue(const imgGroup& inputImgs, imgQueue& queue, const int cropedWidth, const int cropedHeight, const int lineWidth, const int charSize, const int boundingDist, const int flawNum, vector <MyFlawInfo>& flaws, Mat& imageLabled, const vector<int>& writeImageParams, const int isShowSize, const int isShowArea, const int isShowScore, const int maxSaveImgNum);



	// 数值类型转换为字符串，可设置保留小数位数；最后一个参数是补零的位数
	template <typename T>
	std::string to_string_with_precision(const T a_value, const int n = 2, const int zfill = 0)
	{
		std::ostringstream out;
		if (zfill == 0)
			out << std::fixed << std::setprecision(n) << a_value;
		else
		{

			out << std::setfill('0') << std::setw(zfill) << std::fixed << std::setprecision(n) << a_value;

		}
		return out.str();
	}


	int realSaveDefectImage(const saveDefectImageInput& input, saveDefectImageOutput& output)
	{
		// 打印日志
		LOG.log("[saveDefectImage] 开始运算");

		Mat srcImg = input.srcImg;
		vector <FlawInfo> flawsInfo = input.flawInfos;
		vector <FlawInfoStruct> flawsInfoStruct = input.flawsInfoStructs;
		vector <Logic_Flaw_ResultAlg> flawsFilter = input.flawsFilter;
		string imageSavePath = input.imageSavePath;
		string jsonSavePath = input.jsonSavePath;
		string description = input.description;
		string descriptionCustom = input.descriptionCustom;

		char productInfo[2048], environmentInfo[2048];
		sprintf_s(productInfo, "%s", input.productInfo);
		sprintf_s(environmentInfo, "%s", input.environmentInfo);

		if (srcImg.empty())
			return 1;           // 输入图像为空

		int flawInfoSize = flawsInfo.size();
		int flawInfoStructSize = flawsInfoStruct.size();
		int flawsFilterSize = flawsFilter.size();

		// 解析参数
		AlgParamSaveDefectImage* alg = (AlgParamSaveDefectImage*)&input.algPara;

		int outImageWidth = alg->i_params.block.iImageWidth;
		int outImageHeight = alg->i_params.block.iImageHeight;
		int lineWidth = alg->i_params.block.iLineWidth;
		int charSize = alg->i_params.block.iCharSize;
		int isShowSize = alg->i_params.block.iIsShowSize;
		int isShowArea = alg->i_params.block.iIsShowArea;
		int isShowScore = alg->i_params.block.iIsShowScore;
		int boundingDist = alg->i_params.block.iBoundingDist;
		int isStoreSeparately = alg->i_params.block.iIsStoreSeparately;
		int iIsStoreImageFull = alg->i_params.block.iIsStoreImageFull;
		int iIsStoreJson = alg->i_params.block.iIsStoreJson;
		int iSaveModel = alg->i_params.block.iSaveModel;
		int iMaxImageSavedNum = alg->i_params.block.iMaxImageSavedNum;
		int iThreadNumWriteImage = alg->i_params.block.iThreadNumWriteImage;
		if (iThreadNumWriteImage < 1 || iThreadNumWriteImage > 6) iThreadNumWriteImage = 1;

		int iQualityImage = 90;

		if (_access(jsonSavePath.c_str(), 0) == -1 && alg->i_params.block.iSaveModel != 3)
			if (createDirectory(jsonSavePath) != 0)
				return 3;           // 存储路径异常

		bool isUse = alg->i_params.block.isUse;
		if (!isUse)
			return 0;

		double timeOutThreshold = alg->d_params.block.dTimeOutThreshold;
		double dMaxArea = alg->d_params.block.dTimeOutThreshold;

		int srcImageWidth = srcImg.cols;
		int srcImageHeight = srcImg.rows;

		// 判断参数是否合法
		if (outImageWidth > srcImageWidth || outImageWidth <= 0)
			return 4;           // 图像宽度参数输入有误
		if (outImageHeight > srcImageHeight || outImageHeight <= 0)
			return 5;           // 图像高度参数输入有误
		if (lineWidth > 999999 || lineWidth <= 0)
			return 6;           // 线宽参数输入有误
		if (charSize > 999999 || charSize <= 0)
			return 7;           // 字体大小参数输入有误
		if (timeOutThreshold > 999999 || timeOutThreshold <= 0)
			return 8;           // 算法超时参数输入有误
		if (iMaxImageSavedNum <= 0)
			iMaxImageSavedNum = 99999;

		int flawType = 0;
		if (flawInfoSize < flawInfoStructSize)
			flawType = 1;

		setlocale(LC_ALL, "Chinese_China");

		// 开始计时
		DWORD startTime = GetTickCount64();

		// 没有缺陷数据
		if (flawsFilterSize == 0)
			return 0;

		// 打印日志
		LOG.log("[saveDefectImage] 解析缺陷信息开始。");
		// 生成缺陷小图 Begin

		//	遍历缺陷数据
		vector <MyFlawInfo> myFlawsInfos(flawsFilterSize);
		for (int i = 0; i < flawsFilterSize; ++i)
		{
			string fatherDefectName = flawsFilter[i].szCheck_Flaw_Group;
			string childDefectName = flawsFilter[i].szDisplayName;
			string defectName = fatherDefectName + "_" + childDefectName;
			int defectIndex = flawsFilter[i].nResult_Index;

			// 记录缺陷名称和ID
			myFlawsInfos[i].index = defectIndex;
			myFlawsInfos[i].defectName = defectName;
			myFlawsInfos[i].fatherClass = fatherDefectName;
			myFlawsInfos[i].childClass = childDefectName;
		}

		if (flawType == 0)
			getFlawInfos(flawsInfo, myFlawsInfos);
		else
			getFlawInfos(flawsInfoStruct, myFlawsInfos);

		// 打印日志
		LOG.log("[saveDefectImage] 解析缺陷信息结束。");

		// 生成文件名称，存储路径，增加新产品类型时请在函数内部自行增加实现
		string fileNameBase = "";		// 用于存储标注的缺陷大图
		string jsonFilePath = "";

		// TODO 可能两个描述都有用，没用的话就移除一个
		if (descriptionCustom != "")
			description = descriptionCustom;

		// 打印日志
		LOG.log("[saveDefectImage] 准备待存储的数据开始。");

		// 解析数据，构建文件名，文件存储路径，并创建文件夹
		zkhyPublicClass::productInfoBase productInfoTmp;
		setFlawsInfo(productInfo, environmentInfo, iSaveModel, flawsFilterSize, isStoreSeparately, description, jsonSavePath, imageSavePath, fileNameBase, jsonFilePath, myFlawsInfos, productInfoTmp);


		// 打印日志
		LOG.log("[saveDefectImage] 准备图像开始。");

		// 存图参数
		vector<int> writeImageParams(2);
		writeImageParams[0] = cv::IMWRITE_JPEG_QUALITY;
		writeImageParams[1] = iQualityImage;

		// TODO 暂时只支持输入图像类型为 8UC1, 8UC3, 16UC1, 16UC3
		Mat imageUsed, imageLabled;

		imgGroup imgGroupTmp;
		genImageGroup(srcImg, imgGroupTmp);
		imageLabled = imgGroupTmp.colorImg.clone();

		// 打印日志
		LOG.log("[saveDefectImage] 保存图像开始。");

		writeDefectImages(imgGroupTmp, outImageWidth, outImageHeight, lineWidth, charSize, boundingDist, flawsFilterSize, myFlawsInfos, imageLabled, writeImageParams, isShowSize, isShowArea, isShowScore, iMaxImageSavedNum);

#if 0
		// 打印日志
		ALGLOG("[saveDefectImage] 保存图像开始", algLogger::logLevel::error, algLogger::logType::file);

		// 图像队列
		stopFlag = false;
		std::vector<std::thread> writeImgThreads;

		imgQueue imgPool;
		std::thread joinImgThread(defectImages2Queue, std::ref(imgGroupTmp), std::ref(imgPool), outImageWidth, outImageHeight, lineWidth, charSize, boundingDist, flawsFilterSize, std::ref(myFlawsInfos), std::ref(imageLabled), std::ref(writeImageParams), isShowSize, isShowArea, isShowScore, iMaxImageSavedNum);

		for (int i = 0; i < iThreadNumWriteImage; ++i) {
			writeImgThreads.push_back(std::thread(writeAllImg, std::ref(imgPool)));
		}

		joinImgThread.join();
		stopFlag = true;

		// 打印日志
		ALGLOG("[saveDefectImage] 图像队列写入完成", algLogger::logLevel::error, algLogger::logType::file);

		// 等待所有写入线程完成
		for (auto& t : writeImgThreads) {
			t.join();
		}

#endif

		// 打印日志
		LOG.log("[saveDefectImage] 存储缺陷小图完成。");


		// 存储JPG格式的有标注信息的缺陷大图
		if (iIsStoreImageFull == 1)
		{
			string imageFullPath = myFlawsInfos[0].fileSavePath.imageFull + "/" + fileNameBase + ".jpg";
			imwrite(imageFullPath, imageLabled);
			// 打印日志
			LOG.log("[saveDefectImage] 存储缺陷大图完成。");
		}

		// 生成缺陷小图 End
		output.strJson = nullptr;
		if (iIsStoreJson == 1)
		{
			// 缺陷数据写入 json
			string fileNameTmp = productInfoTmp.productID;

			if (productInfoTmp.QRCode != "")
			{
				imageSavePath += "/" + productInfoTmp.QRCode;
			}
			else
			{
				imageSavePath += "/" + productInfoTmp.productID;
			}

			writeFlawInfo2Json(jsonFilePath, fileNameBase + ".jpg", imageSavePath,myFlawsInfos, output.strJson);
			// 打印日志
			LOG.log("[saveDefectImage] 写入JSON数据完成。");
		}

		// 计算算法耗时
		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 9;                   // 算法超时

		return 0;
	}

	int writeDefectImages(const Mat& inputImage, const int cropedWidth, const int cropedHeight, const int lineWidth, const int charSize, const int boundingDist, const int flawNum, vector <MyFlawInfo>& flaws, Mat& imageLabled, const vector<int>& writeImageParams, const int isShowSize, const int isShowArea, const int isShowScore, const int maxSaveImgNum)
	{
		int realFlawNum = maxSaveImgNum;

		if (flawNum < maxSaveImgNum)
			realFlawNum = flawNum;
		Mat srcImage;
		cv::cvtColor(inputImage, srcImage, cv::COLOR_BGR2GRAY);
		// 缺陷数量少于设置的最大值时，保存所有的缺陷小图；缺陷数量大于设置的最大值时，保存部分缺陷小图
		for (int i = 0; i < realFlawNum; ++i)
		{
			// 00001_S1V1__时间戳_二维码_父分类_子分类(长#宽#面积)_index_

			// 获取缺陷信息字符串,用于图像上显示
			string defectInfoDisp = "类型：" + flaws[i].defectName + "\n";
			if (isShowSize)
			{
				defectInfoDisp += "长：" + to_string_with_precision(flaws[i].length) + "\n" + "宽：" + to_string_with_precision(flaws[i].width) + "\n";
			}
			if (isShowArea)
			{
				defectInfoDisp += "面积：" + to_string_with_precision(flaws[i].area) + "\n";
			}
			if (isShowScore)
			{
				defectInfoDisp += "置信度：" + to_string_with_precision(flaws[i].confidence) + "\n";
			}

			// 生成缺陷小图
			// 获取输入图像的宽高
			int imageWidth = inputImage.cols;
			int imageHeight = inputImage.rows;

			Mat imageUsed = inputImage;

			// 计算缺陷的宽高
			//Point defectLeftTop, defectRightBottom;
			Rect defectRect;
			vector<vector<Point>> contoursForDraw;
			computeBoundingRect(flaws[i].defectContours, defectRect, contoursForDraw);
			int defectWidth = defectRect.width;
			int defectHeight = defectRect.height;


			// 计算裁剪后的图像左上角和右下角坐标
			Point cropedLeftTop, cropedRightBottom;
			cropedLeftTop.x = flaws[i].defectCenter.x - cropedWidth / 2;
			cropedLeftTop.y = flaws[i].defectCenter.y - cropedHeight / 2;
			cropedRightBottom.x = flaws[i].defectCenter.x + cropedWidth / 2;
			cropedRightBottom.y = flaws[i].defectCenter.y + cropedHeight / 2;

			// 缺陷大于裁剪后的图像
			if (defectWidth > cropedWidth)
			{
				cropedLeftTop.x = defectRect.tl().x - boundingDist;
				cropedRightBottom.x = defectRect.br().x + boundingDist;
			}
			if (defectHeight > cropedHeight)
			{
				cropedLeftTop.y = defectRect.tl().y - boundingDist;
				cropedRightBottom.y = defectRect.br().y + boundingDist;
			}

			Rect cropedRectTmp = Rect(cropedLeftTop.x, cropedLeftTop.y, cropedRightBottom.x - cropedLeftTop.x, cropedRightBottom.y - cropedLeftTop.y);

			Rect imageRect(0, 0, imageWidth, imageHeight);

			// 两个矩形的交集
			Rect cropedRect = cropedRectTmp & imageRect;
			// 裁剪图像
			Mat cropedImage = srcImage(cropedRect);

			// 存储BMP格式的无标注信息缺陷小图
			string imageUnlabledPath = flaws[i].fileSavePath.imageUnlabled + "/" + flaws[i].fileName[1];

			imwrite(imageUnlabledPath, cropedImage);

			// 绘制缺陷轮廓，显示缺陷信息，保存为jpg
			Mat imageShow = imageUsed;
			int contoursNum = contoursForDraw.size();
			drawContours(imageShow, contoursForDraw, -1, Scalar(255, 0, 0), lineWidth);
			//drawContours(imageLabled, contoursForDraw, -1, Scalar(255, 0, 0), lineWidth);

			// 最小外接矩形
			Point2f rectPoints[4];
			RotatedRect rect = minAreaRect(contoursForDraw[0]);
			rect.points(rectPoints);
			for (int i = 0; i < 4; ++i)
			{
				line(imageShow, rectPoints[i], rectPoints[(i + 1) % 4], Scalar(0, 0, 255), lineWidth);
				//line(imageLabled, rectPoints[i], rectPoints[(i + 1) % 4], Scalar(0, 0, 255), lineWidth);
			}

			// 缺陷信息
			Size textSize = getTextSize(defectInfoDisp, FONT_HERSHEY_SIMPLEX, 0.1, charSize, 0);
			Point defectInfoLeftBottom = Point(0, 0);
			// 放置在缺陷左下角
			defectInfoLeftBottom.x = defectRect.tl().x;
			defectInfoLeftBottom.y = defectRect.br().y + textSize.height;

			//putText(imageLabled, defectInfo, defectInfoLeftBottom, FONT_HERSHEY_SIMPLEX, 0.1, Scalar(0, 0, 255), charSize);

			zkhyPublicFuncHN::putTextZH(imageLabled, defectInfoDisp.c_str(), defectInfoLeftBottom, Scalar(0, 0, 255), charSize, fontDisp, false, false);

			imageShow = imageShow(cropedRect);
			string imageLabledPath = flaws[i].fileSavePath.imageLabled + "/" + flaws[i].fileName[0];
			//imwrite(imageLabledPath, imageShow, writeImageParams);
			if (flaws[i].fileSavePath.imageLabled != "" || flaws[i].fileName[0] != "")
				imwrite(imageLabledPath, imageShow);
		}

#if 0
		// 缺陷数量超过设置的最大数量时只记录缺陷名字
		for (int i = realFlawNum; i < flawNum; ++i)
		{
			// 00001_S1V1__时间戳_二维码_父分类_子分类(长#宽#面积)_index_

			// 获取缺陷信息字符串,用于图像上显示
			string defectInfoDisp = "类型：" + flaws[i].defectName + "\n";
			// 获取缺陷信息字符串,用于文件名中显示
			string defectInfoFile = "Length" + to_string(flaws[i].length) + "#Width" + to_string(flaws[i].width) + "#Area" + to_string(flaws[i].area);

			if (isShowSize)
			{
				defectInfoDisp += "长：" + to_string(flaws[i].length) + "\n" + "宽：" + to_string(flaws[i].width) + "\n";
			}
			if (isShowArea)
			{
				defectInfoDisp += "面积：" + to_string(flaws[i].area) + "\n";
			}
			if (isShowScore)
			{
				defectInfoDisp += "置信度：" + to_string(flaws[i].confidence) + "\n";
				defectInfoFile += "#Score" + to_string(flaws[i].confidence);
			}

			string fileNameTmp = fileName + "_" + flaws[i].defectName + "(" + defectInfoFile + ")" + "_" + to_string(i);

			flaws[i].fileName[0] = fileNameTmp + "_color.jpg";
			flaws[i].fileName[1] = fileNameTmp + ".bmp";
		}
#endif

		return 0;
	}

	int computeBoundingRect(const vector<vector<Point2f>>& contours, Rect& rect, vector<vector<Point>>& contoursForDraw)
	{
		// 获取轮廓的个数
		int contoursNum = contours.size();
		if (contoursNum == 0)
			return 1;           // 轮廓个数为0

		vector<Point> contoursPoints;
		for (int i = 0; i < contoursNum; ++i)
		{
			vector<Point2f> contourTmp = contours[i];
			int pointsNum = contourTmp.size() - 1; // 最后一个点与第一个点重复
			vector<Point> contoursPointsTemp(pointsNum);
			for (int j = 0; j < pointsNum; ++j)
			{
				contoursPointsTemp[j] = contourTmp[j];
			}
			contoursPoints.insert(contoursPoints.end(), contoursPointsTemp.begin(), contoursPointsTemp.end());
		}
		contoursPoints.push_back(contoursPoints[0]);

		// 获取轮廓的正外接矩形
		rect = boundingRect(contoursPoints);

		contoursForDraw.push_back(contoursPoints);

		return 0;
	}

	int getFlawInfos(const vector <FlawInfo>& flawsInfo, vector <MyFlawInfo>& myFlawsInfo)
	{
		int flawNum = myFlawsInfo.size();
		for (int i = 0; i < flawNum; ++i)
		{
			int index = myFlawsInfo[i].index;
			myFlawsInfo[i].length = flawsInfo[index].rect2Len1;
			myFlawsInfo[i].width = flawsInfo[index].rect2Len2;
			myFlawsInfo[i].area = flawsInfo[index].area;
			myFlawsInfo[i].confidence = flawsInfo[index].score;

			myFlawsInfo[i].defectCenter = flawsInfo[index].pos;
			if (flawsInfo[index].contours.size() == 0)
				myFlawsInfo[i].defectContours.push_back(flawsInfo[index].pts);
			else
				myFlawsInfo[i].defectContours = flawsInfo[index].contours;
			myFlawsInfo[i].outRect = flawsInfo[index].pts;

		}
		return 0;
	}

	int getFlawInfos(const vector <FlawInfoStruct>& flawsInfoStruct, vector <MyFlawInfo>& myFlawsInfo)
	{
		int flawNum = myFlawsInfo.size();
		for (int i = 0; i < flawNum; ++i)
		{
			int index = myFlawsInfo[i].index;
			myFlawsInfo[i].length = flawsInfoStruct[index].FlawBasicInfo.flawWidth;
			myFlawsInfo[i].width = flawsInfoStruct[index].FlawBasicInfo.flawHeight;
			myFlawsInfo[i].area = flawsInfoStruct[index].FlawBasicInfo.flawArea;
			myFlawsInfo[i].confidence = flawsInfoStruct[index].FlawManualFeature.score;

			myFlawsInfo[i].defectCenter = flawsInfoStruct[index].FlawBasicInfo.centerPt;
			if (flawsInfoStruct[index].FlawBasicInfo.flawContours.size() == 0)
				myFlawsInfo[i].defectContours.push_back(flawsInfoStruct[index].FlawBasicInfo.pts);
			else
				myFlawsInfo[i].defectContours = flawsInfoStruct[index].FlawBasicInfo.flawContours;

			myFlawsInfo[i].outRect = flawsInfoStruct[index].FlawBasicInfo.pts;
		}
		return 0;
	}

	int convertImage2RGB(const Mat& inputImage, Mat& outputImage)
	{
		int inputImageType = inputImage.type();

		switch (inputImageType)
		{
		case CV_8UC1:
			cvtColor(inputImage, outputImage, COLOR_GRAY2BGR);
			break;
		case CV_8UC3:
			outputImage = inputImage.clone();
			break;
		case CV_16UC1:
			inputImage.convertTo(outputImage, CV_8UC1, 255.0 / 0xFFFF);
			cvtColor(outputImage, outputImage, COLOR_GRAY2BGR);
			break;
		case CV_16UC3:
			inputImage.convertTo(outputImage, CV_8UC3, 255.0 / 0xFFFF);
			break;
		default:
			return 1;           // 输入图像数据类型不为8位无符号整型
		}
		return 0;
	}


	int writeFlawInfo2Json(const string& fileSaved, const string& fileNameBase, const string& imageSavePath, vector<MyFlawInfo> flawInfos, char*& jsonString )
	{

		cJSON* root = cJSON_CreateObject();

		// 尝试打开文件以判断文件是否存在
		ifstream jsonFile(fileSaved);
		bool fileExist = jsonFile.good();
		if (fileExist)
		{
			cJSON_Delete(root);
			string jsonText;
			while (!jsonFile.eof())
			{
				string tmp;
				jsonFile >> tmp;
				jsonText += tmp;
			}

			root = cJSON_Parse(jsonText.c_str());
		}
		// 关闭文件
		jsonFile.close();


		cJSON* flawList = cJSON_CreateArray();

		int flawNum = flawInfos.size();

		for (auto flaw : flawInfos)
		{
			cJSON* flawTmp = cJSON_CreateObject();

			// fileName
			cJSON* imageTmp = cJSON_CreateObject();
			cJSON_AddStringToObject(imageTmp, "labeled", flaw.fileName[0].c_str());
			cJSON_AddStringToObject(imageTmp, "unlabeled", flaw.fileName[1].c_str());
			cJSON_AddItemToObject(flawTmp, "image", imageTmp);

			// center point
			cJSON* centerTmp = cJSON_CreateObject();
			cJSON_AddNumberToObject(centerTmp, "x", flaw.defectCenter.x);
			cJSON_AddNumberToObject(centerTmp, "y", flaw.defectCenter.y);
			cJSON_AddItemToObject(flawTmp, "center", centerTmp);

			// outRect
			cJSON* outRectTmpX = cJSON_CreateArray();
			cJSON* outRectTmpY = cJSON_CreateArray();
			for (auto point : flaw.outRect)
			{
				cJSON_AddItemToArray(outRectTmpX, cJSON_CreateNumber(point.x));
				cJSON_AddItemToArray(outRectTmpY, cJSON_CreateNumber(point.y));
			}
			cJSON_AddItemToObject(flawTmp, "outRectX", outRectTmpX);
			cJSON_AddItemToObject(flawTmp, "outRectY", outRectTmpY);

			// flaw class
			cJSON_AddStringToObject(flawTmp, "fatherClass", flaw.fatherClass.c_str());
			cJSON_AddStringToObject(flawTmp, "childClass", flaw.childClass.c_str());

			// length width area
			cJSON* flawInfoTmp = cJSON_CreateObject();
			cJSON_AddNumberToObject(flawInfoTmp, "length", flaw.length);
			cJSON_AddNumberToObject(flawInfoTmp, "width", flaw.width);
			cJSON_AddNumberToObject(flawInfoTmp, "area", flaw.area);
			cJSON_AddItemToObject(flawTmp, "flawInfo", flawInfoTmp);

			// add to flawList
			cJSON_AddItemToArray(flawList, flawTmp);
		}

		if (root)
		{
			cJSON_AddStringToObject(root, "DefectImagePath", imageSavePath.c_str()); 
		}
		if (!fileExist)
		{
			// add to root
			// 标注大图的名字
			cJSON_AddStringToObject(root, "imageLabeledAll", fileNameBase.c_str());
			cJSON_AddItemToObject(root, "flaws", flawList);
		}
		else
		{
			// 获取 root 中的 flaws
			cJSON* flawsExisted = cJSON_GetObjectItem(root, "flaws");
			// 将新的缺陷信息添加到已有的缺陷信息中
			cJSON_AddItemReferenceToArray(flawsExisted, flawList);
			cJSON_ReplaceItemInObject(root, "flaws", flawsExisted);
		}

		string jsonText = cJSON_Print(root);

		ofstream ofs;
		ofs.open(fileSaved, ios::out | ios::trunc | ios::binary);
		if (ofs.is_open())
		{
			ofs << jsonText << endl;
			ofs.close();
		}
		else
		{
			cJSON_Delete(root);
			return 1;
		}

		jsonString = cJSON_Print(root);
		cJSON_Delete(root);

		return 0;
	}

	// 批量写入图片时使用，将图像先写入缓存，最后一次性写入磁盘
	int writeImages()
	{
		return 0;
	}

	// TODO 批量在图像上绘制文本信息，防止写入的信息显示时出现重叠的现象
	// 图像上绘制文本信息
	int putText2Image(Mat& image, const string& text, const Point& centerPoint, const Scalar& color, const int fontSize, const char* font, const bool italic, const bool underline)
	{

		return 0;
	}

	int analysisTimeStamp(const string& timeStamp, string& year, string& month, string& day, string& hour, string& minute, string& second)
	{
		// 输入形如 2023-12-20-14-37-01-765
		int charNum = timeStamp.size();

		if (charNum != 23 && charNum != 19)
			return 1;           // 输入时间戳格式有误

		year = timeStamp.substr(0, 4);
		month = timeStamp.substr(5, 2);
		day = timeStamp.substr(8, 2);
		hour = timeStamp.substr(11, 2);
		minute = timeStamp.substr(14, 2);
		second = timeStamp.substr(17, 2);

		return 0;
	}

	int setFlawsInfo(char productInfo[], char environmentInfo[], const int saveModel, const int flawNum, const int isStoreSeparately, const string& description,
		const string& jsonSavePath, const string& imageSavePath, string& fileNameBase, string& jsonFilePath, vector <MyFlawInfo>& myFlawsInfo, zkhyPublicClass::productInfoBase& productInfobase)
	{
		set<string> savePathAll; //记录所有要创建的文件夹路径
		savePathAll.clear();

		// 根据模式生成文件名和存储路径
		switch (saveModel)
		{
		case 1:		//MINILED
		{
			// 00001_S1V1__时间戳_二维码_父分类_子分类(长#宽#面积)_index_
			// col_description_timeStamp_QRCode_fatherClass_childClass(length#width#area)_index
			zkhyPublicClass::productInfoMINILED productInfoTmp;
			productInfoTmp.getProductInfo(productInfo);

			string fileNameTmp = productInfoTmp.col;
			string savePathTmp = imageSavePath;

			zkhyPublicClass::enviromentInfoBase enviromentInfoTmp;
			enviromentInfoTmp.getEnvironmentInfo(environmentInfo);

			// 获取时间
			string year, month, day, hour, minute, second;
			if (analysisTimeStamp(productInfoTmp.dataTime, year, month, day, hour, minute, second))
				return 13;

			savePathTmp += "/" + year + "_" + month + "_" + day;

			if (description != "")
				fileNameTmp += "_" + description;
			fileNameTmp += "_" + productInfoTmp.dataTime;

			if (productInfoTmp.QRCode != "")
			{
				fileNameTmp += "_" + productInfoTmp.QRCode;
				savePathTmp += "/" + hour + "_" + minute + "_" + second + "#" + productInfoTmp.QRCode;
			}
			else
			{
				savePathTmp += "/" + hour + "_" + minute + "_" + second + "#" + productInfoTmp.productID;
			}
			// 设置json存储名称
			jsonFilePath = savePathTmp + "/" + productInfoTmp.col + "_" + description + "_" + enviromentInfoTmp.timeStamp + ".json";

			// 传出文件名前缀 用于存储包含全部标注的大图
			fileNameBase = fileNameTmp;

			// 将path和文件名写入 myFlawsInfo
			setFlawsSavedName(saveModel, flawNum, isStoreSeparately, fileNameTmp, savePathTmp, myFlawsInfo, savePathAll);

			break;
		}
		case 2:		// 卷带机
		{
			zkhyPublicClass::productInfoReel productInfoTmp;
			productInfoTmp.getProductInfo(productInfo);

			string fileNameTmp = productInfoTmp.RollID + "_" + productInfoTmp.productID;
			string savePathTmp = imageSavePath;

			savePathTmp += "/" + productInfoTmp.lotID;

			if (description != "")
				fileNameTmp += "_" + description;

			if (productInfoTmp.QRCode != "")
			{
				fileNameTmp += "_" + productInfoTmp.QRCode;
			}

			// 设置json存储名称
			jsonFilePath = savePathTmp + "/" + productInfoTmp.lotID + "_" + productInfoTmp.RollID + "_" + description + "_" + ".json";

			// 传出文件名前缀 用于存储包含全部标注的大图
			fileNameBase = fileNameTmp;

			// 将path和文件名写入 myFlawsInfo
			setFlawsSavedName(saveModel, flawNum, isStoreSeparately, fileNameTmp, savePathTmp, myFlawsInfo, savePathAll);

			break;
		}
		case 3:		// 晶圆
		{	// 00001_S1V1__时间戳_二维码_父分类_子分类(长#宽#面积)_index_

			zkhyPublicClass::productInfoWAFER productInfoTmp;
			productInfoTmp.getProductInfo(productInfo);

			string savePathTmp = imageSavePath;//外面选的大路径；路径下面还需要创建：年月、日、waferID、通道、缺陷类型 的文件夹


			// 获取时间
			time_t nowtime;
			time(&nowtime); //获取1970年1月1日0点0分0秒到现在经过的秒数
			tm p;
			localtime_s(&p, &nowtime); //将秒数转换为本地时间,年从1900算起,需要+1900,月为0-11,所以要+1

			// 图片名 时间戳；前6个是行列号；
			string fileNameTmp = to_string_with_precision(productInfoTmp.row, 0, 3) + to_string_with_precision(productInfoTmp.col, 0, 3) +
				to_string_with_precision(p.tm_year + 1900, 0, 0) + "-" + to_string_with_precision(p.tm_mon + 1, 0, 2) + "-" +
				to_string_with_precision(p.tm_mday, 0, 2) + "-" + to_string_with_precision(p.tm_hour, 0, 2) + "-" +
				to_string_with_precision(p.tm_min, 0, 2) + "-" + to_string_with_precision(p.tm_sec, 0, 2);


			savePathTmp = savePathTmp + "\\DefectImg\\" + to_string_with_precision(p.tm_year + 1900, 0, 0) + "-" + to_string_with_precision(p.tm_mon + 1, 0, 2) + "\\" +
				to_string_with_precision(p.tm_mday, 0, 2) + "\\" + (to_string)(productInfoTmp.ProductId) +
				"\\C" + to_string_with_precision(productInfoTmp.channel, 0, 2);


			// 将path和文件名写入 myFlawsInfo
			setFlawsSavedName(saveModel, flawNum, isStoreSeparately, fileNameTmp, savePathTmp, myFlawsInfo, savePathAll);

			break;

		}
		// 添加新产品的文件处理方式
		default:	//默认
		{
			// 00001_S1V1__时间戳_二维码_父分类_子分类(长#宽#面积)_index_
			// id_description_timeStamp_QRCode_fatherClass_childClass(length#width#area)_index
			// 解析 productInfo
			zkhyPublicClass::productInfoBase productInfoTmp;
			productInfoTmp.getProductInfo(productInfo);

			string fileNameTmp = productInfoTmp.productID;
			string savePathTmp = imageSavePath;

			// 解析 enviromentInfo
			zkhyPublicClass::enviromentInfoBase enviromentInfoTmp;
			enviromentInfoTmp.getEnvironmentInfo(environmentInfo);

			if (description != "")
				fileNameTmp += "_" + description;

			fileNameTmp += "_" + enviromentInfoTmp.timeStamp;

			if (productInfoTmp.QRCode != "")
			{
				fileNameTmp += "_" + productInfoTmp.QRCode;
				savePathTmp += "/" + productInfoTmp.QRCode;
			}
			else
			{
				savePathTmp += "/" + productInfoTmp.productID;

				
			}
			productInfobase = productInfoTmp;

			// 设置json存储名称
			jsonFilePath = jsonSavePath + "/" + fileNameTmp + ".json";

			// 传出文件名前缀 用于存储包含全部标注的大图
			fileNameBase = fileNameTmp;

			// 将path和文件名写入 myFlawsInfo, 需要修改时请在函数里增加新的实现
			setFlawsSavedName(saveModel, flawNum, isStoreSeparately, fileNameTmp, savePathTmp, myFlawsInfo, savePathAll);
			break;
		}
		}

		// 判断文件夹是否存在，不存在则创建
		for (auto path : savePathAll)
		{
			if (!createDirectory(path))
				return 3;           // 存储路径异常
		}
		return 0;
	}

	int setFlawsSavedName(const int saveModel, const int flawNum, const int isStoreSeparately, const string& fileName, const string& savePath,
		vector <MyFlawInfo>& myFlawsInfo, set<string>& savePathAll)
	{
		switch (saveModel)
		{
		case 3:   // 晶圆
		{
			string strChannel, strRow, strCol, newFileName;
			{
				size_t strLen = savePath.length();
				strChannel = savePath.substr(strLen - 3, 3);

				strLen = fileName.length();
				strRow = fileName.substr(0, 3);
				strCol = fileName.substr(3, 3);
				newFileName = fileName.substr(6, strLen - 6);
			}

			// 将path和文件名写入 myFlawsInfo
			for (int i = 0; i < flawNum; ++i)
			{
				string defectInfoFile = "Length" + to_string_with_precision(myFlawsInfo[i].length) + "#Width" + to_string_with_precision(myFlawsInfo[i].width) + "#Area" + to_string_with_precision(myFlawsInfo[i].area);

				string fileNameTmp = strChannel + "_" + "Row" + strRow + "Col" + strCol + "_" +
					to_string_with_precision((int)myFlawsInfo[i].defectCenter.x, 0) + "_" +
					to_string_with_precision((int)myFlawsInfo[i].defectCenter.y, 0) + "_" +
					newFileName + "_" + to_string(i) + "_" + myFlawsInfo[i].fatherClass;

				myFlawsInfo[i].fileName[0] = fileNameTmp + ".jpg";
				myFlawsInfo[i].fileName[1] = fileNameTmp + ".bmp";
				if (isStoreSeparately == 1)	// 区分缺陷文件夹存储
				{

					myFlawsInfo[i].fileSavePath.imageLabled = savePath + "\\" + "/Labled/" + myFlawsInfo[i].fatherClass;
					myFlawsInfo[i].fileSavePath.imageUnlabled = savePath + "\\" + "/Unlabled/" + myFlawsInfo[i].fatherClass;

					savePathAll.insert(myFlawsInfo[i].fileSavePath.imageLabled);
					savePathAll.insert(myFlawsInfo[i].fileSavePath.imageUnlabled);
				}
				else
				{
					myFlawsInfo[i].fileSavePath.imageLabled = savePath + "\\" + myFlawsInfo[i].fatherClass;
					myFlawsInfo[i].fileSavePath.imageUnlabled = savePath + "\\" + myFlawsInfo[i].fatherClass;

					savePathAll.insert(myFlawsInfo[i].fileSavePath.imageLabled);
					savePathAll.insert(myFlawsInfo[i].fileSavePath.imageUnlabled);
				}
			}
			break;
		}
		default:
		{
			// 将path和文件名写入 myFlawsInfo
			for (int i = 0; i < flawNum; ++i)
			{
				string defectInfoFile = "Length" + to_string_with_precision(myFlawsInfo[i].length) + "#Width" + to_string_with_precision(myFlawsInfo[i].width) + "#Area" + to_string_with_precision(myFlawsInfo[i].area);

				string fileNameTmp = fileName + "_" + myFlawsInfo[i].defectName + "(" + defectInfoFile + ")" + "_" + to_string(i);

				myFlawsInfo[i].fileName[0] = fileNameTmp + "_color.jpg";
				myFlawsInfo[i].fileName[1] = fileNameTmp + ".bmp";
				if (isStoreSeparately == 1)
				{
					myFlawsInfo[i].fileSavePath.imageLabled = savePath + "/Labled";
					myFlawsInfo[i].fileSavePath.imageUnlabled = savePath + "/Unlabled";
					myFlawsInfo[i].fileSavePath.imageFull = savePath + "/Full";

					savePathAll.insert(myFlawsInfo[i].fileSavePath.imageLabled);
					savePathAll.insert(myFlawsInfo[i].fileSavePath.imageUnlabled);
					savePathAll.insert(myFlawsInfo[i].fileSavePath.imageFull);
				}
				else
				{
					myFlawsInfo[i].fileSavePath.imageLabled = savePath;
					myFlawsInfo[i].fileSavePath.imageUnlabled = savePath;
					myFlawsInfo[i].fileSavePath.imageFull = savePath;

					savePathAll.insert(myFlawsInfo[i].fileSavePath.imageLabled);
				}
			}
			break;

		}

		}
		return 0;
	}

	// 将输入图像 转换成 灰度/彩色，并将两张图存放在 imgGroup 中
	int genImageGroup(const Mat& inputImg, imgGroup& outputImgs)
	{
		int inputImageType = inputImg.type();

		Mat imgCvted, imgTmp;
		switch (inputImageType)
		{
		case CV_8UC1:
			cvtColor(inputImg, imgCvted, COLOR_GRAY2BGR);
			outputImgs.colorImg = imgCvted.clone();
			outputImgs.grayImg = inputImg.clone();
			break;
		case CV_8UC3:
			//cvtColor(inputImg, imgCvted, COLOR_BGR2GRAY);
			outputImgs.colorImg = inputImg.clone();
			outputImgs.grayImg = inputImg.clone();
			break;
		case CV_16UC1:
			inputImg.convertTo(imgTmp, CV_8UC1, 255.0 / 0xFFFF);
			cvtColor(imgTmp, imgCvted, COLOR_GRAY2BGR);
			outputImgs.colorImg = imgCvted.clone();
			outputImgs.grayImg = imgTmp.clone();
			break;
		case CV_16UC3:
			inputImg.convertTo(imgTmp, CV_8UC3, 255.0 / 0xFFFF);
			cvtColor(imgTmp, imgCvted, COLOR_BGR2GRAY);
			outputImgs.colorImg = imgTmp.clone();
			outputImgs.grayImg = imgCvted.clone();
			break;
		default:
			return 1;           // 输入图像数据类型不为8位无符号整型
		}
		return 0;
	}

	int writeDefectImages(const imgGroup& inputImgs, const int cropedWidth, const int cropedHeight, const int lineWidth, const int charSize, const int boundingDist, const int flawNum, vector <MyFlawInfo>& flaws, Mat& imageLabled, const vector<int>& writeImageParams, const int isShowSize, const int isShowArea, const int isShowScore, const int maxSaveImgNum)
	{
		int realFlawNum = maxSaveImgNum;

		if (flawNum < maxSaveImgNum)
			realFlawNum = flawNum;

		Mat colorImg = inputImgs.colorImg;
		Mat grayImg = inputImgs.grayImg;
		// 缺陷数量少于设置的最大值时，保存所有的缺陷小图；缺陷数量大于设置的最大值时，保存部分缺陷小图
		for (int i = 0; i < realFlawNum; ++i)
		{
			// 00001_S1V1__时间戳_二维码_父分类_子分类(长#宽#面积)_index_

			// 获取缺陷信息字符串,用于图像上显示
			string defectInfoDisp = "类型：" + flaws[i].defectName + "\n";
			if (isShowSize)
			{
				defectInfoDisp += "长：" + to_string_with_precision(flaws[i].length) + "\n" + "宽：" + to_string_with_precision(flaws[i].width) + "\n";
			}
			if (isShowArea)
			{
				defectInfoDisp += "面积：" + to_string_with_precision(flaws[i].area) + "\n";
			}
			if (isShowScore)
			{
				defectInfoDisp += "置信度：" + to_string_with_precision(flaws[i].confidence) + "\n";
			}

			// 生成缺陷小图
			// 获取输入图像的宽高
			int imageWidth = grayImg.cols;
			int imageHeight = grayImg.rows;

			// 计算缺陷的宽高
			//Point defectLeftTop, defectRightBottom;
			Rect defectRect;
			vector<vector<Point>> contoursForDraw;
			computeBoundingRect(flaws[i].defectContours, defectRect, contoursForDraw);
			int defectWidth = defectRect.width;
			int defectHeight = defectRect.height;


			// 计算裁剪后的图像左上角和右下角坐标
			Point cropedLeftTop, cropedRightBottom;
			cropedLeftTop.x = flaws[i].defectCenter.x - cropedWidth / 2;
			cropedLeftTop.y = flaws[i].defectCenter.y - cropedHeight / 2;
			cropedRightBottom.x = flaws[i].defectCenter.x + cropedWidth / 2;
			cropedRightBottom.y = flaws[i].defectCenter.y + cropedHeight / 2;

			// 缺陷大于裁剪后的图像
			if (defectWidth > cropedWidth)
			{
				cropedLeftTop.x = defectRect.tl().x - boundingDist;
				cropedRightBottom.x = defectRect.br().x + boundingDist;
			}
			if (defectHeight > cropedHeight)
			{
				cropedLeftTop.y = defectRect.tl().y - boundingDist;
				cropedRightBottom.y = defectRect.br().y + boundingDist;
			}

			Rect cropedRectTmp = Rect(cropedLeftTop.x, cropedLeftTop.y, cropedRightBottom.x - cropedLeftTop.x, cropedRightBottom.y - cropedLeftTop.y);

			Rect imageRect(0, 0, imageWidth, imageHeight);

			// 两个矩形的交集
			Rect cropedRect = cropedRectTmp & imageRect;
			// 裁剪图像
			Mat cropedImage = grayImg(cropedRect);

			// 存储BMP格式的无标注信息缺陷小图
			string imageUnlabledPath = flaws[i].fileSavePath.imageUnlabled + "/" + flaws[i].fileName[1];

			imwrite(imageUnlabledPath, cropedImage);

			// 绘制缺陷轮廓，显示缺陷信息，保存为jpg
			Mat imageShow = colorImg;
			int contoursNum = contoursForDraw.size();
			drawContours(imageShow, contoursForDraw, -1, Scalar(255, 0, 0), lineWidth);
			drawContours(imageLabled, contoursForDraw, -1, Scalar(255, 0, 0), lineWidth);

			// 最小外接矩形
			Point2f rectPoints[4];
			RotatedRect rect = minAreaRect(contoursForDraw[0]);
			rect.points(rectPoints);
			for (int i = 0; i < 4; ++i)
			{
				line(imageShow, rectPoints[i], rectPoints[(i + 1) % 4], Scalar(0, 0, 255), lineWidth);
				line(imageLabled, rectPoints[i], rectPoints[(i + 1) % 4], Scalar(0, 0, 255), lineWidth);
			}

			// 缺陷信息
			Size textSize = getTextSize(defectInfoDisp, FONT_HERSHEY_SIMPLEX, 0.1, charSize, 0);
			Point defectInfoLeftBottom = Point(0, 0);
			// 放置在缺陷左下角
			defectInfoLeftBottom.x = defectRect.tl().x;
			defectInfoLeftBottom.y = defectRect.br().y + textSize.height;

			//putText(imageLabled, defectInfo, defectInfoLeftBottom, FONT_HERSHEY_SIMPLEX, 0.1, Scalar(0, 0, 255), charSize);

			zkhyPublicFuncHN::putTextZH(imageShow, defectInfoDisp.c_str(), defectInfoLeftBottom, Scalar(0, 0, 255), charSize, fontDisp, false, false);
			zkhyPublicFuncHN::putTextZH(imageLabled, defectInfoDisp.c_str(), defectInfoLeftBottom, Scalar(0, 0, 255), charSize, fontDisp, false, false);

			imageShow = imageShow(cropedRect);
			string imageLabledPath = flaws[i].fileSavePath.imageLabled + "/" + flaws[i].fileName[0];
			//imwrite(imageLabledPath, imageShow, writeImageParams);
			if (flaws[i].fileSavePath.imageLabled != "" || flaws[i].fileName[0] != "")
				imwrite(imageLabledPath, imageShow);
		}

		return 0;
	}

	int imgQueue::push(const imgWithName& img)
	{
		std::lock_guard<std::mutex> lock(mtx_);
		imgQueue_.push(img);
		cv_.notify_one();		// 调用notify_one()函数可以唤醒一个等待线程
		return 0;
	}

	int imgQueue::pop(imgWithName& img)
	{
		try {
			std::unique_lock<std::mutex> lock(mtx_);
			cv_.wait(lock, [this] { return !imgQueue_.empty(); });
			img = imgQueue_.front();
			imgQueue_.pop();
		}
		catch (std::exception e)
		{
			// do something
			return 1;
		}
		return 0;
	}


	int writeAllImg(imgQueue& queue) {
		while (!stopFlag || !queue.isEmpty()) {
			imgWithName imgTmp;
			int res = queue.pop(imgTmp);
			try
			{
				if (imgTmp.imgType == 0)	// bmp
				{
					imwrite(imgTmp.name, imgTmp.img);
				}
				if (imgTmp.imgType == 1)		// jpg
				{
					imwrite(imgTmp.name, imgTmp.img, { cv::IMWRITE_JPEG_QUALITY, 80 });
				}
			}
			catch (std::exception e)
			{
				// do something
				return 1;
			}
		}
		return 0;
	}

	int defectImages2Queue(const imgGroup& inputImgs, imgQueue& queue, const int cropedWidth, const int cropedHeight, const int lineWidth, const int charSize, const int boundingDist, const int flawNum, vector <MyFlawInfo>& flaws, Mat& imageLabled, const vector<int>& writeImageParams, const int isShowSize, const int isShowArea, const int isShowScore, const int maxSaveImgNum)
	{
		int realFlawNum = maxSaveImgNum;

		if (flawNum < maxSaveImgNum)
			realFlawNum = flawNum;

		Mat colorImg = inputImgs.colorImg;
		Mat grayImg = inputImgs.grayImg;
		// 缺陷数量少于设置的最大值时，保存所有的缺陷小图；缺陷数量大于设置的最大值时，保存部分缺陷小图
		for (int i = 0; i < realFlawNum; ++i)
		{
			// 00001_S1V1__时间戳_二维码_父分类_子分类(长#宽#面积)_index_

			// 获取缺陷信息字符串,用于图像上显示
			string defectInfoDisp = "类型：" + flaws[i].defectName + "\n";
			if (isShowSize)
			{
				defectInfoDisp += "长：" + to_string_with_precision(flaws[i].length) + "\n" + "宽：" + to_string_with_precision(flaws[i].width) + "\n";
			}
			if (isShowArea)
			{
				defectInfoDisp += "面积：" + to_string_with_precision(flaws[i].area) + "\n";
			}
			if (isShowScore)
			{
				defectInfoDisp += "置信度：" + to_string_with_precision(flaws[i].confidence) + "\n";
			}

			// 生成缺陷小图
			// 获取输入图像的宽高
			int imageWidth = grayImg.cols;
			int imageHeight = grayImg.rows;

			// 计算缺陷的宽高
			//Point defectLeftTop, defectRightBottom;
			Rect defectRect;
			vector<vector<Point>> contoursForDraw;
			computeBoundingRect(flaws[i].defectContours, defectRect, contoursForDraw);
			int defectWidth = defectRect.width;
			int defectHeight = defectRect.height;


			// 计算裁剪后的图像左上角和右下角坐标
			Point cropedLeftTop, cropedRightBottom;
			cropedLeftTop.x = flaws[i].defectCenter.x - cropedWidth / 2;
			cropedLeftTop.y = flaws[i].defectCenter.y - cropedHeight / 2;
			cropedRightBottom.x = flaws[i].defectCenter.x + cropedWidth / 2;
			cropedRightBottom.y = flaws[i].defectCenter.y + cropedHeight / 2;

			// 缺陷大于裁剪后的图像
			if (defectWidth > cropedWidth)
			{
				cropedLeftTop.x = defectRect.tl().x - boundingDist;
				cropedRightBottom.x = defectRect.br().x + boundingDist;
			}
			if (defectHeight > cropedHeight)
			{
				cropedLeftTop.y = defectRect.tl().y - boundingDist;
				cropedRightBottom.y = defectRect.br().y + boundingDist;
			}

			Rect cropedRectTmp = Rect(cropedLeftTop.x, cropedLeftTop.y, cropedRightBottom.x - cropedLeftTop.x, cropedRightBottom.y - cropedLeftTop.y);

			Rect imageRect(0, 0, imageWidth, imageHeight);

			// 两个矩形的交集
			Rect cropedRect = cropedRectTmp & imageRect;
			// 裁剪图像
			Mat cropedImage = grayImg(cropedRect);

			// 存储BMP格式的无标注信息缺陷小图
			string imageUnlabledPath = flaws[i].fileSavePath.imageUnlabled + "/" + flaws[i].fileName[1];


			// 绘制缺陷轮廓，显示缺陷信息，保存为jpg
			Mat imageShow = colorImg;
			int contoursNum = contoursForDraw.size();
			drawContours(imageShow, contoursForDraw, -1, Scalar(255, 0, 0), lineWidth);
			drawContours(imageLabled, contoursForDraw, -1, Scalar(255, 0, 0), lineWidth);

			// 最小外接矩形
			Point2f rectPoints[4];
			RotatedRect rect = minAreaRect(contoursForDraw[0]);
			rect.points(rectPoints);
			for (int i = 0; i < 4; ++i)
			{
				line(imageShow, rectPoints[i], rectPoints[(i + 1) % 4], Scalar(0, 0, 255), lineWidth);
				line(imageLabled, rectPoints[i], rectPoints[(i + 1) % 4], Scalar(0, 0, 255), lineWidth);
			}

			// 缺陷信息
			Size textSize = getTextSize(defectInfoDisp, FONT_HERSHEY_SIMPLEX, 0.1, charSize, 0);
			Point defectInfoLeftBottom = Point(0, 0);
			// 放置在缺陷左下角
			defectInfoLeftBottom.x = defectRect.tl().x;
			defectInfoLeftBottom.y = defectRect.br().y + textSize.height;

			//putText(imageLabled, defectInfo, defectInfoLeftBottom, FONT_HERSHEY_SIMPLEX, 0.1, Scalar(0, 0, 255), charSize);

			zkhyPublicFuncHN::putTextZH(imageShow, defectInfoDisp.c_str(), defectInfoLeftBottom, Scalar(0, 0, 255), charSize, fontDisp, false, false);
			zkhyPublicFuncHN::putTextZH(imageLabled, defectInfoDisp.c_str(), defectInfoLeftBottom, Scalar(0, 0, 255), charSize, fontDisp, false, false);

			imageShow = imageShow(cropedRect);
			string imageLabledPath = flaws[i].fileSavePath.imageLabled + "/" + flaws[i].fileName[0];

			imgWithName imgUnlabelTmp;
			imgUnlabelTmp.img = cropedImage;
			imgUnlabelTmp.name = imageUnlabledPath;
			imgUnlabelTmp.imgType = 0;

			queue.push(imgUnlabelTmp);

			if (flaws[i].fileSavePath.imageLabled != "" || flaws[i].fileName[0] != "")
			{
				imgWithName imgLabelTmp;
				imgLabelTmp.img = imageShow;
				imgLabelTmp.name = imageLabledPath;
				imgLabelTmp.imgType = 1;
				queue.push(imgLabelTmp);
			}
		}

		return 0;
	}
}

namespace alglib::ops::zkhyProHN::alg {
	//-----------------------------saveDefectImage------------------------------------------//
	//功能：存储缺陷小图函数入口
	int saveDefectImage(const saveDefectImageInput& input, saveDefectImageOutput& output)
	{
		try {
			int ret = realSaveDefectImage(input, output);
			if (ret != 0)
			{
				AlgErrMsgSaveDefectImage algErrMsg;
				std::string errorMsg = "[saveDefectImage] ERROR: 存储缺陷小图失败，错误信息：" + algErrMsg.msgs[ret];
				LOG.log(errorMsg, LEVEL::err);
			}
			else
			{
				LOG.log("[saveDefectImage] 运算完成。");
			}
			return ret;
		}
		catch (...) {
			LOG.log("[saveDefectImage] ERROR: 未知异常", LEVEL::err);
			return 10;
		}
	}


}
