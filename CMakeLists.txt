cmake_minimum_required(VERSION 2.8)
project( magic-video-fixer )

# OpenCV
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

# Sources and headers
set(magic-video-fixer_SOURCES
    src/main.cpp
    src/settings.cpp
    src/ssim_thread.cpp
    src/ssim.cpp
    src/Frame.cpp
    src/Sequence.cpp
    src/Hungarian.cpp
)

set(magic-video-fixer_HEADERS
    src/settings.h
    src/ssim_thread.h
    src/ssim.h
    src/Frame.h
    src/Sequence.h
    src/Hungarian.h
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Werror -Wno-long-long -pedantic")

add_executable(magic-video-fixer ${magic-video-fixer_SOURCES} ${magic-video-fixer_HEADERS})

target_link_libraries( magic-video-fixer
    ${OpenCV_LIBS}
    pthread)
