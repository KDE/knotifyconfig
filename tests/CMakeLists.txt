project(knotifytest)

########### knotifytest ###############

include(ECMMarkAsTest)

find_package(Qt5Test ${REQUIRED_QT_VERSION} REQUIRED)
set_package_properties(Qt5Test PROPERTIES PURPOSE "Required for tests")

find_package(KF5ConfigWidgets ${KF5_VERSION} REQUIRED)
set_package_properties(KF5ConfigWidgets PROPERTIES PURPOSE "Required for tests")

find_package(KF5Notifications ${KF5_VERSION} REQUIRED)
set_package_properties(KF5Notifications PROPERTIES PURPOSE "Required for tests")

find_package(KF5WidgetsAddons ${KF5_VERSION} REQUIRED)
set_package_properties(KF5WidgetsAddons PROPERTIES PURPOSE "Required for tests")

find_package(KF5XmlGui ${KF5_VERSION} REQUIRED)
set_package_properties(KF5XmlGui PROPERTIES PURPOSE "Required for tests")

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

set(knotifytest_SRCS main.cpp knotifytestwindow.cpp )

qt5_wrap_ui(knotifytest_SRCS knotifytestview.ui)

add_executable(knotifytest ${knotifytest_SRCS})
ecm_mark_as_test(knotifytest)

target_link_libraries(knotifytest
   Qt5::Core
   Qt5::Test
   KF5::NotifyConfig
   KF5::Notifications
   KF5::I18n
   KF5::XmlGui
   KF5::Completion
   KF5::ConfigWidgets # KStandardAction
   KF5::WidgetsAddons # KMessageBox
)
