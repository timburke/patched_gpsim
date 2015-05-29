#ifndef __momo_server_h__
#define __momo_server_h__

#include "momo_master_behave.h"
#include "momo_slave_behave.h"
#include "../src/gpsim_time.h"
#include <glib.h>

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

class MomoServer : public TriggerObject, MomoDevice, MomoDataSource
{
	private:
	GMutex				shared_data;

	MomoSlaveBehave 	slave;
	MomoMasterBehave 	master;

	Cycle_Counter		*cycles;

	public:
	MomoServer();
	virtual ~MomoServer();

	virtual uint8_t generate_call(std::vector<uint8_t> &out_params);
	virtual void 	process_response(const std::vector<uint8_t> &response);
};

};

#endif
