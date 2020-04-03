#include "../include/big_integer.h"

#include <deque>

using namespace org::atib::numerics;

const std::unordered_map<number_base, byte>
        big_integer::number_base_number_of_bits_{{number_base::binary,      1},
                                                 {number_base::octal,       3},
                                                 {number_base::decimal,     4},
                                                 {number_base::hexadecimal, 4}};

const std::unordered_map<number_base, std::unordered_set<byte>>
        big_integer::number_base_allowed_digits_{
        {number_base::binary, {{0, 1, '0', '1'}}},
        {number_base::octal,
                              {{0, 1, 2,   3, 4, 5, 6, 7, '0', '1', '2', '3', '4', '5', '6', '7'}}},
        {number_base::decimal,
                              {{0, 1, 2,   3, 4, 5, 6, 7, 8,   9,   '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'}}},
        {number_base::hexadecimal,
                              {{0, 1, 2,   3, 4, 5, 6, 7, 8,   9,   10,  11,  12,  13,  14,  15,  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F'}}}
};

const std::unordered_map<number_base, const char *>
        big_integer::number_base_prefix_{{number_base::binary,      "0b"},
                                         {number_base::octal,       "0o"},
                                         {number_base::decimal,     ""},
                                         {number_base::hexadecimal, "0x"}};

const std::unordered_map<number_base, byte> big_integer::number_base_multiplier_{
        {number_base::binary,      2},
        {number_base::octal,       8},
        {number_base::decimal,     10},
        {number_base::hexadecimal, 16}
};

const big_integer big_integer::nan{big_integer::NaN};
const big_integer big_integer::zero{};
const big_integer big_integer::plus_one{"1"};
const big_integer big_integer::minus_one{"-1"};

big_integer org::atib::numerics::operator+(const big_integer &lhs,
                                           const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
        return lhs.add_two_big_integers_together(rhs);
    }

    return big_integer::nan;
}

big_integer org::atib::numerics::operator-(const big_integer &lhs,
                                           const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
        big_integer negated_rhs{rhs};
        negated_rhs.invert_sign();
        return lhs + negated_rhs;
    }

    return big_integer::nan;
}

big_integer org::atib::numerics::operator*(const big_integer &lhs,
                                           const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
        return lhs.multiply_two_big_integers(rhs);
    }
    return big_integer::nan;
}

big_integer org::atib::numerics::operator/(const big_integer &lhs,
                                           const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
#ifndef BIG_INTEGER_NO_THROW
        if (rhs.is_zero()) {
          if (lhs.is_zero())
            throw zero_divided_by_zero_error{};
          throw division_by_zero_error{};
        }
#else
        if (lhs.is_zero() || rhs.is_zero())
            return rhs.is_zero() ? big_integer::nan : big_integer::zero;
#endif

        const bool is_division_negative{
                (!lhs.is_negative_number() && rhs.is_negative_number()) ||
                (lhs.is_negative_number() && !rhs.is_negative_number())
        };

        big_integer dividend{lhs.is_negative_number() ? lhs.abs() : lhs};
        big_integer divisor{rhs.is_negative_number() ? rhs.abs() : rhs};

        if (dividend < divisor)
            return big_integer::zero;

        if (rhs.is_positive_one())
            return big_integer{lhs.get_decimal_digits()};

        if (rhs.is_negative_one()) {
            big_integer result{lhs.get_decimal_digits()};
            result.invert_sign();
            return result;
        }

        big_integer result{};
        while (dividend >= divisor) {
            size_t shift_count{};

            do {
                ++shift_count;
            } while (dividend >= (divisor << shift_count));

            --shift_count;
            big_integer big_one{big_integer::plus_one};
            result += (big_one << shift_count);
            dividend -= (divisor << shift_count);
        }

        if (!is_division_negative)
            return result;

        result.invert_sign();
        return result;
    }

    return big_integer::nan;
}

big_integer org::atib::numerics::operator%(const big_integer &lhs,
                                           const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
#ifndef BIG_INTEGER_NO_THROW
        if (rhs.is_zero()) {
          if (lhs.is_zero())
            throw zero_divided_by_zero_error{};
          throw division_by_zero_error{};
        }
#else
        if (lhs.is_zero() || rhs.is_zero())
            return rhs.is_zero() ? big_integer::nan : big_integer::zero;
#endif

        big_integer dividend{lhs.is_negative_number() ? lhs.abs() : lhs};
        const big_integer divisor{rhs.is_negative_number() ? rhs.abs() : rhs};

        if (dividend < divisor)
            return dividend;

        if (rhs.is_positive_one() || rhs.is_negative_one())
            return big_integer::zero;

        while (dividend >= divisor) {
            size_t shift_count{};

            do {
                ++shift_count;
            } while (dividend >= (divisor << shift_count));

            --shift_count;
            dividend -= (divisor << shift_count);
        }

        return dividend;
    }

    return big_integer::nan;
}

bool org::atib::numerics::operator==(const big_integer &lhs,
                                     const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan())
        return lhs.get_decimal_digits() == rhs.get_decimal_digits();
    return lhs.is_nan() && rhs.is_nan();
}

bool org::atib::numerics::operator!=(const big_integer &lhs,
                                     const big_integer &rhs) {
    return !(lhs == rhs);
}

bool org::atib::numerics::operator<(const big_integer &lhs,
                                    const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
        const size_t lhs_size{lhs.get_decimal_digits().size()};
        const size_t rhs_size{rhs.get_decimal_digits().size()};

        if (lhs.is_negative_number() && !rhs.is_negative_number())
            return true;

        if (!lhs.is_negative_number() && rhs.is_negative_number())
            return false;

        if (lhs_size != rhs_size) {
            if (lhs_size < rhs_size)
                return !lhs.is_negative_number() && !rhs.is_negative_number();

            return lhs.is_negative_number() && rhs.is_negative_number();
        } else {
            const auto lhs_digits{lhs.get_decimal_digits()};
            const auto rhs_digits{rhs.get_decimal_digits()};
            for (size_t i{}; i < lhs_size; ++i) {
                if (lhs_digits.at(i) < rhs_digits.at(i))
                    return true;
                if (lhs_digits.at(i) > rhs_digits.at(i))
                    return false;
            }
        }
    }
    return false;
}

