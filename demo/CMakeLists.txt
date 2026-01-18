cmake_minimum_required(VERSION 3.16)
project(ImageCropper VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (qt_dir "/home/vitasan/Qt/6.9.0") #Путь до библиотек QT

set (Qt6_DIR ${qt_dir}/gcc_64/lib/cmake/Qt6)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(ImageCropper WIN32 MACOSX_BUNDLE
    ../src/imagecropper.cpp ../src/imagecropper.h ../src/imagecropper_p.h
    ../src/imagecropper_e.h
    demowidget.cpp demowidget.h
    main.cpp
)
target_include_directories(ImageCropper PRIVATE
    ../src
    ${qt_dir}/gcc_64/include
    ${qt_dir}/gcc_64/include/QtWidgets
    ${qt_dir}/gcc_64/include/QtCore
    ${qt_dir}/gcc_64/include/QtGui
)

target_link_libraries(ImageCropper PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

install(TARGETS ImageCropper
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET ImageCropper
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})
