# Switch back to DFU mode.
import usb

for bus in usb.busses ():
    for dev in bus.devices:
        if dev.idVendor == 0x1d50 and dev.idProduct == 0x6052:
            d = dev.open ()
            d.controlMsg (usb.TYPE_VENDOR | usb.RECIP_DEVICE, 0, 0)
