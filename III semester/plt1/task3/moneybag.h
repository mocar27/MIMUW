#ifndef JNPZ3_MONEYBAG_H
#define JNPZ3_MONEYBAG_H

#include <cstddef>
#include <ostream>

using namespace std;

class Moneybag {

private:
    size_t livre;
    size_t solidus;
    size_t denier;

public:
    typedef size_t coin_number_t;

    Moneybag(size_t l, size_t s, size_t d);
    Moneybag(const Moneybag& m);

    constexpr coin_number_t livre_number() const {
        return this->livre;
    }
    constexpr coin_number_t solidus_number() const {
        return this->solidus;
    }
    constexpr coin_number_t denier_number() const {
        return this->denier;
    }
    

    Moneybag &operator=(const Moneybag &m);
    Moneybag operator+(const Moneybag &m);
    Moneybag operator-(const Moneybag &m);
    Moneybag operator*(size_t num);
    friend Moneybag operator*(size_t num, const Moneybag &m);

    Moneybag &operator+=(const Moneybag &m);
    Moneybag &operator-=(const Moneybag &m);
    Moneybag &operator*=(size_t num);

    friend bool operator==(const Moneybag &m1, const Moneybag &m2);
    bool operator!=(const Moneybag &m);
    partial_ordering operator<=>(const Moneybag &m);


    friend ostream &operator<<(ostream& os, const Moneybag &m);
    explicit operator bool() const;
};

class Value {

private:
    __uint128_t coins;

public:
    explicit Value(const Moneybag& m);
    Value(uint64_t num);
    Value();

    bool operator==(const Value &v);
    bool operator==(__uint128_t num);
    friend bool operator==(__uint128_t num, const Value &v);

    bool operator!=(const Value &v);
    bool operator!=(__uint128_t num);
    friend bool operator!=(__uint128_t num, const Value &v);
    Value & operator=(const Value &v);
    Value & operator=(const __uint128_t v);
    strong_ordering operator<=>(const Value &v);
    strong_ordering operator<=>(__uint128_t num);
    friend strong_ordering operator<=>(__uint128_t num, const Value &v);

    explicit operator string();
};

// ======= CONST MONEYBAG OBJECTS =======
const Moneybag Livre = Moneybag(1,0,0);
const Moneybag Solidus = Moneybag(0,1,0);
const Moneybag Denier = Moneybag(0,0,1);

#endif // JNPZ3_MONEYBAG_H