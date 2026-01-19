#include "alglibrary/zkhyProjectHuaNan/version.h"
#include "alglibrary/zkhyProjectHuaNan/git_info.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace alglib::core;

namespace alglib::ops::zkhyProHN::alg {
	string getVersion(void)
	{
		return 	"zkhyProHN(" + string(GIT_BRANCH) + ":" + string(GIT_COMMIT_ID) + ");";
	}
}


