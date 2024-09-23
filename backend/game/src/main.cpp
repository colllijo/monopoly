#include <iostream>
#include <string>
#include <event2/event.h>
#include <amqpcpp.h>
#include <amqpcpp/libevent.h>

int main(int argc, char *argv[]) {
	struct event_base *event_base = event_base_new();

	AMQP::LibEventHandler handler(event_base);
	AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://localhost/"));
	AMQP::TcpChannel channel(&connection);

	channel.declareQueue("test_queue");
	channel.consume("test_queue", AMQP::noack).onReceived([](const AMQP::Message &message, uint64_t deliveryTag, bool redelivered) {
		std::cout << "Received message: " << std::string(message.body(), message.bodySize()) << std::endl;
	});

	event_base_dispatch(event_base);
	event_base_free(event_base);

	std::cout << "Program end." << std::endl;

	return 0;
}
