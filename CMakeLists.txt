cmake_minimum_required(VERSION 3.16)
project(aimee_module_git VERSION 0.3.2 LANGUAGES NONE)

include(GNUInstallDirs)
find_program(GO_EXECUTABLE NAMES go REQUIRED)
set(MODULE_BINARY "${CMAKE_CURRENT_BINARY_DIR}/aimee-module-git")
add_custom_command(
    OUTPUT "${MODULE_BINARY}"
    COMMAND ${CMAKE_COMMAND} -E env CGO_ENABLED=0 ${GO_EXECUTABLE} build -trimpath
            -o "${MODULE_BINARY}" ./runtime
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/runtime/main.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/modules/git/ci_grade.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/modules/git/cred_resolve.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/modules/git/forge_request.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/modules/git/git.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/modules/git/verify_state.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/bus/client.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/bus/client_connect.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/bus/module_caller.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/bus/module_protocol.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/bus/module_runtime.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/bus/region.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/bus/ring.go
        ${CMAKE_CURRENT_SOURCE_DIR}/server-go/bus/wire.go
        ${CMAKE_CURRENT_SOURCE_DIR}/go.mod
        ${CMAKE_CURRENT_SOURCE_DIR}/go.sum
    VERBATIM)
add_custom_target(aimee-module-git ALL DEPENDS "${MODULE_BINARY}")
configure_file(grants/module.grant.in ${CMAKE_CURRENT_BINARY_DIR}/git.grant @ONLY)
install(PROGRAMS "${MODULE_BINARY}" DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/git.grant
        DESTINATION ${CMAKE_INSTALL_DATADIR}/aimee/module-grants)
