#ifndef COMBINATIONS_DATETIME_H
#define COMBINATIONS_DATETIME_H

#include <ctime>

enum class OffsetType : char { Year = 'y', Quoter = 'q', Month = 'm', Day = 'd' };

struct Period {
    Period(const OffsetType& type, std::size_t amount);

    OffsetType type;
    std::size_t amount;
};

struct Expiration {
    Expiration() = default;
    explicit Expiration(const std::tm& tm);
    bool check_expiration(const Period& period, const Expiration& expiration);

    // Сравнение на равенство: ==, !=
    friend bool operator==(const Expiration& day1, const Expiration& day2);
    friend bool operator!=(const Expiration& day1, const Expiration& day2);

    // Сравнение: <, <=
    friend bool operator<(const Expiration& day1, const Expiration& day2);
    friend bool operator<=(const Expiration& day1, const Expiration& day2);

    // Сравнение: >, >=
    friend bool operator>(const Expiration& day1, const Expiration& day2);
    friend bool operator>=(const Expiration& day1, const Expiration& day2);

private:
    int year;
    int month;
    int day;
};

#endif  // COMBINATIONS_DATETIME_H
