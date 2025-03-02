#ifndef AUTOPTR_HEAD_FILE
#define AUTOPTR_HEAD_FILE

#include <atomic>
#include <typeinfo> 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CHandleBase
{
    //变量定义
protected:
    std::atomic<int>	         m_Atomic;   
    bool                         m_bNoDelete; 

    //函数定义
protected:    
    //构造函数
    CHandleBase() : m_Atomic(0), m_bNoDelete(false)
    {
    }

    //拷贝构造
    CHandleBase(const CHandleBase&) : m_Atomic(0), m_bNoDelete(false)
    {
    }

    //析构函数
    virtual ~CHandleBase()
    {
    }

    //函数定义
public:
    //复制操作    
    CHandleBase& operator=(const CHandleBase&) 
    { 
        return *this; 
    }

    //获取计数
    int getRef() const
    {
        return m_Atomic;
    }

    //增加计数
    void incRef() 
    { 
        ++m_Atomic; 
    }

    //减少计数
    void decRef()
    {
        if((--m_Atomic) == 0 && !m_bNoDelete)
        {
            m_bNoDelete = true;
            delete this;
        }
    }    
    
    //设置不自动释放	 
    void setNoDelete(bool b)  
    { 
        m_bNoDelete = b;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//智能指针模板类
template<typename T>
class CWHAutoPtr
{
public:
    typedef T            element_type;
    T *                  m_ptr;
    
public:    

    //用原生指针初始化, 计数+1. 	 
    CWHAutoPtr(T* p = 0)
    {
        m_ptr = p;

        if(m_ptr)
        {
            m_ptr->incRef();
        }
    }

    
	//用其他智能指针r的原生指针初始化, 计数+1. 	
    template<typename Y>
    CWHAutoPtr(const CWHAutoPtr<Y>& r)
    {
        m_ptr = r.m_ptr;

        if(m_ptr)
        {
            m_ptr->incRef();
        }
    }

    //拷贝构造
    CWHAutoPtr(const CWHAutoPtr& r)
    {
        m_ptr = r.m_ptr;

        if(m_ptr)
        {
            m_ptr->incRef();
        }
    }

    //析构函数    
    ~CWHAutoPtr()
    {
        if(m_ptr)
        {
            m_ptr->decRef();
        }
    }

    //普通指针赋值 
    CWHAutoPtr& operator=(T* p)
    {
        if(m_ptr != p)
        {
            if(p)
            {
                p->incRef();
            }

            T* ptr = m_ptr;
            m_ptr = p;

            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    //其他类型智能指针赋值  	 
    template<typename Y>
    CWHAutoPtr& operator=(const CWHAutoPtr<Y>& r)
    {
        if(m_ptr != r.m_ptr)
        {
            if(r.m_ptr)
            {
                r.m_ptr->incRef();
            }

            T* ptr = m_ptr;
            m_ptr = r.m_ptr;

            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    //该类型其他执政指针赋值 	 
    CWHAutoPtr& operator=(const CWHAutoPtr& r)
    {
        if(m_ptr != r.m_ptr)
        {
            if(r.m_ptr)
            {
                r.m_ptr->incRef();
            }

            T* ptr = m_ptr;
            m_ptr = r.m_ptr;

            if(ptr)
            {
                ptr->decRef();
            }
        }
        return *this;
    }

    //将其他类型的智能指针换成当前类型的智能指针. 	 
    template<class Y>
    static CWHAutoPtr dynamicCast(const CWHAutoPtr<Y>& r)
    {
        return CWHAutoPtr(dynamic_cast<T*>(r.m_ptr));
    }

    //将其他原生类型的指针转换成当前类型的智能指针. 	 
    template<class Y>
    static CWHAutoPtr dynamicCast(Y* p)
    {
        return CWHAutoPtr(dynamic_cast<T*>(p));
    }

    //获取原生指针.     
    T* get() const
    {
        return m_ptr;
    }

    //调用     
    T* operator->() const
    {
        if(!m_ptr)
        {
            throwNullHandleException();
        }

        return m_ptr;
    }

    //引用     
    T& operator*() const
    {
        if(!m_ptr)
        {
            throwNullHandleException();
        }

        return *m_ptr;
    }

    //是否有效     
    operator bool() const
    {
        return m_ptr ? true : false;
    }

    //交换指针. 	 
    void swap(CWHAutoPtr& other)
    {
        std::swap(m_ptr, other.m_ptr);
    }

protected:

    //抛出异常    
    void throwNullHandleException() const;
};

//抛出异常
template<typename T> inline void
CWHAutoPtr<T>::throwNullHandleException() const
{
    throw TEXT("autoptr null handle error!");
}

//==判断.  
template<typename T, typename U>
inline bool operator==(const CWHAutoPtr<T>& lhs, const CWHAutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();
    
	return (l == r);
}

//不等于判断. 
template<typename T, typename U>
inline bool operator!=(const CWHAutoPtr<T>& lhs, const CWHAutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();
	
	return (l != r);
}

//小于判断, 用于放在map等容器中
template<typename T, typename U>
inline bool operator<(const CWHAutoPtr<T>& lhs, const CWHAutoPtr<U>& rhs)
{
    T* l = lhs.get();
    U* r = rhs.get();
    if(l && r)
    {
        return (l < r);
    }
    else
    {
        return !l && r;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
