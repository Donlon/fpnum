#pragma once

#include <string>
#include <cmath>

/// @brief Structure for floating point number representation
/// @param e exponent
/// @param m mantissa
/// @param b bias
template<unsigned int _e, unsigned int _m, unsigned int _b = (1u << _e) - 1 + _m>
struct fpnum {
    // v = m(with hidden bit) * 2^(e - bias)
    static const unsigned int exp_n = _e;
    static const unsigned int man_n = _m;
    static const unsigned int bias = _b;
    static const unsigned int exp_max = (1u << _e) - 1;
    static const unsigned int man_max = (1u << _m) - 1;
    static const unsigned int size = _e + _m + 1;

    // static const fpnum<_e, _m, _b> zero = fpnum<e, m>(0, 0, 0);
    // static const fpnum<_e, _m, _b> max = fpnum<e, m>(0, -1, -1);
    // static const fpnum<_e, _m, _b> min = fpnum<e, m>(1, -1, -1);

    enum {
        Positive = 0,
        Negative = 1
    };

#pragma pack(push, 1)
    union {
        struct {
            unsigned int mantissa: _m; // LSB
            unsigned int exponent: _e; // MSB
            unsigned int sign: 1;
        };
        unsigned bits: 1 + _e + _m;
    };
#pragma pack(pop)

    fpnum(int sgn, unsigned int exp, unsigned int man) // NOLINT
            : sign(sgn), exponent(exp), mantissa(man) {
        static_assert(_m <= 16);
    }

    fpnum<_e, _m, _b> &operator=(const fpnum<_e, _m, _b> &other) {
        this->bits = other.bits;
        return *this;
    }

    template<int sgn, unsigned int exp, unsigned int man>
    static fpnum<_e, _m, _b> from() {
        static_assert(exp <= exp_max, "exponent value is too big");
        static_assert(man <= man_max, "mantissa value is too big");
        static_assert(sgn == Positive || sgn == Negative, "sign should be either 0 or 1");
        return fpnum<_e, _m, _b>(sgn, exp, man);
    }

    static fpnum<_e, _m, _b> fromValue(float value) {
        int sign;
        if (value < 0) {
            sign = Negative;
            value = -value;
        } else {
            sign = Positive;
        }
        int e = floorf(log2f(value / (float) (1u << _m)) + bias);
        if (e < 0) {
            return getZero();
        } else if (e > exp_max) {
            return sign == Positive ? getMax() : getMin();
        }
        float step = exp2f(e - (int) bias);
        int m = roundf(value / step - (float) (1u << _m));
        if (m > man_max) {
            e++;
            m = 0;
        }
        if (e > exp_max) {
            return sign == Positive ? getMax() : getMin();
        }
        return fpnum<_e, _m, _b>(sign, e, m);
    }

    template<unsigned int exp, unsigned int man>
    static fpnum<_e, _m, _b> positiveNum() {
        return from<Positive, exp, man>();
    }

    template<unsigned int exp, unsigned int man>
    static fpnum<_e, _m, _b> negativeNum() {
        return from<Negative, exp, man>();
    }

    bool isPositive() const { // NOLINT
        return sign == Positive && !isZero();
    }

    bool isNegative() const { // NOLINT
        return sign == Negative && !isZero();
    }

    bool isZero() const { // NOLINT
        return exponent == 0 && mantissa == 0;
    }

    fpnum<_e, _m, _b> operator+() const {
        return *this;
    }

    fpnum<_e, _m, _b> operator-() const {
        fpnum<_e, _m, _b> n = *this;
        n.sign ^= 1;
        return n;
    }

    fpnum<_e, _m, _b> operator+(const fpnum<_e, _m, _b> &other) const {
        if (this->isZero()) {
            return other;
        } else if (other.isZero()) {
            return *this;
        }
        if (this->sign == other.sign) {
            int expDiff = (int) this->exponent - (int) other.exponent;
            fpnum<_e, _m, _b> result(this->sign, this->exponent, 0);
            unsigned int man;
            if (expDiff > 0) { // this > other
                man = this->getRealMantissa() + (other.getRealMantissa() >> expDiff);
            } else if (expDiff < 0) { // this > other
                result.exponent = other.exponent;
                man = other.getRealMantissa() + (this->getRealMantissa() >> (-expDiff));
            } else {
                man = other.getRealMantissa() + this->getRealMantissa();
            }
            if (man > (1u << (_m + 1)) - 1) {
                if (result.exponent == exp_max) {
                    return this->sign == Positive ? getMax() : getMin(); // Overflowed
                }
                result.exponent += 1;
                result.mantissa = man / 2; // hidden bit excluded
            } else {
                result.mantissa = man;
            }
            return result;
        } else { // (+A) + (-B) or (-A) + (+B) = -((+A) + (-B))
            int expDiff = (int) this->exponent - (int) other.exponent;
            fpnum<_e, _m, _b> result(this->sign, this->exponent, 0);
            unsigned int man;

            // |this| - |other|
            if (expDiff > 0) { // this > other
                man = (this->getRealMantissa() << expDiff) - other.getRealMantissa();
            } else if (expDiff < 0) { // this > other
                result.exponent = other.exponent;
                man = (other.getRealMantissa() << (-expDiff)) - this->getRealMantissa();
            } else {
                man = other.getRealMantissa() - this->getRealMantissa();
            }
            // ||A| - |B|| <= max(|A|, |B|) <= max
            if (man == 0) {
                return getZero();
            } else { // Normalization
                int absExpDiff;
                if (expDiff >= 0) {
                    absExpDiff = expDiff;
                } else {
                    absExpDiff = -expDiff;
                }
                // 1111111_0000_0000000..
                unsigned int mask = ~(1 << (man_n + absExpDiff)) + 1;
                size_t shifts = 0;
                while ((mask & man) != (1 << (man_n + absExpDiff))) {
                    shifts++;
                    man <<= 1;
                }
                result.mantissa = (man >> absExpDiff) & man_max;
                if (result.exponent >= shifts) {
                    result.exponent -= shifts;
                } else {
                    return getZero();
                }
            }
            // (-A) + (+B) = -((+A) + (-B)), invert the sign
            // expDiff < 0, invert the sign
            if ((this->sign == Negative) != (expDiff < 0)) {
                result.sign ^= 1;
            }
            return result;
        }
    }

