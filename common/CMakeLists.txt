add_library(wonchat_common STATIC
    protocol.cpp
    protocol.h
    packet.cpp
    packet.h
    messagedispatcher.cpp
    messagedispatcher.h
    password.cpp
    password.h
    config.cpp
    config.h
    pathsafety.cpp
    pathsafety.h
)

target_include_directories(wonchat_common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(wonchat_common PUBLIC Qt5::Core Qt5::Network)
