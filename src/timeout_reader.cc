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

#include "src/timeout_reader.h"

#include <boost/bind.hpp>

namespace robotiq {

TimeoutReader::TimeoutReader(asio::serial_port& serial, std::size_t timeout_ms)
    : m_serial(serial),
      m_timeout_ms(timeout_ms),
      m_timer(serial.get_io_service()),
      m_read_error{true} {}

bool TimeoutReader::read_char(char& c) {
  c = '\0';

  // After a timeout & cancel it seems we need
  // to do a reset for subsequent reads to work.
  m_serial.get_io_service().reset();

  // Asynchronously read 1 character.
  boost::asio::async_read(
      m_serial, asio::buffer(&c, 1),
      bind(&TimeoutReader::read_complete, this, asio::placeholders::error,
           asio::placeholders::bytes_transferred));

  // Setup a deadline time to implement our timeout.
  m_timer.expires_from_now(posix_time::milliseconds(m_timeout_ms));
  m_timer.async_wait(bind(&TimeoutReader::timeout, this, asio::placeholders::error));

  // This will block until a character is read
  // or until it is cancelled.
  m_serial.get_io_service().run();

  if (m_read_error) {
    c = '\0';
  }

  return not m_read_error;
}

void TimeoutReader::read_complete(const system::error_code& error, std::size_t bytes) {
  m_read_error = (error || bytes == 0);
  m_timer.cancel();
}

void TimeoutReader::timeout(const system::error_code& error) {
  if (error) {
    return;
  }
  m_serial.cancel();
}

}  // namespace robotiq
