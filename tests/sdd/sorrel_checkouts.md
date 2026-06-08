# SORREL Checkouts - Glia Architectural Governance

- Artifacts: src/app/command_loader.cpp, rules/rules.xml, src/app/command_factory.cpp
- Measurement: dynamic_cmds_loaded = 1
- Observation: glia_modular_xml_sip = 1

- Artifacts: tests/sdd/facts/*.facts, tests/sdd/cards/*.cpp
- Measurement: waste_scan_code = 0
- Observation: glia_numeric_evidence_sip = 1

- Artifacts: src/app/glia_discovery_cmds.cpp
- Measurement: discovery_sdd_operational = 1
- Observation: glia_discovery_rule_sip = 1

- Artifacts: src/app/glia_discovery_cmds.cpp
- Measurement: gate_check_count = 6
- Observation: glia_reasoning_gates_sip = 1

- Artifacts: src/app/glia_score_cmds.cpp
- Measurement: total_restrictions_violations = 0
- Observation: glia_structural_audit_sip = 1

- Artifacts: src/app/glia_score_cmds.cpp
- Measurement: health_index = 1.0
- Observation: glia_health_scoring_sip = 1

- Artifacts: src/app/glia_score_cmds.cpp, rules/rules.xml, RESTRICTIONS.md
- Measurement: health_index = 0.6, total_restrictions_violations = 0
- Observation: glia_scoring_alignment_sip = 1

- Artifacts: src/app/status_cmd.cpp, src/app/command.h, src/app/glia_main.cpp
- Measurement: tui_integrated = 1, tui_description_coverage = 1
- Observation: glia_tui_advancement_sip = 1

- Artifacts: src/app/glia_score_cmds.cpp, src/app/glia_waste_cmds.cpp, src/util/string_utils.h, src/util/fs_utils.h, tests/sdd/cards/ScoringClass.cpp, rules/rules.xml
- Measurement: total_restrictions_violations = 0, glia_health_index = 1.0
- Observation: glia_scoring_refinement_sip = 1

- Artifacts: src/app/status_cmd.cpp, src/util/string_utils.h, RESTRICTIONS.md, tests/sdd/cards/WorkflowClass.cpp, tests/sdd/facts/glia_audit.facts
- Measurement: tui_command_palette_logic_marker = 1
- Observation: glia_tui_command_palette_sip = 1

- Artifacts: src/app/status_cmd.cpp, src/app/status_cmd.h, tests/sdd/facts/glia_audit.facts, tests/sdd/cards/WorkflowClass.cpp
- Measurement: tui_status_bar_logic_marker = 1, tui_modal_logic_marker = 1, tui_notifications_logic_marker = 1
- Observation: glia_tui_status_bar_sip = 1

- Artifacts: src/app/status_cmd.cpp, src/app/status_cmd.h, src/core/state.h, src/core/state.cpp
- Measurement: tui_tab_logic_marker = 1, tui_workspace_view_marker = 1, tui_theme_logic_marker = 1
- Observation: glia_tui_tabs_sip = 1, glia_tui_theme_system_sip = 1

- Artifacts: src/app/status_cmd.cpp, src/app/status_cmd.h, tests/sdd/facts/glia_audit.facts, tests/sdd/cards/WorkflowClass.cpp
- Measurement: tui_help_pane_logic_marker = 1, tui_breadcrumbs_logic_marker = 1
- Observation: glia_tui_help_breadcrumbs_sip = 1

- Artifacts: src/app/glia_discovery_cmds.cpp
- Measurement: interpretation_gate = 1, tool_gate = 1, navigation_gate = 1
- Observation: glia_gate_check_empirical_sip = 1

- Artifacts: src/app/glia_score_cmds.cpp, tests/sdd/facts/glia_audit.facts, tests/sdd/cards/ScoringClass.cpp
- Measurement: glia_signal_noise_ratio = 13.07, total_restrictions_violations = 163
- Observation: glia_scoring_empirical_refinement_sip = 1
