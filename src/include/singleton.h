/*************************************************
 * 描述：模板单例模式管理器
 *
 * File：singleton.h
 * Author：Cipher
 * Date：2026/1/2-17:20
 * Update：
 * ************************************************/

#ifndef RAREVOYAGER_SINGLETON_H
#define RAREVOYAGER_SINGLETON_H
#include <memory>

namespace RareVoyager
{
	/**
	 * TODO: 重新理解下这里的单例模式
	 * @tparam X 为了创造多个实例对应的Tag
	 * @tparam N 同一个Tag创造多个实例索引
	 */
	template<typename T,class X = void,int N = 0>
	class Singleton
	{
	public:
		static T *GetInstance()
		{
			static T v;
			return &v;
		}
	};

	template<typename T,class X = void,int N = 0>
	class SingletonPtr
	{
	public:
		static std::shared_ptr<T> *GetInstance()
		{
			static std::shared_ptr<T> v(new T);
			return &v;
		}
	};
}



#endif //RAREVOYAGER_SINGLETON_H