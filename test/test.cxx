#include "nanopack/reader.hxx"
#include "nanopack/writer.hxx"
#include <cstddef>
#include <cstdint>
#include <future>
#include <iostream>
#include <nanopack/rpc.hxx>
#include <string_view>
#include <unordered_map>

class TestRpcServer : public NanoPack::RpcServer {
	std::unordered_map<std::string_view,
					   MethodCallResult (TestRpcServer::*)(uint8_t *, size_t,
														   NanoPack::MessageId)>
		handlers;

	MethodCallResult on_method_call(const std::string_view &method,
									uint8_t *request_data, size_t offset,
									NanoPack::MessageId msg_id) override {
		const auto handler = handlers.find(method);
		if (handler == handlers.end()) {
			throw "";
		}
		return (this->*(handler->second))(request_data, offset, msg_id);
	}

	MethodCallResult add(uint8_t *request_data, size_t offset,
						 NanoPack::MessageId msg_id) {
		NanoPack::Reader reader(request_data);
		size_t ptr = offset;
		int32_t a;
		reader.read_int32(ptr, a);
		ptr += 4;
		int32_t b;
		reader.read_int32(ptr, b);
		ptr += 4;
		int32_t result = a + b;
		NanoPack::Writer writer(10);
		writer.append_uint8(2);
		writer.append_uint32(msg_id);
		writer.append_uint8(0);
		writer.append_int32(result);
		return {writer.into_data(), writer.size()};
	}

  public:
	TestRpcServer(NanoPack::RpcServerChannel &channel)
		: NanoPack::RpcServer(channel), handlers(1) {
		handlers.emplace("add", &TestRpcServer::add);
	}
};

class TestRpcClient : public NanoPack::RpcClient {
  public:
	using NanoPack::RpcClient::RpcClient;

	std::future<int32_t> add(int32_t a, int32_t b) {
		NanoPack::Writer writer(17);
		const auto msg_id = new_message_id();
		writer.append_uint8(1);
		writer.append_uint32(msg_id);
		writer.append_uint32(3);
		writer.append_string_view("add");
		writer.append_int32(a);
		writer.append_int32(b);
		return std::async(
			[this](uint32_t msg_id, uint8_t *req_data, size_t req_size) {
				auto res_data =
					send_request_data_async(msg_id, req_data, req_size).get();

				NanoPack::Reader reader(res_data);
				size_t ptr = 0;
				uint8_t err_flag;
				reader.read_uint8(ptr++, err_flag);
				if (err_flag == 1) {
					throw "";
				}

				int32_t result;
				reader.read_int32(ptr, result);

				return result;
			},
			msg_id, writer.into_data(), writer.size());
	}
};

int main() {
	NanoPack::InMemoryChannel channel;
	TestRpcClient client(channel);
	TestRpcServer server(channel);
	channel.bind_to_client(client);
	channel.bind_to_server(server);

	std::cout << client.add(1, 2).get() << std::endl;
	;
}
