// Copyright 2023 Zack Scholl.
//
// Author: Zack Scholl (zack.scholl@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// See http://creativecommons.org/licenses/MIT/ for more information.

/* spec
Amen is for slice based effects (how many slices and how often it jumps)
Break would be the wacky effects (filtering, reversing, etc.)
Sample selects currently playing sample from the current bank
*/

#include "mcp3208.h"
#include "taptempo.h"

void input_handling() {
  // flash bad signs
  while (!fil_is_open) {
    printf("waiting to start\n");
    sleep_ms(10);
  }

  gpio_init(GPIO_TAPTEMPO);
  gpio_set_dir(GPIO_TAPTEMPO, GPIO_IN);
  gpio_pull_up(GPIO_TAPTEMPO);
  gpio_init(GPIO_TAPTEMPO_LED);
  gpio_set_dir(GPIO_TAPTEMPO_LED, GPIO_OUT);
  gpio_put(GPIO_TAPTEMPO_LED, 1);

  MCP3208 *mcp3208 = MCP3208_malloc(spi1, 9, 10, 8, 11);
  TapTempo *taptempo = TapTempo_malloc();
  bool btn_taptempo_on = false;

  while (1) {
    uint16_t val;
    if (gpio_get(GPIO_TAPTEMPO) == 0 && !btn_taptempo_on) {
      btn_taptempo_on = true;
      gpio_put(GPIO_TAPTEMPO_LED, 0);
      val = TapTempo_tap(taptempo);
      if (val > 0) {
        printf("[ectocore] tap bpm -> %d\n", val);
        sf->bpm_tempo = val;
      }
    } else if (gpio_get(GPIO_TAPTEMPO) == 1 && btn_taptempo_on) {
      btn_taptempo_on = false;
      gpio_put(GPIO_TAPTEMPO_LED, 1);
    }

    val = MCP3208_read(mcp3208, KNOB_SAMPLE, false);
    val = (val * (banks[sel_bank_next]->num_samples)) / 1024;
    if (val != sel_sample_cur) {
      sel_sample_next = val;
      fil_current_change = true;
      printf("[ectocore] switch sample %d\n", val);
    }
    sleep_ms(1);
  }
}