bool org::atib::numerics::operator>(const big_integer &lhs,
                                    const big_integer &rhs) {
    return rhs < lhs;
}

bool org::atib::numerics::operator<=(const big_integer &lhs,
                                     const big_integer &rhs) {
    return !(lhs > rhs);
}

bool org::atib::numerics::operator>=(const big_integer &lhs,
                                     const big_integer &rhs) {
    return !(lhs < rhs);
}

big_integer org::atib::numerics::operator|(const big_integer &lhs,
                                           const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
        if (lhs.is_zero())
            return rhs;

        if (rhs.is_zero())
            return lhs;

        const std::vector<bool> &lhs_binary_digits{lhs.get_binary_digits()};
        const std::vector<bool> &rhs_binary_digits{rhs.get_binary_digits()};

        const bool is_lhs_longer{lhs_binary_digits.size() > rhs_binary_digits.size()};

        const std::vector<bool> &longer_binary_number{is_lhs_longer ? lhs_binary_digits
                                                                    : rhs_binary_digits};

        const std::vector<bool> &shorter_binary_number{!is_lhs_longer ? lhs_binary_digits
                                                                      : rhs_binary_digits};

        std::vector<bool> result{longer_binary_number};

        for (size_t i{shorter_binary_number.size()}, j{longer_binary_number.size() - 1}; i > 0U; --i, --j) {
            result[j] = result[j] || shorter_binary_number[i - 1];
        }

        return big_integer{std::move(result)};
    }

    return big_integer::nan;
}

big_integer org::atib::numerics::operator&(const big_integer &lhs,
                                           const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
        if (lhs.is_zero() || rhs.is_zero())
            return big_integer::zero;

        const std::vector<bool> &lhs_binary_digits{lhs.get_binary_digits()};
        const std::vector<bool> &rhs_binary_digits{rhs.get_binary_digits()};

        const bool is_lhs_longer{lhs_binary_digits.size() > rhs_binary_digits.size()};

        const std::vector<bool> &longer_binary_number{is_lhs_longer ? lhs_binary_digits
                                                                    : rhs_binary_digits};

        const std::vector<bool> &shorter_binary_number{!is_lhs_longer ? lhs_binary_digits
                                                                      : rhs_binary_digits};

        std::vector<bool> result(shorter_binary_number.size(), false);

        for (size_t i{shorter_binary_number.size()}, j{longer_binary_number.size()}; i > 0U; --i, --j) {
            result[i - 1] =
                    shorter_binary_number[i - 1] && longer_binary_number[j - 1];
        }

        return big_integer{std::move(result)};
    }

    return big_integer::nan;
}

big_integer org::atib::numerics::operator^(const big_integer &lhs,
                                           const big_integer &rhs) {
    if (!lhs.is_nan() && !rhs.is_nan()) {
        const std::vector<bool> &lhs_binary_digits{lhs.get_binary_digits()};
        const std::vector<bool> &rhs_binary_digits{rhs.get_binary_digits()};

        const bool is_lhs_longer{lhs_binary_digits.size() > rhs_binary_digits.size()};

        const std::vector<bool> &longer_binary_number{is_lhs_longer ? lhs_binary_digits
                                                                    : rhs_binary_digits};

        const std::vector<bool> &shorter_binary_number{!is_lhs_longer ? lhs_binary_digits
                                                                      : rhs_binary_digits};

        std::vector<bool> result{longer_binary_number};

        for (size_t i{shorter_binary_number.size()}, j{longer_binary_number.size() - 1}; i > 0U; --i, --j) {
            result[j] = (result[j] && !shorter_binary_number[i - 1]) || (!result[j] && shorter_binary_number[i - 1]);
        }

        return big_integer{std::move(result)};
    }

    return big_integer::nan;
}

void org::atib::numerics::swap(big_integer &lhs, big_integer &rhs) noexcept {
    lhs.swap(rhs);
}

std::ostream &org::atib::numerics::operator<<(std::ostream &os,
                                              const big_integer &bi) {
    return (os << bi.get_decimal_number());
}

std::istream &org::atib::numerics::operator>>(std::istream &is,
                                              big_integer &bi) {
    std::string line;
    is >> line;
    bi.reset(line);
    return is;
}

const std::vector<int> &big_integer::get_decimal_digits() const {
    return decimal_digits_;
}

const std::vector<bool> &big_integer::get_binary_digits() const {
    return binary_digits_;
}

std::string big_integer::get_decimal_number(
        const std::string &prefix,
        const std::string &postfix,
        const size_t number_of_digits) const {
    const size_t prefix_len{prefix.length()};
    const size_t postfix_len{postfix.length()};
    std::string decimal_number_str{this->decimal_number_str_};
    const size_t sign_offset{is_negative_number_ ? 1U : 0U};

    if (!prefix.empty() && !stl::helper::str_contains(decimal_number_str, prefix)) {
        const size_t first_digit_pos{
                decimal_number_str.find_first_of("0123456789", sign_offset)
        };

        decimal_number_str.replace(
                std::cbegin(decimal_number_str) + sign_offset,
                std::cbegin(decimal_number_str) + first_digit_pos,
                prefix);
    }

    if (!postfix.empty() && !stl::helper::str_ends_with(decimal_number_str, postfix)) {
        const size_t last_digit_pos{decimal_number_str.find_last_of("0123456789")};

        decimal_number_str.replace(
                std::cbegin(decimal_number_str) + last_digit_pos + 1,
                std::cend(decimal_number_str),
                postfix);
    }

    const size_t count_of_decimal_digits{
            decimal_number_str.length() - (prefix_len + postfix_len + sign_offset)
    };

    if (number_of_digits != 0 && number_of_digits > count_of_decimal_digits)
        decimal_number_str.insert(
                std::cbegin(decimal_number_str) + sign_offset + prefix_len,
                number_of_digits - count_of_decimal_digits,
                '0');

    return decimal_number_str;
}

