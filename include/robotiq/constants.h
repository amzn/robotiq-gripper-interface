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

namespace robotiq {

/** \brief Default port for MODBUS RTU (serial) communication, assumes Ubuntu */
const std::string DEFAULT_PORT = "/dev/ttyUSB0";

/** \brief Default port for MODBUS RTU (serial) communication */
const std::size_t DEFAULT_BAUD = 115200;

/** \brief Default inactivity timeout*/
const std::size_t DEFAULT_RECEIVE_TIMEOUT_MS = 200;

/** \brief Default slope scale factor */
const double DEFAULT_SCALE_ALPHA = 1;

/** \brief Default zero crossing scale factor */
const double DEFAULT_SCALE_BETA = 0;

}  // namespace robotiq
