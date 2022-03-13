cmake_minimum_required(VERSION 3.16)

set(KF_VERSION "5.93.0") # handled by release scripts
set(KF_DEP_VERSION "5.92.0") # handled by release scripts
project(KNotifyConfig VERSION ${KF_VERSION})

# Dependencies
include(FeatureSummary)
find_package(ECM 5.92.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://commits.kde.org/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(REQUIRED_QT_VERSION 5.15.2)
include(KDEInstallDirs)

# Required Qt components to build this framework
find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Widgets DBus)

find_package(Qt5 ${REQUIRED_QT_VERSION} QUIET OPTIONAL_COMPONENTS TextToSpeech)
if (NOT TARGET Qt5::TextToSpeech)
  message(STATUS "Qt5TextToSpeech not found, speech features will be disabled")
else()
  add_definitions(-DHAVE_SPEECH)
endif()
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(KDEGitCommitHooks)

find_package(KF5Completion ${KF_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF_DEP_VERSION} REQUIRED)
find_package(KF5KIO ${KF_DEP_VERSION} REQUIRED)

find_package(Canberra)
set_package_properties(Canberra PROPERTIES
    PURPOSE "Needed to preview notification sounds"
    TYPE OPTIONAL)
if (NOT Canberra_FOUND)
    # This is REQUIRED since you cannot tell CMake "either one of those two optional ones are required"
    find_package(Phonon4Qt5 4.6.60 NO_MODULE REQUIRED)
    set_package_properties(Phonon4Qt5 PROPERTIES
        DESCRIPTION "Qt-based audio library"
        PURPOSE "Needed to preview notification sounds when Canberra isn't available")
endif()

# Includes

include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)
include(ECMAddQch)
include(CMakePackageConfigHelpers)

# ECM setup
ecm_setup_version(
  PROJECT
  VARIABLE_PREFIX KNOTIFYCONFIG
  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/knotifyconfig_version.h"
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5NotifyConfigConfigVersion.cmake"
  SOVERSION 5)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f02)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055900)
# Subdirectories
add_definitions(-DTRANSLATION_DOMAIN=\"knotifyconfig5\")
ki18n_install(po)
add_subdirectory(src)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5NotifyConfig")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS KF5NotifyConfig_QCH
        FILE KF5NotifyConfigQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/KF5NotifyConfigQchTargets.cmake\")")
endif()

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KF5NotifyConfigConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5NotifyConfigConfig.cmake"
  INSTALL_DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KF5NotifyConfigConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KF5NotifyConfigConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT
  KF5NotifyConfigTargets
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  FILE KF5NotifyConfigTargets.cmake
  NAMESPACE KF5::
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/knotifyconfig_version.h"
  DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF}/KNotifyConfig"
  COMPONENT Devel
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