    fpnum<_e, _m, _b> operator-(const fpnum<_e, _m, _b> &num) const {
        return *this + (-num);
    }

    fpnum<_e, _m, _b> operator*(const fpnum<_e, _m, _b> &num) const {
        if (this->isZero() || num.isZero()) {
            return getZero();
        }
        int exp = (int) this->exponent + (int) num.exponent - (int) bias; // e - b = (this.e - b) + (num.e - b)
        if (exp > (int) exp_max) {
            return this->sign == num.sign ? getMax() : getMin();
        }

        unsigned int man = this->getRealMantissa() * num.getRealMantissa();
        // Normalization
        unsigned int mask = ~(1 << (2 * man_n)) + 1;
        size_t shifts = 0;
        while ((mask & man) != (1 << (2 * man_n))) {
            shifts++;
            man >>= 1;
        }
        exp += shifts;
        exp += man_n;
        man >>= man_n;
        if (exp > (int) exp_max) {
            return this->sign == num.sign ? getMax() : getMin();
        } else if (exp < 0) {
            return getZero();
        }
        fpnum<_e, _m, _b> result(this->sign == num.sign ? Positive : Negative, exp, man & man_max);
        return result;
    }

    fpnum<_e, _m, _b> &operator+=(const fpnum<_e, _m, _b> &num) {
        this = this + num;
    }

    fpnum<_e, _m, _b> &operator-=(const fpnum<_e, _m, _b> &num) {
        this = this + num;
    }

    bool operator>(const fpnum<_e, _m, _b> &other) const {
        if (this->isZero() && other.isZero()) {
            return false;
        }
        if (this->sign != other.sign) {
            return !this->sign; // this >= 0 ? true : false
        } else if (!this->sign) { // this >= 0 ( && other >= 0)
            return this->exponent > other.exponent
                   || (this->exponent == other.exponent && this->mantissa > other.mantissa);
        } else { // this <= 0 ( && other <= 0)
            return this->exponent < other.exponent
                   || (this->exponent == other.exponent && this->mantissa < other.mantissa);
        }
    }

    bool operator>=(const fpnum<_e, _m, _b> &other) const {
        if (this->sign != other.sign) {
            if (this->isZero() && other.isZero()) {
                return true;
            }
            return !this->sign; // this >= 0 ? true : false
        } else if (!this->sign) { // this >= 0 ( && other >= 0)
            return this->exponent > other.exponent
                   || (this->exponent == other.exponent && this->mantissa >= other.mantissa);
        } else { // this <= 0 ( && other <= 0)
            return this->exponent < other.exponent
                   || (this->exponent == other.exponent && this->mantissa <= other.mantissa);
        }
    }

    bool operator<(const fpnum<_e, _m, _b> &other) const {
        return other > *this;
    }

    bool operator<=(const fpnum<_e, _m, _b> &other) const {
        return other >= *this;
    }

    bool operator==(const fpnum<_e, _m, _b> &other) const {
        return this->bits == other.bits || (this->isZero() && other.isZero());
    }

    bool operator!=(const fpnum<_e, _m, _b> &other) const {
        return !this->operator==(other);
    }

    unsigned int getRealMantissa() const { // NOLINT
        return mantissa + (1u << _m);
    }

    double getValue() const { // NOLINT
        if (isZero()) {
            return 0.;
        }
        double v = (double) (mantissa + (1u << _m)) * exp2l((int) exponent - (int) bias);
        if (sign == Negative) {
            return -v;
        } else {
            return v;
        }
    }

    std::string toString() const { // NOLINT
        return std::to_string(getValue());
    }

    std::string toRepresentation() const { // NOLINT
        std::string s;
        if (sign == Positive) {
            s += '+';
        } else {
            s += '-';
        }
        if (isZero()) {
            s += '0';
        } else {
            s += '1';
            for (int i = man_n - 1; i >= 0; i--) {
                if (mantissa & (1u << i)) {
                    s += '1';
                } else {
                    s += '0';
                }
            }
            s += 'e';
            bool foundLeadingNum = false;
            for (int i = exp_n - 1; i >= 0; i--) {
                if (exponent & (1u << i)) {
                    s += '1';
                    foundLeadingNum = true;
                } else if (foundLeadingNum) {
                    s += '0';
                }
            }
            if (!foundLeadingNum) {
                s += '0';
            }
        }
        return s;
    }

    static const fpnum<_e, _m, _b> &getZero() {
        static const fpnum<_e, _m, _b> zero = fpnum<_e, _m, _b>(Positive, 0, 0);
        return zero;
    }

    static const fpnum<_e, _m, _b> &getMax() {
        static const fpnum<_e, _m, _b> max = fpnum<_e, _m, _b>(Positive, -1, -1);
        return max;
    }

    static const fpnum<_e, _m, _b> &getMin() {
        static const fpnum<_e, _m, _b> min = fpnum<_e, _m, _b>(Negative, -1, -1);
        return min;
    }
};

template<unsigned int _e, unsigned int _m, unsigned int _b>
std::ostream &operator<<(std::ostream &os, const fpnum<_e, _m, _b> &v) {
    os << v.toRepresentation() << "(" << v.getValue() << ")";
    return os;
}
