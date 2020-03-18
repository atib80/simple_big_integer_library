//
// Created by Attila Budai on 3/7/2020.
//

#include "../include/big_integer.h"

using namespace org::atib::numerics;

const std::unordered_map<number_base, byte>
  big_integer::number_base_number_of_bits_{ { number_base::binary, 1 },
    { number_base::octal, 3 },
    { number_base::decimal, 4 },
    { number_base::hexadecimal, 4 } };

const std::unordered_map<number_base, std::unordered_set<byte>>
  big_integer::number_base_allowed_digits_{
    { number_base::binary, { { 0, 1, '0', '1' } } },
    { number_base::octal,
      { { 0, 1, 2, 3, 4, 5, 6, 7, '0', '1', '2', '3', '4', '5', '6', '7' } } },
    { number_base::decimal,
      { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' } } },
    { number_base::hexadecimal,
      { { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F' } } }
  };

big_integer org::atib::numerics::operator+(const big_integer &lhs,
  const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
    return lhs.add_two_decimal_numbers_together(rhs);
  }

  return big_integer{ big_integer::NaN };
}

big_integer org::atib::numerics::operator-(const big_integer &lhs,
  const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
    big_integer negated_rhs{ rhs };
    negated_rhs.invert_sign();
    return lhs + negated_rhs;
  }

  return big_integer{ big_integer::NaN };
}

big_integer org::atib::numerics::operator*(const big_integer &lhs,
  const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
    return lhs.multiply_two_decimal_numbers(rhs);
  }
  return big_integer{ big_integer::NaN };
}

big_integer org::atib::numerics::operator/(const big_integer &lhs,
  const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
#ifndef _BIG_INTEGER_LIBRARY_NO_THROW_
    if (rhs.is_zero()) {
      if (lhs.is_zero())
        throw zero_divided_by_zero_error{};
      throw division_by_zero_error{};
    }
#else
    if (lhs.is_zero())
      return big_integer{};
    if (rhs.is_zero())
      return big_integer{ big_integer::NaN };
#endif

    if (lhs < rhs)
      return big_integer{};

    if (rhs.is_positive_one())
      return big_integer{ lhs.get_decimal_digits() };

    if (rhs.is_negative_one()) {
      big_integer result{ lhs.get_decimal_digits() };
      result.invert_sign();
      return result;
    }

    // get positive values
    // long pDividend = std::abs((long)dividend);
    // long pDivisor = std::abs((long)divisor);

    big_integer dividend{ lhs.is_negative_number() ? lhs.abs() : lhs };
    big_integer divisor{ rhs.is_negative_number() ? rhs.abs() : rhs };

    // big_ result = 0;
    // while (pDividend >= pDivisor) {
    //   // calculate number of left shifts
    //   int numShift = 0;
    //   while (pDividend >= (pDivisor << numShift)) {
    //     numShift++;
    //   }

    //   // dividend minus the largest shifted divisor
    //   result += 1 << (numShift - 1);
    //   pDividend -= (pDivisor << (numShift - 1));
    // }

    // if ((dividend > 0 && divisor > 0) || (dividend < 0 && divisor < 0)) {
    //   return result;
    // } else {
    //   return -result;
    // }

    big_integer result{};
    while (dividend >= divisor) {
      // calculate number of left shifts
      size_t shift_count{};
      big_integer shifted_divisor{};

      do {
        ++shift_count;
        shifted_divisor = divisor << shift_count;
      } while (dividend >= shifted_divisor);

      // dividend minus the largest shifted divisor
      --shift_count;
      big_integer big_one{ "1" };
      result += (big_one << shift_count);
      dividend -= (divisor << shift_count);
    }

    if ((!lhs.is_negative_number() && !rhs.is_negative_number()) || (lhs.is_negative_number() && rhs.is_negative_number()))
      return result;

    result.invert_sign();
    return result;
  }

  return big_integer{ big_integer::NaN };
}

big_integer operator%(const big_integer &lhs, const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
#ifndef _BIG_INTEGER_LIBRARY_NO_THROW_
    if (rhs.is_zero()) {
      if (lhs.is_zero())
        throw zero_divided_by_zero_error{};
      throw division_by_zero_error{};
    }
#else
    if (lhs.is_zero())
      return rhs;
    if (rhs.is_zero())
      return big_integer{ big_integer::NaN };
#endif

    if (lhs < rhs)
      return rhs;

    if (rhs.is_positive_one() || rhs.is_negative_one())
      return big_integer{};

    big_integer dividend{ lhs.is_negative_number() ? lhs.abs() : lhs };
    big_integer divisor{ rhs.is_negative_number() ? rhs.abs() : rhs };

    big_integer result{};
    while (dividend >= divisor) {
      // calculate number of left shifts
      size_t shift_count{};
      big_integer shifted_divisor{};

      do {
        ++shift_count;
        shifted_divisor = divisor << shift_count;
      } while (dividend >= shifted_divisor);

      // dividend minus the largest shifted divisor
      --shift_count;
      big_integer big_one{ "1" };
      result += (big_one << shift_count);
      dividend -= (divisor << shift_count);
    }

    return dividend;
  }

  return big_integer{ big_integer::NaN };
}

bool org::atib::numerics::operator==(const big_integer &lhs,
  const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan())
    return lhs.get_decimal_digits() == rhs.get_decimal_digits();
  return false;
}

bool org::atib::numerics::operator!=(const big_integer &lhs,
  const big_integer &rhs)
{
  return !(lhs == rhs);
}

