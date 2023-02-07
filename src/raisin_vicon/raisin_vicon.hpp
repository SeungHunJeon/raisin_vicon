//
// Created by oem on 23. 2. 6.
//

#ifndef VICON_SRC_VICON_HPP_
#define VICON_SRC_VICON_HPP_

#include "DataStreamClient.h"
#include "iostream"
#include <string>
#include "raisim/raisim_message.hpp"
#include "thread"
#include "eigen3/Eigen/Core"
#include "time.h"
#include <rclcpp/rclcpp.hpp>
#include "raisin_vicon/msg/vicon_pose.hpp"
#include "raisin_parameter/parameter_container.hpp"
#include "std_msgs/msg/empty.hpp"
#include "std_msgs/msg/int16.hpp"
//#include "rai
using namespace std;
using namespace ViconDataStreamSDK::CPP;

struct PoseStruct
{
  float transition[3];
  float rotation[9];
  std::string subject_name;
  std::string segment_name;
  std::string transition_type;
  unsigned int frame_number;
} typedef PoseStruct;

namespace raisin {
class vicon final: public rclcpp::Node{
 public:
  vicon();

  ~vicon();

  bool connect();

  bool disconnect();

  void get_frame();

  void createPublisher();

  void PublisherCallback();

 private:
  ViconDataStreamSDK::CPP::Client client_;
  string host_address;
  unsigned int buffer_size;
  rclcpp::Publisher<raisin_vicon::msg::ViconPose>::SharedPtr viconPublisher_;
  rclcpp::TimerBase::SharedPtr viconTimer_;
  parameter::ParameterContainer & param_;
  std::chrono::time_point<std::chrono::high_resolution_clock> viconLoopBegin_;
  std::chrono::time_point<std::chrono::high_resolution_clock> viconLoopEnd_;
  PoseStruct pose;
};
}


#endif //VICON_SRC_MAIN_HPP_
