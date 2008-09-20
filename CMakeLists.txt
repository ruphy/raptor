 
# Project needs a name of course
project(raptor)
 
# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
find_package(Plasma REQUIRED)

add_subdirectory(widget)
add_subdirectory(engine)
add_subdirectory(view/test)

add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )
 
# We add our source code here
set(raptor_SRCS 
    raptor.cpp
    view/raptoritemsview.cpp # When starting implementing raptoritemsview include also engine/kickoff/*
    view/raptoritemdelegate.cpp
    view/raptorgraphicswidget.cpp
    view/raptorscrollbutton.cpp
    engine/kickoff/applicationmodel.cpp
    engine/kickoff/kickoffabstractmodel.cpp
    engine/kickoff/models.cpp
    engine/kickoff/leavemodel.cpp
)
 
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_raptor ${raptor_SRCS})
target_link_libraries(plasma_applet_raptor 
                      ${PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_SOLID_LIBS} ${KDE4_KIO_LIBS} solidcontrol)
 
install(TARGETS plasma_applet_raptor
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-raptor.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

install(FILES view/pics/raptorarrows.svg DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/widgets/)
install(FILES view/pics/overlay.svg DESTINATION ${DATA_INSTALL_DIR}/desktoptheme/default/widgets/)
