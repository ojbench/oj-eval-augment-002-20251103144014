#include "include/int2048.h"
#include <algorithm>
#include <cstring>

namespace sjtu {

// Helper functions
void removeLeadingZeros(std::vector<int> &digits) {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
}

// Constructors
int2048::int2048() : sign(false), digits(1, 0) {}

int2048::int2048(long long x) {
    if (x == 0) {
        sign = false;
        digits.push_back(0);
        return;
    }

    sign = (x < 0);
    if (x < 0) x = -x;

    while (x > 0) {
        digits.push_back(x % 10000);
        x /= 10000;
    }
}

int2048::int2048(const std::string &s) {
    read(s);
}

int2048::int2048(const int2048 &other) : sign(other.sign), digits(other.digits) {}

// Read from string
void int2048::read(const std::string &s) {
    digits.clear();
    sign = false;

    int start = 0;
    if (s[0] == '-') {
        sign = true;
        start = 1;
    } else if (s[0] == '+') {
        start = 1;
    }

    // Read digits from right to left in groups of 4
    int len = s.length();
    for (int i = len; i > start; i -= 4) {
        int val = 0;
        int power = 1;
        // Read from right to left within the group
        for (int j = i - 1; j >= std::max(start, i - 4); j--) {
            val += (s[j] - '0') * power;
            power *= 10;
        }
        digits.push_back(val);
    }

    removeLeadingZeros(digits);

    // Handle -0 case
    if (digits.size() == 1 && digits[0] == 0) {
        sign = false;
    }
}

// Print
void int2048::print() {
    if (sign) std::cout << '-';
    std::cout << digits.back();
    for (int i = digits.size() - 2; i >= 0; i--) {
        std::cout.width(4);
        std::cout.fill('0');
        std::cout << digits[i];
    }
}

// Helper: compare absolute values
// Returns: -1 if |a| < |b|, 0 if |a| == |b|, 1 if |a| > |b|
int compareAbs(const int2048 &a, const int2048 &b) {
    if (a.digits.size() != b.digits.size()) {
        return a.digits.size() < b.digits.size() ? -1 : 1;
    }
    for (int i = a.digits.size() - 1; i >= 0; i--) {
        if (a.digits[i] != b.digits[i]) {
            return a.digits[i] < b.digits[i] ? -1 : 1;
        }
    }
    return 0;
}

// Helper: add absolute values
int2048 addAbs(const int2048 &a, const int2048 &b) {
    int2048 result;
    result.digits.clear();
    result.sign = false;

    int carry = 0;
    size_t maxLen = std::max(a.digits.size(), b.digits.size());

    for (size_t i = 0; i < maxLen || carry; i++) {
        int sum = carry;
        if (i < a.digits.size()) sum += a.digits[i];
        if (i < b.digits.size()) sum += b.digits[i];

        result.digits.push_back(sum % 10000);
        carry = sum / 10000;
    }

    return result;
}

// Helper: subtract absolute values (assumes |a| >= |b|)
int2048 subAbs(const int2048 &a, const int2048 &b) {
    int2048 result;
    result.digits.clear();
    result.sign = false;

    int borrow = 0;
    for (size_t i = 0; i < a.digits.size(); i++) {
        int diff = a.digits[i] - borrow;
        if (i < b.digits.size()) diff -= b.digits[i];

        if (diff < 0) {
            diff += 10000;
            borrow = 1;
        } else {
            borrow = 0;
        }

        result.digits.push_back(diff);
    }

    removeLeadingZeros(result.digits);
    return result;
}

// Forward declarations
int2048 add(int2048, const int2048 &);
int2048 minus(int2048, const int2048 &);

// Add
int2048 &int2048::add(const int2048 &other) {
    *this = ::sjtu::add(*this, other);
    return *this;
}

int2048 add(int2048 a, const int2048 &b) {
    if (a.sign == b.sign) {
        int2048 result = addAbs(a, b);
        result.sign = a.sign;
        return result;
    } else {
        int cmp = compareAbs(a, b);
        if (cmp == 0) {
            return int2048(0);
        } else if (cmp > 0) {
            int2048 result = subAbs(a, b);
            result.sign = a.sign;
            return result;
        } else {
            int2048 result = subAbs(b, a);
            result.sign = b.sign;
            return result;
        }
    }
}

// Subtract
int2048 &int2048::minus(const int2048 &other) {
    *this = ::sjtu::minus(*this, other);
    return *this;
}

int2048 minus(int2048 a, const int2048 &b) {
    int2048 negB = b;
    negB.sign = !negB.sign;
    if (negB.digits.size() == 1 && negB.digits[0] == 0) {
        negB.sign = false;
    }
    return add(a, negB);
}

// Operator overloading
int2048 int2048::operator+() const {
    return *this;
}

int2048 int2048::operator-() const {
    int2048 result = *this;
    if (!(result.digits.size() == 1 && result.digits[0] == 0)) {
        result.sign = !result.sign;
    }
    return result;
}

int2048 &int2048::operator=(const int2048 &other) {
    if (this != &other) {
        sign = other.sign;
        digits = other.digits;
    }
    return *this;
}

int2048 &int2048::operator+=(const int2048 &other) {
    return add(other);
}

int2048 operator+(int2048 a, const int2048 &b) {
    return add(a, b);
}

int2048 &int2048::operator-=(const int2048 &other) {
    return minus(other);
}

int2048 operator-(int2048 a, const int2048 &b) {
    return minus(a, b);
}

// FFT-based multiplication
namespace {
    const double PI = acos(-1.0);

