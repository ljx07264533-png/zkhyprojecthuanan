#include "alglibrary/zkhyProjectHuaNan/DetFengBianHeight.h"
#include "alglibrary/alglibLocation.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::location;
using namespace alglib::ops::location::alg;

namespace alglib::ops::zkhyProHN {

    //直线拟合
    int fitLine(const Mat& src, GatherLineHighPrecisionOutput& output, RectangleROI ROI, bool binary = false, int binThreshold = 120, int gradThreshold = 15, double ransacDist = 1, int essenceOffset = 40)
    {
        GatherLineHighPrecisionInput input;
        AlgParaGatherLineHighPrecision* alg = (AlgParaGatherLineHighPrecision*)&input.algPara;
        alg->i_params.block.isPreprocessing = binary;
        alg->i_params.block.nThreshold = binThreshold;
        alg->i_params.block.nMinValue = gradThreshold;
        alg->d_params.block.dRansacDis = ransacDist;
        alg->i_params.block.isCalib = 0;
        alg->i_params.block.nEdgePolarity = 1;
        alg->i_params.block.nGetEdgeMode = 0;
        alg->i_params.block.nPointCompoundSentence = 1;
        alg->i_params.block.nOffset = 10;
        alg->i_params.block.nFitLinePara = 1;
        alg->i_params.block.nCoarseStep = 5;
        alg->i_params.block.nEssenceStep = 1;
        alg->i_params.block.nfilterValue = 1;
        alg->d_params.block.dMultiple = 1;
        alg->d_params.block.dValueDif = 10.0;
        alg->i_params.block.nEssenceOffset = alg->i_params.block.nEssenceOffset;

        input.rectangleROI = ROI;
        input.img = src;
        int ret = gatherLineHighPrecision(input, output);

        return ret;
    }

