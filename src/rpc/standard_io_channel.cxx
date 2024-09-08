#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <nanopack/rpc.hxx>
#include <thread>
#include <unistd.h>

NanoPack::StandardIoChannel::StandardIoChannel(int stdin_handle,
											   int stdout_handle)
	: stdin_handle(stdin_handle), stdout_handle(stdout_handle),
	  is_closed(true) {}

void NanoPack::StandardIoChannel::open() {
	is_closed = false;
	std::thread t(&StandardIoChannel::read_from_stdout, this);
	t.detach();
}

void NanoPack::StandardIoChannel::close() { is_closed = true; }

void NanoPack::StandardIoChannel::send_request(uint8_t *data, size_t size) {
	uint8_t msg_size_buf[sizeof(uint32_t)];
	msg_size_buf[0] = size & 0xFF;
	msg_size_buf[1] = size & 0xFF00 >> 8;
	msg_size_buf[2] = size & 0xFF0000 >> 16;
	msg_size_buf[3] = size & 0xFF000000 >> 24;

	std::lock_guard<std::mutex> guard(stdio_mutex);

	write(stdin_handle, msg_size_buf, 4);
	write(stdin_handle, data, size);
}

void NanoPack::StandardIoChannel::send_response(uint8_t *data, size_t size) {
	uint8_t msg_size_buf[sizeof(uint32_t)];
	msg_size_buf[0] = size & 0xFF;
	msg_size_buf[1] = size & 0xFF00 >> 8;
	msg_size_buf[2] = size & 0xFF0000 >> 16;
	msg_size_buf[3] = size & 0xFF000000 >> 24;

	std::lock_guard<std::mutex> guard(stdio_mutex);

	write(stdin_handle, msg_size_buf, 4);
	write(stdin_handle, data, size);
}

void NanoPack::StandardIoChannel::notify_client(uint8_t *response_data) {
	client->response_received(response_data);
}

void NanoPack::StandardIoChannel::notify_server(uint8_t *request_data) {
	server->request_received(request_data);
}

void NanoPack::StandardIoChannel::read_from_stdout() {
	uint8_t msg_size_buf[sizeof(uint32_t)];
	while (!is_closed) {
		read(stdout_handle, msg_size_buf, sizeof(uint32_t));
		uint32_t msg_size = (msg_size_buf[0]) | (msg_size_buf[1] << 8) |
							(msg_size_buf[2] << 16) | (msg_size_buf[3] << 24);

		uint8_t *msg_data = (uint8_t *)std::malloc(msg_size * sizeof(uint8_t));
		read(stdout_handle, msg_data, sizeof(uint32_t));

		switch (msg_data[0]) {
		case NanoPack::RpcMessageType::Request:
			notify_server(msg_data);
			break;

		case NanoPack::RpcMessageType::Response:
			notify_client(msg_data);
			break;

		default:
			break;
		}
	}
}
