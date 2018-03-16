#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/lock.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "http_server.h"
#include "web_server.h"


struct web_server_context_ {
    http_server_t server;
    EventGroupHandle_t event_group;
};


static void uri_api_hello(http_context_t http_ctx, void* arg) 
{
    http_response_begin(http_ctx, 200, "text/plain", HTTP_RESPONSE_SIZE_UNKNOWN);
    const http_buffer_t buf = {
            .data = "Hello World!",
            .data_is_persistent = true
    };
    http_response_write(http_ctx, &buf);
    http_response_end(http_ctx);
}


esp_err_t web_server_start(web_server_t* out_server)
{
    web_server_t ctx = calloc(1, sizeof(*ctx));
    if (ctx == NULL) {
        return ESP_ERR_NO_MEM;
    }
    
    http_server_options_t options = HTTP_SERVER_OPTIONS_DEFAULT();

    esp_err_t err = http_server_start(&options, &(ctx->server));
    if (err != ESP_OK) {
        return err;
    }

    // TODO: register handlers
    http_register_handler(ctx->server, "/api/hello", HTTP_GET, 
            HTTP_HANDLE_RESPONSE, &uri_api_hello, NULL);

    *out_server = ctx;
    return ESP_OK;
}


esp_err_t web_server_stop(web_server_t ctx)
{
    http_server_stop(ctx->server);
    free(ctx);
    return ESP_OK;    
}


