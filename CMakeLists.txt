cmake_minimum_required(VERSION 3.15)
set(CMAKE_AUTOMOC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Qml Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Qml Quick)
if (Qt6_FOUND)
    find_package(Qt NAMES Qt6 REQUIRED COMPONENTS StateMachine)
    find_package(Qt6 REQUIRED COMPONENTS StateMachine)
endif()
find_package(QT NAMES Qt6 Qt5 OPTIONAL_COMPONENTS DBus)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS DBus)

qt_add_library(lqtutils STATIC
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_ui.cpp ${CMAKE_CURRENT_LIST_DIR}/lqtutils_ui.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_net.cpp ${CMAKE_CURRENT_LIST_DIR}/lqtutils_net.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_freq.cpp ${CMAKE_CURRENT_LIST_DIR}/lqtutils_freq.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_fsm.cpp ${CMAKE_CURRENT_LIST_DIR}/lqtutils_fsm.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_autoexec.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_bqueue.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_data.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_enum.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_logging.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_math.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_misc.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_perf.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_prop.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_settings.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_string.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_qsl.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_system.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_threading.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_time.h
    ${CMAKE_CURRENT_LIST_DIR}/lqtutils_models.h
)
if (IOS)
    target_sources(lqtutils PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lqtutils_ui.mm)
endif()

if (ENABLE_FONT_AWESOME)
    qt_add_qml_module(lqtutils
        URI lqtutils
        VERSION 1.0
        QML_FILES
            fontawesome/LQTFontAwesomeFreeRegular.qml
            fontawesome/LQTFontAwesomeFreeSolid.qml
            fontawesome/LQTFontAwesomeBrandsRegular.qml
        RESOURCES
            "fontawesome/Font Awesome 6 Brands-Regular-400.otf"
            "fontawesome/Font Awesome 6 Free-Regular-400.otf"
            "fontawesome/Font Awesome 6 Free-Solid-900.otf"
        SOURCES
            ${CMAKE_CURRENT_LIST_DIR}/lqtutils_fa.cpp ${CMAKE_CURRENT_LIST_DIR}/lqtutils_fa.h
        IMPORT_PATH
            ${CMAKE_CURRENT_LIST_DIR}/fontawesome
        RESOURCE_PREFIX /
        IMPORT_PATH "/fontawesome"
    )
    add_compile_definitions(LQT_FONT_AWESOME_ENABLED)
endif()

target_include_directories(lqtutils
    PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(lqtutils PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Quick
)
if (Qt6DBus_FOUND)
    target_link_libraries(lqtutils PRIVATE Qt${QT_VERSION_MAJOR}::DBus)
endif()
if (Qt6_FOUND)
    target_link_libraries(lqtutils PRIVATE Qt6::StateMachine)
endif()
