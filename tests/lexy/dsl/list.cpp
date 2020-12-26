// Copyright (C) 2020 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <lexy/dsl/list.hpp>

#include "verify.hpp"
#include <lexy/dsl/capture.hpp>
#include <lexy/dsl/label.hpp>
#include <lexy/dsl/option.hpp>
#include <lexy/dsl/sequence.hpp>

TEST_CASE("dsl::list()")
{
    static LEXY_VERIFY_FN auto rule
        = list(LEXY_LIT("ab") >> lexy::dsl::lit_c<'c'> + lexy::dsl::id<0>);
    CHECK(lexy::is_rule<decltype(rule)>);

    struct callback
    {
        const char* str;

        LEXY_VERIFY_FN auto list()
        {
            struct b
            {
                int count = 0;

                using return_type = int;

                LEXY_VERIFY_FN void operator()(lexy::id<0>)
                {
                    ++count;
                }

                LEXY_VERIFY_FN int finish() &&
                {
                    return count;
                }
            };
            return b{};
        }

        LEXY_VERIFY_FN int success(const char* cur, int count)
        {
            LEXY_VERIFY_CHECK(cur - str == 3 * count);
            return count;
        }

        LEXY_VERIFY_FN int error(test_error<lexy::expected_literal> e)
        {
            if (e.position() == str)
            {
                LEXY_VERIFY_CHECK(e.string() == "ab");
                return -1;
            }
            else
            {
                LEXY_VERIFY_CHECK(e.string() == "c");
                return -2;
            }
        }
    };

    auto empty = LEXY_VERIFY("");
    CHECK(empty == -1);

    auto one = LEXY_VERIFY("abc");
    CHECK(one == 1);
    auto two = LEXY_VERIFY("abcabc");
    CHECK(two == 2);
    auto three = LEXY_VERIFY("abcabcabc");
    CHECK(three == 3);

    auto condition_partial = LEXY_VERIFY("a");
    CHECK(condition_partial == -1);
    auto one_condition_partial = LEXY_VERIFY("abca");
    CHECK(one_condition_partial == 1);

    auto partial = LEXY_VERIFY("ab");
    CHECK(partial == -2);
    auto one_partial = LEXY_VERIFY("abcab");
    CHECK(one_partial == -2);
}

TEST_CASE("dsl::list() sep")
{
    static LEXY_VERIFY_FN auto rule = list(lexy::dsl::id<0> + LEXY_LIT("abc"), sep(LEXY_LIT(",")));
    CHECK(lexy::is_rule<decltype(rule)>);

    struct callback
    {
        const char* str;

        LEXY_VERIFY_FN auto list()
        {
            struct b
            {
                int count = 0;

                using return_type = int;

                LEXY_VERIFY_FN void operator()(lexy::id<0>)
                {
                    ++count;
                }

                LEXY_VERIFY_FN int finish() &&
                {
                    return count;
                }
            };
            return b{};
        }

        LEXY_VERIFY_FN int success(const char* cur, int count)
        {
            LEXY_VERIFY_CHECK(cur - str == 4 * count - 1);
            return count;
        }

        LEXY_VERIFY_FN int error(test_error<lexy::expected_literal> e)
        {
            LEXY_VERIFY_CHECK(e.string() == "abc");
            return -1;
        }
    };

    auto empty = LEXY_VERIFY("");
    CHECK(empty == -1);

    auto one = LEXY_VERIFY("abc");
    CHECK(one == 1);
    auto two = LEXY_VERIFY("abc,abc");
    CHECK(two == 2);
    auto three = LEXY_VERIFY("abc,abc,abc");
    CHECK(three == 3);

    auto partial = LEXY_VERIFY("ab");
    CHECK(partial == -1);
    auto one_partial = LEXY_VERIFY("abc,ab");
    CHECK(one_partial == -1);

    auto no_sep = LEXY_VERIFY("abcabc");
    CHECK(no_sep == 1);
}

TEST_CASE("dsl::list() sep capture")
{
    static constexpr auto rule
        = list(lexy::dsl::id<0> + LEXY_LIT("abc"), sep(capture(LEXY_LIT(","))));
    CHECK(lexy::is_rule<decltype(rule)>);

    struct callback
    {
        const char* str;

        LEXY_VERIFY_FN auto list()
        {
            struct b
            {
                int count = 0;

                using return_type = int;

                LEXY_VERIFY_FN void operator()(lexy::id<0>)
                {
                    count += 3;
                }
                LEXY_VERIFY_FN void operator()(lexy::lexeme_for<test_input> lex)
                {
                    LEXY_VERIFY_CHECK(lex.size() == 1);
                    ++count;
                }

                LEXY_VERIFY_FN int finish() &&
                {
                    return count;
                }
            };
            return b{};
        }

        LEXY_VERIFY_FN int success(const char* cur, int count)
        {
            LEXY_VERIFY_CHECK(cur - str == count);
            return count;
        }

        LEXY_VERIFY_FN int error(test_error<lexy::expected_literal> e)
        {
            LEXY_VERIFY_CHECK(e.string() == "abc");
            return -1;
        }
    };

    auto empty = LEXY_VERIFY("");
    CHECK(empty == -1);

    auto one = LEXY_VERIFY("abc");
    CHECK(one == 3);
    auto two = LEXY_VERIFY("abc,abc");
    CHECK(two == 7);
    auto three = LEXY_VERIFY("abc,abc,abc");
    CHECK(three == 11);

    auto partial = LEXY_VERIFY("ab");
    CHECK(partial == -1);
    auto one_partial = LEXY_VERIFY("abc,ab");
    CHECK(one_partial == -1);

    auto no_sep = LEXY_VERIFY("abcabc");
    CHECK(no_sep == 3);
}

