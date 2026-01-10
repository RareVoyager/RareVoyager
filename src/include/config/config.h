/*************************************************
 * 描述：配置系统
 *
 * File：config.h
 * Author：Cipher
 * Date：2026/1/3-17:30
 * Update：
 * ************************************************/

#ifndef RAREVOYAGER_CONFIG_H
#define RAREVOYAGER_CONFIG_H

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <unordered_map>
#include <unordered_set>

#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

#include <include/util.h>
#include <include/logger/logger.h>


/**
 * 原则: 约定优于配置
 */
namespace RareVoyager
{
	static const std::string regex_str = "abcdefghikjlmnopqrstuvwxyz._012345678";

	// TODO: 使用类型萃取 优化代码
#pragma region LexicalCast
	template<class F, class T>
	class LexicalCast
	{
	public:
		/**
		 * @brief: 将 string 转换为T
		 * @param v
		 * @return
		 */
		T operator()(const F& v)
		{
			return boost::lexical_cast<T>(v);
		}
	};

	///=========== 以下是一些std::string 与 stl容器之间的转换，借助中间件 YAML::Node
	/**
	 * @brief: 偏特化版本, 将string 转换为vector
	 * @tparam T
	 */
	template<class T>
	class LexicalCast<std::string, std::vector<T> >
	{
	public:
		std::vector<T> operator()(const std::string& v)
		{
			YAML::Node node = YAML::Load(v);
			std::vector<T> vec;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); ++i)
			{
				ss.str("");
				ss << YAML::Dump(node[i]);
				vec.push_back(LexicalCast<std::string, T>()(ss.str()));
			}
			return vec;
		}
	};

	template<class T>
	class LexicalCast<std::vector<T>, std::string>
	{
	public:
		std::string operator()(const std::vector<T>& v)
		{
			// 将一个包含 YAML 格式数据的字符串解析为内存中的 Node 对象。
			YAML::Node node;
			for (auto i: v)
			{
				node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
			}
			std::stringstream ss;
			ss << YAML::Dump(node);
			return ss.str();
		}
	};

	/**
	 * @brief: 偏特化版本, 将string 转换为 list
	 * @tparam T
	 */
	template<class T>
	class LexicalCast<std::string, std::list<T> >
	{
	public:
		std::list<T> operator()(const std::string& v)
		{
			YAML::Node node = YAML::Load(v);
			std::list<T> v_list;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); ++i)
			{
				ss.str("");
				ss << YAML::Dump(node[i]);
				v_list.push_back(LexicalCast<std::string, T>()(ss.str()));
			}
			return v_list;
		}
	};

	/**
	 * @brief: 偏特化版本, 将list 转换为 string
	 * @tparam T
	 */
	template<class T>
	class LexicalCast<std::list<T>, std::string>
	{
	public:
		std::string operator()(const std::list<T>& v)
		{
			// 将一个包含 YAML 格式数据的字符串解析为内存中的 Node 对象。
			YAML::Node node;
			for (auto i: v)
			{
				node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
			}
			std::stringstream ss;
			ss << YAML::Dump(node);
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::map<std::string, T> >
	{
	public:
		std::map<std::string, T> operator()(const std::string& v)
		{
			YAML::Node node = YAML::Load(v);
			std::map<std::string, T> v_map;
			std::stringstream ss;
			for (auto it = node.begin(); it != node.end(); ++it)
			{
				ss.str("");
				ss << YAML::Dump(it->first);
				v_map.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
			}
			return v_map;
		}
	};

	template<class T>
	class LexicalCast<std::map<std::string, T>, std::string>
	{
	public:
		std::string operator()(const std::map<std::string, T>& v)
		{
			YAML::Node node;
			for (auto [_key,_value]: v)
			{
				node[_key] = YAML::Load(LexicalCast<T, std::string>()(_value));
			}
			std::stringstream ss;
			ss << YAML::Dump(node);
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::unordered_map<std::string, T> >
	{
	public:
		std::unordered_map<std::string, T> operator()(const std::string& v)
		{
			YAML::Node node = YAML::Load(v);
			std::unordered_map<std::string, T> v_map;
			std::stringstream ss;
			for (auto it = node.begin(); it != node.end(); ++it)
			{
				ss.str("");
				ss << YAML::Dump(it->first);
				v_map.insert(std::make_pair(it->first.Scalar(), LexicalCast<std::string, T>()(ss.str())));
			}
			return v_map;
		}
	};

	template<class T>
	class LexicalCast<std::unordered_map<std::string, T>, std::string>
	{
	public:
		std::string operator()(const std::unordered_map<std::string, T>& v)
		{
			YAML::Node node;
			for (auto [_key,_value]: v)
			{
				node[_key] = YAML::Load(LexicalCast<T, std::string>()(_value));
			}
			std::stringstream ss;
			ss << YAML::Dump(node);
			return ss.str();
		}
	};


	template<class T>
	class LexicalCast<std::set<T>, std::string>
	{
	public:
		std::string operator()(const std::set<T>& v)
		{
			YAML::Node node;
			for (auto& i: v)
			{
				node.push_back(YAML::Dump(YAML::Load(LexicalCast<T, std::string>()(i))));
			}
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::set<T> >
	{
	public:
		std::set<T> operator()(const std::string& v)
		{
			YAML::Node node = YAML::Load(v);
			std::set<T> v_set;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); ++i)
			{
				ss.str("");
				ss << YAML::Dump(node[i]);
				v_set.insert(LexicalCast<std::string, T>()(ss.str()));
			}
			return v_set;
		}
	};

	template<class T>
	class LexicalCast<std::unordered_set<T>, std::string>
	{
	public:
		std::string operator()(const std::unordered_set<T>& v)
		{
			YAML::Node node;
			for (auto& i: v)
			{
				node.push_back(YAML::Dump(YAML::Load(LexicalCast<T, std::string>()(i))));
			}
			std::stringstream ss;
			ss << node;
			return ss.str();
		}
	};

	template<class T>
	class LexicalCast<std::string, std::unordered_set<T> >
	{
	public:
		std::unordered_set<T> operator()(const std::string& v)
		{
			YAML::Node node = YAML::Load(v);
			std::unordered_set<T> v_set;
			std::stringstream ss;
			for (size_t i = 0; i < node.size(); ++i)
			{
				ss.str("");
				ss << YAML::Dump(node[i]);
				v_set.insert(LexicalCast<std::string, T>()(ss.str()));
			}
			return v_set;
		}
	};

#pragma endregion LexicalCast
#pragma region ConfigVarBase
	/**
	 * @brief: 配置变量的基类
	 */
	class ConfigVarBase
	{
	public:
		typedef std::shared_ptr<ConfigVarBase> ptr;

		ConfigVarBase(std::string const& name, std::string const& description);

		virtual ~ConfigVarBase() = default;

		const std::string& getName() const;

		const std::string& getDescription() const;

		virtual std::string toString() = 0;

		virtual bool fromString(const std::string& val) = 0;

		virtual std::string getTypeName() const = 0;

	private:
		std::string m_name;// 名字
		std::string m_description;// 描述
	};
#pragma endregion ConfigVarBase

#pragma region ConfigVar
	/**
	 * @brief: 配置参数模板子类，保存对应类型的参数值，
	 * 通过仿函数实现string和T类型之间的相互转化
	 * @tparam T
	 * 需要类 或 结构体提供
	 */
	template<typename T, typename FromStr = LexicalCast<std::string, T>, typename ToStr = LexicalCast<T, std::string> >
	class ConfigVar : public ConfigVarBase
	{
	public:
		typedef RWMutex RWMutexType;
		typedef std::shared_ptr<ConfigVar> ptr;
		// 回调别名
		typedef std::function<void(const T& old_value, const T& new_value)> on_change_cb;

		ConfigVar(std::string const& name, const T& default_value, std::string const& description = "")
			: ConfigVarBase(name, description), m_val(default_value)
		{
		}

		std::string toString() override
		{
			try
			{
				RWMutexType::ReadLock lock(&m_mutex);
				return ToStr()(m_val);
			}
			catch (std::exception& e)
			{
				RAREVOYAGER_LOG_ERROR(RAREVOYAGER_LOG_ROOT()) << "ConfigVar::toString() exception"
				 << e.what() << "convert:" << typeid(m_val).name() << "to string";
			}
			return "";
		}


		bool fromString(const std::string& val) override
		{
			try
			{
				setValue(FromStr()(val));
			}
			catch (std::exception& e)
			{
				RAREVOYAGER_LOG_ERROR(RAREVOYAGER_LOG_ROOT()) << "ConfigVar::toString() exception"
				<< e.what() << "convert: to string" << typeid(m_val).name();
			}
			return false;
		}

		/**
		 * @brief: set val方法
		 * @param val
		 */
		void setValue(const T& val)
		{
			RWMutexType::ReadLock lock(&m_mutex);
			if (m_val == val)
			{
				return;
			}
			for (auto& [_,_value]: m_cbs)
			{
				// 执行 所有的回调函数
				_value(m_val, val);
			}
			// 通知变化
			RWMutexType::WriteLock write(&m_mutex);
			m_val = val;
		}

		/**
		 * @brief: 获取value 方法
		 * @return
		 */
		const T getValue()
		{
			RWMutexType::ReadLock lock(&m_mutex);
			return m_val;
		}

		std::string getTypeName() const override
		{
			return typeid(T).name();
		}

		uint64_t addListener(on_change_cb callback)
		{
			static uint64_t s_fun_id = 0;
			RWMutexType::WriteLock lock(&m_mutex);
			++s_fun_id;
			m_cbs[s_fun_id] = callback;
			return s_fun_id;
		}

		void delListener(uint64_t key)
		{
			RWMutexType::WriteLock lock(&m_mutex);
			m_cbs.erase(key);
		}

		on_change_cb getListener(uint64_t key)
		{
			RWMutexType::ReadLock lock(&m_mutex);
			auto it = m_cbs.find(key);
			return it == m_cbs.end() ? nullptr : it->second;
		}

		void clearListeners()
		{
			RWMutexType::WriteLock lock(&m_mutex);
			m_cbs.clear();
		}

	private:
		T m_val;
		/**
		 * 回调函数组，根据hash值区分
		 */
		std::map<uint64_t, on_change_cb> m_cbs;
		RWMutexType m_mutex;
	};


#pragma endregion ConfigVar

#pragma region Config
	/**
	 * @brief: ConfigVar的管理类
	 */
	class Config
	{
	public:
		typedef RWMutex RWMutexType;
		typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;

		/**
		 * @brief: 有返回没有创建
		 * @tparam T
		 * @param name
		 * @param default_value
		 * @param description
		 * @return
		 */
		template<class T>
		static typename ConfigVar<T>::ptr Lookup(const std::string& name, const T& default_value, const std::string& description = "")
		{
			RWMutexType::WriteLock lock(&GetMutex());
			auto it = GetDatas().find(name);
			if (it != GetDatas().end())
			{
				auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
				if (!tmp)
				{
					RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "Lookup name " << name << "exists but type not"
<< typeid(T).name() << "real_type" << it->second->getTypeName();
					return nullptr;
				}
				RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "Lookup name " << name << "exists";
				return tmp;
			}

			if (name.find_first_not_of(regex_str) != std::string::npos)
			{
				RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "Lookup name invalid " << name << "exists";
				throw std::invalid_argument(name);
			}

			typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
			GetDatas()[name] = v;
			return v;
		}

		/**
		 * @brief: 查找
		 * @tparam T
		 * @param name
		 * @return
		 */
		template<class T>
		static typename ConfigVar<T>::ptr Lookup(const std::string& name)
		{
			RWMutexType::WriteLock lock(&GetMutex());
			auto it = GetDatas().find(name);
			// return it == s_datas.end() ? nullptr : it->second;
			return it == GetDatas().end() ? nullptr : std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
		}

		static void LoadFromYaml(const YAML::Node& node);

		static ConfigVarBase::ptr LookupBase(const std::string& name);

	private:
		static ConfigVarMap& GetDatas()
		{
			static ConfigVarMap s_datas;
			return s_datas;
		}
		static RWMutexType& GetMutex() {
			static RWMutexType s_mutex;
			return s_mutex;
		}
	};
#pragma endregion Config


}


#endif //RAREVOYAGER_CONFIG_H