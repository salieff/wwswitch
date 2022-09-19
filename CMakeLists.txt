cmake_minimum_required(VERSION 3.5)

project(wwswitch LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(wwswitch
    main.cpp
    wwswitch/debounced_button.cpp
    wwswitch/webserver.cpp
    wwswitch/wwswitchstatistics.cpp)

add_compile_definitions(
    ARDUINO=10819
    ARDUINO_ARCH_ESP8266
    ARDUINO_BOARD=\"ESP8266_GENERIC\"
    ARDUINO_ESP8266_GENERIC
    ESP8266
    __ets__
    F_CPU=80000000L
    FLASHMODE_QIO
    _GNU_SOURCE
    ICACHE_FLASH
    LED_BUILTIN=2
    LWIP_FEATURES=1
    LWIP_IPV6=0
    LWIP_OPEN_SRC
    MMU_ICACHE_SIZE=0x8000
    MMU_IRAM_SIZE=0x8000
    NONOSDK22x_191122=1
    TCP_MSS=536)

set(ARD_DIR "$ENV{HOME}/Arduino/libraries")
set(ESP_DIR "$ENV{HOME}/.arduino15/packages/esp8266/hardware/esp8266/3.0.2")
set(ESP_SDK "${ESP_DIR}/tools/sdk")

include_directories(
    "${ESP_DIR}/cores/esp8266"
    "${ESP_DIR}/libraries/DNSServer/src"
    "${ESP_DIR}/libraries/ESP8266WebServer/src"
    "${ESP_DIR}/libraries/ESP8266HTTPUpdateServer/src"
    "${ESP_DIR}/libraries/ESP8266WiFi/src"
    "${ESP_DIR}/libraries/LittleFS/src"
    "${ESP_DIR}/variants/generic"
    "${ESP_SDK}/include"
    "${ESP_SDK}/libc/xtensa-lx106-elf/include"
    "${ESP_SDK}/lwip2/include"
    "$ENV{HOME}/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/3.0.4-gcc10.3-1757bed/xtensa-lx106-elf/include"
    "${ARD_DIR}/WiFiManager"
    "${ARD_DIR}/ESPAsyncTCP/src"
    "${ARD_DIR}/ESPAsyncWebServer/src"
    "./wwswitch")

add_compile_options(
    -falign-functions=4
    -fdata-sections
    -ffunction-sections
    -fipa-pta
    -fno-exceptions
    -fno-rtti
    -free
    -mlongcalls
    -mtext-section-literals
    -Os
    -std=gnu++17
    -w
    -Wall
    -Werror=return-type
    -Wextra
    -U__STRICT_ANSI__)
