# src/CMAKELists.txt


SET(RAMULATOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(RAMULATOR_SOURCES
  Config.cpp
  Controller.cpp
  DDR3.cpp
  DDR4.cpp
  GDDR5.cpp
  Gem5Wrapper.cpp
  HBM.cpp
  LPDDR3.cpp
  LPDDR4.cpp
  MemoryFactory.cpp
  SALP.cpp
  WideIO.cpp
  WideIO2.cpp
  TLDRAM.cpp
  ALDRAM.cpp
  StatType.cpp
)

add_library(ramulator STATIC ${RAMULATOR_SOURCES})
target_compile_options(ramulator PRIVATE -Wno-missing-field-initializers -Wno-unused-variable -Wno-reorder)
target_compile_definitions(ramulator PUBLIC RAMULATOR)

install(TARGETS ramulator DESTINATION ${PROJECT_SOURCE_DIR})
