// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <string>
#include <type_traits>

namespace DjeeDjay {
namespace Detail {

std::string ToLowerHexString(unsigned long long value, int width);
std::wstring ToLowerHexWString(unsigned long long value, int width);
std::string ToUpperHexString(unsigned long long value, int width);
std::wstring ToUpperHexWString(unsigned long long value, int width);

} // namespace Detail

template <typename T>
std::string ToLowerHexString(T value, int width = 2 * sizeof(T))
{
	return Detail::ToLowerHexString(std::make_unsigned_t<T>(value), width);
}

template <typename T>
std::string ToUpperHexString(T value, int width = 2 * sizeof(T))
{
	return Detail::ToUpperHexString(std::make_unsigned_t<T>(value), width);
}

template <typename T>
std::wstring ToLowerHexWString(T value, int width = 2 * sizeof(T))
{
	return Detail::ToLowerHexWString(std::make_unsigned_t<T>(value), width);
}

template <typename T>
std::wstring ToUpperHexWString(T value, int width = 2 * sizeof(T))
{
	return Detail::ToUpperHexWString(std::make_unsigned_t<T>(value), width);
}

template <typename T>
std::string ToHexString(T value, int width = 2 * sizeof(T))
{
	return ToLowerHexString(std::make_unsigned_t<T>(value), width);
}

template <typename T>
std::wstring ToHexWString(T value, int width = 2 * sizeof(T))
{
	return ToLowerHexWString(std::make_unsigned_t<T>(value), width);
}

} // namespace DjeeDjay
