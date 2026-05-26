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
