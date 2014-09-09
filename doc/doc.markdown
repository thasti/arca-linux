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

# Mission Software modules
should not use threading because of unnecesary complexity and potential problems, only simple algirithms. implemented in c++, network communication based on TCPsockets-library.

* starter script
    * starts all 4 software modules, monitors their status (via PID) and restarts them if they crash
    * blinks the ARCA front LED every second)
    * (optional: activates and resets the watchdog)

* data\_srv
    * reads ADS-B frames (\* to newline) from UART and saves them in a file
    * start a new file every N (N=100) frames (filename = arca-data-datetime-index)
    * notifies data\_tlm\_srv with every new frame

* health\_srv
    * saves all temperature sensor data to file every N (N=5) seconds
    * uses a new file every N (N=50) measurements (filename = arca-health-datetime-index)
    * notifies health\_tlm\_srv with every new measurement

* data\_tlm\_srv
    * socket on port A
    * if client connected, reads N (N=20) ads-b frames from data\_srv, makes a packet and sends it to the client
    * if no client connected, discards the received frames

* health\_tlm\_srv
    * socket on port B
    * if client connected, sends all temperature sensor data to client every N (N=5) seconds
    * if no client connected, discards the data

* uplink\_tlm\_srv
    * socket on port C
    * accepts tlm commands (reset FPGA, reset ARM, sync) and executes them.
    * sends acknoledgement packet

questions: which IPC method to use (mailbox, message queue, pipe?) - should be crash resistant (named?). scenario: server-client communicate via IPC. i) server crashes, restarts, can instantly write to (best) the same IPC method and the client receives the messages. ii) client crashes, restarts, can instantly read (new data) from same IPC method, data while crashed is lost

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
