#pragma once

#include <condition_variable>
#include <map>
#include <mutex>
#include <optional>

template <typename K, typename V>
class ThreadSafeMap
{
public:
	void put(const K& key, const V& value)
	{
		std::lock_guard<std::mutex> lock(exclusion);
		data[key] = std::move(value);
		conditional.notify_all();
	}

	std::optional<V> get(const K& key)
	{
		std::unique_lock<std::mutex> lock(exclusion);
		conditional.wait(lock, [&] { return data.find(key) != data.end(); });

		auto it = data.find(key);
		if (it != data.end())
		{
			V value = std::move(it->second);
			data.erase(it);
			return value;
		}

		return std::nullopt;
	}

private:
	std::map<K, V> data;
	std::mutex exclusion;
	std::condition_variable conditional;
};
