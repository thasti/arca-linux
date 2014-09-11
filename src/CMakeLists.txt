include_directories(${arca_SOURCE_DIR}/inc)
include_directories(${arca_SOURCE_DIR}/lib/inc)
link_directories(${arca_BINARY_DIR}/src)

add_executable(data_srv udp.cpp data_srv.cpp)
add_executable(health_srv udp.cpp health_srv.cpp)
add_executable(tlm_srv tlm_srv.cpp)

target_link_libraries(tlm_srv tcpsockets)
