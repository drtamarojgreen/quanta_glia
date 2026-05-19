#ifndef SDD_ENHANCEMENTS_CLASS_H
#define SDD_ENHANCEMENTS_CLASS_H

#include <map>
#include <string>

void reporting_enhancement_verification(const std::map<std::string, std::string>& facts);
void harvester_enhancement_verification(const std::map<std::string, std::string>& facts);
void harvester_collision_verification(const std::map<std::string, std::string>& facts);
void pruner_enhancement_verification(const std::map<std::string, std::string>& facts);

#endif // SDD_ENHANCEMENTS_CLASS_H
