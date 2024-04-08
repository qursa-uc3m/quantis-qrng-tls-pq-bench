/* Magic key to ensure IOCTL are OK */
#define QUANTIS_IOC_MAGIC 'q'

/* get driver version */
#define QUANTIS_IOCTL_GET_DRIVER_VERSION                                       \
	_IOR(QUANTIS_IOC_MAGIC, 0, unsigned int)

/* get number of detected cards */
#define QUANTIS_IOCTL_GET_CARD_COUNT _IOR(QUANTIS_IOC_MAGIC, 1, unsigned int)

/* get mask of detected modules */
#define QUANTIS_IOCTL_GET_MODULES_MASK _IOR(QUANTIS_IOC_MAGIC, 2, unsigned int)

/* get card board version */
#define QUANTIS_IOCTL_GET_BOARD_VERSION _IOR(QUANTIS_IOC_MAGIC, 3, unsigned int)

/* reset one board */
#define QUANTIS_IOCTL_RESET_BOARD _IO(QUANTIS_IOC_MAGIC, 4)

/* enable mask module */
#define QUANTIS_IOCTL_ENABLE_MODULE _IOW(QUANTIS_IOC_MAGIC, 5, unsigned int)

/* disable mask modules */
#define QUANTIS_IOCTL_DISABLE_MODULE _IOW(QUANTIS_IOC_MAGIC, 6, unsigned int)

/* set debug level, not implemented */
#define QUANTIS_IOCTL_SET_DEBUG_LEVEL                                          \
	_IOW(QUANTIS_IOC_MAGIC, 7, unsigned int) * /

/* get status of modules */
#define QUANTIS_IOCTL_GET_MODULES_STATUS                                       \
	_IOR(QUANTIS_IOC_MAGIC, 8, unsigned int)

#define QUANTIS_IOCTL_GET_PCI_BUS_DEVICE_ID                                    \
	_IOR(QUANTIS_IOC_MAGIC, 9, unsigned int)

#define QUANTIS_IOCTL_GET_AIS31_STARTUP_TESTS_REQUEST_FLAG                     \
	_IOR(QUANTIS_IOC_MAGIC, 10, unsigned int)

#define QUANTIS_IOCTL_CLEAR_AIS31_STARTUP_TESTS_REQUEST_FLAG                   \
	_IOW(QUANTIS_IOC_MAGIC, 11, unsigned int)

#define QUANTIS_IOCTL_GET_SERIAL_MAX_LENGTH 256
#define QUANTIS_IOCTL_GET_SERIAL                                               \
	_IOR(QUANTIS_IOC_MAGIC, 12, char[QUANTIS_IOCTL_GET_SERIAL_MAX_LENGTH])

#define QUANTIS_QRNG_MODE_RNG 0
#define QUANTIS_QRNG_MODE_SAMPLE 1

/* Set the mode that will be used when you reset the board */
#define QUANTIS_IOCTL_SET_QRNG_MODE _IOW(QUANTIS_IOC_MAGIC, 13, unsigned int)
/* Get the mode that will be used when you reset the board */
#define QUANTIS_IOCTL_GET_QRNG_MODE _IOR(QUANTIS_IOC_MAGIC, 14, unsigned int)
/* Get the mode that is currently used */
#define QUANTIS_IOCTL_GET_CURRENT_QRNG_MODE                                    \
	_IOR(QUANTIS_IOC_MAGIC, 15, unsigned int)
