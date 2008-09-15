#even if it seems so this project cannot compile as standalone

project(itemsviewtest)

find_package(KDE4 REQUIRED)
find_package(Plasma REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

set(itemsviewtest_SRCS
    main.cpp
    ../raptoritemsview.cpp
    model/applicationmodel.cpp
    model/kickoffabstractmodel.cpp
    model/leavemodel.cpp
    model/models.cpp
    ../raptoritemdelegate.cpp
)

kde4_add_executable(itemsviewtest ${itemsviewtest_SRCS})
target_link_libraries(itemsviewtest ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} solidcontrol)

install(TARGETS itemsviewtest DESTINATION ${BIN_INSTALL_DIR})