bool org::atib::numerics::operator<(const big_integer &lhs,
  const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
    const size_t lhs_size{ lhs.get_decimal_digits().size() };
    const size_t rhs_size{ rhs.get_decimal_digits().size() };

    if (lhs.is_negative_number() && !rhs.is_negative_number())
      return true;

    if (!lhs.is_negative_number() && rhs.is_negative_number())
      return false;

    if (lhs_size != rhs_size) {
      if (lhs_size < rhs_size)
        return !lhs.is_negative_number() && !rhs.is_negative_number();

      if (rhs_size < lhs_size)
        return lhs.is_negative_number() && rhs.is_negative_number();
    } else {
      for (size_t i{}; i < lhs_size; ++i) {
        if (lhs.get_decimal_digits().at(i) < rhs.get_decimal_digits().at(i))
          return true;
      }
    }
  }
  return false;
}

bool org::atib::numerics::operator>(const big_integer &lhs,
  const big_integer &rhs)
{
  return rhs < lhs;
}

bool org::atib::numerics::operator<=(const big_integer &lhs,
  const big_integer &rhs)
{
  return !(lhs > rhs);
}

bool org::atib::numerics::operator>=(const big_integer &lhs,
  const big_integer &rhs)
{
  return !(lhs < rhs);
}

big_integer operator|(const big_integer &lhs, const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
    const auto &lhs_binary_digits{ lhs.get_binary_digits() };
    const auto &rhs_binary_digits{ rhs.get_binary_digits() };

    const bool is_lhs_longer{ lhs_binary_digits.size() >= rhs_binary_digits.size() };

    const auto &longer_binary_number{ is_lhs_longer ? lhs_binary_digits
                                                    : rhs_binary_digits };

    const auto &shorter_binary_number{ is_lhs_longer ? rhs_binary_digits
                                                     : lhs_binary_digits };

    std::vector<bool> result{ longer_binary_number };

    for (size_t i{}; i < shorter_binary_number.size(); ++i) {
      result[longer_binary_number.size() - shorter_binary_number.size() + i] =
        result[longer_binary_number.size() - shorter_binary_number.size() + i] || shorter_binary_number[i];
    }

    return big_integer{ std::move(result) };
  }

  return big_integer{ big_integer::NaN };
}

big_integer operator&(const big_integer &lhs, const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
    const auto &lhs_binary_digits{ lhs.get_binary_digits() };
    const auto &rhs_binary_digits{ rhs.get_binary_digits() };

    const bool is_lhs_longer{ lhs_binary_digits.size() >= rhs_binary_digits.size() };

    const auto &longer_binary_number{ is_lhs_longer ? lhs_binary_digits
                                                    : rhs_binary_digits };

    const auto &shorter_binary_number{ is_lhs_longer ? rhs_binary_digits
                                                     : lhs_binary_digits };

    std::vector<bool> result(longer_binary_number.size(), false);

    for (size_t i{}; i < shorter_binary_number.size(); ++i) {
      result[longer_binary_number.size() - shorter_binary_number.size() + i] =
        result[longer_binary_number.size() - shorter_binary_number.size() + i] && shorter_binary_number[i];
    }

    return big_integer{ std::move(result) };
  }

  return big_integer{ big_integer::NaN };
}

big_integer operator^(const big_integer &lhs, const big_integer &rhs)
{
  if (!lhs.is_nan() && !rhs.is_nan()) {
    const auto &lhs_binary_digits{ lhs.get_binary_digits() };
    const auto &rhs_binary_digits{ rhs.get_binary_digits() };

    const bool is_lhs_longer{ lhs_binary_digits.size() >= rhs_binary_digits.size() };

    const auto &longer_binary_number{ is_lhs_longer ? lhs_binary_digits
                                                    : rhs_binary_digits };

    const auto &shorter_binary_number{ is_lhs_longer ? rhs_binary_digits
                                                     : lhs_binary_digits };

    std::vector<bool> result{ longer_binary_number };

    for (size_t i{}; i < shorter_binary_number.size(); ++i) {
      result[longer_binary_number.size() - shorter_binary_number.size() + i] =
        (result[longer_binary_number.size() - shorter_binary_number.size() + i] && !shorter_binary_number[i]) || (!result[longer_binary_number.size() - shorter_binary_number.size() + i] && shorter_binary_number[i]);
    }

    return big_integer{ std::move(result) };
  }

  return big_integer{ big_integer::NaN };
}

void swap(big_integer &lhs, big_integer &rhs) noexcept
{
  lhs.swap(rhs);
}

std::ostream &operator<<(std::ostream &os, const big_integer &bi)
{
  return (os << bi.get_decimal_number());
}

std::istream &operator>>(std::istream &is, big_integer &bi)
{
  std::string line;
  is >> line;
  bi.reset(line);
  return is;
}

const std::vector<int> &big_integer::get_decimal_digits() const
{
  return decimal_digits_;
}

const std::vector<bool> &big_integer::get_binary_digits() const
{
  return binary_digits_;
}

const std::string &big_integer::get_decimal_number() const
{
  return decimal_number_str_;
}

