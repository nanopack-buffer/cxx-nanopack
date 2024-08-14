#include <cstdint>
#include <nanopack/rpc.hxx>
#include <string_view>
#include <thread>

NanoPack::RpcServer::RpcServer(NanoPack::RpcServerChannel &channel)
	: channel(channel), call_handlers() {}

void NanoPack::RpcServer::on(const std::string &method, CallHandler handler) {
	call_handlers.emplace(method, handler);
}

void NanoPack::RpcServer::request_received(uint8_t *request_data) {
	const uint32_t msgId = request_data[1] | request_data[2] << 8 |
						   request_data[3] << 16 | request_data[4] << 24;
	const uint32_t method_name_size = request_data[5] | request_data[6] << 8 |
									  request_data[7] << 16 |
									  request_data[8] << 24;
	std::string_view method_name(
		reinterpret_cast<const char *>(request_data[9]), method_name_size);

	const auto entry = call_handlers.find(method_name);
	if (entry == call_handlers.end()) {
		return;
	}

	std::thread t(entry->second, request_data, 9 + method_name_size, msgId);
	t.detach();
}
