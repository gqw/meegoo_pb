cmake_minimum_required(VERSION 3.10)

# 设置项目名称
project(meegoopb
        VERSION 0.0.1
        LANGUAGES C CXX
        DESCRIPTION "A middleware for converting data to protobuf"
        HOMEPAGE_URL "https://github.com/gqw")

# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置编译类型为 Debug
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message("CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# 为 Debug 模式添加 ASan 编译和链接选项
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

file(GLOB_RECURSE SRC_FILES src/*.cpp src/*.cc)
file(GLOB_RECURSE HEADER_FILES include/meegoopb/*.h)

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SRC_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC include include/meegoopb)

# 安装指令
# 安装可执行文件到指定目录，默认是 /usr/local/bin
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin)

# 安装头文件
install(FILES ${HEADER_FILES}
        DESTINATION include/${PROJECT_NAME})


# gtest
find_package(GTest REQUIRED)
find_package(absl REQUIRED)
find_package(Protobuf REQUIRED)
message(STATUS "Protobuf Libraries: ${Protobuf_LIBRARIES}")

set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)
file(GLOB_RECURSE TEST_SRC_FILES test/*.cpp test/*.cc )
add_executable(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})
target_link_libraries(${PROJECT_TEST_NAME} PRIVATE
        GTest::GTest GTest::Main
        libprotobufd.a
        absl::strings
        absl::throw_delegate
        absl::log_internal_check_op
        absl::hash absl::log
        absl::algorithm_container
        absl::container_common
        absl::status
        absl::statusor
        absl::flat_hash_map utf8_range)
target_include_directories(${PROJECT_TEST_NAME} PUBLIC include)

install(TARGETS ${PROJECT_TEST_NAME}
        RUNTIME DESTINATION bin)