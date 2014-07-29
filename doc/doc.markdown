% ARCA Linux Userspace Software Package

# Abstract
This software package implements all data handling and system management needed for the ARCA experiment. It is divided into several modules, following KISS and "one tool per task"-guidelines.

# Software functionality
The following functionality has to be handled by the software:

* saving incoming ADS-B data in files, together with time stamp
    * changing file every N minutes in case of unexpected crash
* TCP Socket for Up/Downlink Command Interface
    * read Temperature Sensors
    * reset FPGA
    * trigger reboot of the Linux ARM
    * sync the filesystem
* TCP Socket for Mode S Packet Downlink
    * periodically downlink ADS-B packets
* wrapper scripts to ensure software restart in case of crash

Further functionality is provided by the operating system, which supports SFTP for final backup of all files and SSH for remote shell access. Kernel drivers are used for temperature sensor reading, GPIOs are used for interfacing with the FPGA. The AT91 UART is used for data communication to the ARM.


