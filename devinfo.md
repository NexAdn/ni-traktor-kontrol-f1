# Device information

## hiddev

  * `/dev/hidraw4` erhalten aus `dmesg -w`, major `244`

## HID info

  * Usage Page 0xff01 (Vendor specific)
  * Usages:
    * 0x2: Buttons
    * 0x3: Endless encoder
    * 0x4: Knobs
    * 0x5: Faders
  * Structure:
    * Collection
      * Collection (Inputs, Report ID=1)
        * Buttons 32x1
        * Wheel 1x8
        * Knobs 4x16
        * Faders 4x16
      * Collection (Outputs, Report ID=128)
        * 80x8 Usage=129
      * Collection (Features, Report ID=240)
        * 4x8 Usage=4
        * 4x8 Usage=5
      * Collection (Features, Report ID=208)
        * 63x8 Usage=209
      * Collection (Features, Report ID=241)
        * 48x8 U=129

## lsusb output

```
Bus 001 Device 025: ID 17cc:1120 Native Instruments Traktor Kontrol F1
Device Descriptor:
  bLength                18
  bDescriptorType         1
  bcdUSB               2.00
  bDeviceClass            0 
  bDeviceSubClass         0 
  bDeviceProtocol         0 
  bMaxPacketSize0        64
  idVendor           0x17cc Native Instruments
  idProduct          0x1120 
  bcdDevice            0.0b
  iManufacturer           1 Native Instruments
  iProduct                2 Traktor Kontrol F1
  iSerial                 6 1E85F728
  bNumConfigurations      1
  Configuration Descriptor:
    bLength                 9
    bDescriptorType         2
    wTotalLength       0x0039
    bNumInterfaces          2
    bConfigurationValue     1
    iConfiguration          4 Traktor Kontrol F1 HID
    bmAttributes         0x80
      (Bus Powered)
    MaxPower              480mA
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        0
      bAlternateSetting       0
      bNumEndpoints           2
      bInterfaceClass         3 Human Interface Device
      bInterfaceSubClass      0 
      bInterfaceProtocol      0 
      iInterface              0 
        HID Device Descriptor:
          bLength                 9
          bDescriptorType        33
          bcdHID               1.10
          bCountryCode            0 Not supported
          bNumDescriptors         1
          bDescriptorType        34 Report
          wDescriptorLength     487
          Report Descriptor: (length is 487)
            Item(Global): Usage Page, data= [ 0x01 0xff ] 65281
                            (null)
            Item(Local ): Usage, data= [ 0x00 ] 0
                            (null)
            Item(Main  ): Collection, data= [ 0x01 ] 1
                            Application
            Item(Local ): Usage, data= [ 0x01 ] 1
                            (null)
            Item(Main  ): Collection, data= [ 0x02 ] 2
                            Logical
            Item(Global): Report ID, data= [ 0x01 ] 1
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Local ): Usage, data= [ 0x02 ] 2
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0x01 ] 1
            Item(Global): Report Size, data= [ 0x01 ] 1
            Item(Global): Report Count, data= [ 0x20 ] 32
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Local ): Usage, data= [ 0x03 ] 3
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0xff 0x00 ] 255
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Global): Report Count, data= [ 0x01 ] 1
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Local ): Usage, data= [ 0x04 ] 4
                            (null)
            Item(Local ): Usage, data= [ 0x04 ] 4
                            (null)
            Item(Local ): Usage, data= [ 0x04 ] 4
                            (null)
            Item(Local ): Usage, data= [ 0x04 ] 4
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0xff 0x0f ] 4095
            Item(Global): Report Size, data= [ 0x10 ] 16
            Item(Global): Report Count, data= [ 0x04 ] 4
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Local ): Usage, data= [ 0x05 ] 5
                            (null)
            Item(Local ): Usage, data= [ 0x05 ] 5
                            (null)
            Item(Local ): Usage, data= [ 0x05 ] 5
                            (null)
            Item(Local ): Usage, data= [ 0x05 ] 5
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0xff 0x0f ] 4095
            Item(Global): Report Size, data= [ 0x10 ] 16
            Item(Global): Report Count, data= [ 0x04 ] 4
            Item(Main  ): Input, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Local ): Usage, data= [ 0x80 ] 128
                            (null)
            Item(Main  ): Collection, data= [ 0x02 ] 2
                            Logical
            Item(Global): Report ID, data= [ 0x80 ] 128
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0x7f ] 127
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Global): Report Count, data= [ 0x50 ] 80
            Item(Main  ): Output, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Local ): Usage, data= [ 0x01 ] 1
                            (null)
            Item(Main  ): Collection, data= [ 0x02 ] 2
                            Logical
            Item(Global): Report ID, data= [ 0xf0 ] 240
            Item(Local ): Usage, data= [ 0x04 ] 4
                            (null)
            Item(Local ): Usage, data= [ 0x04 ] 4
                            (null)
            Item(Local ): Usage, data= [ 0x04 ] 4
                            (null)
            Item(Local ): Usage, data= [ 0x04 ] 4
                            (null)
            Item(Local ): Usage, data= [ 0x05 ] 5
                            (null)
            Item(Local ): Usage, data= [ 0x05 ] 5
                            (null)
            Item(Local ): Usage, data= [ 0x05 ] 5
                            (null)
            Item(Local ): Usage, data= [ 0x05 ] 5
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Physical Maximum, data= [ 0xff 0x00 ] 255
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Global): Report Count, data= [ 0x08 ] 8
            Item(Main  ): Feature, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Local ): Usage, data= [ 0xd0 ] 208
                            (null)
            Item(Main  ): Collection, data= [ 0x02 ] 2
                            Logical
            Item(Global): Report ID, data= [ 0xd0 ] 208
            Item(Local ): Usage, data= [ 0xd1 ] 209
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0xff 0x00 ] 255
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Global): Report Count, data= [ 0x3f ] 63
            Item(Main  ): Feature, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Local ): Usage, data= [ 0x01 ] 1
                            (null)
            Item(Main  ): Collection, data= [ 0x02 ] 2
                            Logical
            Item(Global): Report ID, data= [ 0xf1 ] 241
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Local ): Usage, data= [ 0x81 ] 129
                            (null)
            Item(Global): Logical Minimum, data= [ 0x00 ] 0
            Item(Global): Logical Maximum, data= [ 0xff 0x00 ] 255
            Item(Global): Report Size, data= [ 0x08 ] 8
            Item(Global): Report Count, data= [ 0x30 ] 48
            Item(Main  ): Feature, data= [ 0x02 ] 2
                            Data Variable Absolute No_Wrap Linear
                            Preferred_State No_Null_Position Non_Volatile Bitfield
            Item(Main  ): End Collection, data=none
            Item(Main  ): End Collection, data=none
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x81  EP 1 IN
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               4
      Endpoint Descriptor:
        bLength                 7
        bDescriptorType         5
        bEndpointAddress     0x01  EP 1 OUT
        bmAttributes            3
          Transfer Type            Interrupt
          Synch Type               None
          Usage Type               Data
        wMaxPacketSize     0x0040  1x 64 bytes
        bInterval               4
    Interface Descriptor:
      bLength                 9
      bDescriptorType         4
      bInterfaceNumber        1
      bAlternateSetting       0
      bNumEndpoints           0
      bInterfaceClass       254 Application Specific Interface
      bInterfaceSubClass      1 Device Firmware Update
      bInterfaceProtocol      1 
      iInterface              5 Traktor Kontrol F1 DFU
      Device Firmware Upgrade Interface Descriptor:
        bLength                             7
        bDescriptorType                    33
        bmAttributes                        7
          Will Not Detach
          Manifestation Tolerant
          Upload Supported
          Download Supported
        wDetachTimeout                    250 milliseconds
        wTransferSize                      64 bytes
Device Qualifier (for other device speed):
  bLength                10
  bDescriptorType         6
  bcdUSB               2.00
  bDeviceClass            0 
  bDeviceSubClass         0 
  bDeviceProtocol         0 
  bMaxPacketSize0        64
  bNumConfigurations      1
can't get debug descriptor: Resource temporarily unavailable
Device Status:     0x0000
  (Bus Powered)
```

