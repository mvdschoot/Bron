#ifndef __SINGLETON_HEADER__
#define __SINGLETON_HEADER__

namespace Steve
{
    template<typename T>
    class Singleton {
    public:
        static T& getInstance() {
            static T instance;
            return instance;
        }

        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

    protected:
        Singleton() {}
    };
}

#endif // __SINGLETON_HEADER__