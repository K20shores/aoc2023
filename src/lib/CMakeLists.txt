add_library(aoc2023)

target_sources(aoc2023 
  PRIVATE 
    get_char.cpp
    hash.cpp
    maths.cpp
)

target_include_directories(aoc2023 PUBLIC ${PROJECT_SOURCE_DIR}/include)