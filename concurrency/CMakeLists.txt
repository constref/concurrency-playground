set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(assimp REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

add_executable(demo1 "demo1.h" "demo1.cpp" "platform.h"  "sdlrenderer.h" "sdlrenderer.cpp")
#add_executable(demo2 "demo2.h" "demo2.cpp" "platform.h" "platform.cpp" "sdlplatform.h" "sdlplatform.cpp")
add_executable(5_future "5_future.cpp")
add_executable(concurrency "concurrency.cpp")

target_link_libraries(demo1 PRIVATE assimp::assimp SDL2::SDL2 SDL2::SDL2main SDL2_ttf::SDL2_ttf)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET concurrency PROPERTY CXX_STANDARD 20)
endif()
