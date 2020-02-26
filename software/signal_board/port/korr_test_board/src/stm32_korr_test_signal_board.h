#ifndef __BOARDS_ARM_STM32_KORR_TEST_SIGNAL_BOARD_SRC_H
#define __BOARDS_ARM_STM32_KORR_TEST_SIGNAL_BOARD_SRC_H

#define CONFIG_STM32_STM32F10XX

#include <stdint.h>
#include <arch/chip/chip.h>
#include "board.h"
#include "stm32_gpio.h"


#ifdef CONFIG_FS_PROCFS
#  ifdef CONFIG_NSH_PROC_MOUNTPOINT
#    define STM32_PROCFS_MOUNTPOINT CONFIG_NSH_PROC_MOUNTPOINT
#  else
#    define STM32_PROCFS_MOUNTPOINT "/proc"
#  endif
#endif

/* GPIOs **************************************************************/
/* LEDs */
enum struct led_gpios_es : uint32_t {
  GPIO_LED1 = (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN0),
  GPIO_LED2 = (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN1),
  GPIO_LED3 = (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN2),
  GPIO_LED4 = (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN3)
};

/* BUTTONs */
enum struct user_button_gpios_es : uint32_t {
  GPIO_BTN_USER1 = (GPIO_INPUT | GPIO_CNF_INFLOAT | GPIO_MODE_INPUT | GPIO_EXTI | GPIO_PORTC | GPIO_PIN13),
  GPIO_BTN_USER2 = (GPIO_INPUT | GPIO_CNF_INFLOAT | GPIO_MODE_INPUT | GPIO_EXTI | GPIO_PORTC | GPIO_PIN14),
  GPIO_BTN_USER3 = (GPIO_INPUT | GPIO_CNF_INFLOAT | GPIO_MODE_INPUT | GPIO_EXTI | GPIO_PORTC | GPIO_PIN15)
};

enum struct user_buttons_irq_es : uint32_t {
  MIN_IRQBUTTON = user_buttons_es::BUTTON_USER1,
  MAX_IRQBUTTON = user_buttons_es::BUTTON_USER3,
  NUM_IRQBUTTONS = (user_buttons_es::BUTTON_USER1 - user_buttons_es::BUTTON_USER3 + 1)
};

/* SPI chip selects */
#define FPGA_SPI2_CS (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_SET | GPIO_PORTB | GPIO_PIN12)
#define FPGA_ROUTABLE_SPI3_CS                                                                                          \
  (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_SET | GPIO_PORTA | GPIO_PIN15)

/* FPGA gpio configuration */
#define GPIO_XC6SLX_M0 (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN8)
#define GPIO_XC6SLX_M1 (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTA | GPIO_PIN10)
#define GPIO_XC6SLX_INIT_B (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN9)
#define GPIO_XC6SLX_DONE (GPIO_INPUT | GPIO_CNF_INFLOAT | GPIO_MODE_INPUT | GPIO_PORTC | GPIO_PIN7)
#define GPIO_XC6SLX_PROG_B (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN6)
#define GPIO_XC6SLX_HSWAPEN (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTA | GPIO_PIN9)

/* W25 GPIO configuration */
#define GPIO_W25_HOLD (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN5)
#define GPIO_W25_NWP (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTC | GPIO_PIN4)

/* RGB leds pin */
#define GPIO_WS2812_DIN (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_CLEAR | GPIO_PORTD | GPIO_PIN2)

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifndef __ASSEMBLY__

/****************************************************************************
 * Name: stm32_bringup
 *
 * Description:
 *   Perform architecture specific initialization
 *
 *   CONFIG_LIB_BOARDCTL=y:
 *     If CONFIG_NSH_ARCHINITIALIZE=y:
 *       Called from the NSH library (or other application)
 *     Otherwise, assumed to be called from some other application.
 *
 *   Otherwise CONFIG_BOARD_LATE_INITIALIZE=y:
 *     Called from board_late_initialize().
 *
 *   Otherwise, bad news:  Never called
 *
 ****************************************************************************/
int stm32_bringup(void);

/****************************************************************************
 * Name: stm32_gpio_initialize
 *
 * Description:
 *   Initialize GPIO drivers for use with /apps/examples/gpio
 *
 ****************************************************************************/
#ifdef CONFIG_DEV_GPIO
int stm32_gpio_initialize(void);
#endif

/****************************************************************************
 * Name: stm32_adc_setup
 *
 * Description:
 *   Initialize ADC and register the ADC driver.
 *
 ****************************************************************************/
#ifdef CONFIG_ADC
int stm32_adc_setup(void);
#endif

/****************************************************************************
 * Name: stm32_spidev_initialize
 *
 * Description:
 *   Called to configure SPI chip select GPIO pins for the Hy-Mini STM32v board.
 *
 ****************************************************************************/
void stm32_spidev_initialize(void);

/****************************************************************************
 * Name: stm32_w25initialize
 *
 * Description:
 *   Called to initialize Winbond W25 memory
 *
 ****************************************************************************/
int stm32_w25initialize(int minor);

/****************************************************************************
 * Name: stm32_rgbled_setup
 *
 * Description:
 *   This function is called by board initialization logic to configure the
 *   RGB LED driver.  This function will register the driver as /dev/rgbled0.
 *
 * Input Parameters:
 *   None
 *
 * Returned Value:
 *   Zero is returned on success.  Otherwise, a negated errno value is
 *   returned to indicate the nature of the failure.
 *
 ****************************************************************************/
#ifdef CONFIG_RGBLED
int stm32_rgbled_setup(void);
#endif

/****************************************************************************
 * Name: stm32_usbinitialize
 *
 * Description:
 *   Called to setup USB-related GPIO pins for the Hy-Mini STM32v board.
 *
 ****************************************************************************/
void stm32_usbinitialize(void);

/****************************************************************************
 * Name: stm32_pwm_setup
 *
 * Description:
 *   Initialize PWM and register the PWM device.
 *
 ****************************************************************************/
#ifdef CONFIG_PWM
int stm32_pwm_setup(void);
#endif

#endif /* __ASSEMBLY__ */
#endif /* __BOARDS_ARM_STM32_KORR_TEST_SIGNAL_BOARD_SRC_H */
