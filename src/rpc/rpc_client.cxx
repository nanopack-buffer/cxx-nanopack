#include <cstddef>
#include <cstdint>
#include <future>
#include <limits>
#include <nanopack/rpc.hxx>

NanoPack::RpcClient::RpcClient(NanoPack::RpcClientChannel &channel)
	: dev(), rng(dev()), dist(0, std::numeric_limits<uint32_t>::max()),
	  pending_async_requests() {
	this->channel = &channel;
}

std::future<uint8_t *>
NanoPack::RpcClient::send_request_data_async(MessageId msgId, uint8_t *data,
											 size_t size) {
	std::promise<uint8_t *> promise;
	auto future = promise.get_future();
	pending_async_requests.emplace(msgId, std::move(promise));
	channel->send_request(data, size);
	return future;
}

uint32_t NanoPack::RpcClient::new_message_id() {
	MessageId id;
	do {
		id = dist(rng);
	} while (pending_async_requests.contains(id));
	return id;
}

void NanoPack::RpcClient::response_received(uint8_t *response_data) {
	const uint32_t msgId = response_data[1] | response_data[2] << 8 |
						   response_data[3] << 16 | response_data[4] << 24;
	const auto entry = pending_async_requests.find(msgId);
	if (entry == pending_async_requests.end()) {
		return;
	}
	entry->second.set_value(response_data + 5);
	pending_async_requests.erase(msgId);
}
