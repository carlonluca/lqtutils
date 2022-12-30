cmake_minimum_required(VERSION 3.15)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Qml Quick)
if (Qt6_FOUND)
    find_package(Qt6 REQUIRED COMPONENTS StateMachine)
endif()

add_library(lqtutils STATIC
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_ui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_net.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_freq.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_fsm.cpp
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
