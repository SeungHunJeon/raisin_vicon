//
// Created by oem on 23. 2. 6.
//

#include "raisin_vicon/raisin_vicon.hpp"
//#include "raisim_message.hpp"

int main () {
  auto node = std::make_shared<raisin::vicon>();
  node->connect();
  for (int i = 0; i < 1000000000000; i++) {
    node->get_frame();
  }
  node->disconnect();

  return 0;
}

