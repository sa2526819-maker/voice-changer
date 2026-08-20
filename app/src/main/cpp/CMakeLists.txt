cmake_minimum_required(VERSION 3.10.2)
project("voicechanger")

include(FetchContent)
FetchContent_Declare(
    oboe
    GIT_REPOSITORY https://github.com/google/oboe
    GIT_TAG main
)
FetchContent_MakeAvailable(oboe)

add_library(native-lib SHARED audio_processor.cpp)
find_library(log-lib log)

target_link_libraries(native-lib oboe ${log-lib})

