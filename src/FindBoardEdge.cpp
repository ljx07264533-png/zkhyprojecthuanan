#include "alglibrary/zkhyProjectHuaNan/FindBoardEdge.h"
#include "alglibrary/zkhyProjectHuaNan/DetFengBianHeight.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::location;
using namespace alglib::ops::location::alg;

namespace alglib::ops::zkhyProHN {

}

namespace alglib::ops::zkhyProHN::alg {
    int FindBoardEdge(const FindBoardEdgeInput& input, FindBoardEdgeOutput& result)
    {
        FlawInfo flaw;

        cv::Mat src = input.src;
        if (src.empty())
        {
            return 26;           // 输入图像为空
        }
        // 解析参数
        AlgParaFindBoardEdge* alg = (AlgParaFindBoardEdge*)&input.algPara;
        //region信息
        int upYBegin = alg->i_params.block.upYBegin;						//上侧region的起始y
        int upWidth = alg->i_params.block.upWidth;						//上侧region的宽
        int upHeight = alg->i_params.block.upHeight;						//上侧region的高
        double UDGrayDiff = alg->d_params.block.UDGrayDiff;                 //上下roi灰度差
        double LRGrayDiff = alg->d_params.block.LRGrayDiff;                 //上下roi灰度差

        // [1]划取图像左侧和右侧两个roi，用于判断是不是板子首尾图
        cv::Mat leftRegion = src(cv::Rect(0, upYBegin, upWidth, upHeight));
        cv::Scalar leftRegionMean = cv::mean(leftRegion);   //计算图像左侧区域的灰度平均值
        cv::Mat rightRegion = src(cv::Rect(src.cols - upWidth, upYBegin, upWidth, upHeight));
        cv::Scalar rightRegionMean = cv::mean(rightRegion); //计算图像右侧区域的灰度平均值

        // [2]划取图像底部ROI，用于计算图像背景的灰度值
        cv::Mat bottomRegion = src(cv::Rect(0, src.rows - 10, src.cols, 10));
        cv::Scalar bottomRegionMean = cv::mean(bottomRegion);
        const double grayThreshold = bottomRegionMean[0] + UDGrayDiff;

        RectangleROI liMianRoi;
        RectangleROI liMianLeftRoi{ cv::Point2f(src.cols / 2,0),cv::Point2f(src.cols / 2,250),src.cols / 2 + 1,ROIDirection::Negative, cv::Point2f(0,0) };
        RectangleROI liMianRightRoi{ cv::Point2f(src.cols / 2,0),cv::Point2f(src.cols / 2,250),src.cols / 2 + 1,ROIDirection::Positive,cv::Point2f(0,0) };

        // [3]根据图像的左右侧灰度值判断是首图还是尾图
        if ((leftRegionMean[0] < grayThreshold && rightRegionMean[0] > grayThreshold) || rightRegionMean[0] - leftRegionMean[0] > LRGrayDiff)//左立面检测
        {
            liMianRoi = liMianLeftRoi;
            flaw.type = 0;    //首图

        }
        else if ((rightRegionMean[0] < grayThreshold && leftRegionMean[0] > grayThreshold) || leftRegionMean[0] - rightRegionMean[0] > LRGrayDiff)//右立面检测
        {
            liMianRoi = liMianRightRoi;
            flaw.type = 1;    //尾图
        }
        else
        {
            return 0;
        }

        //[4]边缘位置计算
        GatherLineHighPrecisionOutput edgeLine;   //存放拟合直线算子的结果
        try
        {
            //[1] 板材侧边缘直线拟合
            bool isBinary = alg->i_params.block.isBinary;               //是否未二值化
            int binaryThreshold = alg->i_params.block.binThreshold;    //二值化阈值，当不开启未开值化时无效
            int edgeThreshold = alg->i_params.block.edgeThreshold;     //找边缘点的灰度差阈值
            double ransacDist = alg->d_params.block.ransacDist;         //剔除异常点的ransac距离
            int essenceOffset = alg->i_params.block.nEssenceOffset;     //精定位宽度

            int retFitLine = fitLine(src, edgeLine, liMianRoi, isBinary, binaryThreshold, edgeThreshold, ransacDist, essenceOffset);  //检测封边高低用二值化，检测立面不用（因为亮暗板边缘灰度差异较大，不好设阈值）
            if (retFitLine != 0) {
                flaw.pos = Point2f(-1, -1);    //找边失败
                return retFitLine;
            }

            flaw.pos = Point2f((edgeLine.twoPtsLinePic[0][0].x + edgeLine.twoPtsLinePic[0][1].x) / 2, 0);
            vector<Point2f> pts;
            pts.push_back(Point2f(flaw.pos.x - 10, 0));
            pts.push_back(Point2f(flaw.pos.x + 10, 0));
            pts.push_back(Point2f(flaw.pos.x + 10, 300));
            pts.push_back(Point2f(flaw.pos.x - 10, 300));
            pts.push_back(Point2f(flaw.pos.x - 10, 0));
            flaw.pts = pts;

            result.flawinfo.push_back(flaw);

            //画出高精度拟合算子找出的所有点
            /*Mat temp;
            cvtColor(src, temp, COLOR_GRAY2BGR);
            for (int i = 0; i < liMianLine.linePtsPic[0].size(); i++)
            {
                circle(temp, liMianLine.linePtsPic[0][i], 1, Scalar(0, 0, 255), -1);
            }
            for (int i = 0; i < liMianLine.linePtsPicRetrial[0].size(); i++)
            {
                circle(temp, liMianLine.linePtsPicRetrial[0][i], 2, Scalar(0, 255, 0), -1);
            }*/
            //cv:line(temp, liMianLine.twoPtsLinePic[0][0], liMianLine.twoPtsLinePic[0][1], Scalar(255, 255, 0), 1);

        }

        catch (...)
        {
            return 27;
        }

        return 0;
    }

}
