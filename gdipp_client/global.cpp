#include "stdafx.h"
#include "global.h"

namespace gdipp
{

HMODULE h_self = NULL;
bool os_support_directwrite;
RPC_BINDING_HANDLE h_gdipp_rpc;

config_file config_file_instance(L"client.conf");
config config_instance(config_file_instance);
client_config_static client_config_instance;
gamma gamma_instance;
hook hook_instance;
mem_man mem_man_instance;
//render_config_delta_cache render_config_delta_cache_instance(config_file_instance);

}
