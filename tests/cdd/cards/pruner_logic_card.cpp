#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>

struct RepoRecord {
    float usage_score;
    float redundancy_score;
    float ethics_risk_score;
};

float calculate_composite_score(const RepoRecord& repo, int age_days) {
    float usage_weight = 0.5;
    float age_weight = 0.2;
    float redundancy_weight = 0.2;
    float ethics_risk_weight = 0.1;

    float max_age_for_norm = 365.0;
    float normalized_age = std::min((float)age_days / max_age_for_norm, 1.0f);

    float usage_contribution = (1.0f - repo.usage_score) * usage_weight;
    float age_contribution = normalized_age * age_weight;
    float redundancy_contribution = repo.redundancy_score * redundancy_weight;
    float ethics_contribution = repo.ethics_risk_score * ethics_risk_weight;

    return usage_contribution + age_contribution + redundancy_contribution + ethics_contribution;
}

std::string make_pruning_decision(float score) {
    if (score >= 0.9) return "DELETE";
    if (score >= 0.7) return "ARCHIVE";
    if (score >= 0.5) return "REVIEW";
    return "KEEP";
}

int main() {
    // Scenario: Old, unused, redundant repo (Should result in DELETE)
    RepoRecord repo = {0.0, 1.0, 0.0};
    float score = calculate_composite_score(repo, 365);
    std::string decision = make_pruning_decision(score);

    bool score_ok = std::abs(score - 0.9f) < 0.001f;
    bool decision_ok = (decision == "DELETE");

    std::cout << "pruner_logic_operational = " << (score_ok && decision_ok ? "true" : "false") << std::endl;

    return 0;
}
