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
#include <memory>
#include <string>

#include <include/util.h>
#include <boost/lexical_cast.hpp>
#include <include/logger/logger.h>

namespace RareVoyager
{
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

		virtual bool formString(const std::string& val) = 0;

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
	 */
	template<typename T>
	class ConfigVar : public ConfigVarBase
	{
	public:
		typedef std::shared_ptr<ConfigVar> ptr;

		ConfigVar(std::string const& name, const T& default_value, std::string const& description = "")
			: ConfigVarBase(name, description), m_val(default_value)
		{
		}

		std::string toString() override
		{
			try
			{
				// 使用boost库的lexical_cast z强制转换方法。原理是将源数据转换为std::string再转换
				return boost::lexical_cast<std::string>(m_val);
			}
			catch (std::exception& e)
			{
				RAREVOYAGER_LOG_ERROR(RAREVOYAGER_LOG_ROOT()) << "ConfigVar::toString() exception"
				 << e.what() << "convert:" << typeid(m_val).name() << "to string";
			}
			return "";
		}


		bool formString(const std::string& val) override
		{
			try
			{
				m_val = boost::lexical_cast<T>(val);
				return true;
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
			m_val = val;
		}

		/**
		 * @brief: 获取value 方法
		 * @return
		 */
		T getValue()
		{
			return m_val;
		}

	private:
		T m_val;
	};

#pragma endregion ConfigVar

#pragma region Config
	/**
	 * @brief: ConfigVar的管理类
	 */
	class Config
	{
	public:
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
			auto tmp = Lookup<T>(name);
			if (tmp)
			{
				RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "Lookup name " << name << "exists";
				return tmp;
			}
			if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") != std::string::npos)
			{
				RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "Lookup name invalid " << name << "exists";
				throw std::invalid_argument(name);
			}

			typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
			s_datas[name] = v;
			return v;
		}

		template<class T>
		static typename ConfigVar<T>::ptr Lookup(const std::string& name)
		{
			auto it = s_datas.find(name);
			// return it == s_datas.end() ? nullptr : it->second;
			return it == s_datas.end() ? nullptr : std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
		}


	private:
		static ConfigVarMap s_datas;
	};
#pragma endregion Config

}


#endif //RAREVOYAGER_CONFIG_H