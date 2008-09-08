project(widget-test)

find_package(KDE4)
include(KDE4Defaults)

set(widget-test_SRC
    main.cpp
    widget.cpp
)

include_directories(${KDE4_INCLUDES})

kde4_add_executable(widget-test ${widget-test_SRC})
target_link_libraries(widget-test ${KDE4_KDEUI_LIBS})

install(TARGETS widget-test DESTINATION ${BIN_INSTALL_DIR})
# install(FILES widget-test.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES widget-testui.rc  DESTINATION  ${DATA_INSTALL_DIR}/widget-test)
