#include "combinations/DateTime.hpp"

#include <tuple>

std::pair<int, int> add_quoter(int amount, int year, int month) {
    month += amount * 3;
    year += month / 12;
    month %= 12;
    return {year, month};
}

// Period
Period::Period(const OffsetType& type_, std::size_t amount_) : type(type_), amount(amount_) {}

// Expiration
Expiration::Expiration(const std::tm& tm) : year(tm.tm_year), month(tm.tm_mon), day(tm.tm_mday) {}

// Сравнение на равенство: ==, !=
bool operator==(const Expiration& day1, const Expiration& day2) {
    return day1.year == day2.year && day1.month == day2.month && day1.day == day2.day;
}
bool operator!=(const Expiration& day1, const Expiration& day2) {
    return !(day1 == day2);
}

// Сравнение: <, <=
bool operator<(const Expiration& day1, const Expiration& day2) {
    if (day1.year == day2.year) {
        if (day1.month == day2.month) {
            return day1.day < day2.day;
        }
        return day1.month < day2.month;
    }
    return day1.year < day2.year;
}
bool operator<=(const Expiration& day1, const Expiration& day2) {
    return (day1 == day2) || (day1 < day2);
}

// Сравнение: >, >=
bool operator>(const Expiration& day1, const Expiration& day2) {
    return day2 < day1;
}
bool operator>=(const Expiration& day1, const Expiration& day2) {
    return (day1 == day2) || (day1 > day2);
}

bool Expiration::check_expiration(const Period& period, const Expiration& expiration) {
    auto tmp = *this;

    switch (period.type) {
    case OffsetType::Quoter:
        std::tie(tmp.year, tmp.month) = add_quoter(period.amount, tmp.year, tmp.month);
        if (expiration < tmp) {
            return false;
        }
        std::tie(tmp.year, tmp.month) = add_quoter(1, tmp.year, tmp.month);
        return expiration <= tmp;
    case OffsetType::Year:
        tmp.year += period.amount;
        break;
    case OffsetType::Month:
        tmp.month += period.amount;
        break;
    case OffsetType::Day:
        tmp.day += period.amount;
        break;
    }

    std::tm tm{};
    tm.tm_year = tmp.year;
    tm.tm_mon  = tmp.month;
    tm.tm_mday = tmp.day;
    std::mktime(&tm);
    return expiration == Expiration(tm);
}
