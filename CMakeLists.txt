cmake_minimum_required(VERSION 4.0)
project(Weather_Station)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_executable(Weather_Station
        src/main.cpp
        src/controller/WeatherStationController.cpp
        src/publisher/MosquittoAdapter.cpp
        src/publisher/MQTTPublisher.cpp
        src/sensors/BMP280Sensor.cpp
        src/sensors/SensorFactory.cpp
        src/sensors/SHT41Sensor.cpp
)

