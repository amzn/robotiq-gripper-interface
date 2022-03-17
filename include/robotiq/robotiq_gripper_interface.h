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

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "robotiq/constants.h"
#include "robotiq/types.h"

namespace robotiq {

/**
 * @brief This class is the simplified interface to the Robotiq Adaptive gripper.  The
 * Robotiq manual is available here:
 * https://assets.robotiq.com/website-assets/support_documents/document/2F-85_2F-140_Instruction_Manual_e-Series_PDF_20190206.pdf
 *
 * Note that the class was tested with a 2F-85 2-finger gripper, Robotiq pinout to RS-485
 * board, an RS-485 serial to usb converter, and a Z6 workstation running Ubuntu 16.04.
 */
class RobotiqGripperInterface {
 public:
  explicit RobotiqGripperInterface();
  RobotiqGripperInterface(const RobotiqGripperInterface&) = delete;
  RobotiqGripperInterface(RobotiqGripperInterface&&) = delete;
  RobotiqGripperInterface& operator=(const RobotiqGripperInterface& other) = delete;
  RobotiqGripperInterface& operator=(RobotiqGripperInterface&& other) = delete;
  virtual ~RobotiqGripperInterface();

  /**
   * @brief Connects to the gripper over MODBUS RTU.  This connection uses a serial port
   * with RS-485.  Note that the default port value assumes a Linux box with a serial to
   * usb converter.
   *
   * The gripper can be accessed independently on a Windows machine using the Robotiq User
   * Interface application.
   *
   * Effect of scale factors:
   *   $$y = (alpha / 255) * u + beta$$
   *   $$u = (255 / alpha) * (y - beta)$$
   * where:
   *   $u$ is the commanded position message (uint8), range: [0, 255]
   *   $y$ is the scaled position value (double), range based on alpha, beta.
   *
   * Example 1: Default, y = [0: opened, 1: closed]
   *   alpha = 1, beta = 0
   *
   * Example 2: y = [0.086: opened, 0: closed]
   *   alpha = -0.086, beta = 0.086
   *
   * @param[in] port  Serial port for communication (Ubuntu default: /dev/ttyUSB0)
   * @param[in] baud  Baud rate (default: 115200)
   * @param[in] scale_alpha Linear slope factor for position scaling
   * @param[in] scale_beta Linear zero crossing factor for position scaling
   * @return True if succeeded.
   */
  bool connect(const std::string& port = DEFAULT_PORT, std::size_t baud = DEFAULT_BAUD,
               double scale_alpha = DEFAULT_SCALE_ALPHA,
               double scale_beta = DEFAULT_SCALE_BETA);

  /**
   * @brief Resets (deactivates) the gripper.
   *
   * @param[in]  blocking  Waits to return until the gripper has completed the action.
   * @return True if succeeded.
   */
  bool reset(bool blocking = true);

  /**
   * @brief Activates the gripper, which will cause the gripper to move.
   *
   * @param[in]  blocking  Waits to return until the gripper has completed the action.
   * @return True if succeeded.
   */
  bool activate(bool blocking = true);

  /**
   * @brief Checks if the gripper is activated.
   *
   * @return True if activated.
   */
  bool is_activated();

  /**
   * @brief Closes the gripper until position reached or obstacle encountered.
   *
   * @param[in]  blocking  Waits to return until the gripper has completed the action.
   * @return True if succeeded.
   */
  bool close_gripper(bool blocking = true);

  /**
   * @brief Opens the gripper until position reached or obstacle encountered.
   *
   * @param[in]  blocking  Waits to return until the gripper has completed the action.
   * @return True if succeeded.
   */
  bool open_gripper(bool blocking = true);

  /**
   * @brief Sets the gripper position.
   *
   * @param[in]  position  Desired position, scaled by the scale factors
   * @param[in]  blocking  Waits to return until the gripper has completed the action.
   * @return True if succeeded.
   */
  bool set_gripper_position(double position, bool blocking = true);

  /**
   * @brief Returns the gripper feedback.
   */
  GripperFeedback get_feedback();

  /**
   * @brief Sets the time out in ms for receiving messages from the gripper.
   */
  void set_timeout(std::size_t timeout_ms);

  /**
   * @brief Returns the time out in ms for receiving messages from the gripper.
   */
  std::size_t get_timeout() const;

 private:
  /** Writes the raw word (unscaled) to position */
  bool set_raw_gripper_position(uint8_t position, bool blocking);

  /** Scales the raw word to position */
  double word_to_position(uint8_t word) const;

  /** Scales the position to raw word */
  uint8_t position_to_word(double position) const;

  // Pointer to implementation idiom is used to hide implementation from consumers
  struct Implementation;
  std::unique_ptr<Implementation> m_impl;
};

}  // namespace robotiq
