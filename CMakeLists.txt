cmake_minimum_required(VERSION 3.27)

project(CurrencyChronicle)

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 1ad1cd2)
FetchContent_MakeAvailable(cpr)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

set(CMAKE_CXX_STANDARD 17)

add_executable(CurrencyChronicle main.cpp)

target_link_libraries(CurrencyChronicle PRIVATE cpr::cpr)
target_link_libraries(CurrencyChronicle PRIVATE nlohmann_json::nlohmann_json)