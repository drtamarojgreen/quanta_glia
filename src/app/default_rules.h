#ifndef GLIA_APP_DEFAULT_RULES_H
#define GLIA_APP_DEFAULT_RULES_H

#include <string>

namespace glia::app {

const std::string DEFAULT_RULES_XSD = R"XML(<?xml version="1.0" encoding="UTF-8"?>
<xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">
  <xs:element name="GliaRules">
    <xs:complexType>
      <xs:sequence>
        <xs:element name="Command" maxOccurs="unbounded">
          <xs:complexType>
            <xs:sequence>
              <xs:element name="Name" type="xs:string"/>
              <xs:element name="Description" type="xs:string"/>
              <xs:choice>
                <xs:element name="ShellTemplate" type="xs:string"/>
                <xs:element name="Steps">
                  <xs:complexType>
                    <xs:sequence>
                      <xs:element name="Step" type="xs:string" maxOccurs="unbounded"/>
                    </xs:sequence>
                  </xs:complexType>
                </xs:element>
              </xs:choice>
              <xs:element name="Type" type="xs:string" default="generic" minOccurs="0"/>
            </xs:sequence>
          </xs:complexType>
        </xs:element>
      </xs:sequence>
    </xs:complexType>
  </xs:element>
</xs:schema>
)XML";

const std::string DEFAULT_RULES_XML = R"XML(<?xml version="1.0" encoding="UTF-8"?>
<GliaRules>
  <Command>
    <Name>git-find-commit</Name>
    <Description>Search commit messages for a pattern</Description>
    <ShellTemplate>git log --grep</ShellTemplate>
  </Command>
  <Command>
    <Name>git-cleanup-local</Name>
    <Description>Delete local branches merged into main</Description>
    <ShellTemplate>#complex:git-cleanup-local</ShellTemplate>
    <Type>complex</Type>
  </Command>
  <Command>
    <Name>build-lint-cmake</Name>
    <Description>Check CMakeLists.txt for errors</Description>
    <ShellTemplate>cmake-lint CMakeLists.txt</ShellTemplate>
  </Command>
  <Command>
    <Name>test-unit</Name>
    <Description>Run unit tests</Description>
    <ShellTemplate>ctest --output-on-failure</ShellTemplate>
  </Command>
  <Command>
    <Name>env-shell</Name>
    <Description>Open shell in a container service</Description>
    <ShellTemplate>docker-compose exec</ShellTemplate>
  </Command>
  <Command>
    <Name>env-clean-volumes</Name>
    <Description>Remove unused Docker volumes</Description>
    <ShellTemplate>docker volume prune -f</ShellTemplate>
  </Command>
  <Command>
    <Name>env-doctor</Name>
    <Description>Diagnose environment issues</Description>
    <ShellTemplate>docker info &amp;&amp; git --version &amp;&amp; cmake --version</ShellTemplate>
  </Command>
  <Command>
    <Name>doc-spellcheck</Name>
    <Description>Spellcheck Markdown files</Description>
    <ShellTemplate>mdspell '**/*.md'</ShellTemplate>
  </Command>
  <Command>
    <Name>doc-todo-list</Name>
    <Description>Extract TODOs from codebase</Description>
    <ShellTemplate>#complex:doc-todo-list</ShellTemplate>
    <Type>complex</Type>
  </Command>
  <Command>
    <Name>doc-update-changelog</Name>
    <Description>Update CHANGELOG.md from git history</Description>
    <ShellTemplate>git-chglog -o CHANGELOG.md</ShellTemplate>
  </Command>
  <Command>
    <Name>lint-check-format</Name>
    <Description>Verify code formatting</Description>
    <ShellTemplate>clang-format --dry-run --Werror</ShellTemplate>
  </Command>
  <Command>
    <Name>lint-security</Name>
    <Description>Static security analysis (SAST)</Description>
    <ShellTemplate>bandit -r .</ShellTemplate>
  </Command>
  <Command>
    <Name>lint-secrets</Name>
    <Description>Scan for accidentally committed secrets</Description>
    <ShellTemplate>gitleaks detect --source .</ShellTemplate>
  </Command>
  <Command>
    <Name>lint-yaml</Name>
    <Description>Validate YAML configuration files</Description>
    <ShellTemplate>yamllint .</ShellTemplate>
  </Command>
  <Command>
    <Name>proj-estimate</Name>
    <Description>Estimate project size (LOC, files)</Description>
    <ShellTemplate>#complex:proj-estimate</ShellTemplate>
    <Type>complex</Type>
  </Command>
  <Command>
    <Name>sys-info</Name>
    <Description>Display system information</Description>
    <ShellTemplate>uname -a &amp;&amp; lscpu</ShellTemplate>
  </Command>
  <Command>
    <Name>env-rebuild</Name>
    <Description>Force rebuild of development environment</Description>
    <Steps>
      <Step>docker-compose down --volumes --remove-orphans</Step>
      <Step>docker-compose build --no-cache</Step>
      <Step>docker-compose up -d</Step>
      <Step>glia env-doctor</Step>
    </Steps>
  </Command>
</GliaRules>
)XML";

}

#endif
