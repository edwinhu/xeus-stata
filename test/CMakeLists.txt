# Tests for xeus-stata

find_package(GTest)

if(GTest_FOUND)
    # Add test executable
    # add_executable(test_xeus_stata
    #     test_parser.cpp
    #     test_session.cpp
    # )

    # target_link_libraries(test_xeus_stata
    #     PRIVATE
    #         GTest::GTest
    #         GTest::Main
    # )

    # add_test(NAME test_xeus_stata COMMAND test_xeus_stata)

    message(STATUS "Tests will be added in future development")
else()
    message(STATUS "GTest not found, skipping tests")
endif()
