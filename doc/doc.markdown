% ARCA Linux Userspace Software Package

# Abstract
This software package implements all data handling and system management needed for the ARCA experiment. It is divided into several modules, following KISS and "one tool per task"-guidelines.

# Software functionality
The following functionality has to be handled by the software:

* saving incoming ADS-B data in files, together with time stamp
    * changing file every N minutes in case of unexpected crash
* TCP Socket for Up/Downlink Command Interface
    * reset FPGA
    * trigger reboot of the Linux ARM
    * sync the filesystem
* UDP Socket for Mode S Packet Downlink
    * periodically downlink ADS-B packets
* UDP Socket for Health data downlink
* wrapper scripts to ensure software restart in case of crash

Further functionality is provided by the operating system, which supports SFTP for final backup of all files and SSH for remote shell access. Kernel drivers are used for temperature sensor reading, GPIOs are used for interfacing with the FPGA. The AT91 UART is used for data communication to the ARM.

# Mission Software modules
should not use threading because of unnecesary complexity and potential problems, only simple algirithms. implemented in c++, network communication based on TCPsockets-library and UDP sockets.

* starter script
    * starts all 4 software modules, monitors their status (via PID) and restarts them if they crash
    * blinks the ARCA front LED every second)
    * (optional: activates and resets the watchdog)

* data\_srv
    * reads ADS-B frames (\* to newline) from UART and saves them in a file
    * start a new file every N seconds (filename = data-mac-index.txt)
    * sends a UDP packet containing time, number of frames and Mode S frames every M frames

* health\_srv
    * saves all temperature sensor data to file every N (N=5) seconds
    * uses a new file every N seconds (filename = health-mac-index.txt)
    * sends a UDP packet containing timestamp and all measurements

* tlm\_srv
    * socket on port C
    * accepts tlm commands (reset FPGA, reset ARM, sync) and executes them.
    * sends acknoledgement packet, error handling

# Mission control software (Ground station)
can use threading as software failure (caused by race conditions etc) is more tolerable, even though not encouraged. The three modules described below could also be realised as three different programs.

* ground station software
    * connects to port A, on reception of packets:
        * updates statistics (display)
        * sends data to planeplotter
	    * has to host a TCP socket for planeplotter to connect to - send AVR-format
        * saves to file (filename: arca-data-datetime.txt)
    * connects to port B, on reception of packets:
        * displays temperature values
        * saves to file (filename: arca-health-datetime.txt)
    * connects to port C, on button-pressing (tlm command)
        * issues command to experiment
        * logs command to file with date, time (filename: arca-tlm-datetime.txt)
    * UI: graphs for temperatures, statistics (frame count, frames per second), buttons for TLM command (with confirmation questions)
