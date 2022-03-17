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
#include "src/helpers.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

using namespace boost;

namespace robotiq {

// Messages for reading holding registers (FC03 from the manual)
static std::string READ_FEEDBACK = "090307D00003040E";

// Messages for preseting multiple registers (FC16 from the manual)
static std::string PRESET_RESET = "091003E80003060000000000007330";
static std::string PRESET_ACTIVATE = "091003E800030601000000000072E1";

// Position commands are prefixed by preset for multiple registers (FC16 from the manual)
// and a postfix to set max current and velocity.  The word is ended with a CRC check on
// the unique message.
static std::string PRESET_POSITION_PREFIX = "091003E8000306090000";
static std::string PRESET_POSITION_POSTFIX = "FFFF";

// Expected response for preset messages
static std::string PRESET_RESPONSE = "091003E800030130";

struct RobotiqGripperInterface::Implementation {
  Implementation();
  bool is_connected{false};
  asio::io_service m_io_service;
  asio::serial_port m_serial;
  std::size_t m_timeout_ms;
  double m_scale_alpha;
  double m_scale_beta;
};

RobotiqGripperInterface::Implementation::Implementation()
    : m_serial(m_io_service), m_timeout_ms{DEFAULT_RECEIVE_TIMEOUT_MS} {}

RobotiqGripperInterface::RobotiqGripperInterface()
    : m_impl{std::make_unique<Implementation>()} {}

RobotiqGripperInterface::~RobotiqGripperInterface() {
  if (m_impl->is_connected) {
    m_impl->m_serial.close();
  }
}

bool RobotiqGripperInterface::connect(const std::string& port,
                                      std::size_t baud,
                                      double scale_alpha,
                                      double scale_beta) {
  m_impl->m_scale_alpha = scale_alpha;
  m_impl->m_scale_beta = scale_beta;

  if (m_impl->is_connected) {
    m_impl->m_serial.close();
  }

  system::error_code error;
  m_impl->m_serial.open(port, error);
  if (error) {
    std::cout << "RobotiqGripperInterface::connect failed with error: " << error.message()
              << "\n";
    return m_impl->is_connected;
  }

  m_impl->is_connected = true;
  m_impl->m_serial.set_option(asio::serial_port_base::baud_rate(baud));
  m_impl->m_serial.set_option(asio::serial_port_base::character_size(8));
  m_impl->m_serial.set_option(
      asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
  m_impl->m_serial.set_option(
      asio::serial_port_base::parity(asio::serial_port_base::parity::none));
  return m_impl->is_connected;
}

bool RobotiqGripperInterface::reset(bool blocking) {
  if (not m_impl->is_connected) {
    std::cout << "[RobotiqGripperInterface] Warning: reset() ignored since the gripper "
                 "is not connected\n";
    return m_impl->is_connected;
  }

  std::string r = write_read(m_impl->m_serial, PRESET_RESET, m_impl->m_timeout_ms);
  if (r.compare(PRESET_RESPONSE) != 0) {
    return false;
  }

  if (blocking) {
    bool done = false;
    while (not done) {
      GripperFeedback y = get_feedback();
      done = y.status.gact == ActivationStatus::NOT_ACTIVATED ? true : false;
    }
  }

  return true;
}

bool RobotiqGripperInterface::is_activated() {
  if (not m_impl->is_connected) {
    std::cout
        << "[RobotiqGripperInterface] Warning: activate() ignored since the gripper "
           "is not connected\n";
    return m_impl->is_connected;
  }
  GripperFeedback y = get_feedback();
  return (y.status.gact == ActivationStatus::ACTIVATED);
}

bool RobotiqGripperInterface::activate(bool blocking) {
  if (not m_impl->is_connected) {
    std::cout
        << "[RobotiqGripperInterface] Warning: activate() ignored since the gripper "
           "is not connected\n";
    return m_impl->is_connected;
  }

  std::string r = write_read(m_impl->m_serial, PRESET_ACTIVATE, m_impl->m_timeout_ms);
  if (r.compare(PRESET_RESPONSE) != 0) {
    return false;
  }

  if (blocking) {
    bool done = false;
    while (not done) {
      GripperFeedback y = get_feedback();
      done = y.status.gact == ActivationStatus::ACTIVATED ? true : false;
    }
  }

  // the activated flag seems to go high early
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  return true;
}

bool RobotiqGripperInterface::close_gripper(bool blocking) {
  return set_raw_gripper_position(255, blocking);
}

bool RobotiqGripperInterface::open_gripper(bool blocking) {
  return set_raw_gripper_position(0, blocking);
}

bool RobotiqGripperInterface::set_gripper_position(double position, bool blocking) {
  return set_raw_gripper_position(position_to_word(position), blocking);
}

GripperFeedback RobotiqGripperInterface::get_feedback() {
  GripperFeedback feedback;
  if (not m_impl->is_connected) {
    std::cout
        << "[RobotiqGripperInterface] Warning: get_feedback() ignored since the gripper "
           "is not connected\n";
    return feedback;
  }

  std::string r = write_read(m_impl->m_serial, READ_FEEDBACK, m_impl->m_timeout_ms);
  if (r.size() != 22) {
    std::cout << "[RobotiqGripperInterface] Warning: get_feedback() returned an "
                 "unexpected number of bytes, consider increasing the timeout setting\n";
    return feedback;
  }

  // Note: bit masking is derived from the tables in Section 4.4 of the manual.
  unsigned char byte0 = *hex_to_bin(r.substr(6, 2)).c_str();
  feedback.status.gobj = static_cast<ObjectStatus>((byte0 & 0xC0) >> 6);      // bits 7-6
  feedback.status.gsta = static_cast<FingerStatus>((byte0 & 0x30) >> 4);      // bits 5-4
  feedback.status.ggto = static_cast<ActionStatus>((byte0 & 0x08) >> 3);      // bits 3
  feedback.status.gact = static_cast<ActivationStatus>((byte0 & 0x01) >> 0);  // bits 0

  unsigned byte2 = *hex_to_bin(r.substr(10, 2)).c_str();
  unsigned gflt = static_cast<unsigned>(byte2 & 0x0F);  // bits 3-0
  switch (gflt) {
    case 0:
      feedback.status.gflt = FaultStatus::NONE;
      break;
    case 5:
      feedback.status.gflt = FaultStatus::ACTION_DELAYED;
      break;
    case 7:
      feedback.status.gflt = FaultStatus::ACTIVATION_NEEDED;
      break;
    case 8:
      feedback.status.gflt = FaultStatus::MAX_TEMP_EXCEEDED;
      break;
    case 9:
      feedback.status.gflt = FaultStatus::COMM_TIMEOUT;
      break;
    case 10:
      feedback.status.gflt = FaultStatus::UNDER_VOLTAGE;
      break;
    case 11:
      feedback.status.gflt = FaultStatus::AUTOMATIC_RELEASE_IN_PROGRESS;
      break;
    case 12:
      feedback.status.gflt = FaultStatus::INTERNAL_FAULT;
      break;
    case 13:
      feedback.status.gflt = FaultStatus::ACTIVATION_FAULT;
      break;
    case 14:
      feedback.status.gflt = FaultStatus::OVERCURRENT;
      break;
    case 15:
      feedback.status.gflt = FaultStatus::AUTOMATIC_RELEASE_COMPLETED;
      break;
    default:
      feedback.status.gflt = FaultStatus::UNKNOWN;
  }

  // Access the streaming feedback values
  unsigned byte3 = *hex_to_bin(r.substr(12, 2)).c_str();
  uint8_t pos_cmd = static_cast<uint8_t>(byte3 & 0xFF);
  feedback.raw_commanded_position = pos_cmd;
  feedback.commanded_position = word_to_position(pos_cmd);

  unsigned byte4 = *hex_to_bin(r.substr(14, 2)).c_str();
  uint8_t pos_fbk = static_cast<uint8_t>(byte4 & 0xFF);
  feedback.raw_position = pos_fbk;
  feedback.position = word_to_position(pos_fbk);

  unsigned byte5 = *hex_to_bin(r.substr(16, 2)).c_str();
  unsigned cur_fbk = static_cast<unsigned>(byte5 & 0xFF);
  feedback.current = static_cast<double>(cur_fbk) / 255.0;

  return feedback;
}

void RobotiqGripperInterface::set_timeout(std::size_t timeout_ms) {
  m_impl->m_timeout_ms = timeout_ms;
}

std::size_t RobotiqGripperInterface::get_timeout() const { return m_impl->m_timeout_ms; }

bool RobotiqGripperInterface::set_raw_gripper_position(uint8_t position, bool blocking) {
  if (not m_impl->is_connected) {
    std::cout << "[RobotiqGripperInterface] Warning: set_raw_gripper_position() ignored "
                 "since the gripper is not connected\n";
    return m_impl->is_connected;
  }

  // Create the message and append the modbus CRC check
  std::string message =
      PRESET_POSITION_PREFIX + uint8_to_hex(position) + PRESET_POSITION_POSTFIX;
  message += crc16_modbus(message);

  if (blocking) {
    std::string r = write_read(m_impl->m_serial, message, m_impl->m_timeout_ms);
    if (r.compare(PRESET_RESPONSE) != 0) {
      return false;
    }

    bool done = false;
    while (not done) {
      GripperFeedback y = get_feedback();
      done = y.status.gobj == ObjectStatus::IN_MOTION ? false : true;
    }
  } else {
    write(m_impl->m_serial, message);
  }
  
  return true;
}

double RobotiqGripperInterface::word_to_position(uint8_t word) const {
  return (m_impl->m_scale_alpha / 255.0) * static_cast<double>(word) + m_impl->m_scale_beta;
}

uint8_t RobotiqGripperInterface::position_to_word(double position) const {
  double scaled_position = 255.0 / m_impl->m_scale_alpha * (position - m_impl->m_scale_beta);
  scaled_position = std::max(scaled_position, 0.0);
  scaled_position = std::min(scaled_position, 255.0);
  return static_cast<uint8_t>(scaled_position);
}

}  // namespace robotiq
