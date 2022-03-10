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

// Define the communication parameters
std::string port = robotiq::DEFAULT_PORT;
std::size_t baud = robotiq::DEFAULT_BAUD;

bool parse_args(int argc, char* argv[]) {
  for (int i = 1; i < argc; i += 2) {
    if (std::string(argv[i]) == "--help") {
      std::cout << "  --port <value> Optional serial port ID\n";
      std::cout << "  --baud <value> Optional baud rate\n";
      return false;
    } else if (std::string(argv[i]) == "--port") {
      port = std::string(argv[i + 1]);
    } else if (std::string(argv[i]) == "--baud") {
      baud = std::atoi(argv[i + 1]);
    }
  }
  return true;
}

int main(int argc, char* argv[]) {
  // Load the args
  if (not parse_args(argc, argv)) {
    return 0;
  }

  // Open the serial port
  robotiq::RobotiqGripperInterface gripper;
  std::cout << "Connected: " << gripper.connect(port, baud) << "\n";

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
