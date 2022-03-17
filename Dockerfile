FROM ubuntu:18.04

# Install G++, CMake, Boost, and GTest
RUN apt-get clean \
    && apt-get update \
    && apt-get -y install --no-install-recommends \
        g++ \
        cmake \
        libboost-all-dev \
        libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Build and link GTest
RUN cd /usr/src/gtest \
    && cmake . \
    && make \
    && cp *.a /usr/lib

# Copy package in
COPY . /robotiq-gripper-interface
