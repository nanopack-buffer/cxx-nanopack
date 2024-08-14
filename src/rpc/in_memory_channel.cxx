#include <cstddef>
#include <cstdint>
#include <nanopack/rpc.hxx>

NanoPack::InMemoryChannel::InMemoryChannel() {}

void NanoPack::InMemoryChannel::send_request(uint8_t *data, size_t size) {
	notify_server(data);
}

void NanoPack::InMemoryChannel::send_response(uint8_t *data, size_t size) {
	notify_client(data);
}

void NanoPack::InMemoryChannel::notify_client(uint8_t *response_data) {
	client->response_received(response_data);
}

void NanoPack::InMemoryChannel::notify_server(uint8_t *response_data) {
	server->request_received(response_data);
}
