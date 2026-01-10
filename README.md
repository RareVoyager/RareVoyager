# sylar高性能 C++ 服务器 学习日志

## 使用到的三方库

1. boost 的lexical_cast
2. yaml

## 本项目运行在window平台, 仅为了方便学习。

## day1-3：

最近才想起来写学习日志。因此将这三天的学习心得放到一天来写。

首先是学习了日志系统的编写。sylar 将一整个日志系统拆分成几个类。

1. `Logger:` 用户使用类。继承自 `std::enable_shared_from_this<Logger> `用来让对象在成员函数内部，获得指向自身的指针
2. `LogLevel:` 日志等级类。主要分了6个等级。还有一个将等级转换为字符串的方法。其中包含了一些宏魔法来简化流程。
3. `LogEvent:`记录了一条完整的用户日志。包括**时间、线程id，协程id等等**。这是日志的核心。
4. `LogFormatter:`日志格式化工厂。输出用户希望的日志格式。这里的 sylar**init()方法** 初学可能会有些晦涩难懂。我将其转换为一道类似leetcode题目。未来将会尝试重写这段逻辑。 [字符串解析.md](doc\logger\字符串解析.md) 
5. `LogAppender:`输出器的基类。目前衍生出来输出到控制台的和输出到文件中的日志器。
6. `LogManager:`日志管理类，单例模式。 有**宏魔法(RAREVOYAGER_LOG_ROOT())**来获取这个单例。实现了输出器的统一管理。
7. `LogEventWarp:`LogEvent 包装类。这个类尤其是他的析构函数，是实现宏**RAREVOYAGER_LOG_LEVEL**的关键。
8. `FormatItem:`输出信息基类。关键是将信息写入到字符串流中，最后统一处理。



他们之间的调度关系如下:


## TODO: 重新编译一次boost库

## day4 - day6
主要做完了配置系统。未来将配置系统的LexicalCast模块用C++模板的类型萃取重写逻辑。
