
#include "momo_server.h"
#include "momo_socket_routines.h"

namespace MomoModule
{

MomoServer::MomoServer(const char *new_name) : MomoDevice(new_name), slave(scl, sda), master(scl, sda), port_sym("port", 0, "Port Number")
{
	pthread_mutex_init(&shared_data, NULL);
	cycles = &get_cycles();

	master.set_data_source(this);

	addSymbol(&port_sym);

	init_sockets();

	ServerData *data = new ServerData;
	data->server = this;
	data->port = 0;

	pthread_t thread;
	if (pthread_create(&thread, NULL, listen_on_port, data) != 0)
	{
		printf("Could not start listening thread.\n");
		exit(1);
	}
}

MomoServer::~MomoServer()
{
	pthread_mutex_destroy(&shared_data);
	finish_sockets();
}

void MomoServer::set_port(int port)
{
	port_sym.set(port);
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