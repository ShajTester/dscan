

set (SRC main.cpp)

add_executable(${PROJECT_NAME} ${SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)



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
