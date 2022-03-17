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
#include <string>

#include <boost/asio/serial_port.hpp>

namespace robotiq {

/** Writes a message to the serial port and returns the received message */
std::string write_read(boost::asio::serial_port& serial, const std::string& message,
                          std::size_t timeout_ms);

/** Writes a message to the serial port and does not wait for a response*/
void write(boost::asio::serial_port& serial, const std::string& message);

/** Converts a binary string to a hexidecimal string */
std::string bin_to_hex(const std::string& input);

/** Converts a hexidecimal string to a binary string*/
std::string hex_to_bin(const std::string& input);

/** Converts an integer value to a fixed width hex string*/
std::string uint8_to_hex(uint8_t value);

/** Computes the modbus CRC (cyclic redundancy check) for a hexidecimal string*/
std::string crc16_modbus(const std::string& input);

}  // namespace robotiq
