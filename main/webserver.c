#include <freertos/FreeRTOS.h>
#include <esp_wifi.h>
#include <sys/time.h>
#include <esp_http_server.h>
#include <freertos/task.h>
#include <esp_ota_ops.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <time.h>

#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_app_trace.h"
#include "stepper.h"
#include "config.h"

static const char *TAG = "webserver";

//#define SSID "FloatWIT_web" 


// Lets html to be loaded from seprate file
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[] asm("_binary_index_html_end");


static esp_err_t config_led()
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);
    return ESP_OK;
}


// Sends the html page when root ip is requested
esp_err_t index_get_handler(httpd_req_t *req)
{
	esp_err_t error = httpd_resp_send(req, (const char *) index_html_start, index_html_end - index_html_start);
	if (error != ESP_OK){
        ESP_LOGI(TAG, "Error %d while sending Response", error);
    } else {
        ESP_LOGI(TAG, "Response sent Successfully");
    }
    return error;
}

//----------callback functions----------//
static esp_err_t dirHIGH_handler(httpd_req_t *req)
{
	const char resp[] = "URI POST responce";
	ESP_LOGI(TAG, "Recieved dirhigh request");
	stp_set_dir(1);
	httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}

static esp_err_t dirLOW_handler(httpd_req_t *req)
{
	const char resp[] = "URI POST responce";
	ESP_LOGI(TAG, "Recieved dirlow request");
	stp_set_dir(0);
	httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;	
}

// Simple example of using query parameters
static esp_err_t led_status_handler(httpd_req_t *req)
{
	esp_err_t ret = ESP_OK;
	const char resp[] = "URI POST Response";
	char val [8] = {0};
	char buf [12] = {0};
	u_int16_t buf_len;
	buf_len = httpd_req_get_url_query_len(req);

	ESP_LOGI(TAG, "[REQ] %d", buf_len);

	if (buf_len > sizeof(buf)){
		ESP_LOGW(TAG, "query string exceds size of buffer");
		return ESP_ERR_INVALID_SIZE;
	}

	if (buf_len > 0){
		ESP_RETURN_ON_ERROR(httpd_req_get_url_query_str(req, buf, buf_len + 1), TAG, "no query str");
		ESP_LOGI(TAG, "Recieved led_status request\n[Query String] => '%s'", buf);
		ret |= httpd_query_key_value(buf, "status", val, sizeof(val));
		if (strcmp("on", val) == 0) {
			gpio_set_level(LED, 1);
		}else if (strcmp("off", val) == 0){
			gpio_set_level(LED, 0);
		}
		else{
			ret |= httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Query prams not found");
			ESP_LOGW(TAG, "Query prams not found");
		}
	}

	ESP_LOGI(TAG, "Recieved request");
	//ESP_ERROR_CHECK(gpio_set_level(LED, 0));
	ret |= httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	return ret;
}

static esp_err_t ledON_handler(httpd_req_t *req)
{
	const char resp[] = "URI POST Response";
	ESP_LOGI(TAG, "Recieved ledon request");
	ESP_ERROR_CHECK(gpio_set_level(LED, 1));
	httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}

	
static esp_err_t downloadData_handler(httpd_req_t *req)
{
	const char resp[] = "Recieved download data GET request";
	ESP_LOGI(TAG, "Recieved download data request");
	// TODO: Retrive data from sd card using wifi 
	const uint32_t data_len = 2000L;
	char c[data_len];
	for (int i = 0; i < data_len; i++) {
		c[i] = 0x68;
	}
	httpd_resp_set_type(req, "text/csv");
	httpd_resp_send(req, c, data_len);
	return ESP_OK;
}

static esp_err_t enable_handler(httpd_req_t *req)
{
	const char resp[] = "URI POST Response";
	ESP_LOGI(TAG, "Recieved enable stepper request");
	stp_enable();

	httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}

static esp_err_t disable_handler(httpd_req_t *req)
{
	const char resp[] = "URI POST Response";
	ESP_LOGI(TAG, "Recieved disable stepper request");
	stp_disable();
	httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
	return ESP_OK;
}

