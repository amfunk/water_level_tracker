#include <stdio.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/aon_timer.h"

#define LOWER_THRESHOLD 1
#define UPPER_THRESHOLD 10000

#define ADC 26
#define VALVE 1
#define BUTTON 2

bool is_filling = false;
bool is_set_thresh = false;
uint32_t lower = 1;
uint32_t upper = 1000;

void read_water_level ()
{
  uint32_t data = 0;
  while (1) {
    data = adc_read();
    if (multicore_fifo_wready()) {
      printf("Adding data to FIFO: %u\n",data);
      multicore_fifo_push_blocking(data);
    } else {
      printf("FIFO not ready for data...\n");
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
  // TODO actually open valve
  printf("OPENING VALVE\n");



  is_filling = true;

  return;
}

void close_valve ()
{
  // TODO actually close valve
  printf("CLOSING VALVE\n");

  is_filling = false;

  return;
}

void set_thresholds ()
{
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
      // Checks to see if button is held for 2 seconds to set thresholds
      while (ts.tv_sec < 2)
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

int main ()
{
  stdio_init_all();

  adc_init();
  adc_gpio_init(ADC); //26-29 are valid inputs for adc

  gpio_init(VALVE);
  gpio_init(BUTTON);
  bool state = 0;

  uint32_t data = 0;
  multicore_launch_core1(read_water_level);

  while (1) {
    // Check if button pressed
    state = gpio_get(BUTTON);
    if (state)
    {
      aon_timer_start_with_timeofday();
      struct timespec ts;
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

    //TODO I should change this code to consume data for setting thresholds and to check against thresholds
    //Maybe extract into two functions that run in while loop depending on if we are currently setting or not

    data = multicore_fifo_pop_blocking();
    printf("Reading data from FIFO: %u\n",data);
    if (data < LOWER_THRESHOLD) {
      // If water level is too low, open valve
      open_valve();
      
    } else if (data > UPPER_THRESHOLD) {
      // Water level has refilled sufficiently
      close_valve();
    }
  }

  return 0;
}
