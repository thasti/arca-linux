include_directories(${arca_SOURCE_DIR}/lib/inc)
add_library(tcpsockets tcpacceptor.cpp tcpconnector.cpp tcpstream.cpp)
