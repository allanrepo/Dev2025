#pragma once
#include "Singleton.h"
#include <functional>
#include "Logger.h"

// raw pointer is best choice here. this class has no ownership of the objects it will point to, meaning it will never set/get or call its methods.
// so even if the object it points to is destroyed, this class won't care. 
// using smart pointers is unnecessary and will just add overhead from reference counting

namespace cache
{
    template<typename T>
    class ContextCache : public core::Singleton<ContextCache<T>>
    {
    private:
        friend class core::Singleton<ContextCache<T>>;

        T* m_state = nullptr;

    public:
        // Returns true if newState is different from cached state or force is true
        bool CanBind(T* state, bool force = false) const
        {
            return force || (state != m_state);
        }

        // Binds newState if needed, using provided binder
        void Bind(T* state, std::function<void(T*)> binder, bool force = false)
        {
            if (CanBind(state, force))
            {
                binder(state);
                m_state = state;
            }
        }

        void Reset()
        {
            m_state = nullptr;
        }

        void Bind(T* state, bool force = false)
        {
            if (CanBind(state, force))
            {
                m_state = state;
            }
        }

        T* Get()
        {
            return m_state;
        }

    };


}
