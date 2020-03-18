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
  std::numeric_limits<int64_t>::min()
};
static constexpr const int64_t max_value_int64_t{
  std::numeric_limits<int64_t>::max()
};

static constexpr const double min_value_double{
  std::numeric_limits<double>::min()
};
static constexpr const double max_value_double{
  std::numeric_limits<double>::max()
};

static mt19937 rand_engine{
  static_cast<unsigned>(steady_clock::now().time_since_epoch().count())
};

static unordered_map<number_base, unsigned> number_base_multiplier{
  { number_base::binary, 2 },
  { number_base::octal, 8 },
  { number_base::decimal, 10 },
  { number_base::hexadecimal, 16 }
};

static unordered_map<number_base, const char *> number_base_name{
  { number_base::binary, "binary" },
  { number_base::octal, "octal" },
  { number_base::decimal, "decimal" },
  { number_base::hexadecimal, "hexadecimal" }
};

int get_random_digit_for_specified_number_base(
  const number_base base,
  const bool is_negative_value_allowed = true)
{
  if (base == number_base::binary) {
    return is_negative_value_allowed
             ? uniform_int_distribution<int>{ -1, 1 }(rand_engine)
             : uniform_int_distribution<int>{ 0, 1 }(rand_engine);
  }

  if (base == number_base::octal) {
    return is_negative_value_allowed
             ? uniform_int_distribution<int>{ -7, 7 }(rand_engine)
             : uniform_int_distribution<int>{ 0, 7 }(rand_engine);
  }

  if (base == number_base::decimal) {
    return is_negative_value_allowed
             ? uniform_int_distribution<int>{ -9, 9 }(rand_engine)
             : uniform_int_distribution<int>{ 0, 9 }(rand_engine);
  }

  if (base == number_base::hexadecimal) {
    return is_negative_value_allowed
             ? uniform_int_distribution<int>{ -15, 15 }(rand_engine)
             : uniform_int_distribution<int>{ 0, 15 }(rand_engine);
  }

  return 0;
}

int64_t get_random_integral_value(const int64_t lower_bound,
  const int64_t upper_bound)
{
  return uniform_int_distribution<int64_t>{ lower_bound,
    upper_bound }(rand_engine);
}

double get_random_floating_point_value(const double lower_bound,
  const double upper_bound)
{
  return uniform_real_distribution<double>{ lower_bound,
    upper_bound }(rand_engine);
}

