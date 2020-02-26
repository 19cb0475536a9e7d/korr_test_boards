#include <nuttx/config.h>

#include <debug.h>
#include <errno.h>

#include <arch/board/board.h>
#include <nuttx/analog/adc.h>
#include <nuttx/board.h>

#include "chip.h"
#include "stm32_adc.h"
#include "stm32_korr_test_signal_board.h"

#ifdef CONFIG_ADC

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Configuration ********************************************************************/
/* Up to 2 ADC interfaces are supported */

#  if STM32_NADC < 2
#    undef CONFIG_STM32_ADC2
#  endif

#  if STM32_NADC < 1
#    undef CONFIG_STM32_ADC1
#  endif

#  if defined(CONFIG_STM32_ADC1) || defined(CONFIG_STM32_ADC2)
#    ifndef CONFIG_STM32_ADC1
#      warning "Channel information only available for ADC1"
#    endif

/* The number of ADC channels in the conversion list */

#    define ADC1_NCHANNELS 1

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* Identifying number of each ADC channel to be used with Variable Resistor (Pontentiometer)*/

#    ifdef CONFIG_STM32_ADC1
static const uint8_t g_chanlist[ADC1_NCHANNELS] = {0}; /* ADC12_IN0 */

/* Configurations of pins used byte each ADC channels */

static const uint32_t g_pinlist[ADC1_NCHANNELS] = {GPIO_ADC12_IN0};
#    endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_adc_setup
 *
 * Description:
 *   Initialize ADC and register the ADC driver.
 *
 ****************************************************************************/

int stm32_adc_setup(void) {
#    ifdef CONFIG_STM32_ADC1
  static bool initialized = false;
  struct adc_dev_s *adc;
  int ret;
  int i;

  /* Check if we have already initialized */

  if (!initialized) {
    /* Configure the pins as analog inputs for the selected channels */

    for (i = 0; i < ADC1_NCHANNELS; i++) {
      stm32_configgpio(g_pinlist[i]);
    }

    /* Call stm32_adcinitialize() to get an instance of the ADC interface */

    adc = stm32_adcinitialize(1, g_chanlist, ADC1_NCHANNELS);
    if (adc == NULL) {
      aerr("ERROR: Failed to get ADC interface\n");
      return -ENODEV;
    }

    /* Register the ADC driver at "/dev/adc0" */

    ret = adc_register("/dev/adc0", adc);
    if (ret < 0) {
      aerr("ERROR: adc_register failed: %d\n", ret);
      return ret;
    }

    /* Now we are initialized */

    initialized = true;
  }

  return OK;
#    else
  return -ENOSYS;
#    endif
}

#  endif /* CONFIG_STM32_ADC1 || CONFIG_STM32_ADC2 || CONFIG_STM32_ADC3 */
#endif   /* CONFIG_ADC */