    //根据拟合直线函数输出的结果，判断封边高低
    int fengBianHeightDet(const FBHeightDetInput& input, FBHeightDetOutput& result)
    {
        const float pi = 3.14159;
        const float thresholdX = input.threshold / input.pixelAcc * tan(input.angle * pi / 180);    //将检规转换成横向像素偏离量
        //const float thresholdY = input.fengBianDiThresholdY;
        if (input.output.linePtsPicRetrial.size() < 1)
            return 0;
        if (input.output.linePtsPicRetrial[0].size() < 1)
            return 0;
        double datumX = input.output.linePtsPicRetrial[0][input.output.linePtsPicRetrial[0].size() - 1].x;    //边缘点ransac之后靠近封边带那端的第一个点的x坐标

        vector<float> edgePointX;   //存原始边界点中y在nearEdge上面的点的x坐标
        vector<float> edgePointY;   //存原始边界点中y在nearEdge上面的点的y坐标。把x，y坐标分别存放是为了，利用algorithm找x的最值

        for (int i = 0; i < input.output.linePtsPic[0].size(); i++)   //从原始边界点中找位于ransac之后的最下一个点往上offset下面的点
        {
            if (input.output.linePtsPic[0][i].y < input.output.linePtsPicRetrial[0][input.output.linePtsPicRetrial[0].size() - 1].y - input.offset)
                continue;   //只取靠下边封边带的点
            edgePointX.push_back(input.output.linePtsPic[0][i].x);
            edgePointY.push_back(input.output.linePtsPic[0][i].y);
        }
        auto posMinX = std::min_element(edgePointX.begin(), edgePointX.end());
        int posMinXSN = std::distance(edgePointX.begin(), posMinX); //minX在vector的序号
        float minX = *posMinX;

        if (minX - datumX <= -thresholdX)  //封边高检测
        {
            vector<Point2f> tempPts;    //存fitLine找到的所有点中，x距datumX大于thresholdX的点
            vector<Point2f> points; //对tempPts中的点进行分割后的其中的一堆点集
            vector<vector<Point2f>> contours;   //存对tempPts中的点进行分割后的结果
            for (int i = 0; i < edgePointX.size(); i++)//在fitLine找到的所有点中，找出x距datumX大于thresholdX的所有点
            {
                if (edgePointX[i] - datumX <= -thresholdX)
                    tempPts.push_back(Point2f(edgePointX[i], edgePointY[i]));
            }
            points.push_back(tempPts[0]);
            for (int i = 1; i < tempPts.size(); i++)  //tempPts中的点进行分割
            {
                if (abs(tempPts[i].x - tempPts[i - 1].x) <= 6 && tempPts[i].y - tempPts[i - 1].y <= 3)  //从第0点开始，相近的点push进一起
                {
                    points.push_back(tempPts[i]);
                }
                else    //当当前点与上一个点不相近时，从当前点开始分割，将当前点之前的一堆点push进contours中，清空points并将当前点push进去
                {
                    contours.push_back(points);
                    points.clear();
                    points.push_back(tempPts[i]);
                }
            }
            contours.push_back(points); //把最后一轮的points push进去
            for (int i = 0; i < contours.size(); i++)
            {
                if (contours[i].size() > 10)
                {
                    auto compareX = [](const cv::Point2f& a, const cv::Point2f& b) {
                        return a.x < b.x;
                        };
                    auto minElement = std::min_element(contours[i].begin(), contours[i].end(), compareX);

                    //result.height = ( datumX - minElement->x ) / tan ( input.angle * pi / 180 ) * input.pixelAcc;
                    result.height = (datumX - minElement->x) / tan(input.angle * pi / 180);
                    result.box = cv::Rect(minElement->x - 3, contours[i][0].y - 5, datumX - minElement->x + 6, contours[i][contours[i].size() - 1].y - contours[i][0].y + 8);

                    /*result.flaw.FlawBasicInfo.flawWidth = ( datumX - minElement->x ) / tan ( input.angle * pi / 180 );    //这里不用乘像素当量了，软件会处理
                    result.flaw.FlawBasicInfo.type = 7;

                    //bbox
                    cv::Point2f leftTopPt;
                    leftTopPt.x = minElement->x - 3;
                    leftTopPt.y = contours[ i ][ 0 ].y - 5;
                    float width = datumX - minElement->x + 6;
                    float height = contours[ i ][ contours[ i ].size ( ) - 1 ].y - contours[ i ][ 0 ].y + 8;
                    result.flaw.FlawBasicInfo.pts.push_back ( leftTopPt );
                    result.flaw.FlawBasicInfo.pts.push_back ( Point2f(leftTopPt.x+width, leftTopPt.y) );
                    result.flaw.FlawBasicInfo.pts.push_back ( Point2f ( leftTopPt.x + width, leftTopPt.y + height ) );
                    result.flaw.FlawBasicInfo.pts.push_back ( Point2f ( leftTopPt.x, leftTopPt.y + height ) );
                    result.flaw.FlawBasicInfo.pts.push_back ( leftTopPt );*/
                    //这里先不按软件缺陷结构体改，还是先输出height和bbox，然后在外面给flaw的相关成员写值，反正封边高也还要在外层再做个判断

                    return 1;
                }
            }
        }

        {   //封边低检测
            vector<Point2f> tempPts;    //存fitLine找到的所有点中，x距datumX大于thresholdX+5的点
            vector<Point2f> points; //对tempPts中的点进行分割后的其中的一块点集
            vector<vector<Point2f>> contours;   //存对tempPts中的点进行分割后的结果
            float var = 3;
            for (int i = 0; i < edgePointX.size(); i++)//在fitLine找到的所有点中，找出x距datumX大于thresholdX的所有点
            {
                if (edgePointX[i] - datumX > thresholdX + var)
                    tempPts.push_back(Point2f(edgePointX[i], edgePointY[i]));
            }
            if (tempPts.size() < 3)
                return 0;
            points.push_back(tempPts[0]);
            for (int i = 1; i < tempPts.size(); i++)  //tempPts中的点进行分割
            {
                if (abs(tempPts[i].x - tempPts[i - 1].x) <= 6 && tempPts[i].y - tempPts[i - 1].y <= 3)  //从第0点开始，相近的点push进一起
                {
                    points.push_back(tempPts[i]);
                }
                else    //当当前点与上一个点不相近时，从当前点开始分割，将当前点之前的一堆点push进contours中，清空points并将当前点push进去
                {
                    contours.push_back(points);
                    points.clear();
                    points.push_back(tempPts[i]);
                }
            }
            contours.push_back(points); //把最后一轮的points push进去
            for (int i = 0; i < contours.size(); i++)
            {
                if (contours[i].size() > 10)
                {
                    //result.box = cv::Rect(minX, edgePointY[posMinXSN] - 30, datumX - minX + 5, 60);
                    auto compareX = [](const cv::Point2f& a, const cv::Point2f& b) {
                        return a.x < b.x;
                        };
                    auto minElement = std::min_element(contours[i].begin(), contours[i].end(), compareX);

                    result.height = abs(datumX - minElement->x) / tan(input.angle * pi / 180);
                    result.box = cv::Rect(minElement->x - 3, contours[i][0].y - 5, 10, contours[i][contours[i].size() - 1].y - contours[i][0].y + 8);

                    return 2;
                }
            }
        }

        return 0;
    }


}

