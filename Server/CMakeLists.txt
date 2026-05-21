set(SOURCES
    main.cpp
    server.cpp
    mytcpserver.cpp
    mytcpsocket.cpp
    msghandler.cpp
    operatedb.cpp
    clienttask.cpp
    logmanager.cpp
)

set(HEADERS
    server.h
    mytcpserver.h
    mytcpsocket.h
    msghandler.h
    operatedb.h
    clienttask.h
    logmanager.h
)

add_executable(WonChatServer ${SOURCES} ${HEADERS})

target_link_libraries(WonChatServer PRIVATE
    wonchat_common
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Widgets
)

# Resource file with connect.config
target_sources(WonChatServer PRIVATE config.qrc)
