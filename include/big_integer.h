//
// Created by Attila Budai on 2/28/2020.
//

#ifndef BIGINTEGER_V1_BIG_INTEGER_H
#define BIGINTEGER_V1_BIG_INTEGER_H

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "stl_helper_functions.hpp"


#define _BIG_INTEGER_LIBRARY_NO_THROW_

namespace org {
namespace atib {
  namespace numerics {

    using byte = char;
    using number_base = stl::helper::number_base;

    inline constexpr byte operator""_b(const unsigned long long value)
    {
      return static_cast<byte>(value);
    }

    static constexpr const byte _big_integer_library_major_version_ = 0_b;
    static constexpr const byte _big_integer_library_minor_version_ = 0_b;
    static constexpr const byte _big_integer_library_patch_level_version_ = 1_b;

    static constexpr const char *_big_integer_library_version_ = "0.0.1";

    class division_by_zero_error : std::invalid_argument
    {
    public:
      division_by_zero_error()
        : std::invalid_argument(std::string{ "Division by zero is not allowed!" })
      {
      }
    };

    class zero_divided_by_zero_error : std::invalid_argument
    {
    public:
      zero_divided_by_zero_error()
        : std::invalid_argument(
          std::string{ "Zero divided by zero is not allowed!" }) {}
    };

    class big_integer final
    {

      std::vector<int> decimal_digits_;
      std::vector<bool> binary_digits_;
      std::string hexadecimal_number_str_;
      std::string decimal_number_str_;
      std::string octal_number_str_;
      std::string binary_number_str_;
      size_t first_{};
      size_t last_{};
      number_base base_;
      bool is_negative_number_{};

    public:
      static constexpr const char *NaN{ "NaN" };
      static const std::unordered_map<number_base, byte>
        number_base_number_of_bits_;
      static const std::unordered_map<number_base, std::unordered_set<byte>>
        number_base_allowed_digits_;

      static const std::unordered_map<number_base, const char *> number_base_prefix_;

      big_integer();

      template<
        typename NumberType,
        typename = std::enable_if_t<std::is_integral<NumberType>::value || std::is_floating_point<NumberType>::value>>
      explicit big_integer(const NumberType number)
        : big_integer{ number,
            std::conditional_t<std::is_integral<NumberType>::value,
              std::true_type,
              std::false_type>{} } {}

      explicit big_integer(const char *);

      explicit big_integer(const std::string &);

      explicit big_integer(std::vector<int>,
        const number_base = number_base::decimal);

      explicit big_integer(std::vector<bool>);

      big_integer(const big_integer &) = default;

      big_integer(big_integer &&) noexcept = default;

      big_integer &operator=(const big_integer &) = default;

      big_integer &operator=(big_integer &&) = default;

      ~big_integer() = default;

      void reset(const std::string &);

      number_base get_number_base() const noexcept { return base_; }

      std::string get_big_integer(const number_base base = number_base::decimal) const;

      explicit operator std::string() const noexcept;

      explicit operator const char *() const noexcept;

      big_integer operator-() const;

      big_integer &operator+=(const big_integer &rhs);

      big_integer &operator-=(const big_integer &rhs);

      big_integer &operator*=(const big_integer &rhs);

      big_integer &operator++();

      big_integer operator++(int) const;

      big_integer &operator--();

      big_integer operator--(int) const;

      big_integer operator<<(const size_t);

      big_integer operator>>(const size_t);

      big_integer &operator<<=(const size_t);

      big_integer &operator>>=(const size_t);

      size_t operator[](const size_t) const;

      explicit operator bool() const;

      void swap(big_integer &rhs) noexcept;

      big_integer add_two_decimal_numbers_together(const big_integer &rhs) const;

      big_integer multiply_two_decimal_numbers(const big_integer &rhs) const;

      void invert_sign();

      const std::vector<int> &get_decimal_digits() const;

      const std::vector<bool> &get_binary_digits() const;

      std::string get_hexadecimal_number(const std::string &prefix = "0x",
        const std::string &postfix = "",
        const size_t number_of_digits = 0) const;

      std::string get_decimal_number(const std::string &prefix = "",
        const std::string &postfix = "",
        const size_t number_of_digits = 0) const;

      std::string get_octal_number(const std::string &prefix = "0o",
        const std::string &postfix = "",
        const size_t number_of_digits = 0) const;

      std::string get_binary_number(const std::string &prefix = "0b",
        const std::string &postfix = "",
        const size_t number_of_digits = 0) const;

      bool is_zero() const noexcept;

      bool is_positive_one() const noexcept;

      bool is_negative_one() const noexcept;

      bool is_negative_number() const;

      bool is_nan() const noexcept;

      big_integer abs() const;

    private:
      template<typename IntegralType>
      big_integer(const IntegralType number, std::true_type)
        : big_integer{ std::to_string(number) } {}

      template<typename FloatingPointType>
      big_integer(const FloatingPointType number, std::false_type)
      {
        std::string value{ std::to_string(number) };
        const size_t dot_pos{ value.rfind('.') };
        if (dot_pos != std::string::npos) {
          value = value.substr(0, dot_pos);
        }
        reset(value);
      }

      void update_big_integer_string_representations();

      void set_big_integer_to_default_zero_value();

      void set_big_integer_value_to_positive_one();

      void set_big_integer_value_to_negative_one();

      void set_big_integer_value_nan();

      void check_and_process_binary_number_digits(std::vector<bool>);

      bool check_and_process_number_input_digits(std::vector<int>);

      bool check_and_process_number_input_characters(const std::string &);

      static char get_correct_digit_character_for_specified_value(
        const char ch) noexcept;

      inline byte get_number_of_bits_needed_for_number_base() const noexcept;

      inline byte get_number_base_multiplier() const noexcept;

      inline byte get_correct_digit_value(const char ch) const noexcept;

      void find_2s_complement_for_binary_number();

      void convert_number_to_decimal_and_store();

      void get_binary_digits_for_decimal_number();

      void get_octal_digits_for_decimal_number();

      void get_hexadecimal_digits_for_decimal_number();

      static std::string div_by_two(const std::string &number_str);

      static byte odds_to_one(const char ch);
    };

    big_integer operator+(const big_integer &, const big_integer &);

    big_integer operator-(const big_integer &, const big_integer &);

    big_integer operator*(const big_integer &, const big_integer &);

    big_integer operator/(const big_integer &, const big_integer &);

    big_integer operator%(const big_integer &, const big_integer &);

    bool operator==(const big_integer &, const big_integer &);

    bool operator!=(const big_integer &, const big_integer &);

    bool operator<(const big_integer &, const big_integer &);

    bool operator>(const big_integer &, const big_integer &);

    bool operator<=(const big_integer &, const big_integer &);

    bool operator>=(const big_integer &, const big_integer &);

    big_integer operator|(const big_integer &, const big_integer &);

    big_integer operator&(const big_integer &, const big_integer &);

    big_integer operator^(const big_integer &, const big_integer &);

    void swap(big_integer &lhs, big_integer &rhs) noexcept;

    std::ostream &operator<<(std::ostream &, const big_integer &);

    std::istream &operator>>(std::istream &, big_integer &);

  }// namespace numerics
}// namespace atib
}// namespace org

#endif// BIGINTEGER_V1_BIG_INTEGER_H
