# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr HINTS $ENV{ZEPHYR_BASE})
project(iotclt_embedded)

target_sources(app PRIVATE
               src/main.c
               src/mqtt_publisher.c
               src/wifi_handling.c)
target_include_directories(app PUBLIC include)
