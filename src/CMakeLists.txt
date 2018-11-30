

set (SRC main.cpp i2c.cpp)

add_executable(${PROJECT_NAME} ${SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

message("~${CMAKE_BUILD_TYPE}~")
if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL ""))
	message("trace 2")
	target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=0)
	target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_TRACE_ON=1)
	target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_DEBUG_ON=1)
else((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL ""))
	message("trace 3")
	target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_ACTIVE_LEVEL=100)
	target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_TRACE_ON=1)
	target_compile_definitions(${PROJECT_NAME} PUBLIC SPDLOG_DEBUG_ON=1)
endif((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL ""))

message("trace 4")

#include_directories($ENV{SINC})

#include_directories($ENV{SINC}/glib-2.0)
#include_directories($ENV{SLIB}/glib-2.0/include)

#find_library(GIO gio-2.0)
#find_library(GLIB glib-2.0)

#target_link_libraries(daemon
#  ${GIO}
#  ${GLIB}
#)

# message("ENV SINC :" $ENV{SINC})
# message("ENV SLIB :" $ENV{SLIB})
#message("GIO :" ${GIO})
#message("GLIB :" ${GLIB})

install (TARGETS ${PROJECT_NAME}
	RUNTIME
	DESTINATION bin)
