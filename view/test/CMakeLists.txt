#even if it seems so this project cannot compile as standalone

project(itemsviewtest)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )

set(itemsviewtest_SRCS
    main.cpp
)

kde4_add_executable(itemsviewtest ${itemsviewtest_SRCS})
target_link_libraries(itemsviewtest ${KDE4_KDEUI_LIBS})

install(TARGETS itemsviewtest DESTINATION ${BIN_INSTALL_DIR})
