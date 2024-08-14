#include "nanopack/rpc.hxx"

void NanoPack::RpcClientChannel::bind_to_client(NanoPack::RpcClient &client) {
	this->client = &client;
}
