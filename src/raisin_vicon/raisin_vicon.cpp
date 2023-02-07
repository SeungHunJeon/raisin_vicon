//
// Created by oem on 23. 2. 6.
//

#include "raisin_vicon.hpp"

namespace raisin {
vicon::vicon():
  rclcpp::Node("vicon"),
  param_(parameter::ParameterContainer::getRoot()["vicon"]){
  param_.loadFromPackageParameterFile("raisin_vicon");
  this->host_address = static_cast<string>(param_("host_address"));
  this->buffer_size = static_cast<int>(param_("buffer_size"));
  createPublisher();
}
//
vicon::~vicon() {
//  delete this->client_;
//  delete this->cfg_;
}

bool vicon::connect() {

  while (!client_.IsConnected().Connected) {
    bool success = client_.Connect(host_address);
    if(success)
      break;
    RSINFO("Connecting")
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }


  client_.EnableSegmentData();
  client_.EnableMarkerData();
  RSFATAL_IF(client_.EnableMarkerData().Result != Result::Success, "Enable Marker Data Failed")
  RSFATAL_IF(client_.EnableSegmentData().Result != Result::Success, "Enable Segment Data Failed")
  client_.SetStreamMode(StreamMode::ClientPull);
  client_.SetBufferSize(buffer_size);
  return true;
}

bool vicon::disconnect() {
  if (!client_.IsConnected().Connected)
    return true;
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  client_.DisableMarkerData();
  client_.Disconnect();
  RSFATAL_IF(client_.IsConnected().Connected, "Disconnect Failed")
  return true;
}

void vicon::get_frame() {
  std::clock_t start, finish;
  double duration;

  start = clock();

  client_.GetFrame();
  Output_GetFrameNumber frame_number = client_.GetFrameNumber();
  RSINFO("Frame number : " << frame_number.FrameNumber)
  unsigned int subject_num = client_.GetSubjectCount().SubjectCount;
  RSINFO("subject num : " << subject_num)
  for (unsigned int subject_idx = 0; subject_idx < subject_num; subject_idx ++) {
    string subject_name = client_.GetSubjectName(subject_idx).SubjectName;
    RSINFO("subject name : " << subject_name)
    unsigned  int segment_num = client_.GetSegmentCount(subject_name).SegmentCount;
    for (unsigned  int segment_idx = 0; segment_idx < segment_num; segment_idx ++) {
      string segment_name = client_.GetSegmentName(subject_name, segment_idx).SegmentName;
      RSINFO("segment_name : " << segment_name)
      Output_GetSegmentGlobalTranslation trans =
          client_.GetSegmentGlobalTranslation(subject_name, segment_name);
      Output_GetSegmentGlobalRotationMatrix rot =
          client_.GetSegmentGlobalRotationMatrix(subject_name, segment_name);

      for(size_t i = 0; i<3; i++)
        pose.transition[i] = trans.Translation[i];
      for(size_t i = 0; i<9; i++)
        pose.rotation[i] = rot.Rotation[i];

      pose.subject_name = subject_name;
      pose.segment_name = segment_name;
      pose.frame_number = frame_number.FrameNumber;
      pose.transition_type = "Global";

//      RSINFO("translation : " << pose.translation)
//      RSINFO("rotation : " << pose.rotation)



    }
  }

  PublisherCallback();
  finish = clock();

  duration = (double)(finish - start) / CLOCKS_PER_SEC;

  RSINFO("time using : " << duration)
  RSINFO("----------------------------")
}

void vicon::createPublisher()
{
  rclcpp::QoS qos(rclcpp::KeepLast(1));

  rclcpp::Rate viconCommunicationRate(param_("vicon_streaming_rate"));
  {
    viconPublisher_ = this->create_publisher<raisin_vicon::msg::ViconPose>(
      "vicon/pose", qos);

    viconTimer_ = this->create_wall_timer(
      viconCommunicationRate.period(), std::bind(&vicon::PublisherCallback, this));
  }
}

void vicon::PublisherCallback() {
  auto message = raisin_vicon::msg::ViconPose();
//  get_frame();

  message.transition_x = pose.transition[0];
  message.transition_y = pose.transition[1];
  message.transition_z = pose.transition[2];
  message.rotation_0 = pose.rotation[0];
  message.rotation_1 = pose.rotation[1];
  message.rotation_2 = pose.rotation[2];
  message.rotation_3 = pose.rotation[3];
  message.rotation_4 = pose.rotation[4];
  message.rotation_5 = pose.rotation[5];
  message.rotation_6 = pose.rotation[6];
  message.rotation_7 = pose.rotation[7];
  message.rotation_8 = pose.rotation[8];

  viconLoopBegin_ = std::chrono::high_resolution_clock::now();
  viconPublisher_->publish(message);
  viconLoopEnd_ = std::chrono::high_resolution_clock::now();
}

}




