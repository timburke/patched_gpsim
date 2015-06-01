#include "momo_socket_routines.h"
#include <pthread.h>

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

/*
 * Thread routine that listens forever on a certain port waiting for connections
 * Argument is a ServerData struct
 */
void* listen_on_port(void *arg)
{
	ServerData 			*data = (ServerData *)arg;
	SOCKET 				sock, conn;
	struct 	sockaddr_in saddr;
	int 	err;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(data->port);

	err = bind(sock, (struct sockaddr *)&saddr, sizeof(saddr));
	if (err < 0)
	{
		printf("Could not bind requested socket.\n");
		exit(1);
	}

	err = listen(sock, 10);
	if (err < 0)
	{
		printf("Could not start listening for connections.\n");
		exit(2);
	}

	//Find out what port number we're listen on
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(sock, (struct sockaddr *)&sin, &len) == -1)
	{
		printf("Could not get port number.\n");
		exit(3);
	}
	
	data->server->set_port(ntohs(sin.sin_port));

	//Listen for connections and spawn a thread to handle each one
	while (true)
	{
		conn = accept(sock, NULL, NULL);

		ServerData *new_data = new ServerData();

		new_data->server = data->server;
		new_data->socket = conn;

		pthread_t 	handler;
		pthread_create(&handler, NULL, handle_connection, new_data);
	}

	delete data;
	return NULL;
}

void send_error(SOCKET sock, uint32_t error_code)
{
	ServerResponse resp;
	resp.magic_number = kServerMagicNumber;
	resp.response = error_code;
	resp.length = 0;

	resp.to_network();

	write_socket(sock, &resp, sizeof(resp));
	close_socket(sock);
}

int read_socket(SOCKET sock, void *in_buffer, int length)
{
	int bytes_read = 0;
	char *buffer = (char *)in_buffer;

	while (bytes_read < length)
	{
		int return_status = recv(sock, buffer + length - bytes_read, length - bytes_read, MSG_WAITALL);

		if (return_status <= 0)
			return return_status;

		bytes_read += return_status;
	}

	return bytes_read;
}

int write_socket(SOCKET sock, void *in_buffer, int length)
{
	int bytes_written = 0;
	char *buffer = (char *)in_buffer;

	while (bytes_written < length)
	{
		int return_status = send(sock, buffer + bytes_written, length - bytes_written, 0);
		if (return_status <= 0)
			return return_status;

		bytes_written += return_status;
	}

	return bytes_written;
}

/*
 * All connections must begin by sending a ServerCommand packet that instructs
 * the server how to handle their connection.
 */
void* handle_connection(void *arg)
{
	ServerData 		*data = (ServerData *)arg;
	ServerCommand 	cmd;
	bool		  	stop = false;

	unsigned char	payload[kMaxCommandPayloadLength];
	
	while (!stop)
	{
		int retval = read_socket(data->socket, &cmd, sizeof(cmd));
		if (retval <= 0)
			break;

		cmd.to_host();
		
		//Make sure this is a valid packet
		if (cmd.magic_number != kServerMagicNumber)
		{
			send_error(data->socket, kInvalidMagicNumberResponse);
			break;
		}
		else if (cmd.command_id >= kNumCommands)
		{
			send_error(data->socket, kUnknownCommandResponse);
			break;
		}
		else if (cmd.length > kMaxCommandPayloadLength)
		{
			send_error(data->socket, kPayloadTooLongResponse);
			break;
		}

		retval = read_socket(data->socket, &payload, cmd.length);
		if (retval <= 0)
			break;

		//Now process the command here
		stop = process_command(data, cmd, payload);
	}

	if (stop)
		close_socket(data->socket);

	delete data;
	return NULL;
}

bool process_command(ServerData *data, ServerCommand &cmd, unsigned char *payload)
{
	switch(cmd.command_id)
	{
		default:
		send_error(data->socket, kUnknownCommandResponse);
		return true;
	}

	return false;
}

};