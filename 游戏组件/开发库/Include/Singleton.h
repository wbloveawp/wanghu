#ifndef SINGLETON_HEAD__FILE
#define SINGLETON_HEAD__FILE

#include <mutex>
#include <atomic>
#include <cassert>
#include <cstdlib>
      
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//定义对象创建策略 
template<typename T>
class CreateUsingNew
{
public:
    static T* create() 
    { 
        return new T; 
    }

    static void destroy(T *t) 
    { 
        delete t; 
    }
};

template<typename T>
class CreateUsingNew1
{
public:
    static T* create() 
    { 
        return new T; 
    }

    static void destroy(T *t) 
    { 
        delete t; 
    }
};

template<typename T>
class CreateStatic
{
public:  
    union MaxAlign 
    { 
        char t_[sizeof(T)]; 
        long double longDouble_; 
    }; 

    static T* create() 
    { 
        static MaxAlign t; 
        return new(&t) T; 
    }

    static void destroy(T *t) 
    {
        t->~T();
    }
};

template<typename T>
class CreateRealStatic
{
public:
    static T* create()
    {
        static T t;
        return &t;
    }

    static void destroy(T *t)
    {
    }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//定义对象的声明周期管理
template<typename T>
class DefaultLifetime
{
public:
    static void deadReference()
    {
        throw std::logic_error("singleton object has dead.");
    }

    static void scheduleDestruction(T*, void (*pFun)())
    {
        std::atexit(pFun);
    }
};

//对象被销毁后可以重生(全局任何时候都需要)
template<typename T>
class PhoneixLifetime
{
private:
    static bool m_bDestroyedOnce;

public:
    static void deadReference()
    {
        m_bDestroyedOnce = true;
    }

    static void scheduleDestruction(T*, void (*pFun)())
    {
        if(!m_bDestroyedOnce)
            std::atexit(pFun);
    } 
};

template <class T> 
bool PhoneixLifetime<T>::m_bDestroyedOnce = false; 

template <typename T>
class NoDestroyLifetime
{
    static void scheduleDestruction(T*, void (*)())
    {
    } 

    static void deadReference() 
    {
    } 
}; 

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//单例类型
template
<
    typename T,
    template<typename> class CreatePolicy   = CreateUsingNew,
    template<typename> class LifetimePolicy = DefaultLifetime
>
class CSingleton 
{
    typedef T  instance_type;
    typedef volatile T volatile_type; 
    typedef CreatePolicy<T> TCreatePolicy;

protected:
    static std::atomic<T*>       m_pInstance;
    static bool             m_bDestroyed;

public:
    // 获取实例
    static T *getInstance()
    {
        static std::mutex mutex_singleton;

        auto sin= m_pInstance.load();
        if ( !sin ){
            std::lock_guard<std::mutex> myLock(mutex_singleton);
            sin= m_pInstance.load();
            if( !sin ){
                if(m_bDestroyed)
                {
                    LifetimePolicy<T>::deadReference();
                    m_bDestroyed = false;
                }

                sin = CreatePolicy<T>::create();
                m_pInstance.store(sin);
                LifetimePolicy<T>::scheduleDestruction(m_pInstance, &destroySingleton);
            }
        }

        return sin;
    }

    virtual ~CSingleton(){};

protected:
    //销毁单例
    static void destroySingleton()
    {
        assert(!m_bDestroyed);
        CreatePolicy<T>::destroy((T*)m_pInstance);
        m_pInstance = NULL;
        m_bDestroyed = true;
    }

    //函数定义
protected:
    CSingleton() = default;
    CSingleton (const CSingleton &) = default;
    CSingleton &operator=(const CSingleton &) = default;
};

template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy>
bool CSingleton<T, CreatePolicy, LifetimePolicy>::m_bDestroyed = false;

template <class T, template<class> class CreatePolicy, template<class> class LifetimePolicy>
std::atomic<T*> CSingleton<T, CreatePolicy, LifetimePolicy>::m_pInstance = {nullptr};

#endif
