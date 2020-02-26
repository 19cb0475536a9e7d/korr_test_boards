#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <debug.h>

#include <nuttx/board.h>
#include <arch/board/board.h>

#include "chip.h"
#include "up_arch.h"
#include "up_internal.h"
#include "stm32.h"
#include "stm32_korr_test_signal_board.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static inline void set_led(bool v)
{
  ledinfo("Turn LED %s\n", v? "on":"off");
  stm32_gpiowrite(GPIO_LED1, !v);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_autoled_initialize
 ****************************************************************************/

#ifdef CONFIG_ARCH_LEDS
void board_autoled_initialize(void)
{
  /* Configure LED GPIO for output */

  stm32_configgpio(GPIO_LED1);
}

/****************************************************************************
 * Name: board_autoled_on
 ****************************************************************************/

void board_autoled_on(int led)
{
  ledinfo("board_autoled_on(%d)\n",led);

  switch (led)
    {
    case LED_STARTED:
    case LED_HEAPALLOCATE:
      /* As the board provides only one soft controllable LED, we simply
       * turn it on when the board boots.
       */

      set_led(true);
      break;

    case LED_PANIC:
      /* For panic state, the LED is blinking */

      set_led(true);
      break;
    }
}

/****************************************************************************
 * Name: board_autoled_off
 ****************************************************************************/

void board_autoled_off(int led)
{
  switch (led)
    {
    case LED_PANIC:
      /* For panic state, the LED is blinking */

      set_led(false);
      break;
    }
}

#endif /* CONFIG_ARCH_LEDS */
