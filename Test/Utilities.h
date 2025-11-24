#pragma once
#include <functional>
#include <iostream>

namespace utilities
{
	// A utility class that allows you to register a callback that will be called when the object goes out of scope.
	// This is useful for cleanup tasks that should happen automatically when the object is destroyed.
	// has option to dismiss the callback before it is called
	class OnOutOfScope
	{
    private:
        std::function<void()> callback;
        bool active;

    public:
        explicit OnOutOfScope(std::function<void()> cb)
            : callback(std::move(cb)), active(true)
        {
        }

        ~OnOutOfScope()
        {
            if (active && callback) callback();
        }

        void dismiss() 
        { 
            active = false; 
        }
	};
}
