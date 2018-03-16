#ifndef __WEB__H__
#define __WEB__H__

#include "http_server.h"

typedef struct web_server_context_* web_server_t;
esp_err_t web_server_start(web_server_t* out_server);
esp_err_t web_server_stop(web_server_t server);

#endif
