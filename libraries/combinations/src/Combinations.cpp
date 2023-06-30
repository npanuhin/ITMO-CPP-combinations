#include "combinations/Combinations.hpp"

#include <cstring>

#include "combinations/Combination.hpp"
#include "combinations/DateTime.hpp"
#include "pugixml.hpp"

struct Combinations::Implementation {
    std::vector<std::unique_ptr<Combination>> combinations;

    void add(Combination* combination) { combinations.emplace_back(combination); }
};

// =====================================================================================================================

Combinations::Combinations() : implementation(new Implementation()) {}

Combinations::~Combinations() = default;

bool Combinations::load(const std::filesystem::path& resource) {
    pugi::xml_document doc;
    doc.load_file(resource.c_str());
    if (!doc) {
        return false;
    }

    const auto combinations = doc.child("combinations");
    if (!combinations) {
        return false;
    }

    for (const auto& combination : combinations) {
        const auto& nodes = combination.first_child();

        std::vector<Leg> legs;
        for (const auto& node : nodes) {
            auto& leg = legs.emplace_back();

            // Type
            leg.type = static_cast<InstrumentType>(node.attribute("type").value()[0]);

            // Ratio
            const auto& ratio = node.attribute("ratio");
            if (ratio.value()[0] == '+') {
                leg.ratio = true;
            } else if (ratio.value()[0] == '-' && ratio.value()[1] == '\0') {
                leg.ratio = false;
            } else {
                leg.ratio = ratio.as_double();
            }

            // Strike
            if (const auto& strike = node.attribute("strike")) {
                leg.strike = strike.value()[0];

            } else if (const auto& strike_offset = node.attribute("strike_offset")) {
                int tmp    = static_cast<int>(std::strlen(strike_offset.value()));
                leg.strike = strike_offset.value()[0] == '-' ? -tmp : tmp;
            }

            // Expiration
            const auto& expiration = node.attribute("expiration");
            if (expiration) {
                leg.expiration = expiration.value()[0];

            } else if (const auto& expiration_offset = node.attribute("expiration_offset")) {
                if (expiration_offset.value()[0] == '+' || expiration_offset.value()[0] == '-') {
                    int tmp        = static_cast<int>(std::strlen(expiration_offset.value()));
                    leg.expiration = expiration_offset.value()[0] == '+' ? tmp : -tmp;
                } else {
                    char* durPtr;
                    int tmp = std::strtol(expiration_offset.value(), &durPtr, 10);
                    if (!tmp) {
                        ++tmp;
                    }
                    leg.expiration = Period(static_cast<OffsetType>(*durPtr), tmp);
                }
            }
        }

        std::string cardinality = nodes.attribute("cardinality").value();
        std::string name        = combination.attribute("name").value();

        switch (cardinality[1]) {
        case 'o':  // More
            implementation->add(new More(std::move(legs[0]), std::move(name), nodes.attribute("mincount").as_ullong()));
            break;
        case 'i':  // Fixed
            implementation->add(new Fixed(std::move(legs), std::move(name)));
            break;
        case 'u':  // Multiply
            implementation->add(new Multiple(std::move(legs), std::move(name)));
            break;
        }
    }
    return true;
}

std::string Combinations::classify(const std::vector<Component>& components, std::vector<int>& order) const {
    std::vector<int> tmp_order(components.size());

    for (const auto& comb : implementation->combinations) {
        if (comb->check(components, tmp_order)) {
            order.resize(tmp_order.size());
            for (std::size_t i = 0; i < tmp_order.size(); ++i) {
                order[tmp_order[i]] = static_cast<int>(i) + 1;
            }
            return comb->name;
        }
    }

    return "Unclassified";
}
