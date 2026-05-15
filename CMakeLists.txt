cmake_minimum_required(VERSION 3.16)

project(pict_client_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick WebSockets Protobuf)

qt_standard_project_setup(REQUIRES 6.6)

qt_add_executable(pict_client_app #${PROJECT_NAME}
    assets/message.proto
    assets/howTo.txt
    vcpkg.json
    src/main.cpp
)

qt_add_qml_module(${PROJECT_NAME}
    URI pict_client
    QML_FILES
        qml/Main.qml
        SOURCES inc/listStringModel.h
        SOURCES inc/filehelper.h src/filehelper.cpp
        SOURCES inc/websocketclient.h src/websocketclient.cpp
        SOURCES inc/unifiedstoragemodel.h src/unifiedstoragemodel.cpp
        SOURCES inc/msghandler.h src/msghandler.cpp
        QML_FILES qml/CustomFileDialog.qml
        QML_FILES
        QML_FILES qml/SecondCustomFileDialog.qml
        QML_FILES qml/FolderDialog.qml
        SOURCES inc/foldermodel.h
        SOURCES inc/auxilary.h
        SOURCES src/auxilary.cpp
        RESOURCES icons/login.svg icons/logout.svg icons/user.svg
        QML_FILES
        SOURCES
        SOURCES
        RESOURCES icons/userMask.svg
        QML_FILES qml/UserLogin.qml
        QML_FILES qml/UserLogoff.qml
        QML_FILES qml/WarnDialog.qml
        SOURCES inc/authhandler.h
        SOURCES src/authhandler.cpp
)

qt_add_protobuf(PictSerializer
    GENERATE_PACKAGE_SUBFOLDERS
    PROTO_FILES
        assets/message.proto
)
target_include_directories(${PROJECT_NAME} PRIVATE "inc")

target_sources(PictSerializer
  PRIVATE
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTifIER automatically since Qt 6.1.
# if you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(${PROJECT_NAME}  PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTifIER com.example.pict_client
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Qt6::Quick
    Qt6::WebSockets
    Qt6::Protobuf
    PictSerializer
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
