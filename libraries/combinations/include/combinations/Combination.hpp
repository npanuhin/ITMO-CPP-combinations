#ifndef COMBINATIONS_COMBINATION_HPP
#define COMBINATIONS_COMBINATION_HPP

#include <map>
#include <variant>
#include <vector>

#include "combinations/Component.hpp"
#include "combinations/DateTime.hpp"
#include "pugixml.hpp"

struct Leg {
    InstrumentType type;
    std::variant<double, bool> ratio;
    std::variant<char, int> strike;
    std::variant<char, int, Period> expiration;
};

struct Combination {
    explicit Combination(std::string&& name);
    virtual ~Combination() = default;

    bool check(const std::vector<Component>& components, std::vector<int>& order);

    const std::string name;

protected:
    virtual bool pre_check(const std::vector<Component>& components)                           = 0;
    virtual bool post_check(const std::vector<Component>& components, std::vector<int>& order) = 0;
};

// Multiple
struct Multiple: Combination {
    Multiple(std::vector<Leg>&& legs, std::string&& string);

protected:
    const std::vector<Leg> legs;

    virtual bool check_amount(const std::vector<Component>& components);
    bool pre_check(const std::vector<Component>& components) override;
    bool post_check(const std::vector<Component>& components, std::vector<int>& order) override;

private:
    bool check_permutation(const std::vector<Component>& components, const std::vector<int>& order);

    template <class T, class... Ts>
    static bool offset_check(std::map<char, T>& map, std::map<int, T>& offset_map, const std::variant<Ts...>& leg,
                             const T& comb);
};

// Fixed
struct Fixed: Multiple {
    Fixed(std::vector<Leg>&& legs, std::string&& string);

protected:
    bool check_amount(const std::vector<Component>& components) override;
};

// More
struct More: Combination {
    More(Leg&& leg, std::string&& name, std::size_t min_count);

protected:
    bool pre_check(const std::vector<Component>& components) override;
    bool post_check(const std::vector<Component>& components, std::vector<int>& order) override;

private:
    Leg leg;
    const std::size_t min_count;
};

// ================================================== Implementation ===================================================

template <class T, class... Ts>
bool Multiple::offset_check(std::map<char, T>& map, std::map<int, T>& offset_map, const std::variant<Ts...>& leg,
                            const T& comb) {
    if (std::holds_alternative<char>(leg)) {
        if (std::get<char>(leg) != '\0') {
            if (!map.emplace(std::get<char>(leg), comb).second && map[std::get<char>(leg)] != comb) {
                return false;
            }
        }
        offset_map.clear();
        offset_map[0] = comb;
    } else {
        if (offset_map.emplace(std::get<int>(leg), comb).second) {
            auto index = std::get<int>(leg);
            for (const auto [q, amount] : offset_map) {
                if ((q < index && amount >= comb) || (q > index && amount <= comb)) {
                    return false;
                }
            }
        } else if (offset_map[std::get<int>(leg)] != comb) {
            return false;
        }
    }
    return true;
}

#endif  // COMBINATIONS_COMBINATION_HPP
