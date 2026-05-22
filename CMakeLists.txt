cmake_minimum_required(VERSION 3.16)

project(glia_app VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/app/main.cpp
    src/app/command.cpp
    src/app/lifecycle.cpp
    src/app/feature_flags.cpp
    src/app/about.cpp
    src/app/init.cpp
    src/app/capabilities.cpp
    src/app/harvest_cmd.cpp
    src/app/prune_cmd.cpp
    src/app/report_cmd.cpp
    src/app/status_cmd.cpp
    src/app/annotate_cmd.cpp
    src/app/audit_cmd.cpp
    src/app/config_cmd.cpp
    src/app/glia_workspace_cmds.cpp
    src/app/glia_git_cmds.cpp
    src/config/config.cpp
    src/harvest/harvester.cpp
    src/prune/pruner.cpp
    src/report/reporter.cpp
    src/audit/audit.cpp
    src/cli/cli.cpp
    src/storage/storage.cpp
    src/annotate/annotator.cpp
    src/curate/curator.cpp
    src/trigger/trigger.cpp
    src/safety/safety.cpp
    src/platform/platform.cpp
)

set(GLIA_SOURCES
    src/app/glia_main.cpp
    src/app/command_loader.cpp
    src/app/command_factory.cpp
    src/app/glia_git_cmds.cpp
    src/app/glia_workspace_cmds.cpp
    src/app/glia_waste_cmds.cpp
    src/app/glia_prune_cmd.cpp
    src/app/glia_init_cmd.cpp
    src/app/harvest_cmd.cpp
    src/app/prune_cmd.cpp
    src/app/report_cmd.cpp
    src/app/status_cmd.cpp
    src/app/audit_cmd.cpp
    src/app/config_cmd.cpp
    src/app/annotate_cmd.cpp
    src/app/about.cpp
    src/app/init.cpp
    src/app/capabilities.cpp
    src/app/command.cpp
    src/app/lifecycle.cpp
    src/app/feature_flags.cpp
    src/config/config.cpp
    src/harvest/harvester.cpp
    src/prune/pruner.cpp
    src/report/reporter.cpp
    src/audit/audit.cpp
    src/cli/cli.cpp
    src/storage/storage.cpp
    src/annotate/annotator.cpp
    src/curate/curator.cpp
    src/trigger/trigger.cpp
    src/safety/safety.cpp
    src/platform/platform.cpp
)

add_executable(sorrel ${SOURCES})
target_include_directories(sorrel PRIVATE src)

add_executable(glia ${GLIA_SOURCES})
target_include_directories(glia PRIVATE src)

# SDD Test binary
add_executable(sorrel_test
    tests/sdd/cards/GliaAppClass.cpp
    tests/sdd/cards/EnhancementsClass.cpp
    tests/sdd/cards/WorkflowClass.cpp
    tests/sdd/cards/WasteClass.cpp
    tests/sdd/cards/RobustnessClass.cpp
    tests/sdd/cards/DynamicCommandClass.cpp
    src/app/command.cpp
    src/app/lifecycle.cpp
    src/app/feature_flags.cpp
    src/app/about.cpp
    src/app/init.cpp
    src/app/capabilities.cpp
    src/app/harvest_cmd.cpp
    src/app/prune_cmd.cpp
    src/app/report_cmd.cpp
    src/app/status_cmd.cpp
    src/app/annotate_cmd.cpp
    src/app/audit_cmd.cpp
    src/app/config_cmd.cpp
    src/app/glia_workspace_cmds.cpp
    src/app/glia_git_cmds.cpp
    src/app/command_loader.cpp
    src/app/command_factory.cpp
    src/app/glia_waste_cmds.cpp
    src/config/config.cpp
    src/harvest/harvester.cpp
    src/prune/pruner.cpp
    src/report/reporter.cpp
    src/audit/audit.cpp
    src/cli/cli.cpp
    src/storage/storage.cpp
    src/annotate/annotator.cpp
    src/curate/curator.cpp
    src/trigger/trigger.cpp
    src/safety/safety.cpp
    src/platform/platform.cpp
)
target_include_directories(sorrel_test PRIVATE src tests/sdd/cpp tests/sdd/cards)

# Custom clean target
set(CLEAN_FILES
    "${CMAKE_SOURCE_DIR}/test_out.txt"
    "${CMAKE_SOURCE_DIR}/test_output_1.txt"
    "${CMAKE_SOURCE_DIR}/test_output_2.txt"
    "${CMAKE_SOURCE_DIR}/test_out_final.txt"
    "${CMAKE_SOURCE_DIR}/build_output.txt"
)

add_custom_target(clean_artifacts
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CLEAN_FILES}
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/__pycache__"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/scripts/__pycache__"
    COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_SOURCE_DIR}/tests/__pycache__"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "Removing temporary logs and Python caches"
)
