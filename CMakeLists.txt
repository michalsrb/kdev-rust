PROJECT(rust)

set(KDEVRUST_VERSION_MAJOR 5)
set(KDEVRUST_VERSION_MINOR 0)
set(KDEVRUST_VERSION_PATCH 0)

# KDevplatform dependency version
set( KDEVPLATFORM_VERSION "${KDEVRUST_VERSION_MAJOR}.${KDEVRUST_VERSION_MINOR}.${KDEVRUST_VERSION_PATCH}" )

cmake_minimum_required(VERSION 2.8.12)
find_package (ECM 0.0.9 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(GenerateExportHeader)
include(FeatureSummary)

find_package(Qt5 REQUIRED Core Widgets Test WebKitWidgets)
find_package(KF5 REQUIRED COMPONENTS Archive ThreadWeaver TextEditor I18n ItemModels KCMUtils)
find_package(KDevPlatform ${KDEVPLATFORM_VERSION} REQUIRED)

# add_subdirectory(app_templates)
add_subdirectory(duchain)
# add_subdirectory(completion)
# add_subdirectory(testprovider)
# add_subdirectory(docs)
add_subdirectory(settings)

set(kdevrustlanguagesupport_PART_SRCS
    processdatastream.cpp
    rustcompilerrunner.cpp
    rustlanguagesupport.cpp
    rustparsejob.cpp
    rusthighlighting.cpp

    codegen/refactoring.cpp
)

kde_enable_exceptions()

qt5_add_resources(kdevrustlanguagesupport_PART_SRCS kdevrustsupport.qrc)

kdevplatform_add_plugin(kdevrustlanguagesupport JSON kdevrustsupport.json
    SOURCES ${kdevrustlanguagesupport_PART_SRCS})
target_link_libraries(kdevrustlanguagesupport
    KDev::Interfaces
    KDev::Language
    kdevrustduchain
    kdevrustsettings
    kdevrustsettingsmanager
#     kdevrustcompletion

    KF5::I18n
    KF5::TextEditor

    dl
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
