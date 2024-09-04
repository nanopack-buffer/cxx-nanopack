#include <cstdint>
#include <nanopack/rpc.hxx>
#include <string_view>
#include <thread>

NanoPack::RpcServer::RpcServer() { this->channel = nullptr; }

void NanoPack::RpcServer::use_channel(NanoPack::RpcServerChannel &channel) {
	this->channel = &channel;
}

void NanoPack::RpcServer::request_received(uint8_t *request_data) {
	const uint32_t msgId = request_data[1] | request_data[2] << 8 |
						   request_data[3] << 16 | request_data[4] << 24;
	const uint32_t method_name_size = request_data[5] | request_data[6] << 8 |
									  request_data[7] << 16 |
									  request_data[8] << 24;
	std::string_view method_name(reinterpret_cast<char *>(request_data + 9),
								 method_name_size);

	std::thread t(&NanoPack::RpcServer::handle_method_call, this, method_name,
				  request_data, 9 + method_name_size, msgId);
	t.detach();
}

void NanoPack::RpcServer::handle_method_call(const std::string_view &method,
											 uint8_t *request_data,
											 size_t offset, MessageId msgId) {
	auto result = on_method_call(method, request_data, offset, msgId);
	channel->send_response(result.data, result.size);
}
