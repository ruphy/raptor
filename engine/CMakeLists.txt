include_directories(
    ${QT_INCLUDES}
    ${KDE4_INCLUDES}
)

set(raptor_engine_SRCS
    RaptorEngine.cpp
    kickoff/applicationmodel.cpp
    kickoff/kickoffabstractmodel.cpp
    kickoff/models.cpp
    kickoff/leavemodel.cpp
)

kde4_add_plugin(plasma_engine_raptor ${raptor_engine_SRCS})
target_link_libraries(plasma_engine_raptor ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} ${KDE4_SOLID_LIBS} ${QT_QTGUI_LIBRARY} kworkspace solidcontrol)

install(TARGETS plasma_engine_raptor DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-engine-raptor.desktop DESTINATION ${SERVICES_INSTALL_DIR})
