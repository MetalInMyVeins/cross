cmake_minimum_required(VERSION 3.20)
project(cross)
set(pn ${CMAKE_PROJECT_NAME})
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS ON)
option(DEBUG_BUILD ON)
option(WIN_GUI OFF)
option(LINUX32 OFF)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

set(DEBUG_C_FLAGS
  -std=c17 -g
)
set(RELEASE_C_FLAGS
  -std=c17
  -O3 -flto -ffast-math -march=native -mtune=native -fomit-frame-pointer -funroll-loops -fno-stack-protector
  -fvisibility=hidden -fcf-protection=full -fstack-clash-protection -D_FORTIFY_SOURCE=3 -fPIE
)

set(DEBUG_CXX_FLAGS
  -std=c++20 -g -Wall -Wextra -Wpedantic -Werror
  -Wno-unused-parameter -Wno-unused-private-field -Wno-unused-variable
)
set(RELEASE_CXX_FLAGS
  -std=c++20 -Wall -Wextra -Wpedantic -Werror
  -Wno-unused-parameter -Wno-unused-private-field -Wno-unused-variable
  -O3 -flto -ffast-math -march=native -mtune=native -fomit-frame-pointer -funroll-loops -fno-stack-protector
  -fvisibility=hidden -fcf-protection=full -fstack-clash-protection -D_FORTIFY_SOURCE=3 -fPIE
)

if(DEBUG_BUILD)
  set(COMPILE_C_FLAGS ${DEBUG_C_FLAGS})
  set(COMPILE_CXX_FLAGS ${DEBUG_CXX_FLAGS})
else()
  set(COMPILE_C_FLAGS ${RELEASE_C_FLAGS})
  set(COMPILE_CXX_FLAGS ${RELEASE_CXX_FLAGS})
endif()

if (LINUX32)
  set(COMPILE_C_FLAGS "${COMPILE_C_FLAGS};-m32")
  set(COMPILE_CXX_FLAGS "${COMPILE_CXX_FLAGS};-m32")
endif()

add_compile_options(
  "$<$<COMPILE_LANGUAGE:C>:${COMPILE_C_FLAGS}>"
  "$<$<COMPILE_LANGUAGE:CXX>:${COMPILE_CXX_FLAGS}>"
)

if(WIN32)
  set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
  list(APPEND CMAKE_PREFIX_PATH /usr/x86_64-w64-mingw32)
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
endif()

cmake_policy(SET CMP0167 NEW)
find_package(FLTK REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/vendor/include
  /usr/include/bullet
)
if(WIN32)
  include_directories(
    ${CMAKE_SOURCE_DIR}/vendor/win64/include
    ${CMAKE_SOURCE_DIR}/vendor/win64/include/bullet
    ${CMAKE_SOURCE_DIR}/vendor/include
  )
endif()

add_library(lib${pn} STATIC
  ${CMAKE_SOURCE_DIR}/src/dummy.cxx
)

add_library(vendor STATIC
  ${CMAKE_SOURCE_DIR}/vendor/src/glad.c
)

add_executable(${pn} ${CMAKE_SOURCE_DIR}/src/main.cxx)
if(WIN32)
  target_link_options(lib${pn} PRIVATE -Wl,--strip-all -Wl,--build-id=none -s -pie -Wl,-z,now,-z,relro,-z,noexecstack)
  if(WIN_GUI)
    set(WIN_GUI_COMPILE_OPTION "-mwindows")
  else()
    set(WIN_GUI_COMPILE_OPTION "")
  endif()
  target_link_libraries(${pn}
    lib${pn} vendor
    -L${CMAKE_SOURCE_DIR}/vendor/win64/lib
    fltk Boost::system Boost::filesystem -lglfw3
    BulletSoftBody BulletDynamics BulletCollision LinearMath
    -lassimp -lz
    -static
    -lopengl32 -lgdi32 -lole32 -luuid -lcomctl32 -loleaut32 -lws2_32 ${WIN_GUI_COMPILE_OPTION}
  )

  if(NOT DEBUG_BUILD)
    add_custom_command(
      TARGET ${pn} POST_BUILD
      COMMAND /usr/bin/x86_64-w64-mingw32-strip "$<TARGET_FILE:${pn}>"
      COMMENT "stripping..."
    )
    add_custom_command(
      TARGET ${pn} POST_BUILD
      COMMAND /usr/bin/upx --best "$<TARGET_FILE:${pn}>"
      COMMENT "compressing..."
    )
  endif()
elseif (LINUX)
  target_link_libraries(${pn} lib${pn} vendor
    fltk Boost::system Boost::filesystem glfw
    BulletSoftBody BulletDynamics BulletCollision LinearMath
    assimp
  )
  
  if(NOT DEBUG_BUILD)
    add_custom_command(
      TARGET ${pn} POST_BUILD
      COMMAND /usr/bin/strip "$<TARGET_FILE:${pn}>"
      COMMENT "stripping..."
    )
    add_custom_command(
      TARGET ${pn} POST_BUILD
      COMMAND /usr/bin/upx --best "$<TARGET_FILE:${pn}>"
      COMMENT "compressing..."
    )
  endif()
endif()

message("> Information:")
message("> System: ${CMAKE_SYSTEM_NAME}")
message("> Selected C Compiler: ${CMAKE_C_COMPILER}")
message("> Selected C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("> Executable Path: ${EXECUTABLE_OUTPUT_PATH}")
message("> Library Path: ${LIBRARY_OUTPUT_PATH}")
message("> Debug Build: ${DEBUG_BUILD}")
message("> WIN_GUI: ${WIN_GUI}")
message("> Compile C Flags: ${COMPILE_C_FLAGS}")
message("> Compile C++ Flags: ${COMPILE_CXX_FLAGS}")

set_target_properties(lib${pn} PROPERTIES
  OUTPUT_NAME ${pn}
)

