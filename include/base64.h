/**
 *
 * @author Anders Lind (96395432+imgibson@users.noreply.github.com)
 * @date 2024-06-10
 *
 * Copyright (c) 2024 Anders Lind (https://github.com/imgibson). All rights reserved.
 *
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <exception>
#include <tuple>

namespace base64 {

static std::size_t encoded_length(const void* buffer, std::size_t length) noexcept {
#ifdef _DEBUG
    assert(buffer);
#endif
    std::ignore = buffer;
    return (length + 2) / 3 * 4;
}

static std::size_t encode(const void* buffer, std::size_t length, char (&result)[N]) noexcept {
#ifdef _DEBUG
    assert(buffer);
#endif
    static_assert(N > 0);
    const char kCharMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::size_t size = encoded_length(buffer, length);
    if (size >= N) {
        return size;
    }
    size = 0;
    const char* bufptr = static_cast<const char*>(buffer);
    while (length >= 3) {
        result[size++] = kCharMap[(bufptr[0] & 0xfc) >> 2];
        result[size++] = kCharMap[(bufptr[0] & 0x03) << 4 | (bufptr[1] & 0xf0) >> 4];
        result[size++] = kCharMap[(bufptr[1] & 0x0f) << 2 | (bufptr[2] & 0xc0) >> 6];
        result[size++] = kCharMap[(bufptr[2] & 0x3f)];
        bufptr += 3;
        length -= 3;
    };
    if (length >= 2) {
        result[size++] = kCharMap[(bufptr[0] & 0xfc) >> 2];
        result[size++] = kCharMap[(bufptr[0] & 0x03) << 4 | (bufptr[1] & 0xf0) >> 4];
        result[size++] = kCharMap[(bufptr[1] & 0x0f) << 2];
        result[size++] = '=';
    } else if (length >= 1) {
        result[size++] = kCharMap[(bufptr[0] & 0xfc) >> 2];
        result[size++] = kCharMap[(bufptr[0] & 0x03) << 4];
        result[size++] = '=';
        result[size++] = '=';
    }
    result[size] = '\0';
    return size;
}

static std::size_t decoded_length(const char* str, std::size_t length) {
#ifdef _DEBUG
    assert(str);
#endif
    if (length == 0) {
        return 0;
    }
    if (length % 4 != 0) {
        throw std::exception();
    }
    const auto is_valid = [](char c) -> bool {
        return std::strchr("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/", c) != nullptr;
    };
    const auto is_trail = [](char c) -> bool {
        return c == '=';
    };
    std::size_t i = 0;
    while (i < length && is_valid(str[i])) {
        ++i;
    };
    std::size_t j = i;
    while (j < length && is_trail(str[j])) {
        ++j;
    };
    if (j != length || (j - i) > 2) {
        throw std::exception();
    }
    return length / 4 * 3 - (j - i);
}

template <std::size_t N>
static std::size_t decode(const char* str, std::size_t length, char (&result)[N]) {
#ifdef _DEBUG
    assert(str);
#endif
    static_assert(N > 0);
    const auto strmap = [](char c) -> char {
        const char kCharMap[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        const char* pos = std::strchr(kCharMap, c);
        if (!pos) {
            throw std::exception();
        }
        return static_cast<char>(pos - kCharMap);
    };
    std::size_t size = 0;
    const auto append = [&](char c) -> void {
        if (size >= N) {
            throw std::exception();
        }
        result[size++] = c;
    };
    while (length > 4) {
        char buffer[4];
        for (std::size_t i = 0; i < 4; ++i) {
            buffer[i] = strmap(*str++);
        }
        append(buffer[0] << 2 | buffer[1] >> 4);
        append(buffer[1] << 4 | buffer[2] >> 2);
        append(buffer[2] << 6 | buffer[3]);
        length -= 4;
    };
    if (length >= 2) {
        while (length > 2 && str[length - 1] == '=') {
            --length;
        };
        char buffer[4]{};
        for (std::size_t i = 0; i < length; ++i) {
            buffer[i] = strmap(*str++);
        }
        if (length >= 4) {
            append(buffer[0] << 2 | buffer[1] >> 4);
            append(buffer[1] << 4 | buffer[2] >> 2)
            append(buffer[2] << 6 | buffer[3]);
        } else if (length >= 3) {
            append(buffer[0] << 2 | buffer[1] >> 4);
            append(buffer[1] << 4 | buffer[2] >> 2);
        } else {
            append(buffer[0] << 2 | buffer[1] >> 4);
        }
    } else {
        throw std::exception();
    }
    return size;
}

static std::size_t decoded_length(const char* str) {
#ifdef _DEBUG
    assert(str);
#endif
    return decoded_length(str, std::strlen(str));
}

template <std::size_t N>
static std::size_t decode(const char* str, char (&result)[N]) {
#ifdef _DEBUG
    assert(str);
#endif
    return decode(str, std::strlen(str), result);
}

} // namespace base64