TEST_CASE("dsl::list() trailing_sep")
{
    static constexpr auto rule
        = list(LEXY_LIT("abc") >> lexy::dsl::id<0>, trailing_sep(LEXY_LIT(",")));
    CHECK(lexy::is_rule<decltype(rule)>);

    struct callback
    {
        const char* str;

        LEXY_VERIFY_FN auto list()
        {
            struct b
            {
                int count = 0;

                using return_type = int;

                LEXY_VERIFY_FN void operator()(lexy::id<0>)
                {
                    ++count;
                }

                LEXY_VERIFY_FN int finish() &&
                {
                    return count;
                }
            };
            return b{};
        }

        LEXY_VERIFY_FN int success(const char* cur, int count)
        {
            if (cur[-1] == ',')
                LEXY_VERIFY_CHECK(cur - str == 4 * count);
            else
                LEXY_VERIFY_CHECK(cur - str == 4 * count - 1);
            return count;
        }

        LEXY_VERIFY_FN int error(test_error<lexy::expected_literal> e)
        {
            LEXY_VERIFY_CHECK(e.position() == str);
            LEXY_VERIFY_CHECK(e.string() == "abc");
            return -1;
        }
    };

    auto empty = LEXY_VERIFY("");
    CHECK(empty == -1);
    auto partial = LEXY_VERIFY("ab");
    CHECK(partial == -1);

    auto one = LEXY_VERIFY("abc");
    CHECK(one == 1);
    auto two = LEXY_VERIFY("abc,abc");
    CHECK(two == 2);
    auto three = LEXY_VERIFY("abc,abc,abc");
    CHECK(three == 3);

    auto no_sep = LEXY_VERIFY("abcabc");
    CHECK(no_sep == 1);

    auto trailing = LEXY_VERIFY("abc,");
    CHECK(trailing == 1);
}

TEST_CASE("dsl::list() trailing_sep capture")
{
    static constexpr auto rule
        = list(LEXY_LIT("abc") >> lexy::dsl::id<0>, trailing_sep(capture(LEXY_LIT(","))));
    CHECK(lexy::is_rule<decltype(rule)>);

    struct callback
    {
        const char* str;

        LEXY_VERIFY_FN auto list()
        {
            struct b
            {
                int count = 0;

                using return_type = int;

                LEXY_VERIFY_FN void operator()(lexy::id<0>)
                {
                    count += 3;
                }
                LEXY_VERIFY_FN void operator()(lexy::lexeme_for<test_input> lex)
                {
                    LEXY_VERIFY_CHECK(lex.size() == 1);
                    ++count;
                }

                LEXY_VERIFY_FN int finish() &&
                {
                    return count;
                }
            };
            return b{};
        }

        LEXY_VERIFY_FN int success(const char* cur, int count)
        {
            LEXY_VERIFY_CHECK(cur - str == count);
            return count;
        }

        LEXY_VERIFY_FN int error(test_error<lexy::expected_literal> e)
        {
            LEXY_VERIFY_CHECK(e.position() == str);
            LEXY_VERIFY_CHECK(e.string() == "abc");
            return -1;
        }
    };

    auto empty = LEXY_VERIFY("");
    CHECK(empty == -1);
    auto partial = LEXY_VERIFY("ab");
    CHECK(partial == -1);

    auto one = LEXY_VERIFY("abc");
    CHECK(one == 3);
    auto two = LEXY_VERIFY("abc,abc");
    CHECK(two == 7);
    auto three = LEXY_VERIFY("abc,abc,abc");
    CHECK(three == 11);

    auto no_sep = LEXY_VERIFY("abcabc");
    CHECK(no_sep == 3);

    auto trailing = LEXY_VERIFY("abc,");
    CHECK(trailing == 4);
}

