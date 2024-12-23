cmake_minimum_required(VERSION 3.10)

# 项目名称
project(informationSystem)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 设置目录变量
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_SOURCE_DIR}/inc)

# 添加头文件目录
include_directories(${INC_DIR})

# 查找源文件
file(GLOB SRC ${SRC_DIR}/*.cpp)

# 创建可执行文件
add_executable(informationSystem ${SRC})

# 引入 sqlite3 库
find_package(SQLite3 REQUIRED)
target_include_directories(informationSystem PRIVATE ${SQLite3_INCLUDE_DIRS})
target_link_libraries(informationSystem PRIVATE ${SQLite3_LIBRARIES})

# 引入 nlohmann/json
find_package(nlohmann_json REQUIRED)
target_link_libraries(informationSystem PRIVATE nlohmann_json::nlohmann_json)
