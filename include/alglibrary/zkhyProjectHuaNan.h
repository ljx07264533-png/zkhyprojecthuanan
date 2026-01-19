/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo£¬ÏêÏ¸ÄÚÈÝ:todo
***************************************************/
#ifndef ZKHYPROJECT_HUANAN_H
#define ZKHYPROJECT_HUANAN_H

#include "alglibrary/zkhyProjectHuaNan/version.h"

#include "alglibrary/zkhyProjectHuaNan/areaFlatFieldCorrect.h"
#include "alglibrary/zkhyProjectHuaNan/camCalib.h"
#include "alglibrary/zkhyProjectHuaNan/createProjTransMat.h"
#include "alglibrary/zkhyProjectHuaNan/distortionCalib.h"
#include "alglibrary/zkhyProjectHuaNan/genAreaFlatFieldCorreFactor.h"
#include "alglibrary/zkhyProjectHuaNan/loadCalibParams.h"
#include "alglibrary/zkhyProjectHuaNan/loadProjTransMat.h"
#include "alglibrary/zkhyProjectHuaNan/projTrans.h"
#include "alglibrary/zkhyProjectHuaNan/targetAffineTrans.h"
#include "alglibrary/zkhyProjectHuaNan/waferImageProcess.h"
#include "alglibrary/zkhyProjectHuaNan/gatherLine.h"
#include "alglibrary/zkhyProjectHuaNan/sideWireMeasure.h"
#include "alglibrary/zkhyProjectHuaNan/sideConnectMeasure.h"
#include "alglibrary/zkhyProjectHuaNan/DetFengBianHeight.h"
#include "alglibrary/zkhyProjectHuaNan/FindBoardEdge.h"
#include "alglibrary/zkhyProjectHuaNan/PutImageTogether.h"
#include "alglibrary/zkhyProjectHuaNan/LoadMarkTeachInfo.h"
#include "alglibrary/zkhyProjectHuaNan/MarkDetect.h"
#include "alglibrary/zkhyProjectHuaNan/MarkTeach.h"
#include "alglibrary/zkhyProjectHuaNan/autoMark.h"
#include "alglibrary/zkhyProjectHuaNan/creatTexture.h"
#include "alglibrary/zkhyProjectHuaNan/creatVariationTexture.h"
#include "alglibrary/zkhyProjectHuaNan/segementVariation.h"
#include "alglibrary/zkhyProjectHuaNan/inspectTexture.h"
#include "alglibrary/zkhyProjectHuaNan/loadModel.h"
#include "alglibrary/zkhyProjectHuaNan/lodSegement.h"
#include "alglibrary/zkhyProjectHuaNan/miniLEDGlueDetect.h"
#include "alglibrary/zkhyProjectHuaNan/panelLayOut.h"
#include "alglibrary/zkhyProjectHuaNan/inspectSegement.h"
#include "alglibrary/zkhyProjectHuaNan/selectRegion.h"
#include "alglibrary/zkhyProjectHuaNan/BGABall2dDefectDet.h"
#include "alglibrary/zkhyProjectHuaNan/BGAInfoSetup.h"
#include "alglibrary/zkhyProjectHuaNan/EdgeAlign.h"
#include "alglibrary/zkhyProjectHuaNan/GenPackagingTemplate.h"
#include "alglibrary/zkhyProjectHuaNan/getBGABall3DFeature.h"
#include "alglibrary/zkhyProjectHuaNan/getPKGInfoSetupItemList.h"
#include "alglibrary/zkhyProjectHuaNan/ImagesCombine.h"
#include "alglibrary/zkhyProjectHuaNan/loadPointsCloud.h"
#include "alglibrary/zkhyProjectHuaNan/Packaging2dBodyMeasure.h"
#include "alglibrary/zkhyProjectHuaNan/PKGInfoSetup.h"
#include "alglibrary/zkhyProjectHuaNan/pointCloudToDeepImage.h"
#include "alglibrary/zkhyProjectHuaNan/QFNInfoSetup.h"
#include "alglibrary/zkhyProjectHuaNan/QFNPad2dDefectDet.h"
#include "alglibrary/zkhyProjectHuaNan/savePointsCloud.h"
#include "alglibrary/zkhyProjectHuaNan/SideAlign.h"
#include "alglibrary/zkhyProjectHuaNan/emptyCheckerBlob.h"
#include "alglibrary/zkhyProjectHuaNan/saveDefectImage.h"
#include "alglibrary/zkhyProjectHuaNan/saveDefectInfo.h"
#include "alglibrary/zkhyProjectHuaNan/saveDetectedInfo.h"
#include "alglibrary/zkhyProjectHuaNan/Laseretchingmeasurement.h"


#endif // ZKHYPROJECT_HUANAN_H