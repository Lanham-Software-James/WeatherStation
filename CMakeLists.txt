cmake_minimum_required(VERSION 3.20)

project(Weather_Station LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCES
        src/main.cpp
        src/controller/WeatherStationController.cpp
        src/publisher/MosquittoAdapter.cpp
        src/publisher/MQTTPublisher.cpp
        src/sensors/BMP280Sensor.cpp
        src/sensors/SensorFactory.cpp
        src/sensors/SHT41Sensor.cpp
)

add_executable(Weather_Station ${SOURCES})

target_include_directories(Weather_Station PRIVATE include)