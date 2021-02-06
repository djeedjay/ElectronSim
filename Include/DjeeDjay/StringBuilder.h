// (C) Copyright Gert-Jan de Vos 2021.

/*
 * StringBuilder.h
 *
 * Example:
 *
 *  f(const std::string&);
 *  g(const std::wstring&);
 *
 *	f(StringBuilder() << "Hello world! " << 12);
 *	g(WStringBuilder() << L"Hello world! " << 12);
 */

#pragma once

#include <string>
#include <sstream>

namespace DjeeDjay {

template <typename CharType, typename Traits = std::char_traits<CharType>, typename Allocator = std::allocator<CharType> >
class BasicStringBuilder
{
public:
	typedef typename std::basic_string<CharType, Traits, Allocator> string_type;

	template <typename T>
	BasicStringBuilder& operator<<(const T& t)
	{
		m_ss << t;
		return *this;
	}

	string_type str() const
	{
		return m_ss.str();
	}

	operator string_type() const
	{
		return str();
	}

private:
	std::basic_ostringstream<CharType, Traits, Allocator> m_ss;
};

typedef BasicStringBuilder<char> StringBuilder;
typedef BasicStringBuilder<wchar_t> WStringBuilder;

} // namespace DjeeDjay
