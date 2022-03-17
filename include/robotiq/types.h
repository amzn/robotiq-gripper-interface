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

#include "robotiq/constants.h"

namespace robotiq {

/*
 * The Robotiq manual is available here:
 * https://assets.robotiq.com/website-assets/support_documents/document/2F-85_2F-140_Instruction_Manual_e-Series_PDF_20190206.pdf
 */

/** Basic status based on gACT, gGTO, and gSTA bits defined in 4.4 of the manual */
enum BasicStatus {
  NOT_CONNECTED, /** Returned if the gripper is not connected */
  RESET,         /** Activation needs to be run */
  ACTIVATING,    /** Activation is in progress */
  READY,         /** Ready to receive a command */
  MOVING,        /** In motion */
};

/** Corresponds to gACT defined in 4.4 of the manual */
enum ActivationStatus {
  NOT_ACTIVATED,
  ACTIVATED,
};

/** Corresponds to gGTO defined in 4.4 of the manual */
enum ActionStatus {
  STOPPED,
  GOTO_POSITION,
};

/** Corresponds to gSTA defined in 4.4 of the manual */
enum FingerStatus {
  IN_RESET,
  ACTIVATION_IN_PROGRESS,
  ACTIVATION_COMPLETE,
};

/** Corresponds to gOBJ defined in 4.4 of the manual */
enum ObjectStatus {
  IN_MOTION,
  STOPPED_WHILE_OPENING, /** Encountered an obstacle while opening */
  STOPPED_WHILE_CLOSING, /** Encountered an obstacle while closing */
  AT_REQUESTED_POSITION, /** Reach requested position - object may not be grasped */
};

/** Corresponds to gFLT defined in 4.4 of the manual */
enum FaultStatus {
  NONE,
  ACTION_DELAYED,
  ACTIVATION_NEEDED,
  MAX_TEMP_EXCEEDED,
  COMM_TIMEOUT,
  UNDER_VOLTAGE,
  AUTOMATIC_RELEASE_IN_PROGRESS,
  INTERNAL_FAULT,
  ACTIVATION_FAULT,
  OVERCURRENT,
  AUTOMATIC_RELEASE_COMPLETED,
  UNKNOWN,
};

/** Detailed status containing gACT, gGTO, gSTA, gOBJ, and gFLT */
struct DetailedStatus {
  ActivationStatus gact;
  ActionStatus ggto;
  FingerStatus gsta;
  ObjectStatus gobj;
  FaultStatus gflt;
};

/** Holds the gripper feedback */
struct GripperFeedback {
  double commanded_position{0};      /** Range determined by alpha, beta */
  double position{0};                /** Range determined by alpha, beta */
  double current{0};                 /** Between 0 (min) and 1 (max) */
  uint8_t raw_commanded_position{0}; /** Between 0 (open) and 255 (closed) */
  uint8_t raw_position{0};           /** Between 0 (open) and 255 (closed) */
  DetailedStatus status;             /** Detailed status returned by the gripper*/
};

}  // namespace robotiq
