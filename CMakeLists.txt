cmake_minimum_required(VERSION 3.15)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Qml Quick)
if (Qt6_FOUND)
    find_package(Qt6 REQUIRED COMPONENTS StateMachine)
endif()

add_library(lqtutils STATIC)
target_sources(lqtutils PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_string.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_bqueue.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_data.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_enum.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_fsm.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_math.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_misc.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_perf.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_prop.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_settings.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_threading.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_time.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_ui.h ${CMAKE_CURRENT_LIST_DIR}/lqtutils_ui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_net.h ${CMAKE_CURRENT_LIST_DIR}/lqtutils_net.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_freq.h ${CMAKE_CURRENT_LIST_DIR}/lqtutils_freq.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_fsm.h ${CMAKE_CURRENT_LIST_DIR}/lqtutils_fsm.cpp
)

target_include_directories(lqtutils
    PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(lqtutils PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
)
if (Qt6_FOUND)
    target_link_libraries(lqtutils PRIVATE Qt6::StateMachine)
endif()
