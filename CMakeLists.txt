cmake_minimum_required(VERSION 3.2)
project(MPSSE C)

set(CMAKE_C_STANDARD 11)

set(D2XX_INCLUDE_PATH "${PROJECT_SOURCE_DIR}/libftd2xx1/Include" CACHE PATH "Path to libftd2xx1 headers")
set(D2XX_LIBRARY_PATH "${PROJECT_SOURCE_DIR}/libftd2xx1/x86_64" CACHE PATH "Path to libftdi2xx library")

find_library(D2XX_LIBRARY libftd2xx.so PATHS ${D2XX_LIBRARY_PATH})

add_library(${PROJECT_NAME} SHARED
        ftdi_common.h
        ftdi_infra.c ftdi_infra.h
        ftdi_mid.c ftdi_mid.h
        ftdi_i2c.c ftdi_i2c.h
        ftdi_spi.c ftdi_spi.h
)
target_include_directories(${PROJECT_NAME} PUBLIC Include ${D2XX_INCLUDE_PATH})
target_link_libraries(${PROJECT_NAME} PUBLIC ${D2XX_LIBRARY})
set_target_properties(${PROJECT_NAME} PROPERTIES
        PUBLIC_HEADER "Include/libMPSSE_i2c.h;Include/libMPSSE_spi.h")

add_executable(i2c-sample samples/i2c-sample.c)
target_link_libraries(i2c-sample PUBLIC ${PROJECT_NAME})

add_executable(spi-sample samples/spi-sample.c)
target_link_libraries(spi-sample PUBLIC ${PROJECT_NAME})

install(TARGETS ${PROJECT_NAME} spi-sample i2c-sample PUBLIC_HEADER)

file(GLOB D2XX_BINARIES ${D2XX_LIBRARY_PATH}/*.*)
install(FILES ${D2XX_BINARIES} TYPE LIB)
install(FILES ${D2XX_INCLUDE_PATH}/ftd2xx.h TYPE INCLUDE)
if (NOT WIN32)
    install(FILES ${D2XX_INCLUDE_PATH}/WinTypes.h TYPE INCLUDE)
endif()
