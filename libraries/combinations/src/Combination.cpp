#include "combinations/Combination.hpp"

#include <algorithm>
#include <numeric>
#include <set>

Combination::Combination(std::string&& name) : name(std::move(name)) {}

bool Combination::check(const std::vector<Component>& components, std::vector<int>& order) {
    return pre_check(components) && post_check(components, order);
}

// Fixed
Fixed::Fixed(std::vector<Leg>&& legs, std::string&& string) : Multiple(std::move(legs), std::move(string)) {}
bool Fixed::check_amount(const std::vector<Component>& components) {
    return Multiple::legs.size() != components.size();
}

// Multiple
Multiple::Multiple(std::vector<Leg>&& legs, std::string&& string)
    : Combination(std::move(string)), legs(std::move(legs)) {}
bool Multiple::check_amount(const std::vector<Component>& components) {
    return components.size() % legs.size();
}
bool Multiple::pre_check(const std::vector<Component>& components) {
    if (check_amount(components)) {
        return false;
    }
    std::set<InstrumentType> set;
    for (const auto& i : components) {
        set.insert(i.type);
    }
    for (const auto& i : legs) {
        if (set.find(i.type) == set.end()) {
            return false;
        }
    }
    return true;
}
bool Multiple::check_permutation(const std::vector<Component>& components, const std::vector<int>& order) {
    for (std::size_t i = 0; i < components.size(); i += legs.size()) {
        std::map<char, double> strike;
        std::map<int, double> strike_offset;

        std::map<char, Expiration> expiration;
        std::map<int, Expiration> expiration_offset;

        for (std::size_t j = 0; j < legs.size(); ++j) {
            const auto& comb = components[order[j + i]];
            const auto& leg  = legs[j];

            if (comb.type != leg.type) {
                return false;
            }

            if (std::holds_alternative<double>(leg.ratio)) {
                if (std::get<double>(leg.ratio) != comb.ratio) {
                    return false;
                }
            } else {
                if (std::get<bool>(leg.ratio) != (comb.ratio > 0)) {
                    return false;
                }
            }

            if (!offset_check(strike, strike_offset, leg.strike, comb.strike)) {
                return false;
            }

            if (std::holds_alternative<Period>(leg.expiration)) {
                if (!expiration_offset[0].check_expiration(std::get<Period>(leg.expiration),
                                                           static_cast<Expiration>(comb.expiration))) {
                    return false;
                }
            } else {
                if (!offset_check(expiration, expiration_offset, leg.expiration, Expiration(comb.expiration))) {
                    return false;
                }
            }
        }
    }
    return true;
}
bool Multiple::post_check(const std::vector<Component>& components, std::vector<int>& order) {
    std::iota(order.begin(), order.end(), 0);
    if (check_permutation(components, order)) {
        return true;
    }
    while (std::next_permutation(order.begin(), order.end())) {
        if (check_permutation(components, order)) {
            return true;
        }
    }
    return false;
}

// More
More::More(Leg&& leg, std::string&& name, std::size_t min_count)
    : Combination(std::move(name)), leg(leg), min_count(min_count) {}
bool More::pre_check(const std::vector<Component>& components) {
    return components.size() >= min_count;
}
bool More::post_check(const std::vector<Component>& components, std::vector<int>& order) {
    for (const auto& component : components) {
        if (!(leg.type == component.type || (leg.type == InstrumentType::O && (component.type == InstrumentType::P ||
                                                                               component.type == InstrumentType::C)))) {
            return false;
        }
        if (std::holds_alternative<double>(leg.ratio)) {
            if (std::get<double>(leg.ratio) != component.ratio) {
                return false;
            }
        } else {
            if (std::get<bool>(leg.ratio) != (component.ratio > 0)) {
                return false;
            }
        }
    }
    std::iota(order.begin(), order.end(), 0);
    return true;
}
