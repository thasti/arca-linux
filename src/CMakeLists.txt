include_directories(${arca_SOURCE_DIR}/inc)
include_directories(${arca_SOURCE_DIR}/lib/inc)
link_directories(${arca_BINARY_DIR}/src)

add_executable(data_srv mq.cpp data_srv.cpp)
add_executable(health_srv mq.cpp health_srv.cpp)
add_executable(tlm_srv mq.cpp tlm_srv.cpp)

target_link_libraries(tlm_srv tcpsockets)
target_link_libraries(data_srv -lrt)
target_link_libraries(health_srv -lrt)
target_link_libraries(tlm_srv -lrt)
