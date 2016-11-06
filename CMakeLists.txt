cmake_minimum_required(VERSION 3.2)
project(subsystems)

find_package(PkgConfig)
pkg_check_modules(DBUS REQUIRED "dbus-cxx-1.0")

add_executable(adcs adcs.cpp)
add_executable(com com.cpp)
add_executable(deploy deploy.cpp)
add_executable(eps eps.cpp)

target_link_libraries(adcs ${DBUS_LIBRARIES})
target_link_libraries(com ${DBUS_LIBRARIES})
target_link_libraries(deploy ${DBUS_LIBRARIES})
target_link_libraries(eps ${DBUS_LIBRARIES})

include_directories(${DBUS_INCLUDE_DIRS})

target_compile_options(adcs PUBLIC ${DBUS_CFLAGS_OTHER})
target_compile_options(com PUBLIC ${DBUS_CFLAGS_OTHER})
target_compile_options(deploy PUBLIC ${DBUS_CFLAGS_OTHER})
target_compile_options(eps PUBLIC ${DBUS_CFLAGS_OTHER})