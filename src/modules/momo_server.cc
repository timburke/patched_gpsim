
#include "momo_server.h"

namespace MomoModule
{

MomoServer::MomoServer()
{
	g_mutex_init(&shared_data);
	cycles = &get_cycles();

	master.set_data_source(this);
}

MomoServer::~MomoServer()
{
	g_mutex_clear(&shared_data);
}

};