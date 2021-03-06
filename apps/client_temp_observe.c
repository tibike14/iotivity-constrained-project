/*
// Copyright (c) 2016 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

/*  
Temperature Sensor client: OBSERVING TEMPERATURE

*/


#include "oc_api.h"
#include "port/oc_clock.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>

static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
static int quit = 0;

static void
set_device_custom_property(void *data)
{
  (void)data;
  oc_set_custom_device_property(purpose, "TemperatureObserver");
}

static int
app_init(void)
{
	PRINT("app init.. \n");
  int ret = oc_init_platform("LG", NULL, NULL);
  ret |= oc_add_device("/oic/tempobserverdevice1", "oic.d.phone", "Temperature Observer", "1.0", "1.0",
                       set_device_custom_property, NULL);
  return ret;
}

#define MAX_URI_LENGTH (30)
static char temperature[MAX_URI_LENGTH];
static oc_server_handle_t temperature_server;
static bool temp_state;
static int temp_value;

static oc_string_t name;



static oc_event_callback_retval_t
stop_observe(void *data)
{
  (void)data;
  PRINT("Stopping OBSERVE\n");
  oc_stop_observe(temperature, &temperature_server);
  return DONE;
}


static void
observe_temperature(oc_client_response_t *data)
{
	int i;
  PRINT("OBSERVE_temperature:\n");
  oc_rep_t *rep = data->payload;
  while (rep != NULL) {
    PRINT("key %s, value ", oc_string(rep->name));
    switch (rep->type) {
    case BOOL:
      	PRINT("%d\n", rep->value.boolean);
      	temp_state = rep->value.boolean;
      	break;
	case DOUBLE: 
		PRINT("%.1f\n", rep->value.double_p);
		temp_value = rep->value.double_p;
		break;
	case STRING:
     	PRINT("%s\n", oc_string(rep->value.string));
   	 	if (oc_string_len(name))
        	oc_free_string(&name);
      	oc_new_string(&name, oc_string(rep->value.string), oc_string_len(rep->value.string));
      	break;
	case INT_ARRAY:{
		int *arr = oc_int_array(rep->value.array);
      		for (i = 0; i < (int)oc_int_array_size(rep->value.array); i++) {       			
				PRINT("(%d)", arr[i]);
      		}
      		PRINT("\n");
		}
		break;
    default:
     	 break;
    }
    rep = rep->next;
  }
}	//end of observe_temperature function

static oc_discovery_flags_t
discovery(const char *di, const char *uri, oc_string_array_t types,
          oc_interface_mask_t interfaces, oc_server_handle_t *server,
          void *user_data)
{
  (void)di;
  (void)interfaces;
  (void)user_data;
	PRINT("discovery start\n");
  int i;
  int uri_len = strlen(uri);
  uri_len = (uri_len >= MAX_URI_LENGTH) ? MAX_URI_LENGTH - 1 : uri_len;

  for (i = 0; i < (int)oc_string_array_get_allocated_size(types); i++) {
    char *t = oc_string_array_get_item(types, i);
    if (strlen(t) == 17 && strncmp(t, "oic.r.temperature", 17) == 0) {
      memcpy(&temperature_server, server, sizeof(oc_server_handle_t));

      strncpy(temperature, uri, uri_len);
      temperature[uri_len] = '\0';
      oc_do_observe(temperature, &temperature_server, NULL, &observe_temperature, LOW_QOS,
                    NULL);
      oc_set_delayed_callback(NULL, &stop_observe, 30);
      return OC_STOP_DISCOVERY;
    }
  }
  return OC_CONTINUE_DISCOVERY;
}	//end of discovery function

static void
issue_requests(void)
{
	PRINT("issue request..\n");
  oc_do_ip_discovery("oic.r.temperature", &discovery, NULL);
}

static void
signal_event_loop(void)
{
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&mutex);
}

static void
handle_signal(int signal)
{
  (void)signal;
  signal_event_loop();
  quit = 1;
}

int
main(void)
{
  int init;
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  sigaction(SIGINT, &sa, NULL);

  static const oc_handler_t handler = {.init = app_init,
                                       .signal_event_loop = signal_event_loop,
                                       .requests_entry = issue_requests };

  oc_clock_time_t next_event;

#ifdef OC_SECURITY
  oc_storage_config("./creds");
#endif /* OC_SECURITY */

  init = oc_main_init(&handler);
  if (init < 0)
    return init;

  while (quit != 1) {
    next_event = oc_main_poll();
    pthread_mutex_lock(&mutex);
    if (next_event == 0) {
      pthread_cond_wait(&cv, &mutex);
    } else {
      ts.tv_sec = (next_event / OC_CLOCK_SECOND);
      ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
      pthread_cond_timedwait(&cv, &mutex, &ts);
    }
    pthread_mutex_unlock(&mutex);
  }

  oc_main_shutdown();
  return 0;
}	//END OF MAIN




