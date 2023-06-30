#ifndef COMBINATIONS_COMBINATIONS_HPP
#define COMBINATIONS_COMBINATIONS_HPP

#include <filesystem>
#include <vector>

#include "combinations/Component.hpp"

struct Component;

class Combinations {
    struct Implementation;
    const std::unique_ptr<Implementation> implementation;

public:
    Combinations();
    ~Combinations();

    bool load(const std::filesystem::path& resource);

    std::string classify(const std::vector<Component>& components, std::vector<int>& order) const;
};

#endif  // COMBINATIONS_COMBINATIONS_HPP
