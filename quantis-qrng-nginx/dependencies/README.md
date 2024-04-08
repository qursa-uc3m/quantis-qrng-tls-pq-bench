# Dependencies

## QUANTIS PCIe QRNG 20.2.4 Linux Drivers Update

### Linux 5.* Kernel Compatibility

The following functions in `driver/xdma-core.c` were modified to make the driver compatible with Ubuntu 5.* kernels:

- **Replacing `pci_alloc_consistent` with `dma_alloc_coherent`**: The `pci_alloc_consistent` function is replaced with `dma_alloc_coherent`. Both of these functions allocate memory in a way that it can be used coherently by both the CPU and a DMA-capable device. The difference lies in the updated function, which is more generic and can work with any device, not just PCI devices.

- **Replacing `pci_free_consistent` with `dma_free_coherent`**: Similar to the previous change, `pci_free_consistent` is replaced with `dma_free_coherent`. The functions are used to free memory that was previously allocated with `pci_alloc_consistent` or `dma_alloc_coherent`, respectively.

- **Replacing `pci_map_sg` with `dma_map_sg`**: The `pci_map_sg function` is replaced with `dma_map_sg`. These functions are used to map a scatter-gather list into device space. The newer `dma_map_sg` function is more generic.

- **Replacing `pci_unmap_sg` with `dma_unmap_sg`**: The `pci_unmap_sg` function is replaced with `dma_unmap_sg`. These functions are used to unmap a scatter-gather list from device space.

- **Replacing `pci_set_dma_mask` and `pci_set_consistent_dma_mask` with `dma_set_mask` and `dma_set_coherent_mask`**: These functions are used to set the DMA mask for the device, which defines the range of memory that the device can access via DMA.

### Linux >=6.4.0 Kernel Compatibility

The following change has been made to the `driver/xdma-core.c` file to make the driver compatible with Ubuntu 6.4.0 kernels:

```c
#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 4, 0)
    g_xdma_class = class_create(THIS_MODULE, DRV_NAME);
#else
    g_xdma_class = class_create(DRV_NAME);
#endif
```
