cmake_minimum_required(VERSION 3.19)
project(ICH-test LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# Core C 源文件（纯C层）
set(CORE_SOURCES
    core/linkedlist.c
    core/file_io.c
    core/logger.c
    core/app_init.c
    core/archive_service.c
    core/user_service.c
    core/favorite_service.c
)

set(CORE_HEADERS
    core/models.h
    core/linkedlist.h
    core/file_io.h
    core/logger.h
    core/app_init.h
    core/archive_service.h
    core/user_service.h
    core/favorite_service.h
)

# UI 层源文件
set(UI_SOURCES
    ui/loginwindow.cpp
    ui/archivepage.cpp
    ui/archiveeditdialog.cpp
    ui/favoritepage.cpp
    ui/usermanagepage.cpp
    ui/statspage.cpp
    ui/logviewpage.cpp
)

set(UI_HEADERS
    ui/loginwindow.h
    ui/archivepage.h
    ui/archiveeditdialog.h
    ui/favoritepage.h
    ui/usermanagepage.h
    ui/statspage.h
    ui/logviewpage.h
)

qt_add_executable(ICH-test
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ${CORE_SOURCES}
    ${CORE_HEADERS}
    ${UI_SOURCES}
    ${UI_HEADERS}
)

# 让 #include "models.h" 等能找到 core/ 和 ui/ 下的头文件
target_include_directories(ICH-test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# .c 文件用 C++ 编译器编译（规避中文路径下 MinGW C 编译器检测问题）
set_source_files_properties(${CORE_SOURCES} PROPERTIES LANGUAGE CXX)

# 构建后自动拷贝 data/ 目录到可执行文件所在目录
add_custom_command(TARGET ICH-test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/data"
        "$<TARGET_FILE_DIR:ICH-test>/data"
    COMMENT "Copying data files to build directory..."
)

target_link_libraries(ICH-test
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS ICH-test
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ICH-test
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