std::string big_integer::get_binary_number(
        const std::string &prefix,
        const std::string &postfix,
        const size_t number_of_digits) const {
    const size_t prefix_len{prefix.length()};
    const size_t postfix_len{postfix.length()};
    std::string binary_number_str{this->binary_number_str_};
    const size_t sign_offset{is_negative_number_ ? 1U : 0U};

    if (!prefix.empty() && !stl::helper::str_contains(binary_number_str, prefix)) {
        const size_t first_digit_pos{
                binary_number_str.find_first_of("01", sign_offset)
        };

        binary_number_str.replace(std::cbegin(binary_number_str) + sign_offset,
                                  std::cbegin(binary_number_str) + first_digit_pos,
                                  prefix);
    }

    if (!postfix.empty() && !stl::helper::str_ends_with(binary_number_str, postfix)) {
        const size_t last_digit_pos{binary_number_str.find_last_of("01")};

        binary_number_str.replace(
                std::cbegin(binary_number_str) + last_digit_pos + 1,
                std::cend(binary_number_str),
                postfix);
    }

    const size_t count_of_bin_digits{binary_number_str.length() - (prefix_len + postfix_len + sign_offset)};

    if (number_of_digits != 0 && number_of_digits > count_of_bin_digits)
        binary_number_str.insert(
                std::cbegin(binary_number_str) + sign_offset + prefix_len,
                number_of_digits - count_of_bin_digits,
                '0');

    return binary_number_str;
}

bool big_integer::is_nan() const noexcept {
    return stl::helper::str_starts_with(decimal_number_str_, NaN, true);
}

bool big_integer::is_zero() const noexcept {
    return decimal_number_str_.empty() || decimal_number_str_ == "0";
}

bool big_integer::is_positive_one() const noexcept {
    return decimal_number_str_ == "1";
}

bool big_integer::is_negative_one() const noexcept {
    return decimal_number_str_ == "-1";
}

big_integer big_integer::abs() const {
    auto decimal_digits{this->decimal_digits_};

    if (this->is_negative_number_) {
        for (auto &digit : decimal_digits)
            digit = std::abs(digit);
    }

    return big_integer{std::move(decimal_digits)};
}

bool big_integer::is_negative_number() const {
    return is_negative_number_;
}

org::atib::numerics::big_integer::big_integer()
        : decimal_digits_{0},
          binary_digits_{false},
          hexadecimal_number_str_{"0x0"},
          decimal_number_str_{"0"},
          octal_number_str_{"0o0"},
          binary_number_str_{"0b0"},
          first_{0},
          last_{1},
          base_{number_base::decimal},
          is_negative_number_{} {}

big_integer::big_integer(std::vector<int> digits,
                         const number_base base /*= number_base::decimal*/)
        : base_{base} {
    if (!check_and_process_number_input_digits(std::move(digits))) {
#ifndef BIG_INTEGER_NO_THROW
        throw std::invalid_argument{
          "Input number digits do not form a valid number!"
        };
#else
        set_big_integer_value_nan();
#endif
    }
}

big_integer::big_integer(std::vector<bool> binary_digits)
        : base_{number_base::binary} {
    check_and_process_binary_number_digits(std::move(binary_digits));
}

big_integer::big_integer(const char *number)
        : big_integer{std::string{number}} {}

big_integer::big_integer(const std::string &number)
        : base_{number_base::decimal} {
    if (!check_and_process_number_input_characters(number)) {
#ifndef BIG_INTEGER_NO_THROW
        throw std::invalid_argument{
          "Input number string does not contain a valid number!"
        };
#else
        set_big_integer_value_nan();
#endif
    }

    if (!is_zero() && !is_positive_one() && !is_negative_one()) {
        convert_number_to_decimal_and_store();
        update_big_integer_string_representations();
    }
}

std::string big_integer::get_big_integer(number_base base) const {
    switch (base) {
        case number_base::decimal:
            return get_decimal_number();
        case number_base::hexadecimal:
            return get_hexadecimal_number();
        case number_base::octal:
            return get_octal_number();
        case number_base::binary:
            return get_binary_number();
        default:
            return get_decimal_number();
    }
}

big_integer::operator std::string() const noexcept {
    return get_big_integer(number_base::decimal);
}

big_integer::operator const char *() const noexcept {
    return this->decimal_number_str_.c_str();
}

big_integer big_integer::operator-() const {
    big_integer result{*this};
    result.invert_sign();
    return result;
}

big_integer &big_integer::operator+=(const big_integer &rhs) {
    big_integer result{*this + rhs};
    this->swap(result);
    return *this;
}

big_integer &big_integer::operator-=(const big_integer &rhs) {
    big_integer negated_rhs(rhs);
    negated_rhs.invert_sign();
    big_integer result(this->add_two_big_integers_together(negated_rhs));
    this->swap(result);
    return *this;
}

big_integer &big_integer::operator*=(const big_integer &rhs) {
    big_integer result(this->multiply_two_big_integers(rhs));
    this->swap(result);
    return *this;
}

big_integer &big_integer::operator/=(const big_integer &rhs) {
    big_integer result{*this / rhs};
    this->swap(result);
    return *this;
}

big_integer &big_integer::operator%=(const big_integer &rhs) {
    big_integer result{*this % rhs};
    this->swap(result);
    return *this;
}

big_integer &big_integer::operator++() {
    if (!this->is_nan()) {

        big_integer temp{this->add_two_big_integers_together(big_integer::plus_one)};
        this->swap(temp);
    }

    return *this;
}

big_integer big_integer::operator++(int) {

    if (this->is_nan()) return *this;

    big_integer temp_bi{this->add_two_big_integers_together(big_integer::plus_one)};
    this->swap(temp_bi);

    return temp_bi;
}

big_integer &big_integer::operator--() {

    if (!this->is_nan()) {

        big_integer temp{this->add_two_big_integers_together(big_integer::minus_one)};
        this->swap(temp);
    }

    return *this;
}

big_integer big_integer::operator--(int) {

    if (this->is_nan()) return *this;

    big_integer temp_bi{this->add_two_big_integers_together(big_integer::minus_one)};
    this->swap(temp_bi);

    return temp_bi;
}

big_integer big_integer::operator<<(const size_t count) const {
    std::vector<bool> shifted_binary_digits{this->binary_digits_};
    if (count != 0U)
        shifted_binary_digits.insert(std::cend(shifted_binary_digits), count, false);
    return big_integer{std::move(shifted_binary_digits)};
}

