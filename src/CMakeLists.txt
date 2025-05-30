set(HAVE_CANBERRA ${Canberra_FOUND})
set(HAVE_QTMULTIMEDIA ${Qt6Multimedia_FOUND})
configure_file(knotify-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotify-config.h )

add_library(KF6NotifyConfig)
add_library(KF6::NotifyConfig ALIAS KF6NotifyConfig)

set_target_properties(KF6NotifyConfig PROPERTIES
   VERSION      ${KNOTIFYCONFIG_VERSION}
   SOVERSION    ${KNOTIFYCONFIG_SOVERSION}
   EXPORT_NAME  NotifyConfig
)

target_sources(KF6NotifyConfig PRIVATE
    knotifyconfigactionswidget.cpp
    knotifyconfigelement.cpp
    knotifyeventlist.cpp
    knotifyconfigwidget.cpp
)

ki18n_wrap_ui(KF6NotifyConfig knotifyconfigactionswidgetbase.ui)

ecm_qt_declare_logging_category(KF6NotifyConfig
    HEADER knotifyconfig_debug.h
    IDENTIFIER KNOTIFYCONFIG_LOG
    CATEGORY_NAME kf.notifyconfig
    DESCRIPTION "KNotifyConfig"
    EXPORT KNOTIFYCONFIG
)

ecm_generate_export_header(KF6NotifyConfig
    BASE_NAME KNotifyConfig
    GROUP_BASE_NAME KF
    VERSION ${KF_VERSION}
    USE_VERSION_HEADER
    DEPRECATED_BASE_VERSION 0
)

target_include_directories(KF6NotifyConfig INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KNotifyConfig>")

target_link_libraries(KF6NotifyConfig
        PUBLIC
            Qt6::Widgets
        PRIVATE
            KF6::I18n
            KF6::KIOWidgets     # KUrlRequester
)

if (HAVE_DBUS)
    target_link_libraries(KF6NotifyConfig PRIVATE Qt6::DBus)
endif()

if(Canberra_FOUND)
  target_link_libraries(KF6NotifyConfig PRIVATE Canberra::Canberra)
elseif (Qt6Multimedia_FOUND)
    target_link_libraries(KF6NotifyConfig PRIVATE Qt6::Multimedia)
endif()

ecm_generate_headers(KNotifyConfig_HEADERS
  HEADER_NAMES
  KNotifyConfigWidget

  REQUIRED_HEADERS KNotifyConfig_HEADERS
)

install(TARGETS KF6NotifyConfig EXPORT KF6NotifyConfigTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/knotifyconfig_export.h
  ${KNotifyConfig_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KNotifyConfig COMPONENT Devel)

ecm_qt_install_logging_categories(
    EXPORT KNOTIFYCONFIG
    FILE knotifyconfig.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)

ecm_generate_qdoc(KF6NotifyConfig knotifyconfig.qdocconf)