const std::string &big_integer::get_binary_number(const std::string &prefix, const std::string &postfix, const size_t number_of_digits)
{
  const size_t prefix_len{ prefix.length() };
  const size_t postfix_len{ postfix.length() };
  size_t binary_str_len{ binary_number_str_.length() };

  if (!prefix.empty() && binary_str_len >= prefix_len && !stl::helper::str_starts_with(binary_number_str_, prefix, false)) {
    binary_number_str_.insert(0, prefix);
    binary_str_len += prefix.length();
  }

  if (!postfix.empty() && binary_str_len >= postfix_len && !stl::helper::str_ends_with(binary_number_str_, postfix, false)) {
    binary_number_str_.append(postfix);
  }

  binary_str_len -= (prefix_len + postfix_len);

  if (number_of_digits != 0 && number_of_digits > binary_str_len)
    binary_number_str_.insert(std::cbegin(binary_number_str_) + prefix_len, number_of_digits - binary_str_len, '0');

  return binary_number_str_;
}

bool big_integer::is_nan() const noexcept
{
  return stl::helper::str_starts_with(decimal_number_str_, NaN, true);
}

bool big_integer::is_zero() const noexcept
{
  return decimal_number_str_.empty() || decimal_number_str_ == "0";
}

bool big_integer::is_positive_one() const noexcept
{
  return decimal_number_str_ == "1";
}

bool big_integer::is_negative_one() const noexcept
{
  return decimal_number_str_ == "-1";
}

big_integer big_integer::abs() const
{
  auto decimal_digits{ this->decimal_digits_ };

  if (is_negative_number_) {
    for (auto &digit : decimal_digits)
      digit = std::abs(digit);
  }

  return big_integer{ std::move(decimal_digits) };
}

bool big_integer::is_negative_number() const
{
  return is_negative_number_;
}

org::atib::numerics::big_integer::big_integer()
  : decimal_digits_{ 0 },
    binary_digits_{ false },
    hexadecimal_number_str_{ "0x0" },
    decimal_number_str_{ "0" },
    octal_number_str_{ "0o0" },
    binary_number_str_{ "0b0" },
    first_{ 0 },
    last_{ 1 },
    base_{ number_base::decimal },
    is_negative_number_{ false } {}

big_integer::big_integer(std::vector<int> digits,
  const number_base base /*= number_base::decimal*/)
  : base_{ base }
{
  if (!check_and_process_number_input_digits(std::move(digits))) {
#ifndef _BIG_INTEGER_LIBRARY_NO_THROW_
    throw std::invalid_argument{
      "Input number digits do not form a valid number!"
    };
#else
    set_big_integer_to_default_zero_value();
#endif
  }
}

big_integer::big_integer(std::vector<bool> binary_digits) : base_{ number_base::binary }
{
  check_and_process_binary_number_digits(std::move(binary_digits));
}

big_integer::big_integer(const char *number)
  : big_integer{ std::string{ number } } {}

big_integer::big_integer(const std::string &number) : base_{ number_base::decimal }
{
  if (!check_and_process_number_input_characters(number)) {
#ifndef _BIG_INTEGER_LIBRARY_NO_THROW_
    throw std::invalid_argument{
      "Input number string does not contain a valid number!"
    };
#else
    set_big_integer_to_default_zero_value();
#endif
  }

  if (!is_zero() && !is_positive_one() && !is_negative_one()) {
    convert_number_to_decimal_and_store();
    update_big_integer_string_representations();
  }
}

const std::string &big_integer::get_big_integer(number_base base) const
  noexcept
{
  switch (base) {
  case number_base::decimal:
    return decimal_number_str_;
  case number_base::hexadecimal:
    return hexadecimal_number_str_;
  case number_base::octal:
    return octal_number_str_;
  case number_base::binary:
    return binary_number_str_;
  default:
    return decimal_number_str_;
  }
}

big_integer::operator std::string() const noexcept
{
  return get_big_integer(number_base::decimal);
}

big_integer::operator const char *() const noexcept
{
  return this->get_big_integer(number_base::decimal).c_str();
}

big_integer big_integer::operator-() const
{
  big_integer result{ *this };
  result.invert_sign();
  return result;
}

big_integer &big_integer::operator+=(const big_integer &rhs)
{
  big_integer result(this->add_two_decimal_numbers_together(rhs));
  this->swap(result);
  return *this;
}

big_integer &big_integer::operator-=(const big_integer &rhs)
{
  big_integer negated_rhs(rhs);
  negated_rhs.invert_sign();
  big_integer result(this->add_two_decimal_numbers_together(negated_rhs));
  this->swap(result);
  return *this;
}

big_integer &big_integer::operator*=(const big_integer &rhs)
{
  big_integer result(this->multiply_two_decimal_numbers(rhs));
  this->swap(result);
  return *this;
}

big_integer &big_integer::operator++()
{
  if (decimal_digits_.size() == 1U && decimal_digits_[0] == -1) {
    decimal_digits_[0] = 0;
    decimal_number_str_ = "0";
    is_negative_number_ = false;
  } else {
    byte carry{ 1 };
    size_t zero_pos{ std::string::npos };

    for (long i = static_cast<long>(decimal_digits_.size()) - 1;
         i >= 0 && carry != 0;
         --i) {
      if (decimal_digits_[i] == 9) {
        decimal_digits_[i] = 0;
        decimal_number_str_[first_ + i] = '0';
      } else if (is_negative_number_ && 0 == decimal_digits_[i]) {
        decimal_digits_[i] = -9;
        decimal_number_str_[first_ + i] = '9';
      } else {
        ++decimal_digits_[i];
        decimal_number_str_[first_ + i] = '0' + std::abs(decimal_digits_[i]);
        carry = 0;
        if (0 == decimal_digits_[i])
          zero_pos = static_cast<size_t>(i);
      }
    }

    if (carry != 0) {
      decimal_digits_.insert(std::cbegin(decimal_digits_), 1);
      decimal_number_str_.insert(std::cbegin(decimal_number_str_) + first_,
        '1');
    } else if (zero_pos != std::string::npos && decimal_number_str_.find_first_of("123456789") > zero_pos) {
      decimal_digits_.erase(std::cbegin(decimal_digits_),
        std::cbegin(decimal_digits_) + zero_pos + 1);
      decimal_number_str_.erase(
        std::cbegin(decimal_number_str_) + first_,
        std::cbegin(decimal_number_str_) + first_ + zero_pos + 1);
    }
  }

  this->update_big_integer_string_representations();

  return *this;
}

