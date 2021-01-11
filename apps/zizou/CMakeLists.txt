add_executable(zizou  zizou.cpp)

set_target_properties(zizou  PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES)
target_include_directories(zizou  PRIVATE ${CMAKE_SOURCE_DIR}/libs)
target_link_libraries(zizou  yocto)

if(YOCTO_OPENGL)
target_link_libraries(zizou  yocto_gui)
endif(YOCTO_OPENGL)
