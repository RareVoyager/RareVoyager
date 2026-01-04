#include <iostream>
#include <yaml-cpp/yaml.h>

int main() {
	YAML::Node config = YAML::LoadFile("config.yaml");

	std::cout << "system.name = "
			  << config["system"]["name"].as<std::string>() << std::endl;

	return 0;
}