big_integer big_integer::operator++(int) const
{
  big_integer result{ decimal_digits_ };

  if (result.decimal_digits_.size() == 1U && result.decimal_digits_[0] == -1) {
    result.decimal_digits_[0] = 0;
    result.decimal_number_str_ = "0";
    result.is_negative_number_ = false;
  } else {
    byte carry{ 1 };
    size_t zero_pos{ std::string::npos };

    for (long i = static_cast<long>(decimal_digits_.size()) - 1;
         i >= 0 && carry != 0;
         --i) {
      if (result.decimal_digits_[i] == 9) {
        result.decimal_digits_[i] = 0;
        result.decimal_number_str_[result.first_ + i] = '0';
      } else if (result.is_negative_number_ && 0 == result.decimal_digits_[i]) {
        result.decimal_digits_[i] = -9;
        result.decimal_number_str_[result.first_ + i] = '9';
      } else {
        ++result.decimal_digits_[i];
        result.decimal_number_str_[result.first_ + i] =
          '0' + std::abs(result.decimal_digits_[i]);
        carry = 0;
        if (0 == result.decimal_digits_[i])
          zero_pos = static_cast<size_t>(i);
      }
    }

    if (carry != 0) {
      result.decimal_digits_.insert(std::cbegin(result.decimal_digits_), 1);
      result.decimal_number_str_.insert(
        std::cbegin(result.decimal_number_str_) + result.first_, '1');
    } else if (zero_pos != std::string::npos && decimal_number_str_.find_first_of("123456789") > zero_pos) {
      result.decimal_digits_.erase(
        std::cbegin(result.decimal_digits_),
        std::cbegin(result.decimal_digits_) + zero_pos + 1);
      result.decimal_number_str_.erase(
        std::cbegin(result.decimal_number_str_) + result.first_,
        std::cbegin(result.decimal_number_str_) + result.first_ + zero_pos + 1);
    }
  }

  result.update_big_integer_string_representations();

  return result;
}

big_integer &big_integer::operator--()
{
  if (decimal_digits_[0] == 0) {
    decimal_digits_[0] = -1;
    decimal_number_str_ = "-1";
    is_negative_number_ = true;
  } else {
    byte carry{ 1 };
    size_t zero_pos{ std::string::npos };

    for (long i = static_cast<long>(decimal_digits_.size()) - 1;
         i >= 0 && carry != 0;
         --i) {
      if (!is_negative_number_ && decimal_digits_[i] == 0) {
        decimal_digits_[i] = 9;
        decimal_number_str_[first_ + i] = '9';
      } else if (is_negative_number_ && -9 == decimal_digits_[i]) {
        decimal_digits_[i] = 0;
        decimal_number_str_[first_ + i] = '0';
      } else {
        --decimal_digits_[i];
        decimal_number_str_[first_ + i] = '0' + std::abs(decimal_digits_[i]);
        carry = 0;
        if (0 == decimal_digits_[i])
          zero_pos = static_cast<size_t>(i);
      }
    }

    if (zero_pos != std::string::npos && decimal_number_str_.find_first_of("123456789", first_) > zero_pos) {
      decimal_digits_.erase(std::cbegin(decimal_digits_),
        std::cbegin(decimal_digits_) + zero_pos + 1);
      decimal_number_str_.erase(
        std::cbegin(decimal_number_str_) + first_,
        std::cbegin(decimal_number_str_) + first_ + zero_pos + 1);
    }
  }

  this->update_big_integer_string_representations();

  return *this;
}

big_integer big_integer::operator--(int) const
{
  big_integer result{ decimal_digits_ };

  if (result.decimal_digits_[0] == 0) {
    result.decimal_digits_[0] = -1;
    result.decimal_number_str_ = "-1";
    result.is_negative_number_ = true;
  } else {
    byte carry{ 1 };
    size_t zero_pos{ std::string::npos };

    for (long i = static_cast<long>(result.decimal_digits_.size()) - 1;
         i >= 0 && carry != 0;
         --i) {
      if (!result.is_negative_number_ && result.decimal_digits_[i] == 0) {
        result.decimal_digits_[i] = 9;
        result.decimal_number_str_[first_ + i] = '9';
      } else if (result.is_negative_number_ && -9 == result.decimal_digits_[i]) {
        result.decimal_digits_[i] = 0;
        result.decimal_number_str_[first_ + i] = '0';
      } else {
        --result.decimal_digits_[i];
        result.decimal_number_str_[first_ + i] =
          '0' + std::abs(result.decimal_digits_[i]);
        carry = 0;
        if (0 == result.decimal_digits_[i])
          zero_pos = static_cast<size_t>(i);
      }
    }

    if (zero_pos != std::string::npos && result.decimal_number_str_.find_first_of("123456789", first_) > zero_pos) {
      result.decimal_digits_.erase(
        std::cbegin(result.decimal_digits_),
        std::cbegin(result.decimal_digits_) + zero_pos + 1);
      result.decimal_number_str_.erase(
        std::cbegin(result.decimal_number_str_) + first_,
        std::cbegin(result.decimal_number_str_) + first_ + zero_pos + 1);
    }
  }

  result.update_big_integer_string_representations();

  return result;
}