big_integer big_integer::operator>>(const size_t count) const {
    if (count >= this->binary_digits_.size())
        return big_integer::zero;

    if (count != 0U) {
        std::vector<bool> shifted_binary_digits{this->binary_digits_};
        shifted_binary_digits.erase(std::cend(shifted_binary_digits) - count,
                                    std::cend(shifted_binary_digits));
        return big_integer{std::move(shifted_binary_digits)};
    }

    return big_integer{*this};
}

big_integer &big_integer::operator<<=(const size_t count) {
    if (count != 0U) {
        std::vector<bool> shifted_binary_digits{std::move(this->binary_digits_)};
        shifted_binary_digits.insert(std::cend(shifted_binary_digits), count, false);
        big_integer result{std::move(shifted_binary_digits)};
        this->swap(result);
    }

    return *this;
}

big_integer &big_integer::operator>>=(const size_t count) {
    if (count >= this->binary_digits_.size()) {
        this->set_big_integer_to_default_zero_value();
    } else if (count != 0U) {
        std::vector<bool> shifted_binary_digits{std::move(this->binary_digits_)};
        shifted_binary_digits.erase(std::cend(shifted_binary_digits) - count,
                                    std::cend(shifted_binary_digits));
        big_integer result{std::move(shifted_binary_digits)};
        this->swap(result);
    }
    return *this;
}

big_integer &big_integer::operator&=(const big_integer &rhs) {
    big_integer result{*this & rhs};
    this->swap(result);
    return *this;
}

big_integer &big_integer::operator|=(const big_integer &rhs) {
    big_integer result{*this | rhs};
    this->swap(result);
    return *this;
}

big_integer &big_integer::operator^=(const big_integer &rhs) {
    big_integer result{*this ^ rhs};
    this->swap(result);
    return *this;
}

size_t big_integer::operator[](const size_t factor) const {
    if (factor > last_ - first_ - 1)
        return 0U;

    return static_cast<size_t>(decimal_number_str_[last_ - 1 - factor] - '0');
}

big_integer::operator bool() const {
    return !decimal_number_str_.empty() && decimal_number_str_ != "0";
}

void big_integer::swap(big_integer &rhs) noexcept {
    std::swap(this->decimal_digits_, rhs.decimal_digits_);
    std::swap(this->binary_digits_, rhs.binary_digits_);
    std::swap(this->decimal_number_str_, rhs.decimal_number_str_);
    std::swap(this->binary_number_str_, rhs.binary_number_str_);
    std::swap(this->octal_number_str_, rhs.octal_number_str_);
    std::swap(this->hexadecimal_number_str_, rhs.hexadecimal_number_str_);
    std::swap(this->base_, rhs.base_);
    std::swap(this->is_negative_number_, rhs.is_negative_number_);
    std::swap(this->first_, rhs.first_);
    std::swap(this->last_, rhs.last_);
}

big_integer big_integer::add_two_big_integers_together(
        const big_integer &rhs) const {
    if (this->is_nan() || rhs.is_nan())
        return big_integer::nan;

    if (this->is_zero())
        return rhs;

    if (rhs.is_zero())
        return *this;

    std::deque<int> result{};

    bool is_negative_addition{};

    if ((this->is_negative_number() && !rhs.is_negative_number()) ||
        (!this->is_negative_number() && rhs.is_negative_number())) {
        auto abs_this{!this->is_negative_number() ? *this : this->abs()};
        auto abs_rhs{!rhs.is_negative_number() ? rhs : rhs.abs()};

        if (abs_this == abs_rhs)
            return big_integer::zero;

        const bool is_abs_first_greater_abs_second{abs_this > abs_rhs};
        const bool is_abs_second_greater_than_abs_first{
                !is_abs_first_greater_abs_second
        };

        is_negative_addition =
                (this->is_negative_number() && is_abs_first_greater_abs_second) ||
                (rhs.is_negative_number() && is_abs_second_greater_than_abs_first);

        const std::vector<int> &greater_number_decimal_digits{
                is_abs_first_greater_abs_second ? abs_this.decimal_digits_
                                                : abs_rhs.decimal_digits_
        };


        std::vector<int> &smaller_number_decimal_digits{
                !is_abs_first_greater_abs_second ? abs_this.decimal_digits_
                                                 : abs_rhs.decimal_digits_
        };

        for (auto &digit : smaller_number_decimal_digits)
            digit = -digit;

        size_t n{greater_number_decimal_digits.size()};
        size_t m{smaller_number_decimal_digits.size()};
        int carry{};

        while (n > 0U || m > 0U || carry != 0) {
            int sum{carry};
            carry = 0;

            if (n > 0U) {
                const size_t index{n - 1};
                if (sum + greater_number_decimal_digits[index] < 0) {
                    sum += 10 + greater_number_decimal_digits[index];
                    --carry;
                } else {
                    sum += greater_number_decimal_digits[index];
                }
                --n;
            }
            if (m > 0U) {
                const size_t index{m - 1};
                if (sum + smaller_number_decimal_digits[index] < 0) {
                    sum += 10 + smaller_number_decimal_digits[index];
                    --carry;
                } else {
                    sum += smaller_number_decimal_digits[index];
                }
                --m;
            }

            if (0 == carry)
                carry = sum / 10;

            const byte remainder{static_cast<byte>(std::abs(sum) % 10)};
            result.emplace_front(remainder);
        }

    } else {
        const std::vector<int> &lhs_digits{this->decimal_digits_};
        const std::vector<int> &rhs_digits{rhs.decimal_digits_};

        size_t n{lhs_digits.size()};
        size_t m{rhs_digits.size()};
        int carry{};

        while (n > 0U || m > 0U || carry != 0) {
            int sum{carry};

            if (n > 0U) {
                sum += lhs_digits[n - 1];
                --n;
            }
            if (m > 0U) {
                sum += rhs_digits[m - 1];
                --m;
            }

            carry = sum / 10;

            const byte remainder{static_cast<byte>(std::abs(sum) % 10)};

            result.emplace_front(sum < 0 ? -remainder : remainder);
        }
    }

    const auto first_non_zero_digit_pos =
            std::find_if(std::cbegin(result), std::cend(result), [](const auto digit) { return digit != 0; });

    if (first_non_zero_digit_pos != std::cbegin(result))
        result.erase(std::cbegin(result), first_non_zero_digit_pos);

    if (result.front() < 0 || is_negative_addition) {
        for (auto &digit : result) {
            digit = digit > 0 ? -digit : digit;
        }
    }

    return big_integer{
            std::vector<int>(std::make_move_iterator(std::begin(result)),
                             std::make_move_iterator(std::end(result)))
    };
}

