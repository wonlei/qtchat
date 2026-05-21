set(SOURCES
    main.cpp
    client.cpp
    msghandle.cpp
    index.cpp
    friend.cpp
    chat.cpp
    file.cpp
    onlineuser.cpp
    sharefile.cpp
    uploader.cpp
)

set(HEADERS
    client.h
    msghandle.h
    index.h
    friend.h
    chat.h
    file.h
    onlineuser.h
    sharefile.h
    uploader.h
)

set(FORMS
    client.ui
    index.ui
    friend.ui
    chat.ui
    file.ui
    onlineuser.ui
    sharefile.ui
)

add_executable(WonChatClient ${SOURCES} ${HEADERS} ${FORMS})

target_link_libraries(WonChatClient PRIVATE
    wonchat_common
    Qt5::Core
    Qt5::Network
    Qt5::Sql
    Qt5::Widgets
)

# Resource files
target_sources(WonChatClient PRIVATE config.qrc icon.qrc)
