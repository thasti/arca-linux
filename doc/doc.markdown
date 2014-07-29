% ARCA Linux Userspace Software Package

# Abstract
This software package implements all data handling and system management needed for the ARCA experiment. It is divided into several modules, following KISS and "one tool per task"-guidelines.

# Software functionality
The following functionality has to be handled by the software:

* TCP Socket for Up/Downlink Command Interface
    * read Temperature Sensors
    * reset FPGA
    * trigger reboot of the Linux ARM
    * sync the filesystem
* TCP Socket for Mode S Packet Downlink
    * periodically downlink ADS-B packets together with a unix timestamp

Further functionality is given by the operating system, which provides SFTP for backup of all data and SSH for remote shell access.


