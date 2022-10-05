set(HAVE_CANBERRA ${Canberra_FOUND})
set(HAVE_PHONON ${Phonon4Qt${QT_MAJOR_VERSION}_FOUND})
configure_file(knotify-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotify-config.h )

add_library(KF5NotifyConfig)
add_library(KF5::NotifyConfig ALIAS KF5NotifyConfig)

target_sources(KF5NotifyConfig PRIVATE
    knotifyconfigactionswidget.cpp
    knotifyconfigelement.cpp
    knotifyeventlist.cpp
    knotifyconfigwidget.cpp
)

ki18n_wrap_ui(KF5NotifyConfig knotifyconfigactionswidgetbase.ui)

ecm_qt_declare_logging_category(KF5NotifyConfig
    HEADER knotifyconfig_debug.h
    IDENTIFIER KNOTIFYCONFIG_LOG
    CATEGORY_NAME kf.notifyconfig
    DESCRIPTION "KNotifyConfig"
    EXPORT KNOTIFYCONFIG
)

generate_export_header(KF5NotifyConfig BASE_NAME KNotifyConfig)

target_include_directories(KF5NotifyConfig INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/KNotifyConfig>")

target_link_libraries(KF5NotifyConfig
        PUBLIC
            Qt${QT_MAJOR_VERSION}::Widgets
        PRIVATE
            KF5::I18n
            KF5::KIOWidgets     # KUrlRequester
            Qt${QT_MAJOR_VERSION}::DBus)

if(Canberra_FOUND)
  target_link_libraries(KF5NotifyConfig PRIVATE Canberra::Canberra)
elseif (Phonon4Qt${QT_MAJOR_VERSION}_FOUND)
    target_link_libraries(KF5NotifyConfig PRIVATE Phonon::phonon4qt${QT_MAJOR_VERSION})
endif()

set_target_properties(KF5NotifyConfig PROPERTIES
   VERSION      ${KNOTIFYCONFIG_VERSION}
   SOVERSION    ${KNOTIFYCONFIG_SOVERSION}
   EXPORT_NAME  NotifyConfig
)

ecm_generate_headers(KNotifyConfig_HEADERS
  HEADER_NAMES
  KNotifyConfigWidget

  REQUIRED_HEADERS KNotifyConfig_HEADERS
)

install(TARGETS KF5NotifyConfig EXPORT KF5NotifyConfigTargets ${KF_INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/knotifyconfig_export.h
  ${KNotifyConfig_HEADERS}
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/KNotifyConfig COMPONENT Devel)

if(BUILD_QCH)
    ecm_add_qch(
        KF5NotifyConfig_QCH
        NAME KNotifyConfig
        BASE_NAME KF5NotifyConfig
        VERSION ${KF_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${KNotifyConfig_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt5Widgets_QCH
        INCLUDE_DIRS
            ${CMAKE_CURRENT_BINARY_DIR}
        BLANK_MACROS
            KNOTIFYCONFIG_EXPORT
            KNOTIFYCONFIG_DEPRECATED
            KNOTIFYCONFIG_DEPRECATED_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KNotifyConfig LIB_NAME KF5NotifyConfig DEPS "widgets" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/KNotifyConfig)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

ecm_qt_install_logging_categories(
    EXPORT KNOTIFYCONFIG
    FILE knotifyconfig.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)
