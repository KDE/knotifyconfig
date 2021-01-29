set(HAVE_CANBERRA ${Canberra_FOUND})
set(HAVE_PHONON ${Phonon4Qt5_FOUND})
configure_file(knotify-config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/knotify-config.h )

########### next target ###############

set(knotifyconfig_LIB_SRCS
knotifyconfigactionswidget.cpp
knotifyconfigelement.cpp
knotifyeventlist.cpp
knotifyconfigwidget.cpp
)


set( knotifyconfig_UI knotifyconfigactionswidgetbase.ui )

ki18n_wrap_ui(knotifyconfig_LIB_SRCS ${knotifyconfig_UI} )

ecm_qt_declare_logging_category(knotifyconfig_LIB_SRCS
    HEADER knotifyconfig_debug.h
    IDENTIFIER KNOTIFYCONFIG_LOG
    CATEGORY_NAME kf.notifyconfig
    DESCRIPTION "KNotifyConfig"
    EXPORT KNOTIFYCONFIG
)

add_library(KF5NotifyConfig ${knotifyconfig_LIB_SRCS})
generate_export_header(KF5NotifyConfig BASE_NAME KNotifyConfig)
add_library(KF5::NotifyConfig ALIAS KF5NotifyConfig)

target_include_directories(KF5NotifyConfig INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KNotifyConfig>")

target_link_libraries(KF5NotifyConfig
        PUBLIC
            Qt5::Widgets
        PRIVATE
            KF5::I18n
            KF5::KIOWidgets     # KUrlRequester
            Qt5::DBus)

if(Canberra_FOUND)
  target_link_libraries(KF5NotifyConfig PRIVATE Canberra::Canberra)
elseif (Phonon4Qt5_FOUND)
    target_link_libraries(KF5NotifyConfig PRIVATE Phonon::phonon4qt5)
endif()

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
  DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KNotifyConfig COMPONENT Devel)

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
ecm_generate_pri_file(BASE_NAME KNotifyConfig LIB_NAME KF5NotifyConfig DEPS "widgets" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KNotifyConfig)
install(FILES ${PRI_FILENAME} DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

ecm_qt_install_logging_categories(
    EXPORT KNOTIFYCONFIG
    FILE knotifyconfig.categories
    DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}"
)
