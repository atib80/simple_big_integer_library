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
        {number_base::binary,      2},
        {number_base::octal,       8},
        {number_base::decimal,     10},
        {number_base::hexadecimal, 16}
};

static unordered_map<number_base, const char *> number_base_name{
        {number_base::binary,      "binary"},
        {number_base::octal,       "octal"},
        {number_base::decimal,     "decimal"},
        {number_base::hexadecimal, "hexadecimal"}
};

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

int64_t get_random_integral_value(const int64_t lower_bound = min_value_int64_t,
                                  const int64_t upper_bound = max_value_int64_t) {
    return uniform_int_distribution<int64_t>{lower_bound,
                                             upper_bound}(rand_engine);
}

double get_random_floating_point_value(const double lower_bound = min_value_double,
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

static constexpr const char *number_base_digits{"0123456789ABCDEF"};

std::string dec2bin(const int64_t number) {

    if (0 == number) return "0b0";

    std::string binary_string{};
    binary_string.reserve(65U + (number < 0 ? 1U : 0U));

    if (number < 0) binary_string.push_back('-');
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

    if (0 == number) return "0o0";

    std::string octal_string{};
    octal_string.reserve(65U + (number < 0 ? 1U : 0U));

    if (number < 0) octal_string.push_back('-');
    octal_string.append("0o");

    const uint64_t abs_number = std::abs(number);

    uint64_t bit_mask{0x7000000000000000ULL};

    bool found_first_non_zero_digit{};
    size_t shift_count{60U};

    while (bit_mask != 0U) {

        const uint64_t and_result = (abs_number & bit_mask) >> shift_count;
        if ((and_result != 0U) || found_first_non_zero_digit) {
            assert(and_result < 8U);
            octal_string.push_back(number_base_digits[and_result]);
            found_first_non_zero_digit = true;
        }
        bit_mask >>= 3U;
        shift_count -= 3U;
    }

    return octal_string;
}

std::string dec2hex(int64_t number) {

    if (0 == number) return "0x0";

    std::string hexadecimal_string{};
    hexadecimal_string.reserve(65U + (number < 0 ? 1U : 0U));

    if (number < 0) hexadecimal_string.push_back('-');
    hexadecimal_string.append("0x");

    const uint64_t abs_number = std::abs(number);

    uint64_t bit_mask{0xF000000000000000ULL};

    bool found_first_non_zero_digit{};
    size_t shift_count{60U};

    while (bit_mask != 0U) {

        const uint64_t and_result = (abs_number & bit_mask) >> shift_count;

        if ((and_result != 0U) || found_first_non_zero_digit) {

            assert(and_result < 16U);
            hexadecimal_string.push_back(number_base_digits[and_result]);
            found_first_non_zero_digit = true;
        }
        bit_mask >>= 4U;
        shift_count -= 4U;
    }

    return hexadecimal_string;
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
            get_random_integral_value(min_value_int64_t, max_value_int64_t)
    };
    org::atib::numerics::big_integer bi9{random_number1};
    const auto &bi9_dec_str{bi9.get_decimal_number()};
    const auto random_number1_dec_str{std::to_string(random_number1)};
    REQUIRE(bi9_dec_str == random_number1_dec_str);

    const int64_t random_number2{
            get_random_integral_value(min_value_int64_t, max_value_int64_t)
    };
    org::atib::numerics::big_integer bi10{random_number2};
    const auto &bi10_dec_str{bi10.get_decimal_number()};
    const auto random_number2_dec_str{std::to_string(random_number2)};
    REQUIRE(bi10_dec_str == random_number2_dec_str);

    const int64_t random_number3{
            get_random_integral_value(min_value_int64_t, max_value_int64_t)
    };
    org::atib::numerics::big_integer bi11{random_number3};
    const auto &bi11_dec_str{bi11.get_decimal_number()};
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
            get_random_integral_value(min_value_int64_t, max_value_int64_t)
    };
    org::atib::numerics::big_integer bi1{random_number1};
    REQUIRE(bi1.get_decimal_number() == std::to_string(random_number1));

    const double random_number2{
            get_random_floating_point_value(min_value_double, max_value_double)
    };
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
                static_cast<size_t>(get_random_integral_value(5, 10))
        };
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
        const auto &bi_decimal_number_str{bi.get_decimal_number()};
        REQUIRE(bi_decimal_number_str == number_str);
    }
}

TEST_CASE("explicit big_integer(std::vector<bool>)",
          "Testing big_integer(std::vector<bool>) constructor") {
    org::atib::numerics::big_integer bi1{
            std::vector<bool>{true, false, true, false}
    };

    REQUIRE(bi1.get_binary_number() == "0b1010");
    REQUIRE(bi1.get_decimal_number() == "10");
    REQUIRE(bi1.get_octal_number() == "0o12");
    REQUIRE(bi1.get_hexadecimal_number() == "0xA");

    org::atib::numerics::big_integer bi2{
            std::vector<bool>{true, true, true, true, false, true, false}
    };

    REQUIRE(bi2.get_binary_number() == "0b1111010");
    REQUIRE(bi2.get_decimal_number() == "122");
    REQUIRE(bi2.get_octal_number() == "0o172");
    REQUIRE(bi2.get_hexadecimal_number() == "0x7A");

    org::atib::numerics::big_integer bi3{
            std::vector<bool>{false, true, true, true, true, false, true, false}
    };

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
            const bool binary_digit =
                    static_cast<bool>(get_random_digit_for_specified_number_base(
                            number_base::binary, false));
            digits.emplace_back(binary_digit);
            value <<= 1U;
            value += binary_digit;
        }

        const std::string number_str{std::to_string(value)};

        org::atib::numerics::big_integer bi{std::move(digits)};
        const auto &bi_decimal_number_str{bi.get_decimal_number()};
        REQUIRE(bi_decimal_number_str == number_str);
    }
}

