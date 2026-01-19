// Description: 日志类实现文件
// Date: 2023/03/28
// Developer: XuLong Guo

#pragma once
#include <string>
#include <filesystem>
#include <mutex>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#define DEFAULT_LOG_PATH "./Log/Alg_Log"
#define DEFAULT_LOG_NAME "Alglog"
#define DEFAULT_LOG_PATTERN "[%Y-%m-%d %H:%M:%S.%e] [%l] [%n] [PID %P] %v"
#define DEFAULT_LOG_SIZE 1024*1024*5	// log文件默认大小，5Mb
#define DEFAULT_LOG_NUM 10	// log文件默认数量，10个
#define LOG_PATTERN_WITH_LINE "[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%!:%#] [%n] [PID %P] %v"

// 获取文件名
#ifdef _WIN32
#define __FILENAME__ (strrchr(__FILE__,'\\')?(strrchr(__FILE__,'\\')+1):__FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__,'/')?(strrchr(__FILE__,'/')+1):__FILE__)
#endif //_WIN32

// 定义宏，用于在日志输出前追加文件名、函数名和行号
#define SUFFIX(msg) std::string(std::string("[") + __FILENAME__ + ":" + __FUNCTION__ + ":" + std::to_string(__LINE__) + "] " + msg)

#define ALGLOG(msg,...) algLogger::logger::getInstance().log(SUFFIX(msg),__VA_ARGS__)


#define ALGDLLEXPORT __declspec(dllexport)

// TODO 修改为单例模式，只有一个logger对象
// TODO 关注关键资源的占用

namespace algLogger
{
	// log等级，分为 info, debug, warn, error, critical, trace, off
	enum logLevel : int
	{
		trace = 0,
		debug = 1,
		info = 2,
		warn = 3,
		error = 4,
		critical = 5,
		off = 6,
		numLevels
	};

	// log输出方式，分为控制台输出，文件输出，控制台和文件输出
	enum logType : int
	{
		console = 0,
		file = 1,
		debugView = 2,
		all = 3,
		numLogType
	};

	// log文件类型，分为单一日志文件，循环日志文件，每日日志文件
	enum logFileType : int
	{
		single = 0,
		rotating = 1,
		daily = 2,
		numFileType
	};

	// 异步
	class ALGDLLEXPORT logger
	{
	public:

		static logger& getInstance();	// 获取单例实例
		static void destroyInstance();	// 销毁单例实例

		bool setPattern(const std::string& pattern);	// 设置输出格式
		bool setLogFile(logFileType type, int size = DEFAULT_LOG_SIZE, int num = DEFAULT_LOG_NUM);  // 设置log文件类型，size和num只针对循环日志有效

		int log(const std::string& msg, logLevel level, logType type = logType::console, std::string name = DEFAULT_LOG_NAME, std::string path = DEFAULT_LOG_PATH); // 主要的日志写入函数

		std::string getVersion();

		int createDir();

	private:

		logger();
		~logger();

		// 删除复制构造函数和赋值操作符
		logger(logger const&) = delete;
		void operator=(logger const&) = delete;

		static logger* logger_;
		std::shared_ptr<spdlog::logger> logger_ptr;
		std::mutex logger_lock;

		logLevel level;
		logFileType fileType;

		std::string logPath;
		std::string logName;
		std::string logPattern;

		int logSize;
		int logNum;

		// 内部函数，不对外开放
		int log2Console(const std::string& msg, logLevel level);	// 日志写入控制台
		int log2File(const std::string& msg, logLevel level);	// 日志写入文件
		int log2DebugView(const std::string& msg, logLevel level);	// 日志写入DebugView

		void outputLog(const std::string& msg, logLevel level);		// 输出日志

		void flushLog();	// 刷新日志
	};
} // namespace algSupport