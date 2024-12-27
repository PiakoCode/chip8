include_directories("${PROJECT_SOURCE_DIR}/include")


file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp" "${PROJECT_SOURCE_DIR}/src/*.cc")
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/include/*.hpp")
# 可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(SDL2 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)