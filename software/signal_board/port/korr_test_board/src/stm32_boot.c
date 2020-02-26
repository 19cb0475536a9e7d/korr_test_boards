#include <nuttx/config.h>
#include <nuttx/spi/spi.h>
#include <debug.h>

#include <nuttx/board.h>
#include <arch/board/board.h>

#include "up_arch.h"
#include "stm32_korr_test_signal_board.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_boardinitialize
 *
 * Description:
 *   All STM32 architectures must provide the following entry point.  This entry point
 *   is called early in the initialization -- after all memory has been configured
 *   and mapped but before any devices have been initialized.
 *
 ****************************************************************************/

void stm32_boardinitialize(void)
{
  /* Configure on-board LEDs if LED support has been selected. */

#ifdef CONFIG_ARCH_LEDS
  board_autoled_initialize();
#endif

  /* Configure SPI chip selects if 1) SPI is not disabled, and 2) the weak function
   * stm32_spidev_initialize() has been brought into the link.
   */

#if defined(CONFIG_STM32_SPI1) || defined(CONFIG_STM32_SPI2)
  stm32_spidev_initialize();
#endif

   /* Initialize USB is 1) USBDEV is selected, 2) the USB controller is not
    * disabled, and 3) the weak function stm32_usbinitialize() has been brought
    * into the build.
    */

#if defined(CONFIG_USBDEV) && defined(CONFIG_STM32_USB)
  stm32_usbinitialize();
#endif
}

/****************************************************************************
 * Name: board_late_initialize
 *
 * Description:
 *   If CONFIG_BOARD_LATE_INITIALIZE is selected, then an additional initialization call
 *   will be performed in the boot-up sequence to a function called
 *   board_late_initialize().  board_late_initialize() will be called immediately after
 *   up_initialize() is called and just before the initial application is started.
 *   This additional initialization phase may be used, for example, to initialize
 *   board-specific device drivers.
 *
 ****************************************************************************/

#ifdef CONFIG_BOARD_LATE_INITIALIZE
void board_late_initialize(void)
{
#ifndef CONFIG_LIB_BOARDCTL
  /* Perform board initialization here instead of from the board_app_initialize(). */

  stm32_bringup();
#endif
}
#endif