namespace alglib::ops::zkhyProHN::alg {
    
    int DetFengBianHeight(const DetFengBianHeightInput& input, DetFengBianHeightOutput& result)
    {
        //using namespace DataTypeHN;
        cv::Mat src = input.src;
        if (src.empty())
            return 26;           // 输入图像为空
        RectangleROI Roi = input.Roi;

        // 解析参数
        AlgParaDetFengBianHeight* alg = (AlgParaDetFengBianHeight*)&input.algPara;
        //拟合直线参数
        bool isBinary = alg->i_params.block.isBinary;           //拟合直线是否二值化找点
        int binThreshold = alg->i_params.block.binThreshold;    //二值化阈值
        int edgeThreshold = alg->i_params.block.edgeThreshold;  //找点梯度阈值
        double ransacDist = alg->d_params.block.ransacDist;     //ransac距离
        int essenceOffset = alg->i_params.block.nEssenceOffset; //精定位宽度

        //封边高低算法参数
        double threshold = alg->d_params.block.threshold;       //检规，单位mm
        double pixelAcc = alg->d_params.block.pixelAcc;         //像素精度（像素当量）
        double angle = alg->d_params.block.angle;               //激光与相机夹角，单位是角度
        int offset = alg->i_params.block.offset;                //找异常点的y向起始范围，从ransac之后的靠近封边带那一端的最后一个点往上offset开始，往下找异常点

        GatherLineHighPrecisionOutput laserLine;    //存放拟合直线算子的结果
        int retFengBianHeight = 0;                  //封边带高低检测结果，0:正常，1:封边高，2:封边低
        //检测封边高低输入、输出参数
        FBHeightDetInput fbhInput{ laserLine, threshold, pixelAcc, angle, offset };
        FBHeightDetOutput fbhOutput;

        try {
            int retFitLine = 0;
            retFitLine = fitLine(input.src, laserLine, Roi, isBinary, binThreshold, edgeThreshold, ransacDist);
            if (retFitLine != 0)
            {
                return retFitLine;
            }
            if (laserLine.linePtsPic[0].size() < 50)    //如果激光打在长带上，结束检测
            {
                return 27;     //拟合直线算子找到的点少于50
            }

            try
            {
                retFengBianHeight = fengBianHeightDet(fbhInput, fbhOutput);
            }
            catch (...)
            {
                return 28;     //封边高低检测失败
            }

            if (retFengBianHeight == 1) //为防止封边高过检，对检出的封边高加一个间隙有无判断
            {
                cv::Mat gapRoi = src(cv::Rect(fbhOutput.box.x, fbhOutput.box.y - 15, 50, 25));
                cv::Mat gapReduce;
                cv::reduce(gapRoi, gapReduce, 1, REDUCE_SUM, CV_32F);
                vector<double> reduceRes;

                reduceRes = gapReduce.reshape(1, gapReduce.rows);
                std::sort(reduceRes.begin(), reduceRes.end());
                if (reduceRes[reduceRes.size() - 2] / (reduceRes[1] + 0.0001) < 1.8)
                {
                    return 0;   //封边高误检，不是缺陷
                }
            }
        }
        catch (...)
        {
            return 29;
        }

        if (retFengBianHeight == 1)
        {
            FlawInfo flaw;
            flaw.type = 7;     //封边高
            flaw.rect2Len2 = fbhOutput.height;
            flaw.pos = cv::Point2f(fbhOutput.box.x, fbhOutput.box.y);
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x, fbhOutput.box.y));
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x + fbhOutput.box.width, fbhOutput.box.y));
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x + fbhOutput.box.width, fbhOutput.box.y + fbhOutput.box.height));
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x, fbhOutput.box.y + fbhOutput.box.height));
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x, fbhOutput.box.y));

            result.flawinfo.push_back(flaw);
        }
        else if (retFengBianHeight == 2)
        {
            FlawInfo flaw;
            flaw.type = 8;     //封边低
            flaw.rect2Len2 = fbhOutput.height;
            flaw.pos = cv::Point2f(fbhOutput.box.x, fbhOutput.box.y);
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x, fbhOutput.box.y));
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x + fbhOutput.box.width, fbhOutput.box.y));
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x + fbhOutput.box.width, fbhOutput.box.y + fbhOutput.box.height));
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x, fbhOutput.box.y + fbhOutput.box.height));
            flaw.pts.push_back(cv::Point2f(fbhOutput.box.x, fbhOutput.box.y));

            result.flawinfo.push_back(flaw);
        }

        return 0;
    }

}
