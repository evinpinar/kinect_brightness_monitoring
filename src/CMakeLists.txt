SET(APP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/enumerate.cpp"
        )

add_executable(brightness_monitor ${APP_SOURCES} ${APP_HEADERS} )
set_property(TARGET brightness_monitor PROPERTY CXX_STANDARD 17)
set_target_properties(brightness_monitor PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(brightness_monitor PRIVATE
        CONAN_PKG::opencv
        CONAN_PKG::kinect-azure-sensor-sdk
        )

target_include_directories(brightness_monitor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        )
