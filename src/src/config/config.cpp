#include <algorithm>


#include <include/config/config.h>

namespace RareVoyager
{
#pragma region ConfigVarBase
	ConfigVarBase::ConfigVarBase(std::string const& name, std::string const& description)
		: m_name(name), m_description(description)
	{
		std::transform(m_name.begin(),m_name.end(),m_name.begin(),::tolower);
	}

	const std::string& ConfigVarBase::getName() const
	{
		return m_name;
	}

	const std::string& ConfigVarBase::getDescription() const
	{
		return m_description;
	}

#pragma endregion ConfigVarBase

#pragma region ConfigVar


#pragma endregion ConfigVar

#pragma region Config
	Config::ConfigVarMap Config::s_datas;
#pragma endregion Config

}