//---------- uri handlers ----------//
httpd_uri_t index_get_uri = {
	.uri	  = "/",
	.method   = HTTP_GET,
	.handler  = index_get_handler,
	.user_ctx = NULL
};

httpd_uri_t led_status_uri = {
	.uri	  = "/led",
	.method   = HTTP_POST,
	.handler  = led_status_handler,
	.user_ctx = NULL
};

// httpd_uri_t lefoff_uri = {
// 	.uri	  = "/ledoff",
// 	.method   = HTTP_POST,
// 	.handler  = ledOFF_handler,
// 	.user_ctx = NULL
// };

httpd_uri_t dirhigh_uri ={
	.uri	  = "/dirhigh",
	.method   = HTTP_POST,
	.handler  = dirHIGH_handler,
	.user_ctx = NULL  
};

httpd_uri_t dirlow_uri ={
	.uri	  = "/dirlow",
	.method   = HTTP_POST,
	.handler  = dirLOW_handler,
	.user_ctx = NULL  
};


httpd_uri_t downloadData_uri ={
	.uri	  = "/downloadData",
	.method   = HTTP_GET,
	.handler  = downloadData_handler,
	.user_ctx = NULL  
};

httpd_uri_t enable_uri ={
	.uri	  = "/enable",
	.method   = HTTP_POST,
	.handler  = enable_handler,
	.user_ctx = NULL  
};

httpd_uri_t disable_uri ={
	.uri	  = "/disable",
	.method   = HTTP_POST,
	.handler  = disable_handler,
	.user_ctx = NULL  
};

// httpd_uri_t url = {
// 	.uri	  = "/ledon",
// 	.method   = HTTP_GET,
// 	.handler  = ledON_handler,
// 	.user_ctx = NULL
// };

// httpd_uri_t sync_post = {
// 	.uri	  = "/sync",
// 	.method   = HTTP_POST,
// 	.handler  = sync_post_handler,
// 	.user_ctx = NULL
// };


static esp_err_t http_server_init(void)
{
	static httpd_handle_t http_server = NULL;

	httpd_config_t config = HTTPD_DEFAULT_CONFIG();



	if (httpd_start(&http_server, &config) == ESP_OK) {
		httpd_register_uri_handler(http_server, &index_get_uri);
		// httpd_register_uri_handler(http_server, &ledon_uri);
		// httpd_register_uri_handler(http_server, &lefoff_uri);
		httpd_register_uri_handler(http_server, &led_status_uri);
		httpd_register_uri_handler(http_server, &dirlow_uri);
		httpd_register_uri_handler(http_server, &dirhigh_uri);
		httpd_register_uri_handler(http_server, &disable_uri);
		httpd_register_uri_handler(http_server, &enable_uri);
		httpd_register_uri_handler(http_server, &downloadData_uri);
		
		//httpd_register_uri_handler(http_server, &sync_post);
	}

	return http_server == NULL ? ESP_FAIL : ESP_OK;
}

static esp_err_t softap_init(void)
{
	esp_err_t res = ESP_OK;

    res |= config_led();

	res |= esp_netif_init();
	res |= esp_event_loop_create_default();
	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	res |= esp_wifi_init(&cfg);

	wifi_config_t wifi_config = {
		.ap = {
			.ssid = SSID,
			.ssid_len = strlen(SSID),
			.channel = 6,
			.authmode = WIFI_AUTH_OPEN,
			.max_connection = 10
		},
	};

	res |= esp_wifi_set_mode(WIFI_MODE_AP);
	res |= esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config);
	res |= esp_wifi_start();
	return res;
}

void ws_run(void* a) {
	esp_err_t ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(ret);
	ESP_ERROR_CHECK(softap_init());
	ESP_ERROR_CHECK(http_server_init());

	// Task that inits the stepper code when the server is initialized
    // void *param = NULL; 
    // TaskHandle_t stepper_init_task = NULL;
	// xTaskCreate(stepper_init, "STEPPER", 3584, param, 1, &stepper_init_task);
	while(1) vTaskDelay(10);
}