#include <include/logger/logger.h>
#include <include/util.h>

int main()
{
	auto logger = std::make_shared<RareVoyager::Logger>("root");
	// 添加一个控制台级别日志
	logger->addAppender(std::make_shared<RareVoyager::StdoutLogAppender>());

	//logger->addAppender(std::make_shared<RareVoyager::FileLogAppender>(RDEBUG,RareVoyager::GetCurrentDateStr()));

	// 构造一条完整日志LogEent, 主要包含以下信息
	// auto event = std::make_shared<RareVoyager::LogEvent>(
	// 		__FILE__, __LINE__, 0, std::hash<std::thread::id>{}(std::this_thread::get_id()), 2, (uint32_t)time(0),std::string("xx")
	// 		);

	// 给定日志级别 和 完整给日志记录
	// 打印日志
	// logger->log(RareVoyager::LogLevel::WARN, event);

	// 宏最终拿到了一个 stringstream 对象
	// RAREVOYAGER_LOG_DEBUG(logger) <<"Error message:";
	// RAREVOYAGER_LOG_INFO(logger) <<"Info message:" << "hello " << " = " << "world";

	// --- 场景 2：带整数参数 ---
	int id = 1001;
	// 输出: ... user_id: 1001 process ok
	RAREVOYAGER_LOG_FMT_DEBUG(logger, "user_id: %d process ok", id);

	// --- 场景 3：混合类型 ---
	const char* name = "RareVoyager";
	float progress = 55.5f;
	// 输出: ... Project: RareVoyager, Progress: 55.50%
	RAREVOYAGER_LOG_FMT_DEBUG(logger, "Project: %s, Progress: %.2f%%", name, progress);
	return 0;
}