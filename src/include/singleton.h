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