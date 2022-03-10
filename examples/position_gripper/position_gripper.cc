// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License").
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "robotiq/robotiq_gripper_interface.h"

#include <iostream>

void print_feedback(const robotiq::GripperFeedback& feedback) {
  std::cout << "Gripper feedback:\n";
  std::cout << "  commanded_position:     " << feedback.commanded_position << "\n";
  std::cout << "  position:               " << feedback.position << "\n";
  std::cout << "  current:                " << feedback.current << "\n";
}

int main() {
  const std::string port = robotiq::DEFAULT_PORT;
  const std::size_t baud = robotiq::DEFAULT_BAUD;

  // Define the scale factors.  Here we map the position such that 0.086 is
  // fully open and 0 is fully closed.
  const double alpha = -0.086;
  const double beta = 0.086;

  // Open the serial port
  robotiq::RobotiqGripperInterface gripper;
  std::cout << "Connected: " << gripper.connect(port, baud, alpha, beta) << "\n";

  // Check if gripper is activated and activate otherwise
  if (not gripper.is_activated()) {
    std::cout << "Gripper is not activated... Activating...";
    gripper.activate();
  }
  std::cout << "Gripper is activated!\n";

  // Set the gripper to several positions
  std::cout << "Move to 0.043: " << gripper.set_gripper_position(0.043) << "\n";
  print_feedback(gripper.get_feedback());

  std::cout << "Move to 0: " << gripper.set_gripper_position(0) << "\n";
  print_feedback(gripper.get_feedback());

  std::cout << "Move to 0.086: " << gripper.set_gripper_position(0.086) << "\n";
  print_feedback(gripper.get_feedback());

  return 0;
}
