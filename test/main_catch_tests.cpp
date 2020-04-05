#define CATCH_CONFIG_MAIN

#include "../include/big_integer.h"
#include "../include/catch.hpp"

#if defined(_MSC_VER)
#include <crtdbg.h>
#define ASSERT _ASSERTE_
#else

#include <cassert>

#define ASSERT assert
#endif

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <string>
#include <unordered_map>

using namespace std;
using namespace std::chrono;
using namespace std::string_literals;
using namespace stl::helper;
using namespace org::atib::numerics;

static constexpr const int64_t min_value_int64_t{
    std::numeric_limits<int64_t>::min()};
static constexpr const int64_t max_value_int64_t{
    std::numeric_limits<int64_t>::max()};

static constexpr const double min_value_double{
    std::numeric_limits<double>::min()};
static constexpr const double max_value_double{
    std::numeric_limits<double>::max()};

static mt19937 rand_engine{
    static_cast<unsigned>(steady_clock::now().time_since_epoch().count())};

static unordered_map<number_base, unsigned> number_base_multiplier{
    {number_base::binary, 2},
    {number_base::octal, 8},
    {number_base::decimal, 10},
    {number_base::hexadecimal, 16}};

static unordered_map<number_base, const char*> number_base_name{
    {number_base::binary, "binary"},
    {number_base::octal, "octal"},
    {number_base::decimal, "decimal"},
    {number_base::hexadecimal, "hexadecimal"}};

int get_random_digit_for_specified_number_base(
    const number_base base,
    const bool is_negative_value_allowed = true) {
  if (base == number_base::binary) {
    return is_negative_value_allowed
               ? uniform_int_distribution<int>{-1, 1}(rand_engine)
               : uniform_int_distribution<int>{0, 1}(rand_engine);
  }

  if (base == number_base::octal) {
    return is_negative_value_allowed
               ? uniform_int_distribution<int>{-7, 7}(rand_engine)
               : uniform_int_distribution<int>{0, 7}(rand_engine);
  }

  if (base == number_base::decimal) {
    return is_negative_value_allowed
               ? uniform_int_distribution<int>{-9, 9}(rand_engine)
               : uniform_int_distribution<int>{0, 9}(rand_engine);
  }

  if (base == number_base::hexadecimal) {
    return is_negative_value_allowed
               ? uniform_int_distribution<int>{-15, 15}(rand_engine)
               : uniform_int_distribution<int>{0, 15}(rand_engine);
  }

  return 0;
}

int64_t get_random_integral_value(
    const int64_t lower_bound = min_value_int64_t,
    const int64_t upper_bound = max_value_int64_t) {
  return uniform_int_distribution<int64_t>{lower_bound,
                                           upper_bound}(rand_engine);
}

uint64_t get_random_positive_number(
    const uint64_t lower_bound = std::numeric_limits<uint64_t>::min(),
    const uint64_t upper_bound = std::numeric_limits<uint64_t>::max()) {
  return uniform_int_distribution<uint64_t>{lower_bound,
                                            upper_bound}(rand_engine);
}

double get_random_floating_point_value(
    const double lower_bound = min_value_double,
    const double upper_bound = max_value_double) {
  return uniform_real_distribution<double>{lower_bound,
                                           upper_bound}(rand_engine);
}

number_base get_random_number_base() {
  const auto number_base_id = get_random_integral_value(0, 3);

  switch (number_base_id) {
    case 0:
      return number_base::binary;
    case 1:
      return number_base::octal;
    case 2:
      return number_base::decimal;
    case 3:
    default:
      return number_base::hexadecimal;
  }
}

static constexpr const size_t number_of_tests{10U};
static constexpr const array<const char, 17U> number_base_digits{
    {"0123456789ABCDEF"}};

std::string dec2bin(const int64_t number) {
  if (0 == number)
    return "0b0";

  std::string binary_string{};
  binary_string.reserve(65U + (number < 0 ? 1U : 0U));

  if (number < 0)
    binary_string.push_back('-');
  binary_string.append("0b");

  const uint64_t abs_number = std::abs(number);

  uint64_t bit_mask{0x8000000000000000ULL};

  bool found_first_non_zero_digit{};

  while (bit_mask != 0U) {
    const uint64_t and_result{abs_number & bit_mask};
    if ((and_result != 0U) || found_first_non_zero_digit) {
      binary_string.push_back(and_result != 0U ? '1' : '0');
      found_first_non_zero_digit = true;
    }
    bit_mask >>= 1U;
  }

  return binary_string;
}

std::string dec2oct(const int64_t number) {
  if (0 == number)
    return "0o0";

  std::string octal_string{};
  octal_string.reserve(65U + (number < 0 ? 1U : 0U));

  if (number < 0)
    octal_string.push_back('-');
  octal_string.append("0o");

  const uint64_t abs_number = std::abs(number);

  uint64_t bit_mask{0x7000000000000000ULL};

  bool found_first_non_zero_digit{};
  size_t shift_count{60U};

  while (bit_mask != 0U) {
    const uint64_t and_result = (abs_number & bit_mask) >> shift_count;
    if ((and_result != 0U) || found_first_non_zero_digit) {
      assert(and_result < 8U);
      octal_string.push_back(number_base_digits.at(and_result));
      found_first_non_zero_digit = true;
    }
    bit_mask >>= 3U;
    shift_count -= 3U;
  }

  return octal_string;
}

