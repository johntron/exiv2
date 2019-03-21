#include <clocale>
#include <cstdint>
#include <system_error>

#include "enforce.hpp"
#include "helper_functions.hpp"

#include <gtest/gtest.h>

TEST(string_from_unterminated, terminatedArray)
{
    const char data[5] = {'a', 'b', 'c', 0, 'd'};
    const std::string res = string_from_unterminated(data, 5);

    ASSERT_EQ(res.size(), 3u);
    ASSERT_STREQ(res.c_str(), "abc");
}

TEST(string_from_unterminated, unterminatedArray)
{
    const char data[4] = {'a', 'b', 'c', 'd'};
    const std::string res = string_from_unterminated(data, 4);

    ASSERT_EQ(res.size(), 4u);
    ASSERT_STREQ(res.c_str(), "abcd");
}

struct WideStr2Utf8Test : public ::testing::Test
{
    WideStr2Utf8Test()
    {
        if (std::setlocale(LC_ALL, "en_US.UTF-8") == nullptr) {
            throw std::system_error(errno, std::system_category(), "Could not set locale to 'en_US.UTF-8'");
        }
    }

    ~WideStr2Utf8Test()
    {
        std::setlocale(LC_ALL, "C");
    }
};

TEST_F(WideStr2Utf8Test, asciiString)
{
    const std::wstring wStr{L"this is an ordinary ASCII string"};
    const std::string res = wideStr2Utf8(wStr);

    ASSERT_STREQ(res.c_str(), "this is an ordinary ASCII string");
}

TEST_F(WideStr2Utf8Test, UtfString)
{
    const std::wstring wStr{L"This String cöntains Special characters: zß水"};
    const std::string res = wideStr2Utf8(wStr);

    static const uint8_t expected_utf8[] = {
        0x54, 0x68, 0x69, 0x73, 0x20, 0x53, 0x74, 0x72, 0x69, 0x6E, 0x67, 0x20, 0x63, 0xC3, 0xB6, 0x6E, 0x74,
        0x61, 0x69, 0x6E, 0x73, 0x20, 0x53, 0x70, 0x65, 0x63, 0x69, 0x61, 0x6C, 0x20, 0x63, 0x68, 0x61, 0x72,
        0x61, 0x63, 0x74, 0x65, 0x72, 0x73, 0x3A, 0x20, 0x7A, 0xC3, 0x9F, 0xE6, 0xB0, 0xB4, 0x00};
    ASSERT_STREQ(res.c_str(), reinterpret_cast<const char*>(expected_utf8));
}

TEST_F(WideStr2Utf8Test, invalidUnicodeStringConversion)
{
    static const uint8_t invalid_unicode[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
    const std::wstring wStr(reinterpret_cast<const wchar_t*>(invalid_unicode));
    const std::string res = wideStr2Utf8(wStr);

    ASSERT_STREQ(res.c_str(), "");
}
