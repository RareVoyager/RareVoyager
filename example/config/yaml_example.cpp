
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>
#include <include/util.h>
#include <include/logger/logger.h>


/**
 * Map（键值）
 * Sequence（数组）
 * Scalar（字符串 / 数字 / bool）
 */

const static std::string file_name = "config.yaml";
int main() {
	YAML::Node config;
	config["system"]["name"] = "RareVoyager";
	config["system"]["version"] = ".0.0.1";
	config["system"]["debug"] = true;

	config["window"]["width"] = "1920";
	config["window"]["height"] = "1080";

	std::ofstream ofs(file_name);
	ofs << config;
	ofs.close();
	std::stringstream ss;
	ss << config;

	//
	// YAML::Node read_config = YAML::LoadFile(file_name);
	// RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << read_config["system"]["name"].as<std::string>();
	// RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << read_config["system"]["version"].as<std::string>();
	// RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << read_config["system"]["debug"].as<std::string>();
	// RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << read_config["window"]["width"].as<std::string>();
	// RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << read_config["window"]["height"].as<std::string>();


	YAML::Node node = YAML::Load(ss.str());
	std::cout << node << std::endl;

	std::vector<int> v{1,2,3,5,4,6};
	std::vector v2{std::string("hello"),std::string("world")};
	std::stringstream ss1;
	for (auto i : v)
	{
		ss1 << i;
	}
	for (auto i : v2)
	{
		ss1 << i;
	}

	std::cout << ss1.str() << std::endl;

	return 0;
}
