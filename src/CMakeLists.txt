

set (SRC main.cpp i2c_class.cpp prjconfig.cpp board.cpp)

add_executable(${PROJECT_NAME} ${SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)


install (TARGETS ${PROJECT_NAME}
	RUNTIME
	DESTINATION bin)

