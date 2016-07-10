add_library(kdevrustsettingsmanager
STATIC
    rustsettingsmanager.cpp
)

target_link_libraries(kdevrustsettingsmanager
LINK_PUBLIC
    KDev::Project
    KDev::Util
)

set(rustsettings_SRCS
    rustsettings/rustsettings.cpp
)

ki18n_wrap_ui(rustsettings_SRCS
    rustsettings/rustsettings.ui
)

kconfig_add_kcfg_files(rustsettings_SRCS rustsettings/rustconfig.kcfgc)

add_library(kdevrustsettings
STATIC
    ${rustsettings_SRCS}
)

target_link_libraries(kdevrustsettings
LINK_PUBLIC
    KDev::Project
    KDev::Util
)
