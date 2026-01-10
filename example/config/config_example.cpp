#include "yaml-cpp/node/parse.h"

#include <include/logger/logger.h>
#include <include/config/config.h>

// 定义配置项
// RareVoyager::ConfigVar<int>::ptr g_int_value_config =
// 	RareVoyager::Config::Lookup("system.port", (int)8080, "system port");
//
// RareVoyager::ConfigVar<std::vector<int>>::ptr g_int_vec_config =
// 	RareVoyager::Config::Lookup("system.int_vec", std::vector<int>{1, 2, 3}, "system vec");
//
// RareVoyager::ConfigVar<std::list<int>>::ptr g_int_list_config =
// 	RareVoyager::Config::Lookup("system.int_list", std::list<int>{1, 2, 3}, "system list");
//
// RareVoyager::ConfigVar<std::set<int>>::ptr g_int_set_config =
// 	RareVoyager::Config::Lookup("system.int_set", std::set<int>{1, 2, 3}, "system list");
//
//
// RareVoyager::ConfigVar<std::string>::ptr g_log_name_config =
// 	RareVoyager::Config::Lookup("log.name", std::string("hello"), "log name");
//
// RareVoyager::ConfigVar<std::string>::ptr g_log_level_config =
// 	RareVoyager::Config::Lookup("log.level", std::string("abc"), "system vec");

// RareVoyager::ConfigVar<std::string>::ptr g_log_level1_config =
// 	RareVoyager::Config::Lookup("**/*//", std::string("abc"), "system vec");

// void test_yaml_load() {
// 	// 容器遍历宏
// #define XX(g_var,name,prefix) { \
// 	auto& v = g_var->getValue(); \
// 		for(auto& i:v) { \
// 		  RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << #prefix " " #name": " << i; \
// 	 } \
// 	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << #prefix " " #name"yaml: " << g_var->toString(); \
// 	 }
//
//
// #define XX_V(g_var,name,prefix) { \
// 	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << #prefix " " #name ": "<< g_var->getValue(); \
// 	}
//
// 	XX(g_int_vec_config,"int_vec","before");
// 	XX(g_int_list_config,"int_list","before");
// 	XX(g_int_set_config,"int_set","before");
// 	XX_V(g_int_value_config,"int_value","before");
//
//
// 	// 2. 加载 YAML 文件
// 	try {
// 		YAML::Node root = YAML::LoadFile("../../../test.yaml");
// 		RareVoyager::Config::LoadFromYaml(root);
// 	} catch (const std::exception& e) {
// 		RAREVOYAGER_LOG_ERROR(RAREVOYAGER_LOG_ROOT()) << "Load YAML file failed: " << e.what();
// 		return;
// 	}
//
// 	XX(g_int_vec_config,"int_vec","after");
// 	XX(g_int_list_config,"int_list","after");
// 	XX(g_int_set_config,"int_set","after");
// 	XX_V(g_int_value_config,"int_value","after");
//
// 	g_int_value_config->addListener(1001, [](const int& old_val, const int& new_val){
// 		RAREVOYAGER_LOG_ERROR(RAREVOYAGER_LOG_ROOT()) << " 端口由" << old_val << "改变为" << new_val;
// 	});
//
// }

class Person
{
public:
	Person() = default;
	~Person() = default;

	std::string name;
	int age = 0;
	uint8_t sex = 0;

	std::string toString()
	{
		std::stringstream ss;
		ss << "[name = " << name << "age = " << age << "sex = " << (int)sex << "]";
		return ss.str();
	}

	bool operator==(const Person& oth) const
	{
		return name == oth.name && age == oth.age && sex == oth.sex;
	}
};

namespace RareVoyager {
	template<>
	class LexicalCast<std::string, Person> {
	public:
		Person operator()(const std::string& v) {
			YAML::Node node = YAML::Load(v);
			Person p;
			if (!node.IsMap()) {
				// 如果不是 Map，说明格式不对，返回默认对象或抛出异常
				return p;
			}
			p.name = node["name"].as<std::string>("");
			p.age = node["age"].as<int>(0);
			// 强制转为 int 再转 uint8_t，避免 yaml-cpp 将其视为 char
			p.sex = (uint8_t)node["sex"].as<int>(0);
			return p;
		}
	};

	template<>
	class LexicalCast<Person, std::string> {
	public:
		std::string operator()(const Person& p) {
			YAML::Node node;
			node["name"] = p.name;
			node["age"] = p.age;
			node["sex"] = (int)p.sex; // 转为 int 存储
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};
}

/**
 * @brief: 测试自定义类
 */
void test_class()
{
	RareVoyager::ConfigVar<Person>::ptr g_class_confg =
		RareVoyager::Config::Lookup("class.user",Person(),"class name");

	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << g_class_confg->toString();
	try {
		YAML::Node root = YAML::LoadFile("../../../test.yaml");
		RareVoyager::Config::LoadFromYaml(root);
	} catch (const std::exception& e) {
		RAREVOYAGER_LOG_ERROR(RAREVOYAGER_LOG_ROOT()) << "Load YAML file failed: " << e.what();
		return;
	}
	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << g_class_confg->toString();

}

RareVoyager::ConfigVar<int>::ptr g_system_port =
    RareVoyager::Config::Lookup("system.port", 8080, "System Port");

// 定义一个系统名称，默认值 "RareVoyager"
RareVoyager::ConfigVar<std::string>::ptr g_system_name =
    RareVoyager::Config::Lookup("system.name", std::string("RareVoyager"), "System Name");


void test_callback_chain() {
    RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "======= 测试开始 =======";

    g_system_port->addListener(0x1, [](const int& old_val, const int& new_val) {
        RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT())
            << "[Callback 1] Port 发生变化! " << old_val << " -> " << new_val;
    });

    g_system_port->addListener(0x2, [](const int& old_val, const int& new_val) {
        if (new_val < 1024) {
            RAREVOYAGER_LOG_WARN(RAREVOYAGER_LOG_ROOT())
                << "[Callback 2] 警告: 设置了特权端口 (小于1024): " << new_val;
        } else {
            RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT())
                << "[Callback 2] 端口设置安全。";
        }
    });

    g_system_name->addListener(0xF, [](const std::string& old_val, const std::string& new_val){
        RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT())
            << "[Callback Name] 系统名称变更: " << old_val << " -> " << new_val;
    });

    YAML::Node node = YAML::LoadFile("../../../test_callback.yaml");
	RareVoyager::Config::LoadFromYaml(node);

    RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "--- B. 重复设置端口为 80 ---";
    // 预期：不会有任何回调日志输出，因为 value 没有变
    g_system_port->setValue(80);


    // ==========================================
    // 6. 【清理阶段】删除监听器
    // ==========================================
    RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "--- D. 删除监听器并再次修改 ---";

    // 删除监听器 1
    g_system_port->delListener(0x1);

    // 再次修改
    // 预期：只有 Callback 2 会响应，Callback 1 已经被移除
    g_system_port->setValue(3306);

    RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "======= 测试结束 =======";
}

int main() {
	system("chcp 65001");
	// test_yaml_load();
	// test_class();
	// test_callback_chain();

	// 创建了一个名为my_logger 的 Logger类
	auto log = RAREVOYAGER_LOG_NAME("my_logger");
	// 添加一个输出到文件的实例
	log->addAppender(std::make_shared<RareVoyager::FileLogAppender>("log.log"));
	log->addAppender(std::make_shared<RareVoyager::StdoutLogAppender>());
	log->setFormatter("%d{%Y-%m-%d}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%m");
	RAREVOYAGER_LOG_INFO(log) << "hello world!";
	return 0;
}