big_integer big_integer::multiply_two_big_integers(
        const big_integer &rhs) const {
    if (this->is_zero() || rhs.is_zero())
        return zero;

    if (this->is_positive_one())
        return rhs;

    if (rhs.is_positive_one())
        return *this;

    if (this->is_negative_one())
        return -rhs;

    if (rhs.is_negative_one())
        return -(*this);

    std::vector<int> result{};
    result.resize(decimal_digits_.size() + rhs.decimal_digits_.size(), 0);

    const bool is_negative_product{
            (is_negative_number_ && !rhs.is_negative_number_) || (!is_negative_number_ && rhs.is_negative_number_)
    };

    for (size_t a{rhs.decimal_digits_.size()}; a > 0U; --a) {
        const size_t i{a - 1};
        int carry{};
        for (size_t b{decimal_digits_.size()}; b > 0U; --b) {
            const size_t j{b - 1};
            int product =
                    std::abs(decimal_digits_[j] * rhs.decimal_digits_[i]) + carry;
            carry = product / 10;
            product %= 10;
            result[i + j + 1] += product;
            carry += result[i + j + 1] / 10;
            result[i + j + 1] %= 10;
        }

        if (carry > 0)
            result[i] = carry;
    }

    const auto first_non_zero_digit_pos =
            std::find_if(std::cbegin(result), std::cend(result), [](const auto digit) { return digit != 0; });

    if (first_non_zero_digit_pos != std::cbegin(result))
        result.erase(std::cbegin(result), first_non_zero_digit_pos);

    big_integer multiplication{std::move(result)};

    if (is_negative_product)
        multiplication.invert_sign();

    return multiplication;
}

void big_integer::invert_sign() {
    is_negative_number_ = !is_negative_number_;

    for (auto &digit : decimal_digits_) {
        auto abs_digit = std::abs(digit);
        digit = is_negative_number_ && abs_digit != 0 ? -abs_digit : abs_digit;
    }

    if (!is_negative_number_ && '-' == decimal_number_str_[0]) {
        decimal_number_str_.erase(std::cbegin(decimal_number_str_));
        --first_;
        --last_;
    } else if (is_negative_number_ && '-' != decimal_number_str_[0]) {
        decimal_number_str_.insert(std::cbegin(decimal_number_str_), '-');
        ++first_;
        ++last_;
    }

    update_big_integer_string_representations();
}

std::string big_integer::get_hexadecimal_number(
        const std::string &prefix,
        const std::string &postfix,
        const size_t number_of_digits) const {
    const size_t prefix_len{prefix.length()};
    const size_t postfix_len{postfix.length()};
    std::string hex_number_str{this->hexadecimal_number_str_};
    const size_t sign_offset{is_negative_number_ ? 1U : 0U};

    if (!prefix.empty() && !stl::helper::str_contains(hex_number_str, prefix, sign_offset)) {
        const size_t first_digit_pos{
                hex_number_str.find_first_of("0123456789abcdefABCDEF", sign_offset)
        };

        hex_number_str.replace(std::cbegin(hex_number_str) + sign_offset,
                               std::cbegin(hex_number_str) + first_digit_pos,
                               prefix);
    }

    if (!postfix.empty() && !stl::helper::str_ends_with(hex_number_str, postfix, false)) {
        const size_t last_digit_pos{
                hex_number_str.find_last_of("0123456789abcdefABCDEF")
        };

        hex_number_str.replace(std::cbegin(hex_number_str) + last_digit_pos + 1,
                               std::cend(hex_number_str),
                               postfix);
    }

    const size_t count_of_hex_digits{hex_number_str.length() - (prefix_len + postfix_len + sign_offset)};

    if (number_of_digits != 0 && number_of_digits > count_of_hex_digits)
        hex_number_str.insert(
                std::cbegin(hex_number_str) + sign_offset + prefix_len,
                number_of_digits - count_of_hex_digits,
                '0');

    return hex_number_str;
}

std::string big_integer::get_octal_number(const std::string &prefix,
                                          const std::string &postfix,
                                          const size_t number_of_digits) const {
    const size_t prefix_len{prefix.length()};
    const size_t postfix_len{postfix.length()};
    std::string octal_number_str{this->octal_number_str_};
    const size_t sign_offset{is_negative_number_ ? 1U : 0U};

    if (!prefix.empty() && !stl::helper::str_contains(octal_number_str, prefix, sign_offset)) {
        const size_t first_digit_pos{
                octal_number_str.find_first_of("01234567", sign_offset)
        };

        octal_number_str.replace(std::cbegin(octal_number_str) + sign_offset,
                                 std::cbegin(octal_number_str) + first_digit_pos,
                                 prefix);
    }

    if (!postfix.empty() && !stl::helper::str_ends_with(octal_number_str, postfix)) {
        const size_t last_digit_pos{octal_number_str.find_last_of("01234567")};

        octal_number_str.replace(std::cbegin(octal_number_str) + last_digit_pos + 1,
                                 std::cend(octal_number_str),
                                 postfix);
    }

    const size_t count_of_octal_digits{octal_number_str.length() - (prefix_len + postfix_len + sign_offset)};

    if (number_of_digits != 0 && number_of_digits > count_of_octal_digits)
        octal_number_str.insert(
                std::cbegin(octal_number_str) + sign_offset + prefix_len,
                number_of_digits - count_of_octal_digits,
                '0');

    return octal_number_str;
}

void big_integer::set_big_integer_to_default_zero_value() {
    decimal_digits_.assign({0});
    binary_digits_.assign({false});
    decimal_number_str_ = "0";
    binary_number_str_ = "0b0";
    octal_number_str_ = "0o0";
    hexadecimal_number_str_ = "0x0";
    base_ = number_base::decimal;
    is_negative_number_ = false;
    first_ = 0;
    last_ = 1;
}

void big_integer::set_big_integer_value_to_positive_one() {
    decimal_digits_.assign({1});
    binary_digits_.assign({true});
    decimal_number_str_ = "1";
    binary_number_str_ = "0b1";
    octal_number_str_ = "0o1";
    hexadecimal_number_str_ = "0x1";
    base_ = number_base::decimal;
    is_negative_number_ = false;
    first_ = 0;
    last_ = 1;
}

