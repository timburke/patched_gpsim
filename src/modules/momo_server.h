#ifndef __momo_server_h__
#define __momo_server_h__

#define IN_MODULE

#include "momo_master_behave.h"
#include "momo_slave_behave.h"
#include "../src/gpsim_time.h"
#include <pthread.h>

#ifdef _WIN32
/* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */

typedef int SOCKET; 
#endif



namespace MomoModule
{

/*
 * MomoServer - a socket based interface that lets you send and receive MIB calls 
 * 
 * MomoServer provides an interface for external programs to subscribe to all of the
 * mib calls that are generated inside of this gpsim instance as well as injecting their
 * own MIB calls and receiving the responses.
 *
 * MomoServer can be used to run a module and test its response to various RPC calls
 * as well as to connect multiple virtual devices together, inject errors into a system,
 * prototype, etc.
 */

bool 	init_sockets();
bool 	finish_sockets();
int 	close_socket(SOCKET socket);

class MomoServer : public TriggerObject, MomoDevice, MomoDataSource
{
	private:
	pthread_mutex_t			shared_data;

	MomoSlaveBehavior 		slave;
	MomoMasterBehavior		master;

	Cycle_Counter			*cycles;

	std::vector<pthread_t> 	threads;

	public:
	MomoServer(const char *name);
	virtual ~MomoServer();

	virtual uint8_t generate_call(std::vector<uint8_t> &out_params);
	virtual void 	process_response(const std::vector<uint8_t> &response);

	virtual void callback();
	virtual void new_sda_edge(bool value);
	virtual void new_scl_edge(bool value);

	static Module *construct(const char *name);
};

};

#endif
