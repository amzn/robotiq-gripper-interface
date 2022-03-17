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

#include <string>

#include <boost/asio.hpp>

using namespace boost;

namespace robotiq {

/** Reads serial messages with a dedicated timeout */
class TimeoutReader {
 public:
  TimeoutReader(asio::serial_port& serial, std::size_t timeout_ms);
  bool read_char(char& c);

 private:
  void read_complete(const system::error_code& error, std::size_t bytes);
  void timeout(const system::error_code& error);
  asio::serial_port& m_serial;
  std::size_t m_timeout_ms;
  asio::deadline_timer m_timer;
  bool m_read_error;
};

}  // namespace robotiq
