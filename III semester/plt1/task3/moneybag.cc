#include "moneybag.h"

using namespace std;

// Moneybag functions
// ======= CONSTRUCTORS =======
Moneybag::Moneybag(uint64_t l, uint64_t s, uint64_t d) :
        livre(l),
        solidus(s),
        denier(d) {}

Moneybag::Moneybag(const Moneybag &m) = default;

// ======= GETTERS =======

// ======= OPERATORS =======
Moneybag &Moneybag::operator=(const Moneybag &m) = default;

Moneybag Moneybag::operator+(const Moneybag &m) {
    return Moneybag(*this) += m;
}

Moneybag Moneybag::operator-(const Moneybag &m) {
    return Moneybag(*this) -= m;
}

Moneybag Moneybag::operator*(size_t num) {
    return Moneybag(*this) *= num;
}


Moneybag operator*(size_t num, const Moneybag &m) {
    return Moneybag(m) *= num;
}

Moneybag &Moneybag::operator+=(const Moneybag &m) {
    long long unsigned int result = 0;

    if (__builtin_uaddll_overflow(this->livre, m.livre, &result)) {
        throw out_of_range("Livres number after addition is out of range!");
    }
    if (__builtin_uaddll_overflow(this->solidus, m.solidus, &result)) {
        throw out_of_range("Soliduses number after addition is out of range!");
    }
    if (__builtin_uaddll_overflow(this->denier, m.denier, &result)) {
        throw out_of_range("Deniers number after addition is out of range!");
    }

    this->livre += m.livre;
    this->solidus += m.solidus;
    this->denier += m.denier;

    return *this;
}

Moneybag &Moneybag::operator-=(const Moneybag &m) {
    long long unsigned int result = 0;

    if (__builtin_usubll_overflow(this->livre, m.livre, &result)) {
        throw out_of_range("Livres number after subtraction is less than 0!");
    }
    if (__builtin_usubll_overflow(this->solidus, m.solidus, &result)) {
        throw out_of_range("Soliduses number after subtraction is less than 0!");
    }
    if (__builtin_usubll_overflow(this->denier, m.denier, &result)) {
        throw out_of_range("Deniers number after subtraction is less than 0!");
    }

    this->livre -= m.livre;
    this->solidus -= m.solidus;
    this->denier -= m.denier;

    return *this;
}

Moneybag &Moneybag::operator*=(size_t num) {
    long long unsigned int result = 0;

    if (__builtin_umulll_overflow(this->livre, num, &result)) {
        throw out_of_range("Livres multiplied by number is out of range!");
    }
    if (__builtin_umulll_overflow(this->solidus, num, &result)) {
        throw out_of_range("Soliduses multiplied by number is out of range!");
    }
    if (__builtin_umulll_overflow(this->denier, num, &result)) {
        throw out_of_range("Deniers multiplied by number is out of range!");
    }

    this->livre *= num;
    this->solidus *= num;
    this->denier *= num;

    return *this;
}

bool operator==(const Moneybag &m1, const Moneybag &m2) {
    return (m1.livre == m2.livre) &&
           (m1.solidus == m2.solidus) &&
           (m1.denier == m2.denier);
}

bool Moneybag::operator!=(const Moneybag &m) {
    return (!(*this == m));
}

partial_ordering Moneybag::operator<=>(const Moneybag &m) {
    if ((this->livre == m.livre) &&
        (this->solidus == m.solidus) &&
        (this->denier == m.denier)) {
        return partial_ordering::equivalent;
    }
    if ((this->livre >= m.livre) &&
        (this->solidus >= m.solidus) &&
        (this->denier >= m.denier)) {
        return partial_ordering::greater;
    }
    if ((this->livre <= m.livre) &&
        (this->solidus <= m.solidus) &&
        (this->denier <= m.denier)) {
        return partial_ordering::less;
    }

    return partial_ordering::unordered;
}

ostream &operator<<(ostream &os, const Moneybag &m) {
    os << "(";
    os << m.livre;

    if (m.livre == 1) {
        os << " livr, ";
    } else {
        os << " livres, ";
    }
    os << m.solidus;
    if (m.solidus == 1) {
        os << " solidus, ";
    } else {
        os << " soliduses, ";
    }
    os << m.denier;
    if (m.denier == 1) {
        os << " denier)";
    } else {
        os << " deniers)";
    }

    return os;
}

// ======= CASTERS =======
Moneybag::operator bool() const {
    if (this->livre > 0) {
        return true;
    }
    if (this->solidus > 0) {
        return true;
    }
    if (this->denier > 0) {
        return true;
    }

    return false;
}


// Value functions
// ======= CONSTRUCTORS =======
Value::Value(const Moneybag &m) :
        coins((__uint128_t) m.denier_number() +
              (__uint128_t) ((__uint128_t) m.livre_number() * (__uint128_t) 240) +
              (__uint128_t) ((__uint128_t) m.solidus_number() * (__uint128_t) 12)) {}

Value::Value(uint64_t num) : coins(num) {}

Value::Value() : coins(0) {}

// ======= OPERATORS =======
bool Value::operator==(const Value &v) {
    return this->coins == v.coins;
}

bool Value::operator==(__uint128_t num) {
    return this->coins == num;
}

bool operator==(__uint128_t num, const Value &v) {
    return v.coins == num;
}

bool Value::operator!=(const Value &v) {
    return (!(*this == v));
}

bool Value::operator!=(__uint128_t num) {
    return this->coins != num;
}

bool operator!=(__uint128_t num, const Value &v) {
    return v.coins != num;
}
Value & Value::operator=(const Value &v) {
    this->coins = v.coins;
    return *this;
}
Value & Value::operator=(const __uint128_t v) {
    this->coins = v;
    return *this;
}


strong_ordering Value::operator<=>(const Value &v) {
    if (this->coins == v.coins) {
        return strong_ordering::equivalent;
    }
    if (this->coins > v.coins) {
        return strong_ordering::greater;
    }

    return strong_ordering::less;
}

strong_ordering Value::operator<=>(__uint128_t num) {
    if (this->coins == num) {
        return strong_ordering::equivalent;
    }
    if (this->coins > num) {
        return strong_ordering::greater;
    }

    return strong_ordering::less;
}

strong_ordering operator<=>(__uint128_t num, const Value &v) {
    if (v.coins == num) {
        return strong_ordering::equivalent;
    }
    if (v.coins > num) {
        return strong_ordering::greater;
    }

    return strong_ordering::less;
}

// ======= CASTERS =======
Value::operator string() {
    string s;
    __uint128_t digitGenerator = this->coins;

    if (digitGenerator == 0) {
        return "0";
    }
    while (digitGenerator > 0) {
        s += '0' + digitGenerator % 10;
        digitGenerator /= 10;
    }

    return string(s.rbegin(), s.rend());
}