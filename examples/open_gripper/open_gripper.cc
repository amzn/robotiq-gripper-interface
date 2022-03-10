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

int main() {
  const std::string port = "/dev/ttyUSB0";

  // Open the serial port
  robotiq::RobotiqGripperInterface gripper;
  std::cout << "Connected: " << gripper.connect(port) << "\n";

  // Check if gripper is activated and activate otherwise
  if (not gripper.is_activated()) {
    std::cout << "Gripper is not activated... Activating...";
    gripper.activate();
  }
  std::cout << "Gripper is activated!\n";

  // Open the gripper
  std::cout << "Open gripper: " << gripper.open_gripper() << "\n";

  return 0;
}