TEST_CASE("dsl::opt(list())")
{
    static constexpr auto rule = opt(list(LEXY_LIT("abc") >> lexy::dsl::id<0>));
    CHECK(lexy::is_rule<decltype(rule)>);

    struct callback
    {
        const char* str;

        LEXY_VERIFY_FN auto list()
        {
            struct b
            {
                int count = 0;

                using return_type = int;

                LEXY_VERIFY_FN void operator()(lexy::id<0>)
                {
                    ++count;
                }

                LEXY_VERIFY_FN int finish() &&
                {
                    return count;
                }
            };
            return b{};
        }

        LEXY_VERIFY_FN int success(const char* cur)
        {
            LEXY_VERIFY_CHECK(cur == str);
            return 0;
        }
        LEXY_VERIFY_FN int success(const char* cur, int count)
        {
            LEXY_VERIFY_CHECK(cur - str == 3 * count);
            return count;
        }

        LEXY_VERIFY_FN int error(test_error<lexy::expected_literal> e)
        {
            LEXY_VERIFY_CHECK(e.position() == str);
            LEXY_VERIFY_CHECK(e.string() == "abc");
            return -1;
        }
    };

    auto empty = LEXY_VERIFY("");
    CHECK(empty == 0);
    auto partial = LEXY_VERIFY("ab");
    CHECK(partial == 0);

    auto one = LEXY_VERIFY("abc");
    CHECK(one == 1);
    auto two = LEXY_VERIFY("abcabc");
    CHECK(two == 2);
    auto three = LEXY_VERIFY("abcabcabc");
    CHECK(three == 3);
}

TEST_CASE("dsl::opt(list()) sep")
{
    static constexpr auto rule = opt(list(LEXY_LIT("abc") >> lexy::dsl::id<0>, sep(LEXY_LIT(","))));
    CHECK(lexy::is_rule<decltype(rule)>);

    struct callback
    {
        const char* str;

        LEXY_VERIFY_FN auto list()
        {
            struct b
            {
                int count = 0;

                using return_type = int;

                LEXY_VERIFY_FN void operator()(lexy::id<0>)
                {
                    ++count;
                }

                LEXY_VERIFY_FN int finish() &&
                {
                    return count;
                }
            };
            return b{};
        }

        LEXY_VERIFY_FN int success(const char* cur, int count)
        {
            if (count == 0)
                LEXY_VERIFY_CHECK(cur == str);
            else
                LEXY_VERIFY_CHECK(cur - str == 4 * count - 1);
            return count;
        }

        LEXY_VERIFY_FN int error(test_error<lexy::expected_literal> e)
        {
            LEXY_VERIFY_CHECK(e.position() == str);
            LEXY_VERIFY_CHECK(e.string() == "abc");
            return -1;
        }
    };

    auto empty = LEXY_VERIFY("");
    CHECK(empty == 0);
    auto partial = LEXY_VERIFY("ab");
    CHECK(partial == 0);

    auto one = LEXY_VERIFY("abc");
    CHECK(one == 1);
    auto two = LEXY_VERIFY("abc,abc");
    CHECK(two == 2);
    auto three = LEXY_VERIFY("abc,abc,abc");
    CHECK(three == 3);

    auto no_sep = LEXY_VERIFY("abcabc");
    CHECK(no_sep == 1);
}

TEST_CASE("dsl::opt(list()) trailing_sep")
{
    static constexpr auto rule
        = opt(list(LEXY_LIT("abc") >> lexy::dsl::id<0>, trailing_sep(LEXY_LIT(","))));
    CHECK(lexy::is_rule<decltype(rule)>);

    struct callback
    {
        const char* str;

        LEXY_VERIFY_FN auto list()
        {
            struct b
            {
                int count = 0;

                using return_type = int;

                LEXY_VERIFY_FN void operator()(lexy::id<0>)
                {
                    ++count;
                }

                LEXY_VERIFY_FN int finish() &&
                {
                    return count;
                }
            };
            return b{};
        }

        LEXY_VERIFY_FN int success(const char* cur, int count)
        {
            if (count == 0)
                LEXY_VERIFY_CHECK(cur == str);
            else if (cur[-1] == ',')
                LEXY_VERIFY_CHECK(cur - str == 4 * count);
            else
                LEXY_VERIFY_CHECK(cur - str == 4 * count - 1);
            return count;
        }

        LEXY_VERIFY_FN int error(test_error<lexy::expected_literal> e)
        {
            LEXY_VERIFY_CHECK(e.position() == str);
            LEXY_VERIFY_CHECK(e.string() == "abc");
            return -1;
        }
    };

    auto empty = LEXY_VERIFY("");
    CHECK(empty == 0);
    auto partial = LEXY_VERIFY("ab");
    CHECK(partial == 0);

    auto one = LEXY_VERIFY("abc");
    CHECK(one == 1);
    auto two = LEXY_VERIFY("abc,abc");
    CHECK(two == 2);
    auto three = LEXY_VERIFY("abc,abc,abc");
    CHECK(three == 3);

    auto no_sep = LEXY_VERIFY("abcabc");
    CHECK(no_sep == 1);

    auto trailing = LEXY_VERIFY("abc,");
    CHECK(trailing == 1);
}

