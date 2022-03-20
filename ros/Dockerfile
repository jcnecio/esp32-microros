FROM ros:foxy

ENV ROS_DISTRO=foxy
SHELL ["/bin/bash", "-c"]

RUN . /opt/ros/$ROS_DISTRO/setup.bash

RUN mkdir /microros_ws
WORKDIR /microros_ws
RUN git clone -b $ROS_DISTRO https://github.com/micro-ROS/micro_ros_setup.git src/micro_ros_setup
RUN sudo apt update && rosdep update
RUN rosdep install --from-path src --ignore-src -y
RUN sudo apt-get install python3-pip -y
RUN rm /usr/src/gtest && rm /usr/src/gmock
RUN source /opt/ros/$ROS_DISTRO/setup.bash && colcon build
RUN source /opt/ros/$ROS_DISTRO/setup.bash && source install/local_setup.bash && ros2 run micro_ros_setup create_firmware_ws.sh freertos esp32