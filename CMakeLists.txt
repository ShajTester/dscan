cmake_minimum_required(VERSION 3.2)

project(dscan VERSION 0.1.1)

#find_package(Boost COMPONENTS unit_test_framework REQUIRED)

include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/include")

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

add_subdirectory(src)

# set(CPACK_GENERATOR DEB)

# set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
# set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
# set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

# set(CPACK_PACKAGE_CONTACT kolobrod2002@yandex.ru)

# include (CPack)

configure_file("${PROJECT_SOURCE_DIR}/src/version.h.in" "${PROJECT_BINARY_DIR}/version.h")
configure_file("${PROJECT_SOURCE_DIR}/src/dscan.conf.json" "${PROJECT_BINARY_DIR}/dscan.conf" COPYONLY)

enable_testing()

add_subdirectory(test)

install (FILES ${PROJECT_BINARY_DIR}/dscan.conf DESTINATION /etc/dscan COMPONENT config)

# add_test(test_version_valid test_${project})