TEST_CASE("void reset(const std::string&)",
          "Testing the correct functionality of void reset(const str::string& number") {
    for (size_t i{}; i < 10; ++i) {
        const size_t digit_count{
                static_cast<size_t>(get_random_integral_value(10, 30))
        };
        std::string number{};
        number.reserve(digit_count + 3);

        const number_base base = get_random_number_base();

        if (0 == get_random_digit_for_specified_number_base(number_base::binary, false))
            number.push_back('-');

        number.append(big_integer::number_base_prefix_.at(base));

        int first_digit{};
        do {
            first_digit = get_random_digit_for_specified_number_base(base, false);
        } while (first_digit == 0);

        number.push_back(number_base_digits[first_digit]);

        for (size_t j{1}; j < digit_count; ++j) {

            number.push_back(number_base_digits[get_random_digit_for_specified_number_base(base, false)]);
        }

        const int64_t random_number{get_random_integral_value(min_value_int64_t, max_value_int64_t)};
        const string random_number_str{std::to_string(random_number)};

        org::atib::numerics::big_integer bi1{random_number};

        REQUIRE(bi1.get_decimal_number() == random_number_str);

        bi1.reset(number);

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
                const auto &bi1_decimal_number{bi1.get_decimal_number()};
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

TEST_CASE("number_base::get_number_base() const noexcept",
          "Testing number_base big_integer::get_number_base() public method") {

    const size_t number_of_tests{10U};
    for (size_t i{}; i < number_of_tests; ++i) {
        const size_t digit_count{
                static_cast<size_t>(get_random_integral_value(5, 50))
        };
        std::string number{};
        number.reserve(digit_count + 3);

        const number_base base = get_random_number_base();

        if (0 == get_random_digit_for_specified_number_base(number_base::binary, false))
            number.push_back('-');

        number.append(big_integer::number_base_prefix_.at(base));

        int first_digit{};
        do {
            first_digit = get_random_digit_for_specified_number_base(base, false);
        } while (first_digit == 0);

        number.push_back(number_base_digits[first_digit]);

        for (size_t j{1}; j < digit_count; ++j) {

            number.push_back(number_base_digits[get_random_digit_for_specified_number_base(base, false)]);
        }

        org::atib::numerics::big_integer bi{number};

        REQUIRE(bi.get_number_base() == base);
    }
}

TEST_CASE("string big_integer::get_big_integer() const",
          "Testing string big_integer::get_big_integer() const public method") {
    const size_t number_of_tests{10U};
    for (size_t i{}; i < number_of_tests; ++i) {
        const int64_t random_number{get_random_integral_value()};

        org::atib::numerics::big_integer bi{random_number};

        REQUIRE(bi.get_big_integer(number_base::decimal) == std::to_string(random_number));
        REQUIRE(bi.get_big_integer(number_base::binary) == dec2bin(random_number));
        REQUIRE(bi.get_big_integer(number_base::octal) == dec2oct(random_number));
        REQUIRE(bi.get_big_integer(number_base::hexadecimal) == dec2hex(random_number));
    }
}

TEST_CASE("big_integer::operator std::string() const noexcept",
          "Testing big_integer's explicit operator std::string() const noexcept public method") {
    const size_t number_of_tests{10U};
    for (size_t i{}; i < number_of_tests; ++i) {
        const int64_t random_number{get_random_integral_value()};

        org::atib::numerics::big_integer bi{random_number};

        const std::string bi_decimal_number1 = static_cast<std::string>(bi);
        const std::string bi_decimal_number2{bi};

        REQUIRE(bi_decimal_number1 == std::to_string(random_number));
        REQUIRE(bi_decimal_number2 == std::to_string(random_number));
    }
}

TEST_CASE("big_integer::operator const char *() const noexcept",
          "Testing big_integer's explicit operator const char *() const noexcept public method") {
    const size_t number_of_tests{10U};
    for (size_t i{}; i < number_of_tests; ++i) {
        const int64_t random_number{get_random_integral_value()};

        org::atib::numerics::big_integer bi{random_number};

        const char *bi_decimal_number1 = static_cast<const char *>(bi);
        const char *bi_decimal_number2{bi};
        const std::string random_number_str{std::to_string(random_number)};

        REQUIRE(random_number_str == bi_decimal_number1);
        REQUIRE(random_number_str == bi_decimal_number2);
    }
}

TEST_CASE("big_integer::operator-() const", "Testing big_integer's operator-() const public method") {
    const size_t number_of_tests{10U};
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
    }
}


TEST_CASE("big_integer operator*(const big_integer&, const big_integer&)",
          "Testing the correct functionality of the globally defined "
          "multiplication operator carried out on the "
          "2 provided big_integer arguments.") {
    org::atib::numerics::big_integer lhs{"123"};
    org::atib::numerics::big_integer rhs{"255"};
    org::atib::numerics::big_integer product{lhs * rhs};
    REQUIRE(product.get_decimal_number() == "31365"s);

    org::atib::numerics::big_integer bi1{"361231416"};
    org::atib::numerics::big_integer bi2{"12313198"};
    product = bi1 * bi2;
    REQUIRE(product.get_decimal_number() == "4447913949028368"s);
}