big_integer big_integer::operator<<(const size_t count)
{
  std::vector<bool> shifted_binary_digits{ this->binary_digits_ };
  if (count != 0U)
    shifted_binary_digits.insert(std::cend(shifted_binary_digits), count, false);
  return big_integer{ std::move(shifted_binary_digits) };
}

big_integer big_integer::operator>>(const size_t count)
{
  if (count >= this->binary_digits_.size())
    return big_integer{};

  if (count != 0U) {
    std::vector<bool> shifted_binary_digits{ this->binary_digits_ };
    shifted_binary_digits.erase(std::cend(shifted_binary_digits) - count,
      std::cend(shifted_binary_digits));
    return big_integer{ std::move(shifted_binary_digits) };
  }

  return big_integer{ *this };
}

big_integer &big_integer::operator<<=(const size_t count)
{
  if (count != 0U) {
    std::vector<bool> shifted_binary_digits{ std::move(this->binary_digits_) };
    shifted_binary_digits.insert(std::cend(shifted_binary_digits), count, false);
    big_integer result{ std::move(shifted_binary_digits) };
    this->swap(result);
  }

  return *this;
}

big_integer &big_integer::operator>>=(const size_t count)
{
  if (count >= this->binary_digits_.size()) {
    this->set_big_integer_to_default_zero_value();
  } else if (count != 0U) {
    std::vector<bool> shifted_binary_digits{ std::move(this->binary_digits_) };
    shifted_binary_digits.erase(std::cend(shifted_binary_digits) - count,
      std::cend(shifted_binary_digits));
    big_integer result{ std::move(shifted_binary_digits) };
    this->swap(result);
  }
  return *this;
}

size_t big_integer::operator[](const size_t factor) const
{
  if (factor > last_ - first_ - 1)
    return 0U;

  return static_cast<size_t>(decimal_number_str_[last_ - 1 - factor] - '0');
}

big_integer::operator bool() const
{
  return !decimal_number_str_.empty() && decimal_number_str_ != "0";
}

