set(HAVE_PHONON ${Phonon4Qt5_FOUND})
configure_file(knotify-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotify-config.h )

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include_directories(BEFORE ${PHONON_INCLUDES})

########### next target ###############

set(knotifyconfig_LIB_SRCS
knotifyconfigactionswidget.cpp
knotifyconfigelement.cpp
knotifyeventlist.cpp
knotifyconfigwidget.cpp
)


set( knotifyconfig_UI knotifyconfigactionswidgetbase.ui )

ki18n_wrap_ui(knotifyconfig_LIB_SRCS ${knotifyconfig_UI} )

add_library(KF5NotifyConfig ${knotifyconfig_LIB_SRCS})
generate_export_header(KF5NotifyConfig BASE_NAME KNotifyConfig)
add_library(KF5::NotifyConfig ALIAS KF5NotifyConfig)

target_include_directories(KF5NotifyConfig INTERFACE "$<INSTALL_INTERFACE:${KF5_INCLUDE_INSTALL_DIR}/KNotifyConfig>")

target_link_libraries(KF5NotifyConfig
        PUBLIC
            Qt5::Widgets
        PRIVATE
            KF5::I18n
            KF5::KIOWidgets     # KUrlRequester
            ${PHONON_LIBS}
            Qt5::DBus)

set_target_properties(KF5NotifyConfig PROPERTIES
   VERSION      ${KNOTIFYCONFIG_VERSION_STRING}
   SOVERSION    ${KNOTIFYCONFIG_SOVERSION}
   EXPORT_NAME  NotifyConfig
)

ecm_generate_headers(KNotifyConfig_HEADERS
  HEADER_NAMES
  KNotifyConfigWidget

  REQUIRED_HEADERS KNotifyConfig_HEADERS
)

install(TARGETS KF5NotifyConfig EXPORT KF5NotifyConfigTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/knotifyconfig_export.h
  ${KNotifyConfig_HEADERS}
  DESTINATION ${KF5_INCLUDE_INSTALL_DIR}/KNotifyConfig COMPONENT Devel)


include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KNotifyConfig LIB_NAME KF5NotifyConfig DEPS "widgets" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KF5_INCLUDE_INSTALL_DIR}/KNotifyConfig)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})
