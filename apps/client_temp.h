

#ifndef CLIENT_TEMP_H
#define CLIENT_TEMP_H

/*
static void set_device_custom_property(void *data);

static int app_init(void);

static void get_temperature(oc_client_response_t *data);

static oc_discovery_flags_t discovery(const char *di, const char *uri, oc_string_array_t types,
          oc_interface_mask_t interfaces, oc_server_handle_t *server,
          void *user_data);

static void issue_requests(void);

static void signal_event_loop(void);

static void handle_signal(int signal);
*/

double request_temperature(void);

#endif
