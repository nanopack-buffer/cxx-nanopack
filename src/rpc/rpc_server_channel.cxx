#include <nanopack/rpc.hxx>

void NanoPack::RpcServerChannel::bind_to_server(NanoPack::RpcServer &server) {
	this->server = &server;
}
