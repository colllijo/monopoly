#pragma once

#include <chrono>
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
		// auto timeout = std::chrono::seconds(16);
		std::unique_lock<std::mutex> lock(exclusion);
		// if (!conditional.wait_for(lock, timeout, [&] { return data.find(key) != data.end(); }) )
		// 	throw timeout_exception("Timeout while waiting for response");
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

	class timeout_exception : public std::exception
	{
		public:
			timeout_exception(const std::string& message) : message(message) {}

			const char* what() const noexcept override
			{
				return message.c_str();
			}

		private:
			std::string message;
	};

private:
	std::map<K, V> data;
	std::mutex exclusion;
	std::condition_variable conditional;
};
