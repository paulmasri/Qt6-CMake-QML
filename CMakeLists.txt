cmake_minimum_required(VERSION 3.24)

set(APP_TARGET Qt6-CMake-QML)
set(APP_NAME "QML Test App")

set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")
message(STATUS "CMAKE_INSTALL_PREFIX changed to ${CMAKE_INSTALL_PREFIX} for the duration of this configuration script")

project(${APP_TARGET} VERSION 1.0.0 LANGUAGES CXX)

# Other files: for inclusion (easy access in Qt Creator) but not for compiling
add_custom_target(non-compiled-files SOURCES
    .gitignore
    MakeSelfSignedCertificate.ps1
    README.md
)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

qt_standard_project_setup()

qt_add_executable(${APP_TARGET}
    main.cpp
)

qt_add_qml_module(${APP_TARGET}
    URI Primary
    RESOURCE_PREFIX /
    VERSION 1.0
    QML_FILES
        Main.qml
)

set_target_properties(${APP_TARGET} PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(${APP_TARGET}
    PRIVATE
    Qt6::Quick
)

option(APP_INSTALL "Run install step following the build" OFF)
if (APP_INSTALL)
    message(STATUS "Starting prepare for install step...")
    install(TARGETS ${APP_TARGET}
        BUNDLE DESTINATION .
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    if(${Qt6_VERSION} VERSION_LESS "6.5")
        qt_generate_deploy_qml_app_script(
            TARGET ${APP_TARGET}
            FILENAME_VARIABLE deploy_script
        )
    else()
        qt_generate_deploy_qml_app_script(
            TARGET ${APP_TARGET}
            OUTPUT_SCRIPT deploy_script
        )
    endif()
    install(SCRIPT ${deploy_script})

    include(InstallRequiredSystemLibraries)

    set(APPX_PUBLISHER_ID "Qt.Experiment.Certificate")
    set(APPX_PACKAGE_PRODUCT_NAME "QtExperiment.${APP_TARGET}")
    set(APPX_PACKAGE_PRODUCT_DISPLAY_NAME "${APP_NAME}")
    set(APPX_PACKAGE_PRODUCT_DESCRIPTION "Test app for building QML projects for Windows in CMake")
    set(APPX_PACKAGE_PRODUCT_VERSION "${PROJECT_VERSION}.0")
    set(APPX_PACKAGE_PRODUCT_BACKGROUND_COLOR "transparent")
    set(APPX_PACKAGE_PUBLISHER_ID "CN=${APPX_PUBLISHER_ID}")
    set(APPX_PACKAGE_PUBLISHER_DISPLAY_NAME "Publisher")
    set(APPX_PACKAGE_MIN_VERSION "10.0.19041.0")
    set(APPX_PACKAGE_MAX_VERSION_TESTED "10.0.19041.0")
    set(APPX_PACKAGE_EXECUTABLE "${CMAKE_INSTALL_BINDIR}/${APP_TARGET}.exe")
    set(APPX_PACKAGES_PREFIX "packages")
    set(APPX_PACKAGE_NAME_X64 "${APP_TARGET}_x64.appx")
    set(APPX_BUNDLE_NAME "${APP_TARGET}.appxbundle")

    configure_file(AppxManifest.xml.in "${CMAKE_CURRENT_BINARY_DIR}/AppxManifest.xml")
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/AppxManifest.xml"
        DESTINATION "${CMAKE_INSTALL_PREFIX}"
    )
    install(
        DIRECTORY Assets
        DESTINATION "${CMAKE_INSTALL_PREFIX}"
    )

message(STATUS "Done prepare for install step.")
endif()

option(APP_PACKAGE_SIGN "Enable packaging and signing of the app" OFF)
option(APP_BUNDLE "Enable bundling .appx package as .appxbundle (bundle gets signed instead of package)" OFF)

if(APP_PACKAGE_SIGN)
    if(APP_BUNDLE)
        set(package_bundle_message "packaging, bundling, and signing (the bundle)")
    else()
        set(package_bundle_message "packaging and signing (the package)")
    endif()
    message(STATUS "Starting ${package_bundle_message}...")


    if(NOT DEFINED PFX_SIGNATURE_KEY)
        message(FATAL_ERROR "PFX_SIGNATURE_KEY is not defined. Make sure to set it to the path of your PFX file.")
    endif()

    if(NOT DEFINED PFX_PASSWORD)
        message(FATAL_ERROR "PFX_PASSWORD is not defined. Make sure to set it to the password of your PFX file.")
    endif()

    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/${PFX_SIGNATURE_KEY}")
        message(FATAL_ERROR
            "Make sure to point to the right certificate PFX file for signing.")
    endif()

    set(makeappx_pack_x64_command "makeappx pack /d \"${CMAKE_INSTALL_PREFIX}\" /p \"${CMAKE_BINARY_DIR}/${APPX_PACKAGES_PREFIX}/${APPX_PACKAGE_NAME_X64}\"")
    set(makeappx_pack_x64_execute_process "execute_process(COMMAND ${makeappx_pack_x64_command})")

    if(APP_BUNDLE)
        set(makeappx_bundle_command "makeappx bundle /d \"${CMAKE_BINARY_DIR}/${APPX_PACKAGES_PREFIX}\" /p \"${CMAKE_BINARY_DIR}/${APPX_BUNDLE_NAME}\"")
        set(makeappx_bundle_execute_process "execute_process(COMMAND ${makeappx_bundle_command})")

        set(signtool_command "signtool sign /n \"${APPX_PUBLISHER_ID}\" /v /fd SHA256 /f \"${CMAKE_SOURCE_DIR}/${PFX_SIGNATURE_KEY}\" /p \"${PFX_PASSWORD}\" \"${CMAKE_BINARY_DIR}/${APPX_BUNDLE_NAME}\"")
        set(signtool_execute_process "execute_process(COMMAND ${signtool_command})")
    else()
        set(makeappx_bundle_execute_process "")

        set(signtool_command "signtool sign /n \"${APPX_PUBLISHER_ID}\" /v /fd SHA256 /f \"${CMAKE_SOURCE_DIR}/${PFX_SIGNATURE_KEY}\" /p \"${PFX_PASSWORD}\" \"${CMAKE_BINARY_DIR}/${APPX_PACKAGES_PREFIX}/${APPX_PACKAGE_NAME_X64}\"")
        set(signtool_execute_process "execute_process(COMMAND ${signtool_command})")
    endif()

    install(CODE "
        file(REMOVE_RECURSE \"${CMAKE_BINARY_DIR}/${APPX_PACKAGES_PREFIX}\")
        file(REMOVE \"${CMAKE_BINARY_DIR}/${APPX_BUNDLE_NAME}\")
        ${makeappx_pack_x64_execute_process}
        ${makeappx_bundle_execute_process}
        ${signtool_execute_process}
        ")

    message(STATUS "Done ${package_bundle_message}")
endif()
