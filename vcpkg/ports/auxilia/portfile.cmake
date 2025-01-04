# portfile.cmake
set(SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../source/auxilia")

vcpkg_cmake_configure(
    SOURCE_PATH ${SOURCE_PATH}
)

vcpkg_cmake_install()

# Fix CMake config files location
vcpkg_cmake_config_fixup(
    PACKAGE_NAME auxilia
    CONFIG_PATH lib/cmake/auxilia
)

# Remove debug includes and other unnecessary directories
file(REMOVE_RECURSE 
    "${CURRENT_PACKAGES_DIR}/debug/"
    "${CURRENT_PACKAGES_DIR}/lib"
)

# Install copyright
file(INSTALL "${SOURCE_PATH}/LICENSE" 
     DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" 
     RENAME copyright)

vcpkg_copy_pdbs()