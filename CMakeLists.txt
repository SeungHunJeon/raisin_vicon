cmake_minimum_required(VERSION 3.5)
project(raisin_vicon)

# Default to C99
if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

# Default to C++17
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

set(VICON_SDK VICON_STREAM_SDK)
set(VICON_DIR third_party/${VICON_SDK})
set(VICON_CPP ViconDataStreamSDK_CPP)
set(LOGGER_DIR third_party/raisim_logger)
set(EXEC_NAME ${PROJECT_NAME}_node)

#include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/download_mscl.cmake)
#download_mscl(VERSION "62.1.2")

find_package(ament_cmake REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread)
find_package(rosidl_default_generators REQUIRED)
find_package(raisin_raisimlib REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(raisin_data_logger REQUIRED)
find_package(raisin_parameter REQUIRED)
find_package(raisin_interfaces REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
        "msg/ViconPose.msg"
        DEPENDENCIES sensor_msgs
        )

add_library(${VICON_CPP} SHARED IMPORTED)

set_property(TARGET ${VICON_CPP} PROPERTY IMPORTED_LOCATION
  ${CMAKE_CURRENT_SOURCE_DIR}/${VICON_DIR}/libViconDataStreamSDK_CPP.so
)

install(FILES config/params.yaml
        DESTINATION share/${PROJECT_NAME})

install(FILES
  ${VICON_DIR}/DataStreamClient.h
  ${VICON_DIR}/IDataStreamClientBase.h
  DESTINATION include/${PROJECT_NAME}
)

install(FILES
  ${VICON_DIR}/libViconDataStreamSDK_CPP.so
  ${VICON_DIR}/libboost_chrono.so.1.68.0
  ${VICON_DIR}/libboost_system.so.1.68.0
  ${VICON_DIR}/libboost_thread.so.1.68.0
  ${VICON_DIR}/libboost_timer.so.1.68.0
  DESTINATION lib
)

#function(create exec_name target_name)
#  add_library(${exec_name} STATIC
#          "src/${target_name}.cpp")
#  set_target_properties(${exec_name} PROPERTIES LINKER_LANGUAGE CXX)
#  target_include_directories(${exec_name} PUBLIC "${VICON_DIR}")
#  target_include_directories(${exec_name} PUBLIC "${LOGGER_DIR}")
#  #  target_link_libraries(${exec_name} PUBLIC ${Boost_LIBRARIES})
#  target_link_libraries(${exec_name} ${CMAKE_CURRENT_SOURCE_DIR}/${VICON_DIR}/libViconDataStreamSDK_CPP.so)
#  if (CMAKE_BUILD_TYPE STREQUAL "RELEASE" OR CMAKE_BUILD_TYPE STREQUAL "Release")
#    target_compile_options(${exec_name} PRIVATE -mtune=native -fPIC -O3 -g -march=native)
#  endif()
#endfunction()
#
#create(vicon ${PROJECT_NAME}/${PROJECT_NAME})

#install(TARGETS vicon
#        LIBRARY DESTINATION lib)

#ament_export_include_directories(include)
#ament_export_libraries(${VICON_CPP})

add_executable(${EXEC_NAME}
        src/main.cpp
        src/${PROJECT_NAME}/raisin_vicon.cpp)
#
include_directories(${EXEC_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/${VICON_DIR}")
include_directories(${EXEC_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/${LOGGER_DIR}")
include_directories(${EXEC_NAME} "/home/oem/workspace/raisin_ws/install/raisin_vicon/include")

target_link_libraries(${EXEC_NAME}
        ${MSCL_LIB_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/${VICON_DIR}/libViconDataStreamSDK_CPP.so
        )

rosidl_target_interfaces(${EXEC_NAME}
        ${PROJECT_NAME}
        "rosidl_typesupport_cpp"
        )
#target_include_directories(${EXEC_NAME}
#        PUBLIC
#          ${VICON_DIR}
#          ${LOGGER_DIR})

ament_target_dependencies(${EXEC_NAME}
        rclcpp
        sensor_msgs
        ament_index_cpp
        Eigen3
        raisin_data_logger
        raisin_parameter
        raisin_interfaces
        )

install(TARGETS ${EXEC_NAME} DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_lint_cmake_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
