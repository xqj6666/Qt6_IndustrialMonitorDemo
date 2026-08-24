qt_add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    main.cpp
    ui/mainwindow.cpp
    ui/mainwindow.h
    ui/mainwindow.ui
    ui/pages/monitorpage.h
    ui/pages/monitorpage.cpp
    ui/pages/chartpage.h
    ui/pages/chartpage.cpp
    ui/pages/configpage.cpp
    ui/pages/configpage.h
    ui/widgets/indicatorlight.h
    ui/widgets/indicatorlight.cpp
    core/logger.h
    core/logger.cpp
    communication/modbusclient.h
    communication/modbusclient.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt::SerialBus
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)