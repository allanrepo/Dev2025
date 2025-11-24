#pragma once
#include <unordered_map>
#include <memory>
#include "Singleton.h"
#include <string>
#include "Dictionary.h"

namespace cache
{
	// TODO: add reload support
	// TODO:: add load if not found support, or fallback support
	template<typename T>
	class Repository : public core::Singleton<Repository<T>>
	{
		friend class core::Singleton<Repository<T>>;

	private:
		cache::Dictionary<std::string, std::unique_ptr<T>> registry;

	public:
		Repository() = default;

		bool Register(const std::string& key, std::unique_ptr<T> value)
		{
			return registry.Register(key, std::move(value));
		}

		T& Get(const std::string& key) 
		{
			return *registry.Get(key).get();
		}

		const T& Get(const std::string& key) const
		{
			return *registry.Get(key).get();
		}

		bool Has(const std::string& key) const
		{
			return registry.Has(key);
		}

		bool Unregister(const std::string& key)
		{
			return registry.Unregister(key);
		}

		void Clear()
		{
			registry.Clear();
		}
	};
}
