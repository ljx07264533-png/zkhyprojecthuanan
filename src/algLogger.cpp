
#include "alglibrary/zkhyProjectHuaNan/algLogger.h"


// 初始化静态成员变量
algLogger::logger* algLogger::logger::logger_ = nullptr;

// 获取实例
algLogger::logger& algLogger::logger::getInstance()
{
	static logger instance;	// Guaranteed to be destroyed.
	// Instantiated on first use.
	return instance;
}

// 销毁实例
void algLogger::logger::destroyInstance()
{
	algLogger::logger& loggerTmp = algLogger::logger::getInstance();
	// 销毁实例
	std::lock_guard<std::mutex> lock(loggerTmp.logger_lock);
	spdlog::shutdown();
}

// 设置log等级
int setLogLevel(std::shared_ptr<spdlog::logger>& logger_, algLogger::logLevel level);

// 构造函数，初始化
algLogger::logger::logger()
{
	// 默认日志输出等级为trace
	level = logLevel::trace;
	// 默认日志文件路径
	logPath = DEFAULT_LOG_PATH;
	// 默认日志文件名
	logName = DEFAULT_LOG_NAME;
	// 默认日志输出格式
	logPattern = DEFAULT_LOG_PATTERN;
	// 默认日志文件类型
	fileType = logFileType::rotating;
	// 默认日志文件大小
	logSize = DEFAULT_LOG_SIZE;
	// 默认日志文件个数
	logNum = DEFAULT_LOG_NUM;
}

// 析构函数
algLogger::logger::~logger()
{
	getInstance().destroyInstance();

	if (logger_ != nullptr)
		delete logger_;
}

// 自定义输出格式
bool algLogger::logger::setPattern(const std::string& pattern)
{
	logPattern = pattern;

	return true;
}

// 设置log文件类型
bool algLogger::logger::setLogFile(logFileType type, int size, int num)
{
	fileType = type;
	logSize = size;
	logNum = num;
	return true;
}



// 输出日志
int algLogger::logger::log(const std::string& msg, logLevel newLevel, logType type, std::string name, std::string path)
{
	level = newLevel;
	logName = name;
	logPath = path;

	if (level == logLevel::off)
		return 0;

	if (type == logType::console || type == logType::all)
	{
		log2Console(msg, level);
	}
	if (type == logType::file || type == logType::all)
	{
		createDir();	// 创建log路径
		log2File(msg, level);
	}
	return 0;
}

// TODO 文件和行号输出错误
// 输出到控制台
int algLogger::logger::log2Console(const std::string& msg, logLevel level)
{
	// 如果已经存在同名的logger，先删除
	if (spdlog::get(logName) != nullptr)
	{
		spdlog::drop(logName);
	}

	// 创建控制台日志输出对象
	logger_ptr = spdlog::stdout_color_mt<spdlog::async_factory>(logName);

	outputLog(msg, level);

	return 0;
}

// 输出到文件
int algLogger::logger::log2File(const std::string& msg, logLevel level)
{
	std::string fileNameTmp = logPath + "/" + logName + ".txt";

	// 如果已经存在同名的logger，先删除
	if (spdlog::get(logName) != nullptr)
	{
		spdlog::drop(logName);
	}

	// 创建文件日志输出对象
	switch (fileType)
	{
	case logFileType::single:
		logger_ptr = spdlog::basic_logger_mt<spdlog::async_factory>(logName, fileNameTmp);
		break;
	case logFileType::rotating:
		logger_ptr = spdlog::rotating_logger_mt<spdlog::async_factory>(logName, fileNameTmp, logSize, logNum);
		break;
	case logFileType::daily:
		logger_ptr = spdlog::daily_logger_mt<spdlog::async_factory>(logName, fileNameTmp);
		break;
	default:
		spdlog::error("文件类型错误");
		flushLog();
		return 1;	// 文件类型错误
	}

	outputLog(msg, level);

	return 0;
}

// 日志写入DebugView
int algLogger::logger::log2DebugView(const std::string& msg, logLevel level)
{
	return 0;
}

// 获取spdlog版本号
std::string algLogger::logger::getVersion()
{
	std::string ver = "v.0.1.0 spdlog version " + std::to_string(SPDLOG_VER_MAJOR) + "." + std::to_string(SPDLOG_VER_MINOR) + "." + std::to_string(SPDLOG_VER_PATCH);

	return ver;
}

// 创建文件夹
int algLogger::logger::createDir()
{
	// 判断文件是否存在
	if (!std::filesystem::exists(logPath))
	{
		try
		{
			// 创建文件夹
			std::filesystem::create_directories(logPath);
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			std::string errorMsg = "日志格式错误：" + std::string(e.what());
			spdlog::error(errorMsg);
			flushLog();
			return 1;		//	错误的日志格式
		}
	}

	return 0;
}


int setLogLevel(std::shared_ptr<spdlog::logger>& logger_, algLogger::logLevel level)
{
	switch (level)
	{
	case algLogger::logLevel::trace:
		logger_->set_level(spdlog::level::trace);
		break;
	case algLogger::logLevel::debug:
		logger_->set_level(spdlog::level::debug);
		break;
	case algLogger::logLevel::info:
		logger_->set_level(spdlog::level::info);
		break;
	case algLogger::logLevel::warn:
		logger_->set_level(spdlog::level::warn);
		break;
	case algLogger::logLevel::error:
		logger_->set_level(spdlog::level::err);
		break;
	case algLogger::logLevel::critical:
		logger_->set_level(spdlog::level::critical);
		break;
	case algLogger::logLevel::off:
		logger_->set_level(spdlog::level::off);
		break;
	default:
		return 1;
	}
	return 0;
}

// 输出日志
void algLogger::logger::outputLog(const std::string& msg, logLevel level)
{
	// 设置日志输出格式
	try
	{
		logger_ptr->set_pattern(logPattern);
	}
	catch (const spdlog::spdlog_ex& e) {
		std::string errorMsg = "日志格式错误" + std::string(e.what());
		spdlog::error(errorMsg);
		flushLog();
		return;
	}

	if (setLogLevel(logger_ptr, level) != 0)
	{
		std::string errorMsg = "日志等级错误。";
		spdlog::error(errorMsg);
		flushLog();
		return;
	}

	std::string msgTmp;
	msgTmp = msg;
	// 根据 level 将 msg 输出到控制台
	switch (level)
	{
	case logLevel::trace:
		logger_ptr->trace(msgTmp);
		break;
	case logLevel::debug:
		logger_ptr->debug(msgTmp);
		break;
	case logLevel::info:
		logger_ptr->info(msgTmp);
		break;
	case logLevel::warn:
		logger_ptr->warn(msgTmp);
		break;
	case logLevel::error:
		logger_ptr->error(msgTmp);
		break;
	case logLevel::critical:
		logger_ptr->critical(msgTmp);
		break;
	default:
		break;
	}

	flushLog();
}

// 刷新日志
void algLogger::logger::flushLog()
{
	std::lock_guard<std::mutex> lock(logger_lock);
	if (logger_ptr != nullptr)
		logger_ptr->flush();
}