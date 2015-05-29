
#include "momo_server.h"

namespace MomoModule
{

bool init_sockets()
{
	#ifdef _WIN32
	WSADATA wsa_data;
	return WSAStartup(MAKEWORD(1,1), &wsa_data) == 0;
	#else
	return true;
  	#endif
}

bool finish_sockets()
{
	#ifdef _WIN32
	return WSACleanup() == 0;
	#else
	return true;
	#endif
}

int close_socket(SOCKET sock)
{
	int status = 0;

	#ifdef _WIN32
	status = shutdown(sock, SD_BOTH);
	if (status == 0) { status = closesocket(sock); }
	#else
	status = shutdown(sock, SHUT_RDWR);
	if (status == 0) { status = close(sock); }
	#endif

	return status;
}

MomoServer::MomoServer(const char *new_name) : MomoDevice(new_name), slave(scl, sda), master(scl, sda)
{
	pthread_mutex_init(&shared_data, NULL);
	cycles = &get_cycles();

	master.set_data_source(this);

	init_sockets();
}

MomoServer::~MomoServer()
{
	pthread_mutex_destroy(&shared_data);
	finish_sockets();
}

Module *MomoServer::construct(const char *name)
{
	return new MomoServer(name);
}

uint8_t MomoServer::generate_call(std::vector<uint8_t> &out_params)
{
	return 0;
}

void MomoServer::process_response(const std::vector<uint8_t> &response)
{

}


void MomoServer::callback()
{

}
	
void MomoServer::new_sda_edge(bool value)
{

}

void MomoServer::new_scl_edge(bool value)
{

}

};