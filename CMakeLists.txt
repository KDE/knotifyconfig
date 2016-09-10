cmake_minimum_required(VERSION 2.8.12)

project(KNotifyConfig)

# Dependencies
include(FeatureSummary)
find_package(ECM 5.26.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

set(REQUIRED_QT_VERSION 5.5.0)
set(KF5_VERSION "5.27.0") # handled by release scripts
set(KF5_DEP_VERSION "5.26.0") # handled by release scripts

# Required Qt5 components to build this framework
find_package(Qt5 ${REQUIRED_QT_VERSION} NO_MODULE REQUIRED Widgets DBus)
find_package(Qt5 ${REQUIRED_QT_VERSION} QUIET OPTIONAL_COMPONENTS TextToSpeech)
if (NOT Qt5TextToSpeech_FOUND)
  message(STATUS "Qt5TextToSpeech not found, speech features will be disabled")
else()
  add_definitions(-DHAVE_SPEECH)
endif()
include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
find_package(Phonon4Qt5 4.6.60 NO_MODULE)

find_package(KF5Completion ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5Config ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5I18n ${KF5_DEP_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_DEP_VERSION} REQUIRED)

# Includes

include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMPackageConfigHelpers)

# ECM setup
ecm_setup_version(
  ${KF5_VERSION}
  VARIABLE_PREFIX KNOTIFYCONFIG
  VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/knotifyconfig_version.h"
  PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5NotifyConfigConfigVersion.cmake"
  SOVERSION 5)

# Subdirectories
add_definitions(-DTRANSLATION_DOMAIN=\"knotifyconfig5\")
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/po")
    ki18n_install(po)
endif()
add_subdirectory(src)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

# Create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5NotifyConfig")

ecm_configure_package_config_file(
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
  DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF5}"
  COMPONENT Devel
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
