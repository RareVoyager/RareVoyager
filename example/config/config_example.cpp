#include <include/logger/logger.h>
#include <include/config/config.h>


RareVoyager::ConfigVar<int>::ptr g_int_value_config =
RareVoyager::Config::Lookup("system.port",(int)8080,"system port");

int main()
{

	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << g_int_value_config->getName();
	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << g_int_value_config->getValue();
	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << g_int_value_config->getDescription();
	return 0;
}