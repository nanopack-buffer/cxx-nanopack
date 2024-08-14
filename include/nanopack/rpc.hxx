#ifndef NANOPACK__RPC_HXX
#define NANOPACK__RPC_HXX

#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <mutex>
#include <random>
#include <unordered_map>

namespace NanoPack {

using MessageId = uint32_t;

enum RpcMessageType { REQUEST = 0, RESPONSE = 1 };

class RpcClient;
class RpcServer;

class RpcClientChannel {
  protected:
	RpcClient *client;

  public:
	void bind_to_client(RpcClient &client);

	virtual void send_request(uint8_t *data, size_t size) = 0;

	virtual void notify_client(uint8_t *response_data) = 0;
};

class RpcServerChannel {
  protected:
	RpcServer *server;

  public:
	void bind_to_server(RpcServer &server);

	virtual void send_response(uint8_t *data, size_t size) = 0;

	virtual void notify_server(uint8_t *request_data) = 0;
};

class StandardIoChannel : public RpcClientChannel, public RpcServerChannel {
	int stdin_handle;
	int stdout_handle;
	bool is_closed;

  public:
	StandardIoChannel(int stdin_handle, int stdout_handle);

	void open();

	void close();

	void send_request(uint8_t *data, size_t size) override;

	void send_response(uint8_t *data, size_t size) override;

	void notify_client(uint8_t *response_data) override;

	void notify_server(uint8_t *request_data) override;

  private:
	void read_from_stdout();
};

class InMemoryChannel : public RpcClientChannel, public RpcServerChannel {
  public:
	InMemoryChannel();

	void send_request(uint8_t *data, size_t size) override;

	void send_response(uint8_t *data, size_t size) override;

	void notify_client(uint8_t *response_data) override;

	void notify_server(uint8_t *request_data) override;
};

class RpcClient {
	std::random_device dev;
	std::mt19937 rng;
	std::uniform_int_distribution<std::mt19937::result_type> dist;

	RpcClientChannel &channel;
	std::unordered_map<MessageId, std::promise<uint8_t *>>
		pending_async_requests;

  public:
	RpcClient(RpcClientChannel &channel);

	std::future<uint8_t *> send_request_data_async(MessageId msgId,
												   uint8_t *data, size_t size);

	uint32_t new_message_id();

	void response_received(uint8_t *response_data);
};

class RpcServer {
	struct MethodNameHash {
		using hash_type = std::hash<std::string_view>;
		using is_transparent = void;

		std::size_t operator()(const char *str) const {
			return hash_type{}(str);
		}
		std::size_t operator()(std::string_view str) const {
			return hash_type{}(str);
		}
		std::size_t operator()(std::string const &str) const {
			return hash_type{}(str);
		}
	};

	using CallHandler = void (*)(uint8_t *request_data, size_t offset,
								 MessageId msgId);

	RpcServerChannel &channel;
	std::unordered_map<std::string, CallHandler, MethodNameHash,
					   std::equal_to<>>
		call_handlers;

  public:
	RpcServer(RpcServerChannel &channel);

	void on(const std::string &method, CallHandler handler);

	void request_received(uint8_t *request_data);
};

} // namespace NanoPack

#endif
