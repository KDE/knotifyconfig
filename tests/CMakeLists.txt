project(knotifytest)

########### knotifytest ###############

include(ECMMarkAsTest)

find_package(Qt${QT_MAJOR_VERSION}Test ${REQUIRED_QT_VERSION} REQUIRED)
set_package_properties(Qt${QT_MAJOR_VERSION}Test PROPERTIES PURPOSE "Required for tests")

find_package(KF5ConfigWidgets ${KF_DEP_VERSION} REQUIRED)
set_package_properties(KF5ConfigWidgets PROPERTIES PURPOSE "Required for tests")

find_package(KF5Notifications ${KF_DEP_VERSION} REQUIRED)
set_package_properties(KF5Notifications PROPERTIES PURPOSE "Required for tests")

find_package(KF5WidgetsAddons ${KF_DEP_VERSION} REQUIRED)
set_package_properties(KF5WidgetsAddons PROPERTIES PURPOSE "Required for tests")

find_package(KF5XmlGui ${KF_DEP_VERSION} REQUIRED)
set_package_properties(KF5XmlGui PROPERTIES PURPOSE "Required for tests")

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_executable(knotifytest main.cpp knotifytestwindow.cpp)

ki18n_wrap_ui(knotifytest knotifytestview.ui)

ecm_mark_as_test(knotifytest)

target_link_libraries(knotifytest
   Qt${QT_MAJOR_VERSION}::Core
   Qt${QT_MAJOR_VERSION}::Test
   KF5::NotifyConfig
   KF5::Notifications
   KF5::I18n
   KF5::XmlGui
   KF5::Completion
   KF5::ConfigWidgets # KStandardAction
   KF5::WidgetsAddons # KMessageBox
)
