add_library(kdevrustduchain STATIC
    builder.cpp

    types/rustintegraltype.cpp
    types/rusttupletype.cpp
)

generate_export_header(kdevrustduchain BASE_NAME kdevrustduchain EXPORT_MACRO_NAME KDEVRUSTDUCHAIN_EXPORT)

generate_export_header(kdevrustduchain EXPORT_FILE_NAME rustduchainexport.h)

target_link_libraries(kdevrustduchain
LINK_PRIVATE
    Qt5::Core
LINK_PUBLIC
    KDev::Language
    KDev::Project
    KDev::Util
)