std::string dec2hex(int64_t number) {
  if (0 == number)
    return "0x0";

  std::string hexadecimal_string{};
  hexadecimal_string.reserve(65U + (number < 0 ? 1U : 0U));

  if (number < 0)
    hexadecimal_string.push_back('-');
  hexadecimal_string.append("0x");

  const uint64_t abs_number = std::abs(number);

  uint64_t bit_mask{0xF000000000000000ULL};

  bool found_first_non_zero_digit{};
  size_t shift_count{60U};

  while (bit_mask != 0U) {
    const uint64_t and_result = (abs_number & bit_mask) >> shift_count;

    if ((and_result != 0U) || found_first_non_zero_digit) {
      assert(and_result < 16U);
      hexadecimal_string.push_back(number_base_digits.at(and_result));
      found_first_non_zero_digit = true;
    }
    bit_mask >>= 4U;
    shift_count -= 4U;
  }

  return hexadecimal_string;
}

uint64_t bit_xor(const uint64_t first, const uint64_t second) {
  uint64_t result{};
  uint64_t mask{1ULL << 63U};

  while (mask != 0ULL) {
    result |= (((first & mask) != 0ULL && (second & mask) == 0ULL) ||
               ((first & mask) == 0ULL && (second & mask) != 0ULL))
                  ? mask
                  : 0U;
    mask >>= 1U;
  }

  return result;
}

TEST_CASE("big_integer() default ctor",
          "Testing big_integer's default constructor: big_integer()") {
  org::atib::numerics::big_integer bi{};
  REQUIRE(bi.get_decimal_number() == "0");
  REQUIRE(bi.get_binary_number() == "0b0");
  REQUIRE(bi.get_octal_number() == "0o0");
  REQUIRE(bi.get_hexadecimal_number() == "0x0");

  org::atib::numerics::big_integer bi2;
  REQUIRE(bi2.get_decimal_number() == "0");
  REQUIRE(bi2.get_binary_number() == "0b0");
  REQUIRE(bi2.get_octal_number() == "0o0");
  REQUIRE(bi2.get_hexadecimal_number() == "0x0");
}

TEST_CASE(
    "explicit big_integer(const char*) && explicit big_integer(std::string)",
    "Testing big_integer's parameterized explicit constructors: explicit "
    "big_integer(const char*) && explicit big_integer(std::string)") {
  org::atib::numerics::big_integer bi_zero1{"0"};
  REQUIRE(bi_zero1.get_decimal_number() == "0");

  org::atib::numerics::big_integer bi_zero2{"0b0"};
  REQUIRE(bi_zero2.get_binary_number() == "0b0");

  org::atib::numerics::big_integer bi_zero3{"0o0"};
  REQUIRE(bi_zero3.get_octal_number() == "0o0");

  org::atib::numerics::big_integer bi_zero4{"0x0"};
  REQUIRE(bi_zero4.get_hexadecimal_number() == "0x0");

  org::atib::numerics::big_integer bi1{"0b11111111"};
  REQUIRE(bi1.get_decimal_number() == "255");
  REQUIRE(bi1.get_binary_number() == "0b11111111");
  REQUIRE(bi1.get_octal_number() == "0o377");
  REQUIRE(bi1.get_hexadecimal_number() == "0xFF");

  org::atib::numerics::big_integer bi2{"0b11111111"s};
  REQUIRE(bi2.get_decimal_number() == "255"s);
  REQUIRE(bi2.get_binary_number() == "0b11111111"s);
  REQUIRE(bi2.get_octal_number() == "0o377"s);
  REQUIRE(bi2.get_hexadecimal_number() == "0xFF"s);

  org::atib::numerics::big_integer bi3{"0o77"};
  REQUIRE(bi3.get_decimal_number() == "63");
  REQUIRE(bi3.get_binary_number() == "0b111111");
  REQUIRE(bi3.get_octal_number() == "0o77");
  REQUIRE(bi3.get_hexadecimal_number() == "0x3F");

  org::atib::numerics::big_integer bi4{"0o77"s};
  REQUIRE(bi4.get_decimal_number() == "63"s);
  REQUIRE(bi4.get_binary_number() == "0b111111"s);
  REQUIRE(bi4.get_octal_number() == "0o77"s);
  REQUIRE(bi4.get_hexadecimal_number() == "0x3F"s);

  org::atib::numerics::big_integer bi5{"1024"};
  REQUIRE(bi5.get_decimal_number() == "1024");
  REQUIRE(bi5.get_binary_number() == "0b10000000000");
  REQUIRE(bi5.get_octal_number() == "0o2000");
  REQUIRE(bi5.get_hexadecimal_number() == "0x400");

  org::atib::numerics::big_integer bi6{"1024"s};
  REQUIRE(bi6.get_decimal_number() == "1024"s);
  REQUIRE(bi6.get_binary_number() == "0b10000000000"s);
  REQUIRE(bi6.get_octal_number() == "0o2000"s);
  REQUIRE(bi6.get_hexadecimal_number() == "0x400"s);

  org::atib::numerics::big_integer bi7{"0xA4"};
  REQUIRE(bi7.get_decimal_number() == "164");
  REQUIRE(bi7.get_binary_number() == "0b10100100");
  REQUIRE(bi7.get_octal_number() == "0o244");
  REQUIRE(bi7.get_hexadecimal_number() == "0xA4");
  org::atib::numerics::big_integer bi8{"0xA4"s};
  REQUIRE(bi8.get_decimal_number() == "164"s);
  REQUIRE(bi8.get_binary_number() == "0b10100100"s);
  REQUIRE(bi8.get_octal_number() == "0o244"s);
  REQUIRE(bi8.get_hexadecimal_number() == "0xA4"s);

  const int64_t random_number1{
      get_random_integral_value(min_value_int64_t, max_value_int64_t)};
  org::atib::numerics::big_integer bi9{random_number1};
  const auto& bi9_dec_str{bi9.get_decimal_number()};
  const auto random_number1_dec_str{std::to_string(random_number1)};
  REQUIRE(bi9_dec_str == random_number1_dec_str);

  const int64_t random_number2{
      get_random_integral_value(min_value_int64_t, max_value_int64_t)};
  org::atib::numerics::big_integer bi10{random_number2};
  const auto& bi10_dec_str{bi10.get_decimal_number()};
  const auto random_number2_dec_str{std::to_string(random_number2)};
  REQUIRE(bi10_dec_str == random_number2_dec_str);

  const int64_t random_number3{
      get_random_integral_value(min_value_int64_t, max_value_int64_t)};
  org::atib::numerics::big_integer bi11{random_number3};
  const auto& bi11_dec_str{bi11.get_decimal_number()};
  const auto random_number3_dec_str{std::to_string(random_number3)};
  REQUIRE(bi11_dec_str == random_number3_dec_str);
}

TEST_CASE(
    "template <typename NumberType,"
    "typename = std::enable_if_t<std::is_integral<NumberType>::value ||"
    "std::is_floating_point<NumberType>::value>>"
    "explicit big_integer(const NumberType number)",
    "Testing big_integer's parameterized explicit constructor: explicit "
    "big_integer(const NumberType)") {
  const int64_t random_number1{
      get_random_integral_value(min_value_int64_t, max_value_int64_t)};
  org::atib::numerics::big_integer bi1{random_number1};
  REQUIRE(bi1.get_decimal_number() == std::to_string(random_number1));

  const double random_number2{
      get_random_floating_point_value(min_value_double, max_value_double)};
  org::atib::numerics::big_integer bi2{random_number2};
  std::string value{std::to_string(random_number2)};
  const size_t dot_pos{value.rfind('.')};
  if (dot_pos != std::string::npos) {
    value = value.substr(0, dot_pos);
  }
  REQUIRE(bi2.get_decimal_number() == value);
}

TEST_CASE(
    "explicit big_integer(std::vector<int>, const number_base = "
    "number_base::decimal)",
    "Testing big_integer's parameterized explicit constructor: "
    "big_integer(std::vector<int>, const number_base = number_base::decimal)") {
  for (size_t i{}; i < 10; ++i) {
    const size_t digit_count{
        static_cast<size_t>(get_random_integral_value(5, 10))};
    std::vector<int> digits{};
    digits.reserve(digit_count);

    const number_base base = get_random_number_base();

    uint64_t value{};

    int first_digit{};
    do {
      first_digit = get_random_digit_for_specified_number_base(base);
    } while (first_digit == 0);

    digits.emplace_back(first_digit);
    value += std::abs(first_digit);
    bool is_negative_number{first_digit < 0};

    for (size_t j{1}; j < digit_count; ++j) {
      const auto digit =
          std::abs(get_random_digit_for_specified_number_base(base));
      digits.emplace_back(is_negative_number ? -digit : digit);
      value *= number_base_multiplier.at(base);
      value += digit;
    }

    std::string number_str{};

    if (is_negative_number)
      number_str.push_back('-');

    number_str.append(std::to_string(value));

    org::atib::numerics::big_integer bi{std::move(digits), base};
    const auto& bi_decimal_number_str{bi.get_decimal_number()};
    REQUIRE(bi_decimal_number_str == number_str);
  }
}

TEST_CASE("explicit big_integer(std::vector<bool>)",
          "Testing big_integer(std::vector<bool>) constructor") {
  org::atib::numerics::big_integer bi1{
      std::vector<bool>{true, false, true, false}};

  REQUIRE(bi1.get_binary_number() == "0b1010");
  REQUIRE(bi1.get_decimal_number() == "10");
  REQUIRE(bi1.get_octal_number() == "0o12");
  REQUIRE(bi1.get_hexadecimal_number() == "0xA");

  org::atib::numerics::big_integer bi2{
      std::vector<bool>{true, true, true, true, false, true, false}};

  REQUIRE(bi2.get_binary_number() == "0b1111010");
  REQUIRE(bi2.get_decimal_number() == "122");
  REQUIRE(bi2.get_octal_number() == "0o172");
  REQUIRE(bi2.get_hexadecimal_number() == "0x7A");

  org::atib::numerics::big_integer bi3{
      std::vector<bool>{false, true, true, true, true, false, true, false}};

  REQUIRE(bi3.get_binary_number() == "0b1111010");
  REQUIRE(bi3.get_decimal_number() == "122");
  REQUIRE(bi3.get_octal_number() == "0o172");
  REQUIRE(bi3.get_hexadecimal_number() == "0x7A");

  for (size_t i{}; i < 10; ++i) {
    const size_t digit_count{
        static_cast<size_t>(get_random_integral_value(32, 64))};
    std::vector<bool> digits{};
    digits.reserve(digit_count);

    uint64_t value{};

    for (size_t j{}; j < digit_count; ++j) {
      const bool binary_digit =
          static_cast<bool>(get_random_digit_for_specified_number_base(
              number_base::binary, false));
      digits.emplace_back(binary_digit);
      value <<= 1U;
      value += binary_digit;
    }

    const std::string number_str{std::to_string(value)};

    org::atib::numerics::big_integer bi{std::move(digits)};
    const auto& bi_decimal_number_str{bi.get_decimal_number()};
    REQUIRE(bi_decimal_number_str == number_str);
  }
}

TEST_CASE("void reset(const std::string&)",
          "Testing the correct functionality of void reset(const str::string& "
          "number") {
  for (size_t i{}; i < 10; ++i) {
    const size_t digit_count{
        static_cast<size_t>(get_random_integral_value(10, 30))};
    std::string number{};
    number.reserve(digit_count + 3);

    const number_base base = get_random_number_base();

    if (0 ==
        get_random_digit_for_specified_number_base(number_base::binary, false))
      number.push_back('-');

    number.append(big_integer::number_base_prefix_.at(base));

    int first_digit{};
    do {
      first_digit = get_random_digit_for_specified_number_base(base, false);
    } while (first_digit == 0);

    number.push_back(number_base_digits.at(first_digit));

    for (size_t j{1}; j < digit_count; ++j) {
      number.push_back(number_base_digits.at(
          get_random_digit_for_specified_number_base(base, false)));
    }

    const int64_t random_number{
        get_random_integral_value(min_value_int64_t, max_value_int64_t)};
    const string random_number_str{std::to_string(random_number)};

    org::atib::numerics::big_integer bi1{random_number};

    REQUIRE(bi1.get_decimal_number() == random_number_str);

    bi1.assign(number);

    switch (base) {
      case number_base::binary: {
        const auto bi1_binary_number{bi1.get_binary_number()};
        REQUIRE(bi1_binary_number == number);
        break;
      }

      case number_base::octal: {
        const auto bi1_octal_number{bi1.get_octal_number()};
        REQUIRE(bi1_octal_number == number);
        break;
      }

      case number_base::decimal: {
        const auto& bi1_decimal_number{bi1.get_decimal_number()};
        REQUIRE(bi1_decimal_number == number);
        break;
      }

      case number_base::hexadecimal: {
        const auto bi1_hexadecimal_number{bi1.get_hexadecimal_number()};
        REQUIRE(bi1_hexadecimal_number == number);
        break;
      }
    }
  }
}

TEST_CASE("string big_integer::get_big_integer() const",
          "Testing string big_integer::get_big_integer() const public method") {
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t random_number{get_random_integral_value()};

    org::atib::numerics::big_integer bi{random_number};

    REQUIRE(bi.get_big_integer(number_base::decimal) ==
            std::to_string(random_number));
    REQUIRE(bi.get_big_integer(number_base::binary) == dec2bin(random_number));
    REQUIRE(bi.get_big_integer(number_base::octal) == dec2oct(random_number));
    REQUIRE(bi.get_big_integer(number_base::hexadecimal) ==
            dec2hex(random_number));
  }
}

TEST_CASE("big_integer::operator std::string() const noexcept",
          "Testing big_integer's explicit operator std::string() const "
          "noexcept public method") {
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t random_number{get_random_integral_value()};

    org::atib::numerics::big_integer bi{random_number};

    const std::string bi_decimal_number1 = static_cast<std::string>(bi);
    const std::string bi_decimal_number2{bi};

    const std::string random_number_str{std::to_string(random_number)};

    REQUIRE(bi_decimal_number1 == random_number_str);
    REQUIRE(bi_decimal_number2 == random_number_str);

    const double random_double_value{get_random_floating_point_value()};

    std::string random_double_value_str{std::to_string(random_double_value)};
    random_double_value_str.erase(std::cbegin(random_double_value_str) +
                                      random_double_value_str.find('.'),
                                  std::cend(random_double_value_str));

    org::atib::numerics::big_integer bi2{random_double_value};

    const std::string bi_decimal_number3 = static_cast<std::string>(bi2);
    const std::string bi_decimal_number4{bi2};

    REQUIRE(random_double_value_str == bi_decimal_number3);
    REQUIRE(random_double_value_str == bi_decimal_number4);
  }
}

TEST_CASE("big_integer::operator const char *() const noexcept",
          "Testing big_integer's explicit operator const char *() const "
          "noexcept public method") {
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t random_number{get_random_integral_value()};

    org::atib::numerics::big_integer bi{random_number};

    const char* bi_decimal_number1 = static_cast<const char*>(bi);
    const char* bi_decimal_number2{bi};

    const std::string random_number_str{std::to_string(random_number)};

    REQUIRE(random_number_str == bi_decimal_number1);
    REQUIRE(random_number_str == bi_decimal_number2);

    const double random_double_value{get_random_floating_point_value()};

    std::string random_double_value_str{std::to_string(random_double_value)};
    random_double_value_str.erase(std::cbegin(random_double_value_str) +
                                      random_double_value_str.find('.'),
                                  std::cend(random_double_value_str));

    org::atib::numerics::big_integer bi2{random_double_value};

    const char* bi_decimal_number3 = static_cast<const char*>(bi2);
    const char* bi_decimal_number4{bi2};

    REQUIRE(random_double_value_str == bi_decimal_number3);
    REQUIRE(random_double_value_str == bi_decimal_number4);
  }
}

TEST_CASE("big_integer::operator-() const",
          "Testing big_integer's operator-() const public method") {
  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t random_number{get_random_integral_value()};

    org::atib::numerics::big_integer bi{random_number};
    std::string random_number_str{std::to_string(random_number)};

    REQUIRE(random_number_str == bi.get_decimal_number());
    REQUIRE(bi.is_negative_number() == (random_number < 0));

    bi = -bi;
    random_number = -random_number;
    random_number_str = std::to_string(random_number);

    REQUIRE(random_number_str == bi.get_decimal_number());
    REQUIRE(bi.is_negative_number() == (random_number < 0));

    double random_double_value{get_random_floating_point_value()};

    std::string random_double_value_str{std::to_string(random_double_value)};
    random_double_value_str.erase(std::cbegin(random_double_value_str) +
                                      random_double_value_str.find('.'),
                                  std::cend(random_double_value_str));

    org::atib::numerics::big_integer bi2{random_double_value};

    REQUIRE(random_double_value_str == bi2.get_decimal_number());
    REQUIRE(bi2.is_negative_number() == (random_double_value < 0));

    bi2 = -bi2;
    random_double_value = -random_double_value;
    random_double_value_str = std::to_string(random_double_value);
    random_double_value_str.erase(std::cbegin(random_double_value_str) +
                                      random_double_value_str.find('.'),
                                  std::cend(random_double_value_str));

    REQUIRE(random_double_value_str == bi2.get_decimal_number());
    REQUIRE(bi2.is_negative_number() == (random_double_value < 0));
  }
}

TEST_CASE(
    "big_integer& operator+=(const big_integer&)",
    "Testing big_integer's operator+=(const big_integer&) public method") {
  const int64_t lower_limit{-(1LL << 31)};
  const int64_t upper_limit{(1LL << 31) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand{get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    big_integer bi1{first_operand};
    big_integer bi2{second_operand};

    bi1 += bi2;

    first_operand += second_operand;

    const std::string random_number_str{std::to_string(first_operand)};
    REQUIRE(random_number_str == bi1.get_decimal_number());
  }
}

TEST_CASE(
    "big_integer& operator-=(const big_integer&)",
    "Testing big_integer's operator-=(const big_integer&) public method") {
  const int64_t lower_limit{-(1LL << 31)};
  const int64_t upper_limit{(1LL << 31) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand{get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    big_integer bi1{first_operand};
    big_integer bi2{second_operand};

    bi1 -= bi2;

    first_operand -= second_operand;

    const std::string random_number_str{std::to_string(first_operand)};
    REQUIRE(random_number_str == bi1.get_decimal_number());
  }
}

TEST_CASE(
    "big_integer& operator*=(const big_integer&)",
    "Testing big_integer's operator*=(const big_integer&) public method") {
  const int64_t lower_limit{-(1LL << 24)};
  const int64_t upper_limit{(1LL << 24) - 1};

  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand{get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    big_integer bi1{first_operand};
    big_integer bi2{second_operand};

    bi1 *= bi2;

    first_operand *= second_operand;

    const std::string random_number_str{std::to_string(first_operand)};
    REQUIRE(random_number_str == bi1.get_decimal_number());
  }
}

TEST_CASE(
    "big_integer& operator/=(const big_integer&)",
    "Testing big_integer's operator/=(const big_integer&) public method") {
  const int64_t lower_limit{-(1LL << 31)};
  const int64_t upper_limit{(1LL << 31) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand{get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    big_integer bi1{first_operand};
    big_integer bi2{second_operand};

    bi1 /= bi2;

    first_operand /= second_operand;

    const std::string random_number_str{std::to_string(first_operand)};
    REQUIRE(random_number_str == bi1.get_decimal_number());
  }
}

TEST_CASE(
    "big_integer& operator%=(const big_integer&)",
    "Testing big_integer's operator%=(const big_integer&) public method") {
  const int64_t lower_limit{std::numeric_limits<int64_t>::min()};
  const int64_t upper_limit{std::numeric_limits<int64_t>::max()};
  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand = get_random_integral_value(lower_limit, upper_limit);
    const int64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    bi1 %= bi2;

    first_operand %= second_operand;

    const std::string remainder_str{std::to_string(first_operand)};
    REQUIRE(remainder_str == bi1.get_decimal_number());
  }
}

TEST_CASE("big_integer operator+(const big_integer&, const big_integer&)",
          "Testing correct functionality of the globally defined "
          "addition operator.") {
  const int64_t lower_limit{-(1LL << 31)};
  const int64_t upper_limit{(1LL << 31) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t first_operand{
        get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    const big_integer bi_sum{bi1 + bi2};

    const int64_t sum{first_operand + second_operand};

    const std::string sum_str{std::to_string(sum)};
    REQUIRE(sum_str == bi_sum.get_decimal_number());
  }
}

TEST_CASE("big_integer operator-(const big_integer&, const big_integer&)",
          "Testing correct functionality of the globally defined "
          "minus operator.") {
  const int64_t lower_limit{-(1LL << 31)};
  const int64_t upper_limit{(1LL << 31) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t first_operand{
        get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    const big_integer bi_difference{bi1 - bi2};

    const int64_t difference{first_operand - second_operand};

    const std::string difference_str{std::to_string(difference)};
    REQUIRE(difference_str == bi_difference.get_decimal_number());
  }
}

TEST_CASE("big_integer operator*(const big_integer&, const big_integer&)",
          "Testing correct functionality of the globally defined "
          "multiplication operator.") {
  const int64_t lower_limit{-(1LL << 24)};
  const int64_t upper_limit{(1LL << 24) - 1};

  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t first_operand{
        get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    const big_integer bi_product{bi1 * bi2};

    const int64_t product{first_operand * second_operand};

    const std::string product_str{std::to_string(product)};
    REQUIRE(product_str == bi_product.get_decimal_number());
  }
}

TEST_CASE("big_integer operator/(const big_integer&, const big_integer&)",
          "Testing correct functionality of the globally defined "
          "division operator function.") {
  const int64_t lower_limit{-(1LL << 31)};
  const int64_t upper_limit{(1LL << 31) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t first_operand{
        get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    const big_integer bi_divison{bi1 / bi2};

    const int64_t divison{first_operand / second_operand};

    const std::string divison_str{std::to_string(divison)};
    REQUIRE(divison_str == bi_divison.get_decimal_number());
  }
}

TEST_CASE("big_integer operator%(const big_integer&, const big_integer&)",
          "Testing correct functionality of the globally defined "
          "modulus operator function.") {
  const int64_t lower_limit{std::numeric_limits<int64_t>::min()};
  const int64_t upper_limit{std::numeric_limits<int64_t>::max()};
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const int64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    const big_integer bi_remainder{bi1 % bi2};

    const int64_t remainder{first_operand % second_operand};

    const std::string remainder_str{std::to_string(remainder)};
    REQUIRE(remainder_str == bi_remainder.get_decimal_number());
  }
}

TEST_CASE("big_integer& operator++()",
          "Testing big_integer's operator++() public method") {
  big_integer minus_one{big_integer::minus_one};
  REQUIRE(++minus_one == big_integer::zero);

  big_integer zero{big_integer::zero};
  REQUIRE(++zero == big_integer::plus_one);

  big_integer nan{big_integer::nan};
  REQUIRE(++nan == big_integer::nan);

  const int64_t lower_limit = -(1ULL << 31U);
  const int64_t upper_limit = (1ULL << 31U) - 1;

  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand{get_random_integral_value(lower_limit, upper_limit)};
    big_integer bi1{first_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());

    ++first_operand;
    ++bi1;

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
  }
}

TEST_CASE("big_integer operator++(int) const",
          "Testing big_integer's operator++(int) const public method") {
  big_integer minus_one{big_integer::minus_one};
  REQUIRE(minus_one++ == big_integer::minus_one);
  REQUIRE(minus_one == big_integer::zero);

  big_integer zero{big_integer::zero};
  REQUIRE(zero++ == big_integer::zero);
  REQUIRE(zero == big_integer::plus_one);

  big_integer nan{big_integer::nan};
  REQUIRE(nan++ == big_integer::nan);
  REQUIRE(nan == big_integer::nan);

  const int64_t lower_limit = -(1ULL << 31U);
  const int64_t upper_limit = (1ULL << 31U) - 1;

  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand{get_random_integral_value(lower_limit, upper_limit)};
    big_integer bi1{first_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());

    auto second_operand = first_operand++;
    auto bi2 = bi1++;

    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());
    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
  }
}

TEST_CASE("big_integer& operator--()",
          "Testing big_integer's operator--() public method") {
  big_integer positive_one{big_integer::plus_one};
  REQUIRE(--positive_one == big_integer::zero);

  big_integer zero{big_integer::zero};
  REQUIRE(--zero == big_integer::minus_one);

  big_integer nan{big_integer::nan};
  REQUIRE(--nan == big_integer::nan);

  const int64_t lower_limit = -(1ULL << 31U);
  const int64_t upper_limit = (1ULL << 31U) - 1;

  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand{get_random_integral_value(lower_limit, upper_limit)};
    big_integer bi1{first_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());

    --first_operand;
    --bi1;

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
  }
}

TEST_CASE("big_integer operator--(const) const",
          "Testing big_integer's operator--(int) const public method") {
  big_integer positive_one{big_integer::plus_one};
  REQUIRE(positive_one-- == big_integer::plus_one);
  REQUIRE(positive_one == big_integer::zero);

  big_integer zero{big_integer::zero};
  REQUIRE(zero-- == big_integer::zero);
  REQUIRE(zero == big_integer::minus_one);

  big_integer nan{big_integer::nan};
  REQUIRE(nan-- == big_integer::nan);
  REQUIRE(nan == big_integer::nan);

  const int64_t lower_limit = -(1ULL << 31U);
  const int64_t upper_limit = (1ULL << 31U) - 1;

  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t first_operand{get_random_integral_value(lower_limit, upper_limit)};
    big_integer bi1{first_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());

    auto second_operand = first_operand--;
    auto bi2 = bi1--;

    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());
    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
  }
}

TEST_CASE("big_integer operator<<(const size_t count) const",
          "Testing big_integer's operator<<(const size_t) public method") {
  const uint64_t lower_limit{};
  const uint64_t upper_limit{(1ULL << 32U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    const uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    const size_t shift_count = get_random_integral_value(1, 16);

    const uint64_t first_operand_shifted{first_operand << shift_count};
    const uint64_t second_operand_shifted{second_operand << shift_count};

    const big_integer bi1_shifted{bi1 << shift_count};
    const big_integer bi2_shifted{bi2 << shift_count};

    REQUIRE(std::to_string(first_operand_shifted) ==
            bi1_shifted.get_decimal_number());
    REQUIRE(std::to_string(second_operand_shifted) ==
            bi2_shifted.get_decimal_number());
  }
}

TEST_CASE("big_integer operator>>(const size_t count) const",
          "Testing big_integer's operator>>(const size_t) public method") {
  const uint64_t lower_limit{(1ULL << 32U) - 1};
  const uint64_t upper_limit{(1ULL << 63U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    const uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    const size_t shift_count = get_random_integral_value(1, 16);

    const uint64_t first_operand_shifted{first_operand >> shift_count};
    const uint64_t second_operand_shifted{second_operand >> shift_count};

    const big_integer bi1_shifted{bi1 >> shift_count};
    const big_integer bi2_shifted{bi2 >> shift_count};

    REQUIRE(std::to_string(first_operand_shifted) ==
            bi1_shifted.get_decimal_number());
    REQUIRE(std::to_string(second_operand_shifted) ==
            bi2_shifted.get_decimal_number());
  }
}

TEST_CASE("big_integer& operator<<=(const size_t count)",
          "Testing big_integer's operator<<=(const size_t) public method") {
  const uint64_t lower_limit{};
  const uint64_t upper_limit{(1ULL << 32U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    big_integer bi1{first_operand};
    big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    const size_t shift_count = get_random_integral_value(1, 16);

    first_operand <<= shift_count;
    second_operand <<= shift_count;

    bi1 <<= shift_count;
    bi2 <<= shift_count;

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());
  }
}

TEST_CASE("big_integer& operator>>=(const size_t count)",
          "Testing big_integer's operator>>=(const size_t) public method") {
  const uint64_t lower_limit{(1ULL << 32U) - 1};
  const uint64_t upper_limit{(1ULL << 63U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    big_integer bi1{first_operand};
    big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    const size_t shift_count = get_random_integral_value(1, 16);

    first_operand >>= shift_count;
    second_operand >>= shift_count;

    bi1 >>= shift_count;
    bi2 >>= shift_count;

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());
  }
}

TEST_CASE("big_integer operator|(const big_integer &, const big_integer &)",
          "Testing correct functionality of the globally defined "
          "or operator.") {
  const uint64_t lower_limit{};
  const uint64_t upper_limit{(1ULL << 63U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    const uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    const uint64_t first_operand_and_second_operand{first_operand |
                                                    second_operand};
    const big_integer bi1_and_bi2{bi1 | bi2};

    REQUIRE(std::to_string(first_operand_and_second_operand) ==
            bi1_and_bi2.get_decimal_number());
  }
}

TEST_CASE("big_integer operator&(const big_integer &, const big_integer &)",
          "Testing correct functionality of the globally defined "
          "and operator.") {
  const uint64_t lower_limit{};
  const uint64_t upper_limit{(1ULL << 63U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    const uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    const uint64_t first_operand_and_second_operand{first_operand &
                                                    second_operand};
    const big_integer bi1_and_bi2{bi1 & bi2};

    REQUIRE(std::to_string(first_operand_and_second_operand) ==
            bi1_and_bi2.get_decimal_number());
  }
}

TEST_CASE("big_integer operator^(const big_integer &, const big_integer &)",
          "Testing correct functionality of the globally defined "
          "xor operator.") {
  const uint64_t lower_limit{};
  const uint64_t upper_limit{(1ULL << 63U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    const uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    const uint64_t first_operand_and_second_operand{first_operand ^
                                                    second_operand};
    const big_integer bi1_and_bi2{bi1 ^ bi2};

    REQUIRE(std::to_string(first_operand_and_second_operand) ==
            bi1_and_bi2.get_decimal_number());
  }
}

TEST_CASE(
    "big_integer& operator|=(const big_integer &)",
    "Testing big_integer's operator|=(const big_integer &) public method") {
  const uint64_t lower_limit{};
  const uint64_t upper_limit{(1ULL << 63U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    first_operand |= second_operand;
    bi1 |= bi2;

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
  }
}

TEST_CASE(
    "big_integer& operator&=(const big_integer &)",
    "Testing big_integer's operator&=(const big_integer &) public method") {
  const uint64_t lower_limit{};
  const uint64_t upper_limit{(1ULL << 63U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    first_operand &= second_operand;
    bi1 &= bi2;

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
  }
}

TEST_CASE(
    "big_integer& operator^=(const big_integer &)",
    "Testing big_integer's operator^=(const big_integer &) public method") {
  const uint64_t lower_limit{};
  const uint64_t upper_limit{(1ULL << 63U) - 1};
  for (size_t i{}; i < number_of_tests; ++i) {
    uint64_t first_operand =
        get_random_integral_value(lower_limit, upper_limit);
    const uint64_t second_operand =
        get_random_integral_value(lower_limit, upper_limit);

    big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
    REQUIRE(std::to_string(second_operand) == bi2.get_decimal_number());

    first_operand ^= second_operand;
    bi1 ^= bi2;

    REQUIRE(std::to_string(first_operand) == bi1.get_decimal_number());
  }
}

TEST_CASE("Relational operators",
          "Testing correct functionality of the globally defined "
          "relational operator functions.") {
  const big_integer bi_zero1{0};
  const big_integer bi_zero2{};
  REQUIRE(bi_zero1 == bi_zero2);

  const big_integer bi_positive1{1234567890123456LL};
  const big_integer bi_positive2{"1234567890123456"};
  REQUIRE(bi_positive1 == bi_positive2);

  const big_integer bi_negative1{-1234567890123456LL};
  const big_integer bi_negative2{"-1234567890123456"};
  REQUIRE(bi_negative1 == bi_negative2);

  const int64_t lower_limit{std::numeric_limits<int64_t>::min()};
  const int64_t upper_limit{std::numeric_limits<int64_t>::max()};
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t first_operand{
        get_random_integral_value(lower_limit, upper_limit)};
    const int64_t second_operand{
        get_random_integral_value(lower_limit, upper_limit)};

    const big_integer bi1{first_operand};
    const big_integer bi2{second_operand};

    REQUIRE((first_operand < second_operand) == (bi1 < bi2));
    REQUIRE((first_operand > second_operand) == (bi1 > bi2));
    REQUIRE((first_operand <= second_operand) == (bi1 <= bi2));
    REQUIRE((first_operand >= second_operand) == (bi1 >= bi2));
    REQUIRE((first_operand == second_operand) == (bi1 == bi2));
    REQUIRE((first_operand != second_operand) == (bi1 != bi2));
  }
}

TEST_CASE(
    "size_t operator[](const size_t) const, size_t at(const size_t) const "
    "noexcept",
    "Testing big_integer's operator[](const size_t) const and at(const size_t) "
    "const noexcept public methods") {
  const big_integer positive_one{big_integer::plus_one};
  REQUIRE(positive_one[0] == 1);
  REQUIRE(positive_one.at(0) == 1);

  const big_integer zero{big_integer::zero};
  REQUIRE(zero[0] == 0);
  REQUIRE(zero.at(0) == 0);

  const big_integer negative_one{big_integer::minus_one};
  REQUIRE(negative_one[0] == -1);
  REQUIRE(negative_one.at(0) == -1);

  const big_integer nan{big_integer::nan};
  REQUIRE(nan[0] == 0);
  REQUIRE(nan.at(0) == 0);

  for (size_t i{}; i < number_of_tests; ++i) {
    int64_t random_number{get_random_integral_value()};
    const big_integer bi{random_number};
    size_t factor{};

    while (random_number != 0) {
      const auto remainder = random_number % 10;
      random_number /= 10;

      REQUIRE(remainder == bi[factor]);
      REQUIRE(remainder == bi.at(factor));
      ++factor;
    }
  }
}

TEST_CASE(
    "explicit operator bool() const",
    "Testing big_integer's explicit operator bool() const public method") {
  const big_integer positive_one{big_integer::plus_one};
  REQUIRE(positive_one);

  const big_integer zero{big_integer::zero};
  REQUIRE(!zero);

  const big_integer negative_one{big_integer::minus_one};
  REQUIRE(negative_one);

  const big_integer nan{big_integer::nan};
  REQUIRE(!nan);

  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t random_number{get_random_integral_value()};
    const big_integer bi{random_number};

    REQUIRE(static_cast<bool>(bi) == (random_number != 0));
  }
}

enum class operation { add, sub, mul, div, mod, lsh, rsh };

TEST_CASE("Combination of arithmetic operators",
          "Testing correct functionality of the various arithmetic operator.") {
  std::mt19937 rand_engine{static_cast<unsigned>(
      std::chrono::steady_clock::now().time_since_epoch().count())};
  std::vector<operation> operations{operation::add, operation::sub,
                                    operation::mul, operation::div,
                                    operation::mod};

  const int32_t lower_limit{std::numeric_limits<int32_t>::min()};
  const int32_t upper_limit{std::numeric_limits<int32_t>::max()};

  for (size_t i{}; i < number_of_tests; ++i) {
    std::shuffle(std::begin(operations), std::end(operations), rand_engine);

    std::vector<int32_t> operands{};
    operands.reserve(operations.size() + 1);

    for (size_t j{}; j < operations.size() + 1; ++j) {
      int32_t random_number{};
      do {
        random_number = get_random_integral_value(lower_limit, upper_limit);
        operands.emplace_back(random_number);
      } while (random_number == 0L);
    }

    int64_t result{operands.front()};
    big_integer bi_result{operands.front()};

    for (size_t k{}; k < operations.size(); ++k) {
      switch (operations.at(k)) {
        case operation::add:
          result += operands.at(k + 1);
          bi_result += big_integer{operands.at(k + 1)};
          break;
        case operation::sub:
          result -= operands.at(k + 1);
          bi_result -= big_integer{operands.at(k + 1)};
          break;
        case operation::mul:
          result *= operands.at(k + 1);
          bi_result *= big_integer{operands.at(k + 1)};
          break;
        case operation::div:
          result /= operands.at(k + 1);
          bi_result /= big_integer{operands.at(k + 1)};
          break;
        case operation::mod:
          result %= operands.at(k + 1);
          bi_result %= big_integer{operands.at(k + 1)};
          break;
        default:
          break;
      }
    }

    REQUIRE(std::to_string(result) == bi_result.get_decimal_number());
  }
}

TEST_CASE("std::ostream& operator<<(std::ostream&, const big_integer&)",
          "Testing correct functionality of the globally defined output stream "
          "operator function.") {
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t random_number{get_random_integral_value()};
    const big_integer bi{random_number};
    std::ostringstream oss{};
    oss << bi;
    const std::string random_number_str{std::to_string(random_number)};
    REQUIRE(random_number_str == oss.str());
  }
}

TEST_CASE("std::istream& operator>>(std::istream&, big_integer&)",
          "Testing correct functionality of the globally defined input stream "
          "operator function.") {
  for (size_t i{}; i < number_of_tests; ++i) {
    const int64_t random_number{get_random_integral_value()};
    const std::string random_number_str{std::to_string(random_number)};
    std::istringstream iss{random_number_str};
    big_integer bi;
    iss >> bi;
    REQUIRE(random_number_str == bi.get_decimal_number());
  }
}
