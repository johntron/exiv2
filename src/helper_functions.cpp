// ********************************************************* -*- C++ -*-
/*
 * Copyright (C) 2004-2018 Exiv2 authors
 * This program is part of the Exiv2 distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA.
 */
/*!
  @file    helper_functions.cpp
  @brief   A collection of helper functions
  @author  Dan Čermák (D4N)
           <a href="mailto:dan.cermak@cgc-instruments.com">dan.cermak@cgc-instruments.com</a>
  @date    25-May-18, D4N: created
 */

#include <config.h>

#include "helper_functions.hpp"
#include "unused.h"

#include <cassert>
#include <cstring>
#include <cwchar>

std::string string_from_unterminated(const char* data, size_t data_length)
{
    const size_t StringLength = strnlen(data, data_length);

    return std::string(data, StringLength);
}

std::string wideStr2Utf8(const std::wstring& wStr)
{
    std::mbstate_t state = std::mbstate_t();
    const wchar_t* wstr = wStr.c_str();

#ifdef EXV_HAVE_WCSRTOMBS_S
    size_t converted_length = 0;
    const errno_t err = wcsrtombs_s(&converted_length, nullptr, 0, &wstr, 0, &state);
#ifdef NDEBUG
    UNUSED(err);
#else
    assert(err == 0);
#endif

#else
    const std::size_t converted_length = std::wcsrtombs(nullptr, &wstr, 0, &state);
#endif
    // conversion failed => return an empty string
    if (converted_length == static_cast<std::size_t>(-1)) {
        return std::string("");
    }

    // create a string large enough to store the result + \0
    std::string result(converted_length + 1, 0);

#ifdef EXV_HAVE_WCSRTOMBS_S
    size_t actual_conversion_result = 0;
    const errno_t err_2 =
        wcsrtombs_s(&actual_conversion_result, &result[0], result.size(), &wstr, result.size(), &state);
#ifdef NDEBUG
    UNUSED(err);
#else
    assert(err_2 == 0);
#endif
#else
    const std::size_t actual_conversion_result = std::wcsrtombs(&result[0], &wstr, result.size(), &state);
#endif

#ifdef NDEBUG
    UNUSED(actual_conversion_result);
#else
    assert(actual_conversion_result == converted_length);
#endif

    return result;
}