void big_integer::set_big_integer_value_to_negative_one() {
    decimal_digits_.assign({-1});
    binary_digits_.assign({true});
    decimal_number_str_ = "-1";
    binary_number_str_ = "-0b1";
    octal_number_str_ = "-0o1";
    hexadecimal_number_str_ = "-0x1";
    base_ = number_base::decimal;
    is_negative_number_ = true;
    first_ = 0;
    last_ = 1;
}

void big_integer::set_big_integer_value_nan() {
    decimal_digits_.clear();
    binary_digits_.clear();
    decimal_number_str_ = "NaN";
    binary_number_str_ = "NaN";
    octal_number_str_ = "NaN";
    hexadecimal_number_str_ = "NaN";
    base_ = number_base::decimal;
    is_negative_number_ = false;
    first_ = 0;
    last_ = 0;
}

void big_integer::check_and_process_binary_number_digits(
        std::vector<bool> binary_number) {
    if (binary_number.empty() || (1U == binary_number.size() && 0 == binary_number.front())) {
        set_big_integer_to_default_zero_value();
    } else if (1U == binary_number.size() && 1 == binary_number.front()) {
        set_big_integer_value_to_positive_one();
    } else {
        const auto first_true_iter_pos =
                std::find(std::cbegin(binary_number), std::cend(binary_number), true);

        if (first_true_iter_pos == std::cend(binary_number)) {
            set_big_integer_to_default_zero_value();
        } else {
            binary_number.erase(std::cbegin(binary_number), first_true_iter_pos);

            std::string binary_number_str{};
            binary_number_str.reserve(binary_number.size() + 2);
            binary_number_str.append("0b");

            for (bool binary_digit : binary_number) {
                binary_number_str.push_back(binary_digit ? '1' : '0');
            }

            decimal_number_str_ = binary_number_str;
            binary_number_str_ = std::move(binary_number_str);
            binary_digits_ = std::move(binary_number);
            base_ = number_base::binary;
            is_negative_number_ = false;
            first_ = 2;
            last_ = decimal_number_str_.length();

            convert_number_to_decimal_and_store();
            get_octal_digits_for_decimal_number();
            get_hexadecimal_digits_for_decimal_number();
        }
    }
}

bool big_integer::check_and_process_number_input_digits(
        std::vector<int> number) {
    if (number.empty() || 0 == number.front()) {
        set_big_integer_to_default_zero_value();
        return true;
    }

    if (1U == number.size() && 1 == number.front()) {
        set_big_integer_value_to_positive_one();
        return true;
    }

    if (1U == number.size() && -1 == number.front()) {
        set_big_integer_value_to_negative_one();
        return true;
    }

    const bool is_negative_number{number.front() < 0};

    size_t first{};
    std::string number_str{};
    number_str.reserve(number.size() + (is_negative_number ? 1 : 0) + (base_ != number_base::decimal ? 2 : 0));

    if (is_negative_number) {
        number_str.push_back('-');
        ++first;
    }

    switch (base_) {
        case number_base::binary:
            number_str.append("0b");
            first += 2;
            break;
        case number_base::octal:
            number_str.append("0o");
            first += 2;
            break;
        case number_base::hexadecimal:
            number_str.append("0x");
            first += 2;
            break;

        default:
            break;
    }

    for (int &digit : number) {
        digit = std::abs(digit);
        if (0U == number_base_allowed_digits_.at(base_).count(digit))
            return false;

        number_str.push_back(
                get_correct_digit_character_for_specified_value(digit));
        digit = is_negative_number && digit != 0 ? -digit : digit;
    }

    decimal_number_str_ = std::move(number_str);
    decimal_digits_ = std::move(number);
    is_negative_number_ = is_negative_number;
    first_ = first;
    last_ = decimal_number_str_.length();

    if (base_ != number_base::decimal)
        convert_number_to_decimal_and_store();

    update_big_integer_string_representations();
    return true;
}

bool big_integer::check_and_process_number_input_characters(
        const std::string &number) {
    number_base base = number_base::decimal;
    size_t first{}, last{number.length()};
    bool is_negative_number{};

    if (number[first] == '-' || number[first] == '+') {
        is_negative_number = number[first] == '-';
        ++first;
    }

    if ('b' == number[first] || 'B' == number[first]) {
        base = number_base::binary;
        ++first;
    } else if (stl::helper::str_starts_with(&number[first], "0b", true)) {
        base = number_base::binary;
        first += 2;
    } else if (stl::helper::str_starts_with(&number[first], "0x", true)) {
        base = number_base::hexadecimal;
        first += 2;
    } else if (stl::helper::str_starts_with(&number[first], "0o", true)) {
        base = number_base::octal;
        first += 2;
    } else if ('o' == number[first] || 'O' == number[first]) {
        base = number_base::octal;
        ++first;
    }

    while (first < last && (('0' == number[first]) || (0 != isspace(number[first])))) {
        ++first;
    }

    if (first == last) {
        set_big_integer_to_default_zero_value();
        return true;
    }

    if (base == number_base::binary) {
        if ((number[first] != '0') && (number[first] != '1'))
            return false;

        last = number.find_first_not_of("01'_", first + 1);
    } else if (base == number_base::octal) {
        if ((number[first] < '0') || (number[first] > '7'))
            return false;

        last = number.find_first_not_of("01234567'_", first + 1);
    } else if (base == number_base::decimal) {
        if ((number[first] < '0') || (number[first] > '9'))
            return false;

        last = number.find_first_not_of("0123456789'_", first + 1);
    } else if (base == number_base::hexadecimal) {
        if (number_base_allowed_digits_.at(base).count(number[first]) == 0U)
            return false;

        last = number.find_first_not_of("0123456789abcdefABCDEF'_", first + 1);
    }

    last = std::min(last, number.length());

    if (('\'' == number[last - 1]) || ('_' == number[last - 1]))
        return false;

    if (last - first == 1) {
        if ('0' == number[first]) {
            set_big_integer_to_default_zero_value();
            return true;
        } else if ('1' == number[first]) {
            if (!is_negative_number)
                set_big_integer_value_to_positive_one();
            else
                set_big_integer_value_to_negative_one();
            return true;
        }
    }

    decimal_number_str_.clear();
    decimal_number_str_.reserve(last - first + (is_negative_number ? 1 : 0) + (base != number_base::decimal ? 2 : 0));

    size_t start{};

    if (is_negative_number) {
        decimal_number_str_.push_back('-');
        ++start;
    }

    if (base != number_base::decimal) {
        switch (base) {
            case number_base::binary:
                decimal_number_str_.append("0b");
                start += 2;
                break;

            case number_base::octal:
                decimal_number_str_.append("0o");
                start += 2;
                break;

            case number_base::hexadecimal:
                decimal_number_str_.append("0x");
                start += 2;
                break;

            default:
                break;
        }
    }

    std::for_each(std::cbegin(number) + first, std::cbegin(number) + last, [&](const char digit) {
        if (digit != '_' && digit != '\'')
            decimal_number_str_.push_back(digit);
    });

    base_ = base;
    is_negative_number_ = is_negative_number;
    first_ = start;
    last_ = decimal_number_str_.length();

    return true;
}

