project(knotifytest)

########### knotifytest ###############

include(ECMMarkAsTest)

find_package(Qt${QT_MAJOR_VERSION}Test ${REQUIRED_QT_VERSION} REQUIRED)
set_package_properties(Qt${QT_MAJOR_VERSION}Test PROPERTIES PURPOSE "Required for tests")

find_package(KF6ConfigWidgets ${KF_DEP_VERSION} REQUIRED)
set_package_properties(KF6ConfigWidgets PROPERTIES PURPOSE "Required for tests")

find_package(KF6Notifications ${KF_DEP_VERSION} REQUIRED)
set_package_properties(KF6Notifications PROPERTIES PURPOSE "Required for tests")

find_package(KF6WidgetsAddons ${KF_DEP_VERSION} REQUIRED)
set_package_properties(KF6WidgetsAddons PROPERTIES PURPOSE "Required for tests")

find_package(KF6XmlGui ${KF_DEP_VERSION} REQUIRED)
set_package_properties(KF6XmlGui PROPERTIES PURPOSE "Required for tests")

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

add_executable(knotifytest main.cpp knotifytestwindow.cpp)

ki18n_wrap_ui(knotifytest knotifytestview.ui)

ecm_mark_as_test(knotifytest)

target_link_libraries(knotifytest
   Qt${QT_MAJOR_VERSION}::Core
   Qt${QT_MAJOR_VERSION}::Test
   KF6::NotifyConfig
   KF6::Notifications
   KF6::I18n
   KF6::XmlGui
   KF6::Completion
   KF6::ConfigWidgets # KStandardAction
   KF6::WidgetsAddons # KMessageBox
)