### test_libhid output

```
 NOTICE: hid_init(): libhid 0.2.17 is being initialized.
  TRACE: hid_init(): initialising USB subsystem...
  TRACE: hid_init(): scanning for USB busses...
  TRACE: hid_init(): scanning for USB devices...
 NOTICE: hid_init(): successfully initialised HID library.
  TRACE: hid_new_HIDInterface(): creating a new HIDInterface instance...
  TRACE: hid_force_open(): forcefully opening a device interface according to matching criteria...
  TRACE: hid_get_usb_handle(): acquiring handle for a USB device...
  TRACE: hid_find_usb_device(): enumerating USB busses...
  TRACE: hid_find_usb_device(): enumerating USB devices on bus 002...
  TRACE: hid_find_usb_device(): inspecting USB device 002/001[0]...
  TRACE: hid_compare_usb_device(): comparing match specifications to USB device...
  TRACE: hid_compare_usb_device(): inspecting vendor ID...
  TRACE: hid_compare_usb_device(): no match on vendor ID.
  TRACE: hid_compare_usb_device(): inspecting product ID...
  TRACE: hid_compare_usb_device(): no match on product ID.
  TRACE: hid_compare_usb_device(): no custom matching function supplied.
 NOTICE: hid_find_usb_device(): vendor 0x1d6b of USB device 002/001[0] does not match 0x17cc.
  TRACE: hid_find_usb_device(): enumerating USB devices on bus 001...
  TRACE: hid_find_usb_device(): inspecting USB device 001/017[0]...
  TRACE: hid_compare_usb_device(): comparing match specifications to USB device...
  TRACE: hid_compare_usb_device(): inspecting vendor ID...
  TRACE: hid_compare_usb_device(): no match on vendor ID.
  TRACE: hid_compare_usb_device(): inspecting product ID...
  TRACE: hid_compare_usb_device(): no match on product ID.
  TRACE: hid_compare_usb_device(): no custom matching function supplied.
 NOTICE: hid_find_usb_device(): vendor 0x0408 of USB device 001/017[0] does not match 0x17cc.
  TRACE: hid_find_usb_device(): inspecting USB device 001/004[0]...
  TRACE: hid_compare_usb_device(): comparing match specifications to USB device...
  TRACE: hid_compare_usb_device(): inspecting vendor ID...
  TRACE: hid_compare_usb_device(): no match on vendor ID.
  TRACE: hid_compare_usb_device(): inspecting product ID...
  TRACE: hid_compare_usb_device(): no match on product ID.
  TRACE: hid_compare_usb_device(): no custom matching function supplied.
 NOTICE: hid_find_usb_device(): vendor 0x04ca of USB device 001/004[0] does not match 0x17cc.
  TRACE: hid_find_usb_device(): inspecting USB device 001/022[0]...
  TRACE: hid_compare_usb_device(): comparing match specifications to USB device...
  TRACE: hid_compare_usb_device(): inspecting vendor ID...
  TRACE: hid_compare_usb_device(): no match on vendor ID.
  TRACE: hid_compare_usb_device(): inspecting product ID...
  TRACE: hid_compare_usb_device(): no match on product ID.
  TRACE: hid_compare_usb_device(): no custom matching function supplied.
 NOTICE: hid_find_usb_device(): vendor 0x1d57 of USB device 001/022[0] does not match 0x17cc.
  TRACE: hid_find_usb_device(): inspecting USB device 001/007[0]...
  TRACE: hid_compare_usb_device(): comparing match specifications to USB device...
  TRACE: hid_compare_usb_device(): inspecting vendor ID...
  TRACE: hid_compare_usb_device(): no match on vendor ID.
  TRACE: hid_compare_usb_device(): inspecting product ID...
  TRACE: hid_compare_usb_device(): no match on product ID.
  TRACE: hid_compare_usb_device(): no custom matching function supplied.
 NOTICE: hid_find_usb_device(): vendor 0x8644 of USB device 001/007[0] does not match 0x17cc.
  TRACE: hid_find_usb_device(): inspecting USB device 001/025[0]...
  TRACE: hid_compare_usb_device(): comparing match specifications to USB device...
  TRACE: hid_compare_usb_device(): inspecting vendor ID...
  TRACE: hid_compare_usb_device(): match on vendor ID: 0x17cc.
  TRACE: hid_compare_usb_device(): inspecting product ID...
  TRACE: hid_compare_usb_device(): match on product ID: 0x1120.
  TRACE: hid_compare_usb_device(): no custom matching function supplied.
 NOTICE: hid_find_usb_device(): found a matching USB device 001/025[0].
  TRACE: hid_force_open(): claiming USB device 001/025[0].
  TRACE: hid_os_force_claim(): failed to claim USB device 001/025[0], trying 2 more time(s)...
  TRACE: hid_os_force_claim(): detaching kernel driver from USB device 001/025[0]...
  TRACE: hid_os_force_claim(): trying again to claim USB device 001/025[0]...
 NOTICE: hid_force_open(): successfully claimed USB device 001/025[0].
  TRACE: hid_init_parser(): initialising the HID parser for USB Device 001/025[0]...
  TRACE: hid_init_parser(): allocating space for HIDData structure...
  TRACE: hid_init_parser(): successfully allocated memory for HIDData strcture.
  TRACE: hid_init_parser(): allocating space for HIDParser structure...
  TRACE: hid_init_parser(): successfully allocated memory for HIDParser strcture.
 NOTICE: hid_init_parser(): successfully initialised the HID parser for USB Device 001/025[0].
  TRACE: hid_prepare_hid_descriptor(): initialising the HID descriptor for USB device 001/025[0]...
  TRACE: hid_prepare_hid_descriptor(): retrieving HID descriptor for USB device 001/025[0]...
 NOTICE: hid_prepare_hid_descriptor(): successfully initialised HID descriptor for USB device 001/025[0] (487 bytes).
  TRACE: hid_prepare_report_descriptor(): initialising the report descriptor for USB device 001/025[0]...
  TRACE: hid_prepare_report_descriptor(): retrieving report descriptor for USB device 001/025[0]...
 NOTICE: hid_prepare_report_descriptor(): successfully initialised report descriptor for USB device 001/025[0].
  TRACE: hid_prepare_parser(): setting up the HID parser for USB device 001/025[0]...
  TRACE: hid_reset_parser(): resetting the HID parser for USB device 001/025[0]...
  TRACE: hid_prepare_parser(): dumping the raw report descriptor
  TRACE: hid_prepare_parser(): 0x000: 0x06 0x01 0xff 0x09 0x00 0xa1 0x01 0x09 
  TRACE: hid_prepare_parser(): 0x008: 0x01 0xa1 0x02 0x85 0x01 0x09 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x010: 0x02 0x09 0x02 0x09 0x02 0x09 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x018: 0x02 0x09 0x02 0x09 0x02 0x09 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x020: 0x02 0x09 0x02 0x09 0x02 0x09 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x028: 0x02 0x09 0x02 0x09 0x02 0x09 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x030: 0x02 0x09 0x02 0x09 0x02 0x09 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x038: 0x02 0x09 0x02 0x09 0x02 0x09 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x040: 0x02 0x09 0x02 0x09 0x02 0x09 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x048: 0x02 0x09 0x02 0x09 0x02 0x15 0x00 0x25 
  TRACE: hid_prepare_parser(): 0x050: 0x01 0x75 0x01 0x95 0x20 0x81 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x058: 0x03 0x15 0x00 0x26 0xff 0x00 0x75 0x08 
  TRACE: hid_prepare_parser(): 0x060: 0x95 0x01 0x81 0x02 0x09 0x04 0x09 0x04 
  TRACE: hid_prepare_parser(): 0x068: 0x09 0x04 0x09 0x04 0x15 0x00 0x26 0xff 
  TRACE: hid_prepare_parser(): 0x070: 0x0f 0x75 0x10 0x95 0x04 0x81 0x02 0x09 
  TRACE: hid_prepare_parser(): 0x078: 0x05 0x09 0x05 0x09 0x05 0x09 0x05 0x15 
  TRACE: hid_prepare_parser(): 0x080: 0x00 0x26 0xff 0x0f 0x75 0x10 0x95 0x04 
  TRACE: hid_prepare_parser(): 0x088: 0x81 0x02 0xc0 0x09 0x80 0xa1 0x02 0x85 
  TRACE: hid_prepare_parser(): 0x090: 0x80 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x098: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0a0: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0a8: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0b0: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0b8: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0c0: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0c8: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0d0: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0d8: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0e0: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0e8: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0f0: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x0f8: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x100: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x108: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x110: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x118: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x120: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x128: 0x81 0x09 0x81 0x09 0x81 0x09 0x81 0x09 
  TRACE: hid_prepare_parser(): 0x130: 0x81 0x09 0x81 0x15 0x00 0x25 0x7f 0x75 
  TRACE: hid_prepare_parser(): 0x138: 0x08 0x95 0x50 0x91 0x02 0xc0 0x09 0x01 
  TRACE: hid_prepare_parser(): 0x140: 0xa1 0x02 0x85 0xf0 0x09 0x04 0x09 0x04 
  TRACE: hid_prepare_parser(): 0x148: 0x09 0x04 0x09 0x04 0x09 0x05 0x09 0x05 
  TRACE: hid_prepare_parser(): 0x150: 0x09 0x05 0x09 0x05 0x15 0x00 0x46 0xff 
  TRACE: hid_prepare_parser(): 0x158: 0x00 0x75 0x08 0x95 0x08 0xb1 0x02 0xc0 
  TRACE: hid_prepare_parser(): 0x160: 0x09 0xd0 0xa1 0x02 0x85 0xd0 0x09 0xd1 
  TRACE: hid_prepare_parser(): 0x168: 0x15 0x00 0x26 0xff 0x00 0x75 0x08 0x95 
  TRACE: hid_prepare_parser(): 0x170: 0x3f 0xb1 0x02 0xc0 0x09 0x01 0xa1 0x02 
  TRACE: hid_prepare_parser(): 0x178: 0x85 0xf1 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x180: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x188: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x190: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x198: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x1a0: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x1a8: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x1b0: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x1b8: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x1c0: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x1c8: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x1d0: 0x09 0x81 0x09 0x81 0x09 0x81 0x09 0x81 
  TRACE: hid_prepare_parser(): 0x1d8: 0x09 0x81 0x15 0x00 0x26 0xff 0x00 0x75 
  TRACE: hid_prepare_parser(): 0x1e0: 0x08 0x95 0x30 0xb1 0x02 0xc0 0xc0 
  TRACE: hid_prepare_parser(): parsing the HID tree of USB device 001/025[0]...
 NOTICE: hid_prepare_parser(): successfully set up the HID parser for USB device 001/025[0].
  TRACE: hid_force_open(): add open device to list...
 NOTICE: hid_force_open(): successfully opened USB device 001/025[0].
device identification of HIDInterface 001/025[0]:
  dev_handle:    0x55b3f4d3c440
  device:        0x55b3f4d338d0
  location:      001/025
  manufacturer:  Native Instruments
  product:       Traktor Kontrol F1
  serial number: 1E85F728
  TRACE: hid_reset_parser(): resetting the HID parser for USB device 001/025[0]...
  TRACE: hid_dump_tree(): iterating the parse tree for USB device 001/025[0]...
parse tree of HIDInterface 001/025[0]:
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010002; type: 0x80
  path: 0xff010000.0xff010001.0xff010003; type: 0x80
  path: 0xff010000.0xff010001.0xff010004; type: 0x80
  path: 0xff010000.0xff010001.0xff010004; type: 0x80
  path: 0xff010000.0xff010001.0xff010004; type: 0x80
  path: 0xff010000.0xff010001.0xff010004; type: 0x80
  path: 0xff010000.0xff010001.0xff010005; type: 0x80
  path: 0xff010000.0xff010001.0xff010005; type: 0x80
  path: 0xff010000.0xff010001.0xff010005; type: 0x80
  path: 0xff010000.0xff010001.0xff010005; type: 0x80
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0xff010081; type: 0x90
  path: 0xff010000.0xff010080.0x00510081; type: 0x90
  path: 0xff010000.0xff010080.0x00500082; type: 0x90
  path: 0xff010000.0xff010080.0x004f0081; type: 0x90
  path: 0xff010000.0xff010080.0x004e0081; type: 0x90
  path: 0xff010000.0xff010080.0x004d0081; type: 0x90
  path: 0xff010000.0xff010080.0x004c0081; type: 0x90
  path: 0xff010000.0xff010080.0x004b0081; type: 0x90
  path: 0xff010000.0xff010080.0x004a0081; type: 0x90
  path: 0xff010000.0xff010080.0x00490081; type: 0x90
  path: 0xff010000.0xff010080.0x00480081; type: 0x90
  path: 0xff010000.0xff010080.0x00470081; type: 0x90
  path: 0xff010000.0xff010080.0x00460081; type: 0x90
  path: 0xff010000.0xff010080.0x00450081; type: 0x90
  path: 0xff010000.0xff010080.0x00440081; type: 0x90
  path: 0xff010000.0xff010080.0x00430081; type: 0x90
  path: 0xff010000.0xff010080.0x00420081; type: 0x90
  path: 0xff010000.0xff010080.0x00410081; type: 0x90
  path: 0xff010000.0xff010080.0x00400081; type: 0x90
  path: 0xff010000.0xff010080.0x003f0081; type: 0x90
  path: 0xff010000.0xff010080.0x003e0081; type: 0x90
  path: 0xff010000.0xff010080.0x003d0081; type: 0x90
  path: 0xff010000.0xff010080.0x003c0081; type: 0x90
  path: 0xff010000.0xff010080.0x003b0081; type: 0x90
  path: 0xff010000.0xff010080.0x003a0081; type: 0x90
  path: 0xff010000.0xff010080.0x00390081; type: 0x90
  path: 0xff010000.0xff010080.0x00380081; type: 0x90
  path: 0xff010000.0xff010080.0x00370081; type: 0x90
  path: 0xff010000.0xff010080.0x00360081; type: 0x90
  path: 0xff010000.0xff010080.0x00350081; type: 0x90
  path: 0xff010000.0xff010080.0x00340081; type: 0x90
  path: 0xff010000.0xff010001.0xff010004; type: 0xb0
  path: 0xff010000.0xff010001.0xff010004; type: 0xb0
  path: 0xff010000.0xff010001.0xff010004; type: 0xb0
  path: 0xff010000.0xff010001.0xff010004; type: 0xb0
  path: 0xff010000.0xff010001.0xff010005; type: 0xb0
  path: 0xff010000.0xff010001.0xff010005; type: 0xb0
  path: 0xff010000.0xff010001.0xff010005; type: 0xb0
  path: 0xff010000.0xff010001.0xff010005; type: 0xb0
  path: 0xff010000.0xff0100d0.0xff0100d1; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff0100d0.0x00000000; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  path: 0xff010000.0xff010001.0xff010081; type: 0xb0
  TRACE: hid_reset_parser(): resetting the HID parser for USB device 001/025[0]...
  TRACE: hid_close(): closing USB device 001/025[0]...
  TRACE: hid_close(): closing handle of USB device 001/025[0]...
 NOTICE: hid_close(): successfully closed USB device 001/025[0].
  TRACE: hid_close(): remove hidif from open list...
  TRACE: hid_reset_parser(): resetting the HID parser for USB device 001/025[0]...
  TRACE: hid_close(): freeing memory allocated for HID parser...
double free or corruption (!prev)
[1]    627735 abort      sudo ./test_libhid -d 17cc:1120
```
