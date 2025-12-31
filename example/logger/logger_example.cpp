#include <include/logger/logger.h>

int main() {
    auto logger = std::make_shared<RareVoyager::Logger>("root");
    logger->addAppender(std::make_shared<RareVoyager::StdoutLogAppender>());

    auto event = std::make_shared<RareVoyager::LogEvent>(
        __FILE__, __LINE__, 0, 1, 2, (uint32_t)time(0)
    );

    logger->log(RareVoyager::LogLevel::DEBUG, event);
}

