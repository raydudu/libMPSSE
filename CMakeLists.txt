cmake_minimum_required(VERSION 3.19)
project(libMPSSE C)

set(CMAKE_C_STANDARD 11)

set(D2XX_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libftd2xx1/Include" CACHE PATH "Path to libftd2xx1 headers")
set(D2XX_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/libftd2xx1/x86_64" CACHE PATH "Path to libftdi2xx library")

find_library(D2XX_LIBRARY libftd2xx.so PATHS ${D2XX_LIBRARY_PATH})

add_library(${PROJECT_NAME} SHARED
        ftdi_common.h
        ftdi_infra.c ftdi_infra.h
        ftdi_mid.c ftdi_mid.h
        ftdi_i2c.c ftdi_i2c.h
        ftdi_spi.c ftdi_spi.h
)
target_include_directories(${PROJECT_NAME} PUBLIC Include ${D2XX_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${D2XX_LIBRARY})

add_executable(i2c-sample samples/i2c-sample.c)
target_link_libraries(i2c-sample PUBLIC ${PROJECT_NAME})

add_executable(spi-sample samples/spi-sample.c)
target_link_libraries(spi-sample PUBLIC ${PROJECT_NAME})