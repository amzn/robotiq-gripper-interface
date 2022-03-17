# Robotiq Gripper Interface

This package provides a simplified interface to work with Robotiq grippers via the [MODBUS RTU serial protocol (Section 4.7)](https://assets.robotiq.com/website-assets/support_documents/document/2F-85_2F-140_Instruction_Manual_e-Series_PDF_20190206.pdf).

## Limitations

- Tested on the Robotiq 2F-85, untested on other grippers.
- Tested on Ubuntu 18.04 with a USB serial connection at `/dev/ttyUSB0`.

## Organization

- `examples` provides examples for using the interface.
- `include/robotiq` provides the interface headers for use with the compiled library.
- `src` is the source code that packs/unpacks the serial messages to the gripper in a friendly manner.

## Needed hardware

This method of interface requires the following hardware
- A Robotiq 2F-85 gripper
- An RS-485/USB connector interface, e.g. ACC-ADT-USB-RS485 (for more info see [Section 8 of the manual](https://assets.robotiq.com/website-assets/support_documents/document/2F-85_2F-140_Instruction_Manual_e-Series_PDF_20190206.pdf)).

## Power on and connection

- **IMPORTANT! BEFORE YOU BEGIN** Make sure the gripper is mounted to a solid surface and the nothing will impede the gripper motion as it will fully open and close when activated.  Be sure to spend time understanding the robot behavior in an lab setting before using it to grasp items.
- The Robotiq 2F-85 ships with the following default serial configuration:
```
Baud: 115200
Byte size: 8
Stop bits: 1
Parity: None 
Id: 09
```
- Power on the gripper.  Plug in the USB cable to your computer.  You can use the [Robotiq User Interface](https://robotiq.com/support) (currently Windows only) to make sure the connection is working before using this package.

## Prerequisites

[Option 1: Local] Install CMake, Boost >=1.65, and GTest
```
# Install G++, CMake, Boost, and GTest
apt-get update
apt-get install \
    g++ \
    cmake \
    libboost-all-dev \
    libgtest-dev

# Build and link GTest
cd /usr/src/gtest \
    && cmake . \
    && make \
    && cp *.a /usr/lib
```

[Option 2: Container] As an alternative, a Dockerfile is provided to test the package out without modifying your local environment.  Assuming you have installed [Docker](https://www.docker.com/get-started), build and run the Dockerfile from the package source.  **NOTE**: Change the `--device` argument to point to your device if different.
```
docker build -t robotiq-gripper-interface .
docker run --rm -it --device=/dev/ttyUSB0 robotiq-gripper-interface:latest /bin/bash
cd robotiq-gripper-interface/
```

## Build with Cmake

C++ libraries are built using CMake.
```
mkdir build && cd build
cmake ..
make && make test
```
Run make install to make the library and header available to other packages.
```
make install
```
Return to the package root to run the examples
```
cd ..
```

## Run the examples

With the project built, and the gripper connection verified, you are now ready to run the examples.  Ensure the gripper is mounted to a solid surface, the reach around the gripper is clear, and the power is within your immediate reach in case something goes wrong.
- The first demo illustrates gripper reset and activation.  The gripper will open and close, after which it is ready for use.  If the gripper is not activated it will not accept other commands
```
bin/activate_gripper --port /dev/ttyUSB0
```
- The next demo opens the gripper.  Note, it must be activated for this command to run succesfully.
```
bin/open_gripper --port /dev/ttyUSB0
```
- The next demo closes the gripper.  Note, it must be activated for this command to run succesfully.
```
bin/close_gripper --port /dev/ttyUSB0
```
- The next demo commands the gripper position to a few values and displays the position feedback.
```
bin/position_gripper --port /dev/ttyUSB0
```

## Security

See [CONTRIBUTING](CONTRIBUTING.md#security-issue-notifications) for more information.

## License

This project is licensed under the Apache-2.0 License.

