include_directories(${PLASMA_INCLUDE_DIR})

set(raptor_engine_SRCS
    RaptorEngine.cpp
)

kde4_add_plugin(plasma_engine_raptor ${raptor_engine_SRCS})
target_link_libraries(plasma_engine_raptor ${KDE4_KDECORE_LIBS} ${PLASMA_LIBS})

install(TARGETS plasma_engine_raptor DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-engine-raptor.desktop DESTINATION ${SERVICES_INSTALL_DIR})
