cmake_minimum_required(VERSION 3.10)
project(GeneratedControllerTests)

# Find Google Test
find_package(GTest REQUIRED)

# Find all generated test files
file(GLOB TEST_SOURCES "*.cpp")

# Create the test executable
add_executable(runTests ${TEST_SOURCES})

# Add Drogon include path (best practice: target_include_directories)
target_include_directories(runTests PRIVATE /usr/local/include)

# Link with Drogon, Google Test, and pthread
target_link_libraries(runTests GTest::GTest GTest::Main pthread drogon)

# (Optional) Set C++ standard
set_target_properties(runTests PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)

# Enable test discovery (optional, for ctest)
enable_testing()
add_test(NAME AllGeneratedTests COMMAND runTests)