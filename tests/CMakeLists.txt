enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

set(TEST_SOURCES
    test_frame.cpp
    test_password.cpp
    test_dispatcher.cpp
    test_path.cpp
)

add_executable(wonchat_tests ${TEST_SOURCES})

target_link_libraries(wonchat_tests PRIVATE
    wonchat_common
    GTest::gtest_main
    Qt5::Core
    Qt5::Sql
)

target_include_directories(wonchat_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/Server
)

gtest_discover_tests(wonchat_tests)
