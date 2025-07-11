# Mbed OS Modules

Mbed OS-based modules providing common functionalities and a standardized way of
structuring the firmware.

By using these, the programmer can focus solely on the application
particularities.

## Directory structure

```
|-- Module
|       Base class of all modules.
|
|-- examples
|       Usage examples.
|
|-- intfs
|   |   Interface modules.
|   |
|   |-- TCPFwUpdateModule
|   |      Remote firmware update interface modules.
|   |
|   |-- ctrl
|   |   |-- CtrlIntfModule
|   |   |       Base class of text-based control interface modules.
|   |   |
|   |   |-- TCPCtrlIntfModule
|   |   |       TCP text-based control interface module.
|   |   |
|   |   |-- UARTCtrlIntfModule
|   |   |       UART text-based control interface module.
|
|-- scripts
|   |-- intfs
|   |   |-- TCPFwUpdateModule
|   |   |       Script for interfacing with TCPFwUpdateModule.
```

## Firmware architecture

![Image](https://github.com/user-attachments/assets/9dc9eccb-181a-4c71-87b4-b7fec71c77a6)

`CtrlCoreModule` is the module responsible for receiving, parsing, acting on and
responding to requests (commands) coming from the interface modules.

_`TCPStreamIntfModule`_ is currently under development.

## Modules' inheritance diagram

![Image](https://github.com/user-attachments/assets/c937321d-7b6b-4837-a500-573317237896)

## How to use it

Add this repository as a submodule of your application.

	git submodule add git@github.com:cnpem/mbed-os-mods.git