char big_integer::get_correct_digit_character_for_specified_value(
        const char ch) noexcept {
    if (ch >= 0 && ch <= 9)
        return '0' + ch;
    if (ch >= 10 && ch <= 15)
        return 'A' + (ch - 10);
    if (ch >= '0' && ch <= '9')
        return ch;
    if (ch >= 'a' && ch <= 'f')
        return 'A' + ('a' - ch);
    if (ch >= 'A' && ch <= 'F')
        return ch;

    return 0;
}

byte big_integer::get_correct_digit_value(const char ch) const noexcept {
    switch (base_) {
        case number_base::binary:
        case number_base::octal:
        case number_base::decimal:

            return static_cast<byte>(ch - '0');

        case number_base::hexadecimal:
            if (ch >= '0' && ch <= '9')
                return static_cast<byte>(ch - '0');

            switch (ch) {
                case 'a':
                case 'A':
                    return 10;
                case 'b':
                case 'B':
                    return 11;
                case 'c':
                case 'C':
                    return 12;
                case 'd':
                case 'D':
                    return 13;
                case 'e':
                case 'E':
                    return 14;
                case 'f':
                case 'F':
                    return 15;
                default:
                    return 0;
            }
        default:
            return 0;
    }
}

void big_integer::find_2s_complement_for_binary_number() {
    std::vector<bool> binary_digits{};
    binary_digits.reserve(binary_digits_.size());

    std::transform(std::cbegin(binary_digits_), std::cend(binary_digits_), std::back_inserter(binary_digits),
                   [](const bool bin_digit) { return !bin_digit; });

    bool carry_bit{true};

    for (size_t index{binary_digits.size()}; index > 0; --index) {
        const size_t i{index - 1};
        const bool result{(carry_bit && !binary_digits.at(i)) || (!carry_bit && binary_digits.at(i))};

        carry_bit = carry_bit && binary_digits.at(i);

        binary_digits.at(i) = result;
    }
}

void big_integer::convert_number_to_decimal_and_store() {
    if (base_ != number_base::decimal) {
        // if (binary_digits_.empty()) {
        binary_digits_.clear();
        const byte number_of_bits_for_digit{number_base_number_of_bits_.at(base_)};

        binary_digits_.reserve((last_ - first_) * number_of_bits_for_digit);

        std::for_each(std::cbegin(decimal_number_str_) + first_,
                      std::cbegin(decimal_number_str_) + last_,
                      [&](const auto ch) {
                          const byte digit{get_correct_digit_value(ch)};
                          for (byte i{}; i < number_of_bits_for_digit; ++i) {
                              const byte bit_mask{static_cast<byte>(
                                                          1_b << (number_of_bits_for_digit - 1 - i))};
                              const bool result{(digit & bit_mask) == bit_mask};
                              binary_digits_.emplace_back(result);
                          }
                      });
        // }

        const size_t bcd_bin_buffer_size =
                binary_digits_.size() + 4 * static_cast<size_t>(std::ceil(binary_digits_.size() / 3.0));

        std::vector<bool> bcd_digits(bcd_bin_buffer_size, false);

        for (size_t i{}; i < binary_digits_.size(); ++i) {
            for (size_t j{bcd_bin_buffer_size}; j > 3U;
                 j -= 4) {
                const size_t k{j - 1};
                unsigned value = (bcd_digits[k] ? 1 : 0) + (bcd_digits[k - 1] ? 2 : 0) + (bcd_digits[k - 2] ? 4 : 0) +
                                 (bcd_digits[k - 3] ? 8 : 0);

                if (value >= 5U) {
                    value += 3U;
                    bcd_digits[k] = (value & 1U) == 1U;
                    bcd_digits[k - 1] = (value & 2U) == 2U;
                    bcd_digits[k - 2] = (value & 4U) == 4U;
                    bcd_digits[k - 3] = (value & 8U) == 8U;
                }
            }

            for (size_t j{}; (j < bcd_digits.size() - 1); ++j)
                bcd_digits[j] = bcd_digits[j + 1];

            bcd_digits.back() = binary_digits_[i];
        }

        if (bcd_digits.size() % 4 != 0) {
            const size_t count_of_zeroes = bcd_digits.size() % 4;
            bcd_digits.insert(std::cbegin(bcd_digits), 4 - count_of_zeroes, false);
        }

        const size_t number_of_bcd_digits{bcd_digits.size() / 4};

        decimal_digits_.clear();
        decimal_digits_.reserve(number_of_bcd_digits);

        decimal_number_str_.clear();
        decimal_number_str_.reserve(number_of_bcd_digits + (is_negative_number_ ? 1 : 0));

        bool is_leading_zeroes{true};
        // base_ = number_base::decimal;
        first_ = 0U;

        if (is_negative_number_) {
            decimal_number_str_.push_back('-');
            ++first_;
        }

        for (size_t i{}; i <= bcd_digits.size() - 4; i += 4) {
            const byte value =
                    (bcd_digits[i] ? 8_b : 0_b) + (bcd_digits[i + 1] ? 4_b : 0_b) + (bcd_digits[i + 2] ? 2_b : 0_b) +
                    (bcd_digits[i + 3] ? 1_b : 0_b);

            if (is_leading_zeroes && value == 0)
                continue;

            decimal_digits_.emplace_back(is_negative_number_ ? -value : value);
            decimal_number_str_.push_back('0' + value);
            is_leading_zeroes = false;
        }

        last_ = decimal_number_str_.length();
    } else {
        decimal_digits_.clear();
        decimal_digits_.reserve(last_ - first_);
        std::for_each(std::cbegin(decimal_number_str_) + first_,
                      std::cend(decimal_number_str_),
                      [&](const auto ch) {
                          const byte digit{static_cast<byte>(ch - '0')};
                          decimal_digits_.emplace_back(
                                  is_negative_number_ && 0 != digit ? -digit : digit);
                      });

        get_binary_digits_for_decimal_number();
    }
}

