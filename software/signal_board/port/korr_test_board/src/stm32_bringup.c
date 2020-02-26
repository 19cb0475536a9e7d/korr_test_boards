#include <nuttx/config.h>

#include <sys/mount.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>
#include <debug.h>
#include <errno.h>

#include <nuttx/board.h>
#include <nuttx/timers/oneshot.h>

#ifdef CONFIG_USBMONITOR
#  include <nuttx/usb/usbmonitor.h>
#endif

#include "stm32.h"

#ifdef CONFIG_STM32_OTGFS
#  include "stm32_usbhost.h"
#endif

#ifdef CONFIG_BUTTONS
#  include <nuttx/input/buttons.h>
#endif

#ifdef CONFIG_USERLED
#  include <nuttx/leds/userled.h>
#endif

#include "stm32_korr_test_signal_board.h"

/* Conditional logic in stm32f103_minimum.h will determine if certain features
 * are supported.  Tests for these features need to be made after including
 * stm32f103_minimum.h.
 */

#ifdef HAVE_RTC_DRIVER
#  include <nuttx/timers/rtc.h>
#  include "stm32_rtc.h"
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Checking needed by W25 Flash */

#define HAVE_W25      1

/* Can't support the W25 device if it SPI1 or W25 support is not enabled */

#if !defined(CONFIG_STM32_SPI1) || !defined(CONFIG_MTD_W25)
#  undef HAVE_W25
#endif

/* Can't support W25 features if mountpoints are disabled */

#ifdef CONFIG_DISABLE_MOUNTPOINT
#  undef HAVE_W25
#endif

/* Default W25 minor number */

#if defined(HAVE_W25) && !defined(CONFIG_NSH_W25MINOR)
#  define CONFIG_NSH_W25MINOR 0
#endif

/* Checking needed by MMC/SDCard */

#ifdef CONFIG_NSH_MMCSDMINOR
#  define MMCSD_MINOR CONFIG_NSH_MMCSDMINOR
#else
#  define MMCSD_MINOR 0
#endif

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: stm32_bringup
 *
 * Description:
 *   Perform architecture-specific initialization
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=y :
 *     Called from board_late_initialize().
 *
 *   CONFIG_BOARD_LATE_INITIALIZE=n && CONFIG_LIB_BOARDCTL=y :
 *     Called from the NSH library
 *
 ****************************************************************************/

int stm32_bringup(void)
{
#ifdef CONFIG_ONESHOT
  struct oneshot_lowerhalf_s *os = NULL;
#endif
  int ret = OK;

#ifdef CONFIG_DEV_GPIO
  ret = stm32_gpio_initialize();
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize GPIO Driver: %d\n", ret);
      return ret;
    }
#endif

#ifdef CONFIG_VIDEO_FB
  /* Initialize and register the framebuffer driver */

  ret = fb_register(0, 0);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: fb_register() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_LCD_BACKPACK
  ret = stm32_lcd_backpack_init("/dev/slcd0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize PCF8574 LCD, error %d\n", ret);
      return ret;
    }
#endif

#ifdef CONFIG_SENSORS_ZEROCROSS
  /* Configure the zero-crossing driver */

  ret = stm32_zerocross_initialize();
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize Zero-Cross, error %d\n", ret);
      return ret;
    }
#endif

#ifdef CONFIG_MMCSD
  ret = stm32_mmcsd_initialize(MMCSD_MINOR);
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize SD slot %d: %d\n", ret);
      return ret;
    }
#endif

#ifdef CONFIG_SENSORS_BMP180
  ret = stm32_bmp180initialize("/dev/press0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "Failed to initialize BMP180, error %d\n", ret);
      return ret;
    }
#endif

#ifdef HAVE_W25
  /* Initialize and register the W25 FLASH file system. */

  ret = stm32_w25initialize(CONFIG_NSH_W25MINOR);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize W25 minor %d: %d\n",
             CONFIG_NSH_W25MINOR, ret);
      return ret;
    }
#endif

#ifdef CONFIG_FS_PROCFS
  /* Mount the procfs file system */

  ret = mount(NULL, STM32_PROCFS_MOUNTPOINT, "procfs", 0, NULL);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to mount procfs at %s: %d\n",
             STM32_PROCFS_MOUNTPOINT, ret);
    }
#endif

#ifdef HAVE_AT24
  /* Initialize the AT24 driver */

  ret = stm32_at24_automount(AT24_MINOR);
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_at24_automount failed: %d\n", ret);
      return ret;
    }
#endif /* HAVE_AT24 */

#ifdef CONFIG_PWM
  /* Initialize PWM and register the PWM device. */

  ret = stm32_pwm_setup();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_pwm_setup() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_AUDIO_TONE
  /* Configure and initialize the tone generator. */

  ret = stm32_tone_setup();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_tone_setup() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_LEDS_APA102
  /* Configure and initialize the APA102 LED Strip. */

  ret = stm32_apa102init("/dev/leddrv0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_apa102init() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_LM75_I2C
  /* Configure and initialize the LM75 sensor */

  ret = stm32_lm75initialize("/dev/temp");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_lm75initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_RGBLED
  /* Configure and initialize the RGB LED. */

  ret = stm32_rgbled_setup();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_rgbled_setup() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_SENSORS_HCSR04
  /* Configure and initialize the HC-SR04 distance sensor */

  ret = stm32_hcsr04_initialize("/dev/dist0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_hcsr04_initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_SENSORS_MAX6675
  ret = stm32_max6675initialize("/dev/temp0");
  if (ret < 0)
    {
      serr("ERROR:  stm32_max6675initialize failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_CAN_MCP2515
  /* Configure and initialize the MCP2515 CAN device */

  ret = stm32_mcp2515initialize("/dev/can0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_mcp2515initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_CL_MFRC522
  ret = stm32_mfrc522initialize("/dev/rfid0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_mfrc522initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_ONESHOT
  os = oneshot_initialize(1, 10);
  if (os)
    {
      ret = oneshot_register("/dev/oneshot", os);
    }
#endif

#ifdef CONFIG_BUTTONS
  /* Register the BUTTON driver */

  ret = btn_lower_initialize("/dev/buttons");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: btn_lower_initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_INPUT_NUNCHUCK
  /* Register the Nunchuck driver */

  ret = nunchuck_initialize("/dev/nunchuck0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: nunchuck_initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_SENSORS_QENCODER
  /* Initialize and register the qencoder driver */

  ret = stm32_qencoder_initialize("/dev/qe0", CONFIG_STM32F103MINIMUM_QETIMER);
  if (ret != OK)
    {
      syslog(LOG_ERR,
             "ERROR: Failed to register the qencoder: %d\n",
             ret);
    }
#endif

#ifdef CONFIG_USERLED
  /* Register the LED driver */

  ret = userled_lower_initialize("/dev/userleds");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: userled_lower_initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_SENSORS_APDS9960
  /* Register the APDS-9960 gesture sensor */

  ret = stm32_apds9960initialize("/dev/gest0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_apds9960initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_SENSORS_VEML6070
  /* Register the UV-A light sensor */

  ret = stm32_veml6070initialize("/dev/uvlight0");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_veml6070initialize() failed: %d\n", ret);
    }
#endif

#ifdef CONFIG_ADC
  /* Initialize ADC and register the ADC driver. */

  ret = stm32_adc_setup();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: stm32_adc_setup failed: %d\n", ret);
    }
#endif

#if defined(CONFIG_WL_NRF24L01)
  /* Initialize the NRF24L01 wireless module */

  stm32_wlinitialize();
#endif

  return ret;
}
