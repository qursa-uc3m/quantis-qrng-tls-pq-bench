#ifndef __XDMA_VERSION_H__
#define __XDMA_VERSION_H__

#define DRV_MOD_MAJOR 20
#define DRV_MOD_MINOR 2
#define DRV_MOD_PATCHLEVEL 4

#define DRV_MODULE_VERSION                                                     \
	__stringify(DRV_MOD_MAJOR) "." __stringify(                            \
		DRV_MOD_MINOR) "." __stringify(DRV_MOD_PATCHLEVEL)

#define DRV_MOD_VERSION_NUMBER ((DRV_MOD_MAJOR)*10 + DRV_MOD_MINOR)

#endif /* ifndef __XDMA_VERSION_H__ */