void big_integer::swap(big_integer &rhs) noexcept
{
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

big_integer big_integer::add_two_decimal_numbers_together(
  const big_integer &rhs) const
{
  if (this->is_nan() || rhs.is_nan())
    return big_integer{ "NaN" };

  if (this->is_zero()) return big_integer{ rhs };

  if (rhs.is_zero()) return big_integer{ *this };

  std::vector<int> result{};
  result.reserve(std::max(decimal_digits_.size(), rhs.decimal_digits_.size()) + 1);

  int n = static_cast<int>(decimal_digits_.size()) - 1;
  int m = static_cast<int>(rhs.decimal_digits_.size()) - 1;
  int carry{};

  while (n >= 0 || m >= 0 || carry != 0) {
    int sum{ carry };
    if (n >= 0) {
      sum += decimal_digits_[n--];
    }
    if (m >= 0) {
      sum += rhs.decimal_digits_[m--];
    }

    carry = sum > 9 ? 1 : (sum < -9 ? -1 : 0);

    const byte remainder{ static_cast<byte>(std::abs(sum) % 10) };

    result.emplace_back(sum < 0 ? -remainder : remainder);
  }

  const auto last_non_zero_digit_pos =
    std::find_if(std::crbegin(result), std::crend(result), [](const auto digit) { return digit != 0; })
      .base();

  if (last_non_zero_digit_pos != std::cend(result))
    result.erase(last_non_zero_digit_pos + 1, std::cend(result));

  std::reverse(std::begin(result), std::end(result));

  return big_integer{ std::move(result) };
}

big_integer big_integer::multiply_two_decimal_numbers(
  const big_integer &rhs) const
{
  if (this->is_zero() || rhs.is_zero())
    return big_integer{};

  if (this->is_positive_one()) return big_integer{ rhs };

  if (rhs.is_positive_one()) return big_integer{ *this };

  if (this->is_negative_one()) return big_integer{ -rhs };

  if (rhs.is_negative_number()) return big_integer{ -(*this) };

  std::vector<int> result{};
  result.resize(decimal_digits_.size() + rhs.decimal_digits_.size(), 0_b);

  const bool is_negative_product{
    (is_negative_number_ && !rhs.is_negative_number_) || (!is_negative_number_ && rhs.is_negative_number_)
  };

  for (long i{ static_cast<long>(rhs.decimal_digits_.size()) - 1 }; i >= 0; --i) {
    byte carry{};
    for (long j{ static_cast<long>(decimal_digits_.size()) - 1 }; j >= 0; --j) {
      byte product{ static_cast<byte>(std::abs(decimal_digits_[j]) * std::abs(rhs.decimal_digits_[i]) + carry) };
      carry = product / 10;
      product %= 10;
      result[i + j + 1] += product;
      carry += result[i + j + 1] / 10;
      result[i + j + 1] %= 10;
    }

    if (carry > 0 && i >= 0)
      result[i] = carry;
  }

  const auto first_non_zero_digit_pos =
    std::find_if(std::cbegin(result), std::cend(result), [](const auto digit) { return digit != 0; });

  if (first_non_zero_digit_pos != std::cend(result))
    result.erase(std::cbegin(result), first_non_zero_digit_pos);

  if (is_negative_product) {
    for (auto &digit : result)
      digit = -digit;
  }

  return big_integer{ std::move(result) };
}

void big_integer::invert_sign()
{
  is_negative_number_ = !is_negative_number_;

  for (auto &digit : decimal_digits_) {
    auto abs_digit = std::abs(digit);
    digit = is_negative_number_ && abs_digit > 0 ? -abs_digit : abs_digit;
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

const std::string &big_integer::get_hexadecimal_number(const std::string &prefix, const std::string &postfix, const size_t number_of_digits)
{

  const size_t prefix_len{ prefix.length() };
  const size_t postfix_len{ postfix.length() };
  size_t hex_str_len{ hexadecimal_number_str_.length() };

  if (!prefix.empty() && hex_str_len >= prefix_len && !stl::helper::str_starts_with(hexadecimal_number_str_, prefix, false)) {
    hexadecimal_number_str_.insert(0, prefix);
    hex_str_len += prefix.length();
  }

  if (!postfix.empty() && hex_str_len >= postfix_len && !stl::helper::str_ends_with(hexadecimal_number_str_, postfix, false)) {
    hexadecimal_number_str_.append(postfix);
  }

  hex_str_len -= (prefix_len + postfix_len);

  if (number_of_digits != 0 && number_of_digits > hex_str_len)
    hexadecimal_number_str_.insert(std::cbegin(hexadecimal_number_str_) + prefix_len, number_of_digits - hex_str_len, '0');

  return hexadecimal_number_str_;
}

const std::string &big_integer::get_octal_number(const std::string &prefix, const std::string &postfix, const size_t number_of_digits)
{
  const size_t prefix_len{ prefix.length() };
  const size_t postfix_len{ postfix.length() };
  size_t octal_str_len{ octal_number_str_.length() };

  if (!prefix.empty() && octal_str_len >= prefix_len && !stl::helper::str_starts_with(octal_number_str_, prefix, false)) {
    octal_number_str_.insert(0, prefix);
    octal_str_len += prefix.length();
  }

  if (!postfix.empty() && octal_str_len >= postfix_len && !stl::helper::str_ends_with(octal_number_str_, postfix, false)) {
    octal_number_str_.append(postfix);
  }

  octal_str_len -= (prefix_len + postfix_len);

  if (number_of_digits != 0 && number_of_digits > octal_str_len)
    octal_number_str_.insert(std::cbegin(octal_number_str_) + prefix_len, number_of_digits - octal_str_len, '0');

  return octal_number_str_;
}

void big_integer::set_big_integer_to_default_zero_value()
{
  decimal_digits_.assign({ 0 });
  binary_digits_.assign({ false });
  decimal_number_str_ = "0";
  binary_number_str_ = "0b0";
  octal_number_str_ = "0o0";
  hexadecimal_number_str_ = "0x0";
  base_ = number_base::decimal;
  is_negative_number_ = false;
  first_ = 0;
  last_ = 1;
}

void big_integer::set_big_integer_value_to_positive_one()
{
  decimal_digits_.assign({ 1 });
  binary_digits_.assign({ true });
  decimal_number_str_ = "1";
  binary_number_str_ = "0b1";
  octal_number_str_ = "0o1";
  hexadecimal_number_str_ = "0x1";
  base_ = number_base::decimal;
  is_negative_number_ = false;
  first_ = 0;
  last_ = 1;
}

void big_integer::set_big_integer_value_to_negative_one()
{
  decimal_digits_.assign({ -1 });
  binary_digits_.assign({ true });
  decimal_number_str_ = "-1";
  binary_number_str_ = "-0b1";
  octal_number_str_ = "-0o1";
  hexadecimal_number_str_ = "-0x1";
  base_ = number_base::decimal;
  is_negative_number_ = true;
  first_ = 0;
  last_ = 1;
}

void big_integer::set_big_integer_value_nan()
{
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
  std::vector<bool> binary_number)
{
  if (binary_number.empty() || (1U == binary_number.size() && 0 == binary_number.front())) {
    set_big_integer_to_default_zero_value();
  } else if (1U == binary_number.size() && 1 == binary_number.front()) {
    set_big_integer_value_to_positive_one();
  } else {

    const auto first_true_iter_pos = std::find(std::cbegin(binary_number), std::cend(binary_number), true);

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
  std::vector<int> number)
{
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

  const bool is_negative_number{ number.front() < 0 };

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

  for (auto &digit : number) {
    digit = std::abs(digit);
    if (number_base_allowed_digits_.at(base_).count(digit) == 0U)
      return false;

    number_str.push_back(get_correct_digit_character_for_specified_value(digit));
    digit = is_negative_number && digit > 0 ? -digit : digit;
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
  const std::string &number)
{
  number_base base = number_base::decimal;
  size_t first{}, last{ number.length() };
  bool is_negative_number{};

  if (number[first] == '-' || number[first] == '+') {
    is_negative_number = number[first] == '-';
    first++;
  }

  if ('b' == number[first] || 'B' == number[first]) {
    base = number_base::binary;
    first++;
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
    first++;
  }

  while ((first < last) && ('0' == number[first] || isspace(number[first]))) {
    ++first;
  }

  if (first == last) {
    set_big_integer_to_default_zero_value();
    return true;
  }

  if (base == number_base::binary) {
    if (number[first] != '0' && number[first] != '1')
      return false;

    last = number.find_first_not_of("01'_", first + 1);
  } else if (base == number_base::octal) {
    if (number[first] < '0' || number[first] > '7')
      return false;

    last = number.find_first_not_of("01234567'_", first + 1);
  } else if (base == number_base::decimal) {
    if (number[first] < '0' || number[first] > '9')
      return false;

    last = number.find_first_not_of("0123456789'_", first + 1);
  } else if (base == number_base::hexadecimal) {
    if (number_base_allowed_digits_.at(base).count(number[first]) == 0U)
      return false;

    last = number.find_first_not_of("0123456789abcdefABCDEF'_", first + 1);
  }

  last = std::min(last, number.length());

  if ('\'' == number[last - 1] || '_' == number[last - 1])
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
  const char ch) noexcept
{
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

byte big_integer::get_number_of_bits_needed_for_number_base() const noexcept
{
  switch (base_) {
  case number_base::binary:
    return 1;
  case number_base::octal:
    return 3;
  case number_base::decimal:
  case number_base::hexadecimal:
    return 4;
  }
}

byte big_integer::get_number_base_multiplier() const noexcept
{
  switch (base_) {
  case number_base::binary:
    return 2;
  case number_base::octal:
    return 8;
  case number_base::decimal:
    return 10;
  case number_base::hexadecimal:
    return 16;
  }
}

byte big_integer::get_correct_digit_value(const char ch) const noexcept
{
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

void big_integer::find_2s_complement_for_binary_number()
{
  std::vector<bool> binary_digits{};
  binary_digits.reserve(binary_digits_.size());

  std::transform(std::cbegin(binary_digits_), std::cend(binary_digits_), std::back_inserter(binary_digits), [](const bool bin_digit) { return !bin_digit; });

  bool carry_bit{ true };

  for (int i = binary_digits.size() - 1; i >= 0; --i) {
    const bool result{ (carry_bit && !binary_digits.at(i)) || (!carry_bit && binary_digits.at(i)) };

    carry_bit = carry_bit && binary_digits.at(i);

    binary_digits.at(i) = result;
  }
}

void big_integer::convert_number_to_decimal_and_store()
{
  if (base_ != number_base::decimal) {

    if (binary_digits_.empty()) {
      const byte number_of_bits_for_digit{ number_base_number_of_bits_.at(base_) };

      binary_digits_.reserve((last_ - first_) * number_of_bits_for_digit);

      std::for_each(std::cbegin(decimal_number_str_) + first_,
        std::cbegin(decimal_number_str_) + last_,
        [&](const auto ch) {
          const byte digit{ get_correct_digit_value(ch) };
          for (byte i{}; i < number_of_bits_for_digit; ++i) {
            const byte bit_mask{ static_cast<byte>(
              1_b << (number_of_bits_for_digit - 1 - i)) };
            const bool result{ (digit & bit_mask) == bit_mask };
            binary_digits_.emplace_back(result);
          }
        });
    }

    const size_t bcd_bin_buffer_size =
      binary_digits_.size() + 4 * static_cast<size_t>(std::ceil(binary_digits_.size() / 3.0));

    std::vector<bool> bcd_digits(bcd_bin_buffer_size, false);

    for (size_t i{}; i < binary_digits_.size(); ++i) {
      for (long j = static_cast<long>(bcd_bin_buffer_size) - 1; j >= 3;
           j -= 4) {
        byte value =
          (bcd_digits[j] ? 1_b : 0_b) + (bcd_digits[j - 1] ? 2_b : 0_b) + (bcd_digits[j - 2] ? 4_b : 0_b) + (bcd_digits[j - 3] ? 8_b : 0_b);

        if (value >= 5) {
          value += 3;
          bcd_digits[j] = (value & 1_b) == 1_b;
          bcd_digits[j - 1] = (value & 2_b) == 2_b;
          bcd_digits[j - 2] = (value & 4_b) == 4_b;
          bcd_digits[j - 3] = (value & 8_b) == 8_b;
        }
      }

      for (size_t j{}; j < bcd_digits.size() - 1; ++j)
        bcd_digits[j] = bcd_digits[j + 1];

      bcd_digits.back() = binary_digits_[i];
    }

    if (bcd_digits.size() % 4 != 0) {
      const size_t count_of_zeroes = bcd_digits.size() % 4;
      bcd_digits.insert(std::cbegin(bcd_digits), 4 - count_of_zeroes, false);
    }

    const size_t number_of_bcd_digits{ bcd_digits.size() / 4 };

    decimal_digits_.clear();
    decimal_digits_.reserve(number_of_bcd_digits);

    decimal_number_str_.clear();
    decimal_number_str_.reserve(number_of_bcd_digits + (is_negative_number_ ? 1 : 0));

    bool is_leading_zeroes{ true };
    base_ = number_base::decimal;
    first_ = 0U;

    if (is_negative_number_) {
      decimal_number_str_.push_back('-');
      ++first_;
    }

    for (size_t i{}; i <= bcd_digits.size() - 4; i += 4) {
      const byte value =
        (bcd_digits[i] ? 8_b : 0_b) + (bcd_digits[i + 1] ? 4_b : 0_b) + (bcd_digits[i + 2] ? 2_b : 0_b) + (bcd_digits[i + 3] ? 1_b : 0_b);

      if (value == 0 && is_leading_zeroes)
        continue;

      decimal_digits_.emplace_back(is_negative_number_ ? -value : value);
      decimal_number_str_.push_back('0' + value);
      is_leading_zeroes = false;
    }

    last_ = decimal_number_str_.length();
  } else {
    decimal_digits_.clear();
    decimal_digits_.reserve(last_ - first_);
    std::for_each(
      std::cbegin(decimal_number_str_) + first_,
      std::cend(decimal_number_str_),
      [&](const auto ch) {
        const byte digit{ static_cast<byte>(ch - '0') };
        decimal_digits_.emplace_back(is_negative_number_ ? -digit : digit);
      });

    get_binary_digits_for_decimal_number();
  }
}

void big_integer::get_binary_digits_for_decimal_number()
{
  binary_digits_.clear();
  binary_number_str_.clear();

  std::string number_str{ std::cbegin(decimal_number_str_) + first_,
    std::cend(decimal_number_str_) };

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

  const size_t last_one_pos{ binary_number_str_.find_last_of('1') };
  binary_number_str_.erase(std::cbegin(binary_number_str_) + last_one_pos + 1,
    std::cend(binary_number_str_));

  binary_number_str_.append("b0");

  if (is_negative_number_)
    binary_number_str_.push_back('-');

  std::reverse(std::begin(binary_number_str_), std::end(binary_number_str_));
  std::reverse(std::begin(binary_digits_), std::end(binary_digits_));
}

void big_integer::get_octal_digits_for_decimal_number()
{
  const auto number_of_bits_needed_for_one_octal_digit{
    number_base_number_of_bits_.at(number_base::octal)
  };

  const size_t remainder{ binary_digits_.size() % number_of_bits_needed_for_one_octal_digit };
  if (remainder != 0)
    binary_digits_.insert(std::cbegin(binary_digits_),
      number_of_bits_needed_for_one_octal_digit - remainder,
      false);

  octal_number_str_.clear();
  octal_number_str_.reserve(binary_digits_.size() / number_of_bits_needed_for_one_octal_digit + 2 + (is_negative_number_ ? 1 : 0));

  bool is_leading_zeroes{ true };

  if (is_negative_number_)
    octal_number_str_.push_back('-');

  octal_number_str_.append("0o");

  for (size_t i{};
       i <= binary_digits_.size() - number_of_bits_needed_for_one_octal_digit;
       i += number_of_bits_needed_for_one_octal_digit) {
    const byte value = (binary_digits_[i] ? 4_b : 0_b) + (binary_digits_[i + 1] ? 2_b : 0_b) + (binary_digits_[i + 2] ? 1_b : 0_b);

    if (value == 0 && is_leading_zeroes)
      continue;

    octal_number_str_.push_back('0' + value);
    is_leading_zeroes = false;
  }
}

void big_integer::get_hexadecimal_digits_for_decimal_number()
{
  const auto number_of_bits_needed_for_one_hex_digit{
    number_base_number_of_bits_.at(number_base::hexadecimal)
  };

  const size_t remainder{ binary_digits_.size() % number_of_bits_needed_for_one_hex_digit };
  if (remainder != 0)
    binary_digits_.insert(std::cbegin(binary_digits_),
      number_of_bits_needed_for_one_hex_digit - remainder,
      false);

  hexadecimal_number_str_.clear();
  hexadecimal_number_str_.reserve(binary_digits_.size() / number_of_bits_needed_for_one_hex_digit + 2 + (is_negative_number_ ? 1 : 0));

  bool is_leading_zeroes{ true };

  if (is_negative_number_)
    hexadecimal_number_str_.push_back('-');

  hexadecimal_number_str_.append("0x");

  for (size_t i{};
       i <= binary_digits_.size() - number_of_bits_needed_for_one_hex_digit;
       i += number_of_bits_needed_for_one_hex_digit) {
    const byte value = (binary_digits_[i] ? 8_b : 0_b) + (binary_digits_[i + 1] ? 4_b : 0_b) + (binary_digits_[i + 2] ? 2_b : 0_b) + (binary_digits_[i + 3] ? 1_b : 0_b);

    if (value == 0 && is_leading_zeroes)
      continue;

    hexadecimal_number_str_.push_back(get_correct_digit_character_for_specified_value(value));
    is_leading_zeroes = false;
  }
}

std::string big_integer::div_by_two(const std::string &number_str)
{
  std::string modified_number_str{};
  byte add{};

  for (const char digit : number_str) {
    const byte new_digit{ static_cast<byte>((digit - '0') / 2 + add) };
    modified_number_str.push_back('0' + new_digit);
    add = odds_to_one(digit) * 5;
  }

  if (modified_number_str != "0" && modified_number_str.length() > 1U && modified_number_str.front() == '0') {
    return modified_number_str.substr(1);
  }

  return modified_number_str;
}

byte big_integer::odds_to_one(const char ch)
{
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

void big_integer::reset(const std::string &number)
{
  if (!check_and_process_number_input_characters(number)) {
#ifndef _BIG_INTEGER_LIBRARY_NO_THROW_
    throw std::invalid_argument{
      "Input number string does not contain a valid number!"
    };
#else
    set_big_integer_to_default_zero_value();
#endif
  }

  if (!is_zero() && !is_positive_one() && !is_negative_one()) {
    convert_number_to_decimal_and_store();
    update_big_integer_string_representations();
  }
}

void big_integer::update_big_integer_string_representations()
{
  get_binary_digits_for_decimal_number();
  get_octal_digits_for_decimal_number();
  get_hexadecimal_digits_for_decimal_number();
}
