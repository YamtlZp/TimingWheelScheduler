#ifndef __Singleton_inl
#define __Singleton_inl 1

#include <atomic>

/** 饿汉模式
*/
#define SINGLETON_DEFINE(TypeName)              \
static TypeName* GetInstance()                  \
{                                               \
    static TypeName type_instance;              \
    return &type_instance;                      \
}                                               \
                                                \
TypeName(const TypeName&) = delete;             \
TypeName& operator=(const TypeName&) = delete

/** 单例对象实现
*/
template<class CLASS_TYPE, bool CREATE_ON_CONSTRUCT = true>
class Singleton final
{
public:

    Singleton()
    {
        if (CREATE_ON_CONSTRUCT)
        {
            CreateInstanceifNotExist();
        }
    }

    ~Singleton()
    {
        DestroyInstanceIfExist();
    }

    CLASS_TYPE* GetClassPtr() const
    {
        return m_pClassPtr;
    }

    CLASS_TYPE* operator -> () const
    {
        CreateInstanceifNotExist();
        return m_pClassPtr;
    }

    void CreateInstanceifNotExist()
    {
        if (m_pClassPtr == nullptr)
        {
            CLASS_TYPE* objPtr = (CLASS_TYPE*)malloc(sizeof(CLASS_TYPE));
            CLASS_TYPE* nullPtr = nullptr;
            memset(objPtr, 0, sizeof(CLASS_TYPE));
            if (!m_pClassPtr.compare_exchange_strong(nullPtr, objPtr))
            {
                free(objPtr);
                objPtr = nullptr;
            }
            else
            {
                new (objPtr) CLASS_TYPE();
            }
        }
    }

    void DestroyInstanceIfExist()
    {
        CLASS_TYPE* objPtr = m_pClassPtr;
        CLASS_TYPE* nullPtr = nullptr;
        if (objPtr != nullptr)
        {
            if (m_pClassPtr.compare_exchange_strong(objPtr, nullPtr))
            {
                delete objPtr;
                objPtr = nullptr;
            }
        }
    }

private:
    std::atomic<CLASS_TYPE*> m_pClassPtr = nullptr;
};

#define DECLARE_SINGLETON(CLASS_TYPE) \
    private: \
        friend class Singleton<CLASS_TYPE, false>; \
    public: \
        static CLASS_TYPE& GetInstance(); \
        static Singleton<CLASS_TYPE, false> s_instance;

#define IMPL_SINGLETON(CLASS_TYPE) \
     Singleton<CLASS_TYPE, false> CLASS_TYPE::s_instance; \
     CLASS_TYPE& CLASS_TYPE::GetInstance() \
     { \
         s_instance.CreateInstanceifNotExist(); \
         return *s_instance.GetClassPtr(); \
     }

#endif //__Singleton_inl