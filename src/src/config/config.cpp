#include <algorithm>

#include <yaml-cpp/yaml.h>
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
	// 由于是static 需要显示的声明

	/**
	 * @brief: 从一个 YAML::Node 出发，递归遍历所有子节点，
	 * 把每一个节点对应的“完整路径 + 节点对象”收集到一个 list 里。
	 * @param prefix
	 * @param node
	 * @param output
	 */
	static void ListAllMember(const std::string& prefix,const YAML::Node& node,
	                          std::list<std::pair<std::string,const YAML::Node>>& output)
	{
		// 从字符串中查找“第一个不在给定字符集合中的字符 返回该字符的位置（>= 0）没有则抛出一场
		if (prefix.find_first_not_of(regex_str) != std::string::npos)
		{
			RAREVOYAGER_LOG_ERROR(RAREVOYAGER_LOG_ROOT()) << "Config Invalid name " << prefix;
			return;
		}
		output.emplace_front(std::make_pair(prefix,node));
		// 递归将所有配置属性写入到list
		if (node.IsMap())
		{
			for (auto it = node.begin();it!=node.end();++it)
			{
				ListAllMember(prefix.empty() ? it->first.Scalar()
					: prefix + "." + it->first.Scalar(), it->second, output);
			}
		}
	}
	// 调用fromString 方法
	void Config::LoadFromYaml(const YAML::Node& root)
	{
		std::list<std::pair<std::string,const YAML::Node>> all_node;
		ListAllMember("",root,all_node);

		for (auto [_key,_value] : all_node)
		{
			if (_key.empty())
			{
				continue;
			}
			// 字母转换成小写
			std::transform(_key.begin(),_key.end(),_key.begin(),::tolower);

			ConfigVarBase::ptr var = LookupBase(_key);
			if (var)
			{
				if (_value.IsScalar())
				{
					// 设置 m_val
					var->fromString(_value.Scalar());
				}
				else
				{
					std::stringstream ss;
					ss << _value;
					var->fromString(ss.str());
				}
			}
		}
	}
	ConfigVarBase::ptr Config::LookupBase(const std::string& name) {
		// RWMutexType::ReadLock lock(GetMutex());
		auto it = GetDatas().find(name);
		return it == GetDatas().end() ? nullptr : it->second;
	}
#pragma endregion Config

}