number_base get_random_number_base()
{
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

TEST_CASE("big_integer() default ctor",
  "Testing big_integer's default constructor: big_integer()")
{
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
  "big_integer(const char*) && explicit big_integer(std::string)")
{
  org::atib::numerics::big_integer bi_zero1{ "0" };
  REQUIRE(bi_zero1.get_decimal_number() == "0");

  org::atib::numerics::big_integer bi_zero2{ "0b0" };
  REQUIRE(bi_zero2.get_binary_number() == "0b0");

  org::atib::numerics::big_integer bi_zero3{ "0o0" };
  REQUIRE(bi_zero3.get_octal_number() == "0o0");

  org::atib::numerics::big_integer bi_zero4{ "0x0" };
  REQUIRE(bi_zero4.get_hexadecimal_number() == "0x0");

  org::atib::numerics::big_integer bi1{ "0b11111111" };
  REQUIRE(bi1.get_decimal_number() == "255");
  REQUIRE(bi1.get_binary_number() == "0b11111111");
  REQUIRE(bi1.get_octal_number() == "0o377");
  REQUIRE(bi1.get_hexadecimal_number() == "0xFF");

  org::atib::numerics::big_integer bi2{ "0b11111111"s };
  REQUIRE(bi2.get_decimal_number() == "255"s);
  REQUIRE(bi2.get_binary_number() == "0b11111111"s);
  REQUIRE(bi2.get_octal_number() == "0o377"s);
  REQUIRE(bi2.get_hexadecimal_number() == "0xFF"s);

  org::atib::numerics::big_integer bi3{ "0o77" };
  REQUIRE(bi3.get_decimal_number() == "63");
  REQUIRE(bi3.get_binary_number() == "0b111111");
  REQUIRE(bi3.get_octal_number() == "0o77");
  REQUIRE(bi3.get_hexadecimal_number() == "0x3F");

  org::atib::numerics::big_integer bi4{ "0o77"s };
  REQUIRE(bi4.get_decimal_number() == "63"s);
  REQUIRE(bi4.get_binary_number() == "0b111111"s);
  REQUIRE(bi4.get_octal_number() == "0o77"s);
  REQUIRE(bi4.get_hexadecimal_number() == "0x3F"s);

  org::atib::numerics::big_integer bi5{ "1024" };
  REQUIRE(bi5.get_decimal_number() == "1024");
  REQUIRE(bi5.get_binary_number() == "0b10000000000");
  REQUIRE(bi5.get_octal_number() == "0o2000");
  REQUIRE(bi5.get_hexadecimal_number() == "0x400");

  org::atib::numerics::big_integer bi6{ "1024"s };
  REQUIRE(bi6.get_decimal_number() == "1024"s);
  REQUIRE(bi6.get_binary_number() == "0b10000000000"s);
  REQUIRE(bi6.get_octal_number() == "0o2000"s);
  REQUIRE(bi6.get_hexadecimal_number() == "0x400"s);

  org::atib::numerics::big_integer bi7{ "0xA4" };
  REQUIRE(bi7.get_decimal_number() == "164");
  REQUIRE(bi7.get_binary_number() == "0b10100100");
  REQUIRE(bi7.get_octal_number() == "0o244");
  REQUIRE(bi7.get_hexadecimal_number() == "0xA4");
  org::atib::numerics::big_integer bi8{ "0xA4"s };
  REQUIRE(bi8.get_decimal_number() == "164"s);
  REQUIRE(bi8.get_binary_number() == "0b10100100"s);
  REQUIRE(bi8.get_octal_number() == "0o244"s);
  REQUIRE(bi8.get_hexadecimal_number() == "0xA4"s);

  const int64_t random_number1{
    get_random_integral_value(min_value_int64_t, max_value_int64_t)
  };
  org::atib::numerics::big_integer bi9{ random_number1 };
  const auto &bi9_dec_str{ bi9.get_decimal_number() };
  const auto random_number1_dec_str{ std::to_string(random_number1) };
  REQUIRE(bi9_dec_str == random_number1_dec_str);

  const int64_t random_number2{
    get_random_integral_value(min_value_int64_t, max_value_int64_t)
  };
  org::atib::numerics::big_integer bi10{ random_number2 };
  const auto &bi10_dec_str{ bi10.get_decimal_number() };
  const auto random_number2_dec_str{ std::to_string(random_number2) };
  REQUIRE(bi10_dec_str == random_number2_dec_str);

  const int64_t random_number3{
    get_random_integral_value(min_value_int64_t, max_value_int64_t)
  };
  org::atib::numerics::big_integer bi11{ random_number3 };
  const auto &bi11_dec_str{ bi11.get_decimal_number() };
  const auto random_number3_dec_str{ std::to_string(random_number3) };
  REQUIRE(bi11_dec_str == random_number3_dec_str);
}

TEST_CASE(
  "template <typename NumberType,"
  "typename = std::enable_if_t<std::is_integral<NumberType>::value ||"
  "std::is_floating_point<NumberType>::value>>"
  "explicit big_integer(const NumberType number)",
  "Testing big_integer's parameterized explicit constructor: explicit "
  "big_integer(const NumberType)")
{
  const int64_t random_number1{
    get_random_integral_value(min_value_int64_t, max_value_int64_t)
  };
  org::atib::numerics::big_integer bi1{ random_number1 };
  REQUIRE(bi1.get_decimal_number() == std::to_string(random_number1));

  const double random_number2{
    get_random_floating_point_value(min_value_double, max_value_double)
  };
  org::atib::numerics::big_integer bi2{ random_number2 };
  std::string value{ std::to_string(random_number2) };
  const size_t dot_pos{ value.rfind('.') };
  if (dot_pos != std::string::npos) {
    value = value.substr(0, dot_pos);
  }
  REQUIRE(bi2.get_decimal_number() == value);
}

TEST_CASE(
  "explicit big_integer(std::vector<int>, const number_base = "
  "number_base::decimal)",
  "Testing big_integer's parameterized explicit constructor: "
  "big_integer(std::vector<int>, const number_base = number_base::decimal)")
{
  for (size_t i{}; i < 10; ++i) {
    const size_t digit_count{
      static_cast<size_t>(get_random_integral_value(5, 10))
    };
    std::vector<int> digits{};
    digits.reserve(digit_count);

    const number_base base = get_random_number_base();

    std::cout << "Randomly chosen number base -> " << number_base_name.at(base)
              << '\n';

    uint64_t value{};

    int first_digit{};
    do {
      first_digit = get_random_digit_for_specified_number_base(base);
    } while (first_digit == 0);

    digits.emplace_back(first_digit);
    value += std::abs(first_digit);
    bool is_negative_number{ first_digit < 0 };

    for (size_t i{ 1 }; i < digit_count; ++i) {
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

    org::atib::numerics::big_integer bi{ std::move(digits), base };
    const auto &bi_decimal_number_str{ bi.get_decimal_number() };
    std::cout << bi_decimal_number_str << " == " << number_str << " ? "
              << (bi_decimal_number_str == number_str ? "true" : "false")
              << '\n';
    REQUIRE(bi_decimal_number_str == number_str);
  }
}

TEST_CASE("explicit big_integer(std::vector<bool>)", "Testing big_integer(std::vector<bool>) constructor")
{

  org::atib::numerics::big_integer bi1{ std::vector<bool>{ true, false, true, false } };

  REQUIRE(bi1.get_binary_number() == "0b1010");
  REQUIRE(bi1.get_decimal_number() == "10");
  REQUIRE(bi1.get_octal_number() == "0o12");
  REQUIRE(bi1.get_hexadecimal_number() == "0xA");

  org::atib::numerics::big_integer bi2{ std::vector<bool>{ true, true, true, true, false, true, false } };

  REQUIRE(bi2.get_binary_number() == "0b1111010");
  REQUIRE(bi2.get_decimal_number() == "122");
  REQUIRE(bi2.get_octal_number() == "0o172");
  REQUIRE(bi2.get_hexadecimal_number() == "0x7A");

  org::atib::numerics::big_integer bi3{ std::vector<bool>{ false, true, true, true, true, false, true, false } };

  REQUIRE(bi3.get_binary_number() == "0b1111010");
  REQUIRE(bi3.get_decimal_number() == "122");
  REQUIRE(bi3.get_octal_number() == "0o172");
  REQUIRE(bi3.get_hexadecimal_number() == "0x7A");

  for (size_t i{}; i < 10; ++i) {
    const size_t digit_count{
      static_cast<size_t>(get_random_integral_value(32, 64))
    };
    std::vector<bool> digits{};
    digits.reserve(digit_count);

    uint64_t value{};

    for (size_t j{}; j < digit_count; ++j) {
      const bool binary_digit = static_cast<bool>(get_random_digit_for_specified_number_base(number_base::binary,
        false));
      digits.emplace_back(binary_digit);
      value <<= 1U;
      value += binary_digit;
    }

    const std::string number_str{ std::to_string(value) };

    org::atib::numerics::big_integer bi{ std::move(digits) };
    const auto &bi_decimal_number_str{ bi.get_decimal_number() };
    std::cout << bi_decimal_number_str << " == " << number_str << " ? "
              << (bi_decimal_number_str == number_str ? "true" : "false")
              << '\n';
    REQUIRE(bi_decimal_number_str == number_str);
  }
}

TEST_CASE(
  "big_integer operator*(const big_integer&, const big_integer&)",
  "Checking the correctness of the globally defined multiplication operator carried out on the "
  "2 provided big_integer arguments.")
{
  org::atib::numerics::big_integer lhs{ "123" };
  org::atib::numerics::big_integer rhs{ "255" };
  org::atib::numerics::big_integer product{ lhs * rhs };
  REQUIRE(product.get_decimal_number() == "31365"s);

  org::atib::numerics::big_integer bi1{ "361231416" };
  org::atib::numerics::big_integer bi2{ "12313198" };
  product = bi1 * bi2;
  REQUIRE(product.get_decimal_number() == "4447913949028368"s);
}

// explicit big_integer(std::vector<byte>,
//	const number_base = number_base::decimal);
//
// explicit big_integer(std::vector<bool>);

//            fruits.insert(fruit);
//        }
//
//        return fruits;
//    }();
//
//    REQUIRE(has_key(all_fruits, wstring{ L"orange" }));
//
//    REQUIRE(!has_key(all_fruits, L"grapefruit" ));
//
//    const vector<wstring> random_fruits{
//        L"apple", L"apricot", L"banana", L"orange",
// L"grapefruit", L"lemon", L"pear", L"pineapple", L"plum", L"walnut"
//    };
//
//    mt19937 rand_engine{rd()};
//
//    auto const num_distr = uniform_int_distribution<unsigned>(0,
// random_fruits.size() - 1);
//
//    const auto random_fruit{random_fruits[num_distr(rand_engine)]};
//
//    if (all_fruits.find(random_fruit) != cend(all_fruits))
//        REQUIRE(has_key(all_fruits, random_fruit));
//
//    else
//        REQUIRE(!has_key(all_fruits, random_fruit));
//}
//
// TEST_CASE("bool has_value(const multiset<ValueType>& container, const
// typename multiset<ValueType>::key_type& item)", "Testing global template
// function bool has_value(const multiset<ValueType>& container, const typename
// multiset<ValueType>::key_type& item)")
//{
//    const wstring src{
//        L"apple:|:banana:|:cabbage:|:lemon:|:orange:|:pepper:|:pineapple:|:plum:|:walnut:|:lemon:|:apricot:|:plum"
//    };
//
//    const wstring needle{L":|:"};
//
//    const auto parts{split(src, needle)};
//
//    const multiset<wstring> all_fruits = [&]()
//    {
//        multiset<wstring> fruits{};
//
//        for (const auto& fruit : parts)
//        {
//            fruits.insert(fruit);
//        }
//
//        return fruits;
//    }();
//
//    REQUIRE(has_key(all_fruits, wstring{ L"orange" }));
//
//    REQUIRE(!has_key(all_fruits, L"grapefruit" ));
//
//    const vector<wstring> random_fruits{
//        L"apple", L"apricot", L"banana", L"orange",
// L"grapefruit", L"lemon", L"pear", L"pineapple", L"plum", L"walnut"
//    };
//
//    mt19937 rand_engine{rd()};
//
//    auto const num_distr = uniform_int_distribution<unsigned>(0,
// random_fruits.size() - 1);
//
//    const auto random_fruit{random_fruits[num_distr(rand_engine)]};
//
//    if (all_fruits.find(random_fruit) != cend(all_fruits))
//        REQUIRE(has_key(all_fruits, random_fruit));
//
//    else
//        REQUIRE(!has_key(all_fruits, random_fruit));
//}
//
// TEST_CASE("bool has_key(const unordered_set<ValueType>& container, const
// typename unordered_set<ValueType>::key_type& item)", "Testing global template
// function bool has_key(const unordered_set<ValueType>& container, const
// typename unordered_set<ValueType>::key_type& item)")
//{
//    const wstring src{
//        L"apple:|:banana:|:cabbage:|:lemon:|:orange:|:pepper:|:pineapple:|:plum:|:walnut:|:lemon:|:apricot:|:plum"
//    };
//
//    const wstring needle{L":|:"};
//
//    const auto parts{split(src, needle)};
//
//    const unordered_set<wstring> all_fruits = [&]()
//    {
//        unordered_set<wstring> fruits{};
//
//        for (const auto& fruit : parts)
//        {
//            fruits.insert(fruit);
//        }
//
//        return fruits;
//    }();
//
//    REQUIRE(has_key(all_fruits, wstring{ L"orange" }));
//
//    REQUIRE(!has_key(all_fruits, L"grapefruit" ));
//
//    const vector<wstring> random_fruits{
//        L"apple", L"apricot", L"banana", L"orange",
// L"grapefruit", L"lemon", L"pear", L"pineapple", L"plum", L"walnut"
//    };
//
//    mt19937 rand_engine{rd()};
//
//    auto const num_distr = uniform_int_distribution<unsigned>(0,
// random_fruits.size() - 1);
//
//    const auto random_fruit{random_fruits[num_distr(rand_engine)]};
//
//    if (all_fruits.find(random_fruit) != cend(all_fruits))
//        REQUIRE(has_key(all_fruits, random_fruit));
//
//    else
//        REQUIRE(!has_key(all_fruits, random_fruit));
//}
//
// TEST_CASE("bool has_key(const unordered_multiset<ValueType>& container, const
// typename unordered_multiset<ValueType>::key_type&  item)", "Testing global
// template function bool has_key(const unordered_multiset<ValueType>&
// container, const typename unordered_multiset<ValueType>::key_type& item)"
//)
//{
//    const wstring src{
//        L"apple:|:banana:|:cabbage:|:lemon:|:orange:|:pepper:|:pineapple:|:plum:|:walnut:|:lemon:|:apricot:|:plum"
//    };
//
//    const wstring needle{L":|:"};
//
//    const auto parts{split(src, needle)};
//
//    const unordered_multiset<wstring> all_fruits = [&]()
//    {
//        unordered_multiset<wstring> fruits{};
//
//        for (const auto& fruit : parts)
//        {
//            fruits.insert(fruit);
//        }
//
//        return fruits;
//    }();
//
//    REQUIRE(has_key(all_fruits, L"orange"));
//
//    REQUIRE(!has_key(all_fruits, wstring{ L"grapefruit" }));
//
//    const vector<wstring> random_fruits{
//        L"apple", L"apricot", L"banana", L"orange",
// L"grapefruit", L"lemon", L"pear", L"pineapple", L"plum", L"walnut"
//    };
//
//    mt19937 rand_engine{rd()};
//
//    auto const num_distr = uniform_int_distribution<unsigned>(0,
// random_fruits.size() - 1);
//
//    const auto random_fruit{random_fruits[num_distr(rand_engine)]};
//
//    if (all_fruits.find(random_fruit) != cend(all_fruits))
//        REQUIRE(has_key(all_fruits, random_fruit));
//
//    else
//        REQUIRE(!has_key(all_fruits, random_fruit));
//}
//
// TEST_CASE(
//    "string get_type_name(T&& arg) | wstring get_type_name_w(T&&
// arg) | void show_var_info(const T& arg) | void show_var_info_w(const T& arg)
//||
// bool check_data_types_for_equality(Args&&... args)"
//    ,
//    "Testing global template functions: get_type_name,
// get_type_name_w, show_var_info, show_var_info_w and
// check_data_types_for_equality"
//)
//{
//    const wstring
// src{L"apple:|:banana:|:cabbage:|:lemon:|:orange:|:pepper:|:plum"};
//
//    const wstring needle{L":|:"};
//
//    const auto parts{split(src, needle)};
//
//    cout << '\n' << get_type_name(src) << '\n';
//
//    wcout << get_type_name_w(needle) << L'\n';
//
//    const string info_message{"Hello World!"};
//
//    show_var_info(info_message);
//
//    show_var_info_w(src);
//
//    show_var_info_w(needle);
//
//    cout << "\ncheck_data_types_for_equality(src : wstring, needle :
// wstring) -> " << boolalpha << check_data_types_for_equality(src, needle) <<
//'\n';
//
//    cout << "\ncheck_data_types_for_equality(src : wstring, parts :
// vector<wstring>) -> " << check_data_types_for_equality(src, parts) << '\n';
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//    auto const result_u16string{
// to_u16string(static_cast<short>(12345)) };
//
//
//
//    string result_string(cbegin(result_u16string),
// cend(result_u16string));
//
//
//
//    cout << "\nto_u16string(12345) = \"" << result_string << "\"\n";
//
//    REQUIRE(u16string{ u"12345" } == result_u16string);
//
//    const auto float_to_u16string{ to_u16string(7.51735f) };
//
//    REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//    result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//    cout << "to_u16string(7.51735f) = \"" << result_string <<
//"\"\n";
//
//}
//
// TEST_CASE("u16string to_u16string(const unsigned short value)", "Testing
// correct functionality of global conversion function: u16string
// to_u16string(const unsigned short value)")
//{
//    auto const result_u16string{ to_u16string(static_cast<unsigned
// short>(12345)) };
//
//    const string result_string(cbegin(result_u16string),
// cend(result_u16string));
//
//    cout << "\nto_u16string(12345) = \"" << result_string << "\"\n";
//
//    REQUIRE(u16string{ u"12345" } == result_u16string);
//
//}
//
// TEST_CASE("float stof(const u16string& str, size_t* pos = nullptr, int base =
// 10, bool ignore_leading_white_space_characters = true), u16string
// to_u16string(float value)", "Testing correct functionality of global
// conversion functions: stof, to_u16string")
//{
//    size_t end_pos{};
//    // add code (function) for rounding to N decimal digits
//    auto const float_value { stof(u"7.53", &end_pos, 10) };
//
//    cout << "\nstof(u\"7.53\") = " << float_value << '\n' <<
//"end_pos = " << end_pos << '\n';
//
//    REQUIRE(float_value == 7.53);
//
//    const auto float_to_u16string{ to_u16string(float_value) };
//
//    REQUIRE(float_to_u16string == u"7.53");
//
//
//}

// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
//
// TEST_CASE("u16string to_u16string(const short value)", "Testing correct
// functionality of global conversion function: u16string to_u16string(const
// short value)")
//{
//  auto const result_u16string{ to_u16string(static_cast<short>(12345)) };
//
//  string result_string(cbegin(result_u16string), cend(result_u16string));
//
//  cout << "\nto_u16string(12345)=\"" << result_string << "\"\n";
//
//  REQUIRE(u16string{ u"12345" } ==
// to_u16string(static_cast<short>(12345)));
//
//  const auto float_to_u16string{ to_u16string(7.51735f) };
//
//  REQUIRE(u16string{ u"7.51735" } == float_to_u16string);
//
//  result_string.assign(cbegin(float_to_u16string),
// cend(float_to_u16string));
//
//  cout << "to_u16string(7.51735f)=\"" << result_string << "\"\n";
//
//
//}
