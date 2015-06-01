#ifndef __momo_socket_routines_h__
#define __momo_socket_routines_h__

#include "momo_server.h"

namespace MomoModule
{

struct ServerData
{
	MomoServer 	*server;
	SOCKET 	   	socket;
	int 		port;
};

#define kServerMagicNumber 			0xBAADDAAD
#define kMaxCommandPayloadLength	256

struct ServerCommand
{
	uint32_t	magic_number;
	uint32_t 	command_id;
	uint32_t	length;

	void to_network()
	{
		magic_number = htonl(magic_number);
		command_id = htonl(command_id);
		length = htonl(length);
	}

	void to_host()
	{
		magic_number = ntohl(magic_number);
		command_id = ntohl(command_id);
		length = ntohl(length);
	}
};

enum
{
	kMonitorMIBCallsCommand = 0,
	kQuitGPSIMCommand = 1,
	kNumCommands
};

enum
{
	kNoErrorResponse = 0,
	kInvalidMagicNumberResponse = 1,
	kUnknownCommandResponse = 2,
	kPayloadTooLongResponse = 3
};

struct ServerResponse
{
	uint32_t 	magic_number;
	uint32_t	response;
	uint32_t	length;

	void to_network()
	{
		magic_number = htonl(magic_number);
		response = htonl(response);
		length = htonl(length);
	}

	void to_host()
	{
		magic_number = ntohl(magic_number);
		response = ntohl(response);
		length = ntohl(length);
	}
};

bool 	init_sockets();
bool 	finish_sockets();
int 	close_socket(SOCKET socket);

void* 	listen_on_port(void *arg);
void 	send_error(SOCKET sock, uint32_t error_code);
int 	read_socket(SOCKET sock, void *buffer, int length);
int 	write_socket(SOCKET sock, void *buffer, int length);

void* 	handle_connection(void *arg);
bool 	process_command(ServerData *data, ServerCommand &cmd, unsigned char *payload);

};

#endif
