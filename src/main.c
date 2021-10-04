
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_spi_flash.h"

#include "driver/gpio.h"
#include "sdkconfig.h"

#include "esp_http_server.h"

#include "LED.h"
#include "WiFi.h"
#include "index_page_html.h"

enum Control_Vars
{
    LED_STATE = 1,
};

typedef enum Control_Vars Control_Vars_t;

extern LED_Control_Vals_t gLED_BUILTIN_STATE;
extern unsigned int gLED_BUILTIN_PERIOD;

esp_err_t index_uri_handler(httpd_req_t *req)
{
    printf("Content length: ");
    printf("%d \n", req->content_len);

    printf("Method: ");
    printf("%d \n", req->method);

    printf("URI: ");
    printf("%s \n", req->uri);

    int buf_len = sizeof(index_page_html);
    printf("Buffer length: %d \n", buf_len);

    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
    httpd_resp_send(req, index_page_html, buf_len);

    return ESP_OK;
}

esp_err_t apply_control_variable(char *control_variable, char *control_value)
{
    if (control_variable == NULL || control_value == NULL)
        return ESP_ERR_INVALID_ARG;

    Control_Vars_t control_var = atoi(control_variable);
    LED_Control_Vals_t control_val = atoi(control_value);

    switch (control_var)
    {
    case LED_STATE:
        switch (control_val)
        {
        case OFF:
            return LED_controller(OFF, 0);
            break;
        case ON:
            return LED_controller(ON, 0);
        case BLINK:
            return LED_controller(0, LED_BLINK_DEFAULT_PERIOD);

        default:
            return ESP_ERR_NOT_SUPPORTED;
            break;
        }
        break;

    default:
        return ESP_ERR_NOT_SUPPORTED;
        break;
    }

    return ESP_OK;
}
esp_err_t control_uri_handler(httpd_req_t *req)
{
    char control_variable[40];
    char control_value[20];

    printf("\nControl_handler:\n");
    printf("Content length: %d \n", req->content_len);
    printf("Method: %d \n", req->method);
    printf("URI: %s \n", req->uri);

    // Get the query
    int query_len = httpd_req_get_url_query_len(req);
    char *query_buf = malloc(query_len + 1); // Dynamic array buf
    if (query_buf == NULL)
        return ESP_ERR_NO_MEM;
    ESP_ERROR_CHECK(httpd_req_get_url_query_str(req, query_buf, query_len + 1));

    // Extract the control variable
    ESP_ERROR_CHECK(httpd_query_key_value(query_buf, "variable",
                                          control_variable, sizeof(control_variable)));

    // Extract the control value
    ESP_ERROR_CHECK(httpd_query_key_value(query_buf, "value",
                                          control_value, sizeof(control_value)));

    printf("Query: %s,   Length: %d \n", query_buf, query_len);
    printf("Control variable: %s \n", control_variable);
    printf("control_value: %s \n", control_value);

    ESP_ERROR_CHECK(apply_control_variable(control_variable, control_value));

    httpd_resp_send(req, "OK", 2);

    return ESP_OK;
}

esp_err_t status_uri_handler(httpd_req_t *req)
{
    printf("\nStatus_handler:\n");

    printf("Content length: ");
    printf("%d \n", req->content_len);

    printf("Method: ");
    printf("%d \n", req->method);

    printf("URI: ");
    printf("%s \n", req->uri);

    char LED_STATE[4] = "";
    itoa(gLED_BUILTIN_STATE, LED_STATE, 10);

    char json_buf[100] = "{";
    strcat(strcat(strcat(json_buf, "\"LED_STATE\": \""), LED_STATE), "\"}");
    int buf_len = strlen(json_buf);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_buf, buf_len);

    return ESP_OK;
}

httpd_uri_t control_uri_get = {
    .method = HTTP_GET,
    .uri = "/control",
    .handler = &control_uri_handler,
    .user_ctx = NULL};

httpd_uri_t status_uri_get = {
    .method = HTTP_GET,
    .uri = "/status",
    .handler = &status_uri_handler,
    .user_ctx = NULL};

httpd_uri_t index_uri = {
    .method = HTTP_GET,
    .uri = "/index",
    .handler = &index_uri_handler,
    .user_ctx = NULL};

httpd_uri_t empty_uri = {
    .method = HTTP_GET,
    .uri = "/",
    .handler = &index_uri_handler,
    .user_ctx = NULL};

httpd_handle_t start_webserver()
{
    // Default configuration
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // Empty handle for the HTTP server
    httpd_handle_t server = NULL;

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &empty_uri);
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &status_uri_get);
        httpd_register_uri_handler(server, &control_uri_get);
    }

    return server;
}

void app_main()
{
    printf("Hello world!\n");
    io_init();

    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_init_sta();
    printf("\nWifi initializations complete. \n\n");

    printf("Starting web server ...  \n");
    server = start_webserver();
    printf("Web server started successfully.  \n");

    while (1)
    {
        // While loop should not be empty, it will keep running starving other tasks
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // fflush(stdout);
    }

    // esp_restart();
}
