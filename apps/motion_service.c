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
MOTION SENSOR (service) 

*/



#include "oc_api.h"
#include "port/oc_clock.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>

#include "serial_read_motion.h"

static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
static int quit = 0;
bool motion_state;
oc_string_t name;

static void
set_device_custom_property(void *data)
{
  (void)data;
  oc_set_custom_device_property(purpose, "MotionSensor");
}

static int
app_init(void)
{
  int ret = oc_init_platform("MS_platform", NULL, NULL);
  ret |= oc_add_device("/oic/motionsensor1", "oic.d.motionsensor", "Room1 MotionSensor1", "1.0", "1.0",
                       set_device_custom_property, NULL);
	
	oc_new_string(&name, "Room1_MotionSensor1", 19);	//This is what sent out as UniqueID of device?!

  return ret;
}



static void
get_motion(oc_request_t *request, oc_interface_mask_t interface, void *user_data)
{
	int startup = 1;
  (void)user_data;

  //motion_state = motion_read();
  //PRINT("Mot_state: %d\n", motion_state);

  oc_rep_start_root_object();
  switch (interface) {
  case OC_IF_BASELINE:
    oc_process_baseline_interface(request->resource);
  case OC_IF_RW:
	oc_rep_set_boolean(root, motion_state, motion_state = motion_read());
	oc_rep_set_text_string(root, name, oc_string(name));
    break;
  default:
    break;
  }
  oc_rep_end_root_object();
  oc_send_response(request, OC_STATUS_OK);
  PRINT("MotionSensor state %d\n", motion_state);
}


static void
register_resources(void)
{
  oc_resource_t *res = oc_new_resource("/motion/1", 1, 0);
  oc_resource_bind_resource_type(res, "oic.r.sensor.motion");
  oc_resource_bind_resource_interface(res, OC_IF_RW);
  oc_resource_set_default_interface(res, OC_IF_RW);
  oc_resource_set_discoverable(res, true);
  oc_resource_set_periodic_observable(res, 1);
  oc_resource_set_request_handler(res, OC_GET, get_motion, NULL);
  oc_add_resource(res);
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
                                       .register_resources = register_resources };

  oc_clock_time_t next_event;

#ifdef OC_SECURITY
  oc_storage_config("./creds");
#endif /* OC_SECURITY */

  init = oc_main_init(&handler);
  if (init < 0)
    return init;
  PRINT("1\n");

  while (quit != 1) {

    next_event = oc_main_poll();
    PRINT("2\n");
    pthread_mutex_lock(&mutex);
    if (next_event == 0) {
    	 PRINT("3\n");
      pthread_cond_wait(&cv, &mutex);
    } else {
    	 PRINT("4\n");
      ts.tv_sec = (next_event / OC_CLOCK_SECOND);
      ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
      pthread_cond_timedwait(&cv, &mutex, &ts);
    }
    PRINT("5\n");
    pthread_mutex_unlock(&mutex);
  }
    PRINT("6\n");
  oc_main_shutdown();
  return 0;
}