    void fft(std::vector<std::complex<double>> &a, bool invert) {
        int n = a.size();
        if (n == 1) return;

        for (int i = 1, j = 0; i < n; i++) {
            int bit = n >> 1;
            for (; j & bit; bit >>= 1) {
                j ^= bit;
            }
            j ^= bit;
            if (i < j) std::swap(a[i], a[j]);
        }

        for (int len = 2; len <= n; len <<= 1) {
            double ang = 2 * PI / len * (invert ? -1 : 1);
            std::complex<double> wlen(cos(ang), sin(ang));
            for (int i = 0; i < n; i += len) {
                std::complex<double> w(1);
                for (int j = 0; j < len / 2; j++) {
                    std::complex<double> u = a[i + j];
                    std::complex<double> v = a[i + j + len / 2] * w;
                    a[i + j] = u + v;
                    a[i + j + len / 2] = u - v;
                    w *= wlen;
                }
            }
        }

        if (invert) {
            for (auto &x : a) {
                x /= n;
            }
        }
    }

    std::vector<int> multiplyFFT(const std::vector<int> &a, const std::vector<int> &b) {
        std::vector<std::complex<double>> fa(a.begin(), a.end());
        std::vector<std::complex<double>> fb(b.begin(), b.end());

        int n = 1;
        while (n < (int)(a.size() + b.size())) {
            n <<= 1;
        }
        fa.resize(n);
        fb.resize(n);

        fft(fa, false);
        fft(fb, false);

        for (int i = 0; i < n; i++) {
            fa[i] *= fb[i];
        }

        fft(fa, true);

        std::vector<int> result(n);
        long long carry = 0;
        for (int i = 0; i < n; i++) {
            long long val = (long long)(fa[i].real() + 0.5) + carry;
            result[i] = val % 10000;
            carry = val / 10000;
        }

        while (carry) {
            result.push_back(carry % 10000);
            carry /= 10000;
        }

        return result;
    }
}

// Multiplication
int2048 &int2048::operator*=(const int2048 &other) {
    *this = *this * other;
    return *this;
}

int2048 operator*(int2048 a, const int2048 &b) {
    // Use naive multiplication for small numbers
    const int THRESHOLD = 100;

    if (a.digits.size() < THRESHOLD && b.digits.size() < THRESHOLD) {
        int2048 result;
        result.digits.clear();
        result.digits.resize(a.digits.size() + b.digits.size(), 0);

        for (size_t i = 0; i < a.digits.size(); i++) {
            long long carry = 0;
            for (size_t j = 0; j < b.digits.size() || carry; j++) {
                long long cur = result.digits[i + j] + carry;
                if (j < b.digits.size()) {
                    cur += (long long)a.digits[i] * b.digits[j];
                }
                result.digits[i + j] = cur % 10000;
                carry = cur / 10000;
            }
        }

        removeLeadingZeros(result.digits);
        result.sign = (a.sign != b.sign);

        if (result.digits.size() == 1 && result.digits[0] == 0) {
            result.sign = false;
        }

        return result;
    }

    // Use FFT for large numbers
    int2048 result;
    result.digits = multiplyFFT(a.digits, b.digits);
    removeLeadingZeros(result.digits);
    result.sign = (a.sign != b.sign);

    if (result.digits.size() == 1 && result.digits[0] == 0) {
        result.sign = false;
    }

    return result;
}

// Division and modulo
int2048 &int2048::operator/=(const int2048 &other) {
    *this = *this / other;
    return *this;
}

int2048 operator/(int2048 a, const int2048 &b) {
    // Handle zero dividend
    if (a.digits.size() == 1 && a.digits[0] == 0) {
        return int2048(0);
    }

    // For floor division: if signs differ and there's a remainder, subtract 1
    bool resultSign = (a.sign != b.sign);

    // Work with absolute values
    int2048 dividend = a;
    dividend.sign = false;
    int2048 divisor = b;
    divisor.sign = false;

    // Check if dividend < divisor
    if (compareAbs(dividend, divisor) < 0) {
        if (resultSign) {
            return int2048(-1);
        } else {
            return int2048(0);
        }
    }

    // Long division algorithm
    int2048 quotient;
    quotient.digits.clear();
    quotient.sign = false;

    int2048 current;
    current.digits.clear();
    current.sign = false;

    // Process from most significant to least significant
    for (int i = dividend.digits.size() - 1; i >= 0; i--) {
        // Shift current left by one base-10000 digit and add next digit
        current.digits.insert(current.digits.begin(), dividend.digits[i]);
        removeLeadingZeros(current.digits);

        // Find how many times divisor fits into current
        int q = 0;
        if (compareAbs(current, divisor) >= 0) {
            // Binary search for the quotient digit
            int left = 0, right = 9999;
            while (left <= right) {
                int mid = (left + right) / 2;
                int2048 temp = divisor;

                // Multiply divisor by mid
                long long carry = 0;
                for (size_t j = 0; j < temp.digits.size() || carry; j++) {
                    if (j >= temp.digits.size()) temp.digits.push_back(0);
                    long long prod = (long long)temp.digits[j] * mid + carry;
                    temp.digits[j] = prod % 10000;
                    carry = prod / 10000;
                }
                removeLeadingZeros(temp.digits);

                int cmp = compareAbs(temp, current);
                if (cmp <= 0) {
                    q = mid;
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }

            // Subtract q * divisor from current
            int2048 temp = divisor;
            long long carry = 0;
            for (size_t j = 0; j < temp.digits.size() || carry; j++) {
                if (j >= temp.digits.size()) temp.digits.push_back(0);
                long long prod = (long long)temp.digits[j] * q + carry;
                temp.digits[j] = prod % 10000;
                carry = prod / 10000;
            }
            removeLeadingZeros(temp.digits);

            current = subAbs(current, temp);
        }

        quotient.digits.insert(quotient.digits.begin(), q);
    }

    removeLeadingZeros(quotient.digits);

    // Check if we need to adjust for floor division
    bool hasRemainder = !(current.digits.size() == 1 && current.digits[0] == 0);

    if (resultSign && hasRemainder) {
        // For floor division with different signs and remainder, subtract 1
        int2048 one(1);
        quotient = addAbs(quotient, one);
    }

    quotient.sign = resultSign;
    if (quotient.digits.size() == 1 && quotient.digits[0] == 0) {
        quotient.sign = false;
    }

    return quotient;
}

int2048 &int2048::operator%=(const int2048 &other) {
    *this = *this % other;
    return *this;
}

int2048 operator%(int2048 a, const int2048 &b) {
    // a % b = a - (a / b) * b
    int2048 quotient = a / b;
    int2048 result = a - quotient * b;
    return result;
}

// Stream operators
std::istream &operator>>(std::istream &is, int2048 &num) {
    std::string s;
    is >> s;
    num.read(s);
    return is;
}

std::ostream &operator<<(std::ostream &os, const int2048 &num) {
    if (num.sign) os << '-';
    os << num.digits.back();
    for (int i = num.digits.size() - 2; i >= 0; i--) {
        os.width(4);
        os.fill('0');
        os << num.digits[i];
    }
    return os;
}

// Comparison operators
bool operator==(const int2048 &a, const int2048 &b) {
    if (a.sign != b.sign) return false;
    if (a.digits.size() != b.digits.size()) return false;
    for (size_t i = 0; i < a.digits.size(); i++) {
        if (a.digits[i] != b.digits[i]) return false;
    }
    return true;
}

bool operator!=(const int2048 &a, const int2048 &b) {
    return !(a == b);
}

bool operator<(const int2048 &a, const int2048 &b) {
    if (a.sign != b.sign) return a.sign;

    int cmp = compareAbs(a, b);
    if (a.sign) {
        return cmp > 0;
    } else {
        return cmp < 0;
    }
}

bool operator>(const int2048 &a, const int2048 &b) {
    return b < a;
}

bool operator<=(const int2048 &a, const int2048 &b) {
    return !(a > b);
}

bool operator>=(const int2048 &a, const int2048 &b) {
    return !(a < b);
}

} // namespace sjtu