void big_integer::get_binary_digits_for_decimal_number() {
    binary_digits_.clear();
    binary_number_str_.clear();

    std::string number_str{std::cbegin(decimal_number_str_) + first_,
                           std::cend(decimal_number_str_)};

    const size_t number_of_bits_needed_for_encoding_decimal_digits =
            number_str.length() * number_base_number_of_bits_.at(number_base::decimal);

    binary_digits_.reserve(number_of_bits_needed_for_encoding_decimal_digits);
    binary_number_str_.reserve(number_of_bits_needed_for_encoding_decimal_digits + 2 + (is_negative_number_ ? 1 : 0));

    while (number_str != "0") {
        binary_digits_.emplace_back(
                static_cast<bool>(odds_to_one(number_str.back())));
        binary_number_str_.push_back('0' + odds_to_one(number_str.back()));
        number_str = div_by_two(number_str);
    }

    const size_t last_one_pos{binary_number_str_.find_last_of('1')};
    binary_number_str_.erase(std::cbegin(binary_number_str_) + last_one_pos + 1,
                             std::cend(binary_number_str_));

    binary_number_str_.append("b0");

    if (is_negative_number_)
        binary_number_str_.push_back('-');

    std::reverse(std::begin(binary_number_str_), std::end(binary_number_str_));
    std::reverse(std::begin(binary_digits_), std::end(binary_digits_));
}

void big_integer::get_octal_digits_for_decimal_number() {
    const auto number_of_bits_needed_for_one_octal_digit{
            number_base_number_of_bits_.at(number_base::octal)
    };

    const size_t remainder{binary_digits_.size() % number_of_bits_needed_for_one_octal_digit};
    if (remainder != 0)
        binary_digits_.insert(std::cbegin(binary_digits_),
                              number_of_bits_needed_for_one_octal_digit - remainder,
                              false);

    octal_number_str_.clear();
    octal_number_str_.reserve(
            binary_digits_.size() / number_of_bits_needed_for_one_octal_digit + 2 + (is_negative_number_ ? 1 : 0));

    bool is_leading_zeroes{true};

    if (is_negative_number_)
        octal_number_str_.push_back('-');

    octal_number_str_.append("0o");

    for (size_t i{};
         i <= binary_digits_.size() - number_of_bits_needed_for_one_octal_digit;
         i += number_of_bits_needed_for_one_octal_digit) {
        const byte value = (binary_digits_[i] ? 4_b : 0_b) + (binary_digits_[i + 1] ? 2_b : 0_b) +
                           (binary_digits_[i + 2] ? 1_b : 0_b);

        if (value == 0 && is_leading_zeroes)
            continue;

        octal_number_str_.push_back('0' + value);
        is_leading_zeroes = false;
    }
}

void big_integer::get_hexadecimal_digits_for_decimal_number() {
    const auto number_of_bits_needed_for_one_hex_digit{
            number_base_number_of_bits_.at(number_base::hexadecimal)
    };

    const size_t remainder{binary_digits_.size() % number_of_bits_needed_for_one_hex_digit};
    if (remainder != 0)
        binary_digits_.insert(std::cbegin(binary_digits_),
                              number_of_bits_needed_for_one_hex_digit - remainder,
                              false);

    hexadecimal_number_str_.clear();
    hexadecimal_number_str_.reserve(
            binary_digits_.size() / number_of_bits_needed_for_one_hex_digit + 2 + (is_negative_number_ ? 1 : 0));

    bool is_leading_zeroes{true};

    if (is_negative_number_)
        hexadecimal_number_str_.push_back('-');

    hexadecimal_number_str_.append("0x");

    for (size_t i{};
         i <= binary_digits_.size() - number_of_bits_needed_for_one_hex_digit;
         i += number_of_bits_needed_for_one_hex_digit) {
        const byte value = (binary_digits_[i] ? 8_b : 0_b) + (binary_digits_[i + 1] ? 4_b : 0_b) +
                           (binary_digits_[i + 2] ? 2_b : 0_b) + (binary_digits_[i + 3] ? 1_b : 0_b);

        if (value == 0 && is_leading_zeroes)
            continue;

        hexadecimal_number_str_.push_back(
                get_correct_digit_character_for_specified_value(value));
        is_leading_zeroes = false;
    }
}

std::string big_integer::div_by_two(const std::string &number_str) {
    std::string modified_number_str{};
    byte add{};

    for (const char digit : number_str) {
        const byte new_digit{static_cast<byte>((digit - '0') / 2 + add)};
        modified_number_str.push_back('0' + new_digit);
        add = odds_to_one(digit) * 5;
    }

    if (modified_number_str != "0" && modified_number_str.length() > 1U && modified_number_str.front() == '0') {
        return modified_number_str.substr(1);
    }

    return modified_number_str;
}

byte big_integer::odds_to_one(const char ch) {
    switch (ch) {
        case '1':
        case '3':
        case '5':
        case '7':
        case '9':
            return 1;
        default:
            return 0;
    }
}

void big_integer::reset(const std::string &number) {
    if (!check_and_process_number_input_characters(number)) {
#ifndef BIG_INTEGER_NO_THROW
        throw std::invalid_argument{
          "Input number string does not contain a valid number!"
        };
#else
        set_big_integer_value_nan();
#endif
    }

    if (!is_zero() && !is_positive_one() && !is_negative_one()) {
        convert_number_to_decimal_and_store();
        update_big_integer_string_representations();
    }
}

void big_integer::update_big_integer_string_representations() {
    get_binary_digits_for_decimal_number();
    get_octal_digits_for_decimal_number();
    get_hexadecimal_digits_for_decimal_number();
}
