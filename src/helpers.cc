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

#include <iomanip>

#include <boost/crc.hpp>

#include "src/helpers.h"
#include "src/timeout_reader.h"

namespace robotiq {

std::string write_read(boost::asio::serial_port& serial, const std::string& message,
                       std::size_t timeout_ms) {
  // write
  write(serial, message);

  // read
  TimeoutReader reader(serial, timeout_ms);
  char c;
  std::string result;
  while (reader.read_char(c) && c != '\n') {
    result += c;
  }
  return bin_to_hex(result);
}

void write(boost::asio::serial_port& serial, const std::string& message) {
  std::string binary_msg = hex_to_bin(message);
  asio::write(serial, asio::buffer(binary_msg.c_str(), binary_msg.size()));
}

std::string bin_to_hex(const std::string& input) {
  const static std::string hex_codes = "0123456789ABCDEF";
  std::string hex_string;
  for (std::size_t i = 0; i < input.length(); ++i) {
    unsigned char bin_value = input[i];
    hex_string += hex_codes[(bin_value >> 4) & 0x0F];
    hex_string += hex_codes[bin_value & 0x0F];
  }
  return hex_string;
}

std::string hex_to_bin(const std::string& input) {
  std::string bin_string;
  for (std::size_t i = 0; i < input.length() - 1; i += 2) {
    bin_string += (input[i] >= 'A' ? input[i] - 'A' + 10 : input[i] - '0') * 16 +
                  (input[i + 1] >= 'A' ? input[i + 1] - 'A' + 10 : input[i + 1] - '0');
  }
  return bin_string;
}

std::string uint8_to_hex(uint8_t value) {
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(2) << std::hex << unsigned(value);
  std::string str = stream.str();
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

std::string crc16_modbus(const std::string& input) {
  // Pad the input with zeros every other character to be able to use stringstream
  std::string msg = "";
  for (std::size_t i = 0; i < input.size() / 2; ++i) {
    msg += input.substr(2 * i, 2) + " ";
  }

  // Convert the string to an array of bytes
  std::istringstream istream(msg);
  std::vector<unsigned char> bytes;
  unsigned int c;
  while (istream >> std::hex >> c) {
    bytes.push_back(c);
  }
  std::size_t const buffer_len = input.size() / 2;
  unsigned char buffer[buffer_len];
  std::copy(bytes.begin(), bytes.end(), buffer);

  // Create the modbus crc computer and compute the crc
  typedef boost::crc_optimal<16, 0x8005, 0xFFFF, 0, true, true> crc_modbus_type;
  crc_modbus_type crc;
  crc.process_bytes(buffer, buffer_len);

  // Return the hex string of the crc code
  std::stringstream stream;
  stream << std::setfill('0') << std::setw(4) << std::hex << ntohs(crc.checksum());
  std::string str = stream.str();
  std::transform(str.begin(), str.end(), str.begin(), ::toupper);
  return str;
}

}  // namespace robotiq
