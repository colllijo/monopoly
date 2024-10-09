#pragma once

#include <event2/event.h>

#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class AsyncEventQueue
{
public:
	using EventBasePtr = std::unique_ptr<struct event_base, std::function<void(struct event_base*)>>;
	using EventPtr = std::unique_ptr<struct event, std::function<void(struct event*)>>;

	AsyncEventQueue(const std::function<void(const T&)>& callback)
		: base(event_base_new(), event_base_free),
		event(event_new(base.get(), -1, EV_PERSIST, onEvent, this), event_free),
		thread([this] { event_base_dispatch(base.get()); }),
		callback(callback)
	{
		if (!base) throw std::runtime_error("Failed to create event base");

		timeval tv = {0, 1000};
		event_add(event.get(), &tv);
	}

	void push(const T& data)
	{
		std::lock_guard<std::mutex> lock(mutex);
		queue.push(data);
	}

private:
	EventBasePtr base;
	EventPtr event;

	std::thread thread;
	std::mutex mutex;
	std::queue<T> queue;

	std::function<void(const T&)> callback;

	static void onEvent(evutil_socket_t, short, void* arg)
	{
		auto *service = static_cast<AsyncEventQueue<T> *>(arg);

		std::unique_lock<std::mutex> lock(service->mutex);
		if (service->queue.empty()) return;

		T data = service->queue.front();
		service->queue.pop();
		lock.unlock();

		service->callback(data);
	}
};
