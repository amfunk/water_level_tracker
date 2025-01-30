#include <stdio.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/aon_timer.h"

#define DEBUG 1

#ifdef DEBUG
#define DPRINT printf
#else
#define DPRINT
#endif

#define THRESHOLD 1000
#define MAX_FILL_TIME 60

#define ADC 26
#define VALVE 1
#define BUTTON 2

bool is_filling = false;
bool is_set_thresh = false;
uint32_t lower = 1000;
uint32_t upper = 10000;

// Producer
void read_water_level ()
{
  DPRINT("Entering read_water_level function on secondary core.\n");

  uint32_t data = 0;
  while (1) {
    DPRINT("Reading data from ADC...\n");
    data = adc_read();
    if (multicore_fifo_wready()) {
      DPRINT("Adding data to FIFO: %u\n\n",data);
      multicore_fifo_push_blocking(data);
    } else {
      DPRINT("FIFO not ready for data...\n\n");
    }

    if (is_filling || is_set_thresh ) {
      // Check water level every second until stop filling
      sleep_ms(1000);

    } else if (!is_filling) {
      // Not currently filling reservoir
      sleep_ms(60000);
      // sleep for one minute in betweeen checks
    }
  }
}

void open_valve ()
{
  struct timespec ts;
  ts.tv_sec = 0;
  aon_timer_start(&ts);

  // TODO actually open valve
  DPRINT("OPENING VALVE at time %jd\n\n",(intmax_t)ts.tv_sec);

  is_filling = true;

  return;
}

void close_valve ()
{
  // TODO actually close valve
  DPRINT("CLOSING VALVE\n");

  is_filling = false;
  aon_timer_stop();

  return;
}

void set_thresholds ()
{
  // TODO do I even care about setting thresholds?
  // I could just use two sensors, one at bottom and one at top of tank
  // OR just one at top of tank and refill it whenever it isn't full
  DPRINT("Entering function set_thresholds.\n");

  uint32_t data;
  bool state;

  bool checkLower = 1;
  bool done = 0;

  while (!done)
  {
    state = gpio_get(BUTTON);
    if (state)
    {
      aon_timer_start_with_timeofday();
      struct timespec ts;
      if (!aon_timer_get_time(&ts)) {
        printf("Error: aon_timer_get_time failed!\n");
      }
      DPRINT("timespec values: ts.tv_sec = %d, ts.tv_nsec = %09ld\n",ts.tv_sec,ts.tv_nsec);

      // Checks to see if button is held for 2 seconds to set thresholds
      while (ts.tv_sec < 2) // TODO if I keep this function I need to either baseline current time of day or use aon_timer_start with a timespec containing zeroes
      {
        state = gpio_get(BUTTON);
        if (!state)
        {
          aon_timer_stop();
          break;
        }
      }
      if (aon_timer_is_running())
      {
        aon_timer_stop();
        if (checkLower)
        {
          lower = multicore_fifo_pop_blocking();
          checkLower = 0;
        }
        else
        {
          upper = multicore_fifo_pop_blocking();
          done = 1;
        }
      }
    }
  }

  is_set_thresh = 0;
  return;
}

// Consumer
int main ()
{
  stdio_init_all();

#ifdef DEBUG
  DPRINT("Waiting for USB connection...\n");
  while (!stdio_usb_connected()) {
    DPRINT(".");
    sleep_ms(500);
  }
  DPRINT("Connected to USB!!!\n");
#endif

  adc_init();
  adc_gpio_init(ADC); //26-29 are valid inputs for adc

  gpio_init(VALVE);
  gpio_init(BUTTON);
  bool state = 0;

  uint32_t data = 0;
  multicore_launch_core1(read_water_level);

  DPRINT("Setup is done...entering main loop.\n");

  while (1) {
#if 0
    // Check if button pressed
    state = gpio_get(BUTTON);
    if (state)
    {
      struct timespec ts;
      aon_timer_start_with_timeofday();
      // Checks to see if button is held for 3 seconds to set thresholds
      while (ts.tv_sec < 3)
      {
        state = gpio_get(BUTTON);
        if (!state)
        {
          aon_timer_stop();
          break;
        }
      }
      if (aon_timer_is_running())
      {
        aon_timer_stop();
        // Begin flashing LED rapidly
        is_set_thresh = true;
        set_thresholds();
      }
    }
#endif

    //TODO I should change this code to consume data for setting thresholds and to check against thresholds
    //Maybe extract into two functions that run in while loop depending on if we are currently setting or not
    
    // Let's make sure that there is data in the FIFO to read
    if (multicore_fifo_rvalid()) {
      // If data is available, go get it!
      DPRINT("Preparing to pop data from FIFO...\n");
      data = multicore_fifo_pop_blocking();
      DPRINT("Data from FIFO: %u\n\n",data);
      if (data < THRESHOLD && !is_filling) {
        // If water level is too low, open valve
        open_valve();

      } else if (data >= THRESHOLD && is_filling) {
        // Water level has refilled sufficiently
        close_valve();
      }
    } else {
      // No data?? Give our producer a second to breath
      DPRINT("FIFO is empty...\n\n");
      sleep_ms(1000);
    }

    // Safety check while filling
    if (is_filling) {
      struct timespec ts;
      aon_timer_get_time(&ts);
      DPRINT("SAFETY CHECK: %jd seconds since we started filling\n\n",(intmax_t)ts.tv_sec);
      if ((intmax_t)ts.tv_sec > MAX_FILL_TIME) {
        DPRINT("OVERFLOW ERROR!!!\n\n");
        close_valve();
      }
    }
  }
}
