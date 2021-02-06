// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

/*
 * string_cast: string type conversions for all common Win32 and C++ string types.
 *
 * Example:
 *
 * void f(BSTR bstr)
 * {
 *     std::string s = string_cast<std::string>(bstr);
 * }
 *
 * Supported types:
 *                   From:
 * To:           char*   const char*   wchar_t*   const wchar_t*   BSTR   _bstr_t   CComBSTR
 * std::string    v           v           v            v            v        v         v
 * std::wstring   v           v           v            v            v        v         v
 * _bstr_t        v           v           v            v            v        v         v
 * CComBSTR       v           v           v            v            v        v         v
 * BSTR           v           v           v            v            v        v         v
 */

#include <string>

#if defined(_WIN32) && !defined(NO_WINDOWS)
#	include <comdef.h> // _bstr_t
#	include <atlcomcli.h> // CComBSTR
#endif // _WIN32

namespace DjeeDjay {

std::string Narrow(const wchar_t* p, size_t length);
std::string Narrow(const wchar_t* p);
std::string Narrow(const std::wstring& s);

std::wstring Widen(const char* p, size_t length);
std::wstring Widen(const char* p);
std::wstring Widen(const std::string& s);

template <typename Dst, typename Src>
struct string_cast_helper
{
	typedef Dst result_type;

	static Dst cast(const Src& src)
	{
		return static_cast<Dst>(src);
	}
};

template <typename Dst, typename Src>
typename string_cast_helper<Dst, Src>::result_type string_cast(const Src& src)
{
	return string_cast_helper<Dst, Src>::cast(src);
}

template <typename Src>
struct string_cast_helper<std::string, Src>
{
	typedef std::string result_type;

	template <typename T>
	static std::string cast(const T& src)
	{
		return static_cast<std::string>(src);
	}

	static std::string cast(const std::wstring& src)
	{
		return Narrow(src);
	}

	static std::string cast(const wchar_t* src)
	{
		return Narrow(src);
	}

	static std::string cast(wchar_t* src)
	{
		return Narrow(src);
	}

#if defined(_WIN32) && !defined(NO_WINDOWS)
	static std::string cast(const _bstr_t& src)
	{
		const wchar_t* p = src;
		return Narrow(p, src.length());
	}

	static std::string cast(const ATL::CComBSTR& src)
	{
		return Narrow(src.m_str, src.Length());
	}
#endif
};

template <typename Src>
struct string_cast_helper<std::wstring, Src>
{
	typedef std::wstring result_type;

	template <typename T>
	static std::wstring cast(const T& src)
	{
		return static_cast<std::wstring>(src);
	}

	static std::wstring cast(const std::string& src)
	{
		return Widen(src);
	}

	static std::wstring cast(const char* src)
	{
		return Widen(src);
	}

	static std::wstring cast(char* src)
	{
		return Widen(src);
	}

	static std::wstring cast(const wchar_t* src)
	{
		return src ? static_cast<std::wstring>(src) : std::wstring();
	}

	static std::wstring cast(wchar_t* src)
	{
		return cast(static_cast<const wchar_t*>(src));
	}

#if defined(_WIN32) && !defined(NO_WINDOWS)
	static std::wstring cast(const unsigned short* src)
	{
		// NULL BSTR's may represent empty strings, we'll allow that here:
		return cast(reinterpret_cast<const wchar_t*>(src));
	}

	static std::wstring cast(unsigned short* src)
	{
		return cast(static_cast<const unsigned short*>(src));
	}

	static std::wstring cast(const _bstr_t& src)
	{
		const char* p = src;
		return std::wstring(p, p + src.length());
	}

	static std::wstring cast(const ATL::CComBSTR& src)
	{
		return src.m_str ? static_cast<std::wstring>(src.m_str) : std::wstring();
	}
#endif
};

#if defined(_WIN32) && !defined(NO_WINDOWS)

template <typename Src>
struct string_cast_helper<ATL::CComBSTR, Src>
{
	typedef ATL::CComBSTR result_type;

	template <typename T>
	static ATL::CComBSTR cast(const T& src)
	{
		return static_cast<ATL::CComBSTR>(src);
	}

	static ATL::CComBSTR cast(const std::string& src)
	{
		return ATL::CComBSTR(int(src.size()), src.c_str());
	}

	static ATL::CComBSTR cast(const std::wstring& src)
	{
		return ATL::CComBSTR(int(src.size()), src.c_str());
	}

	static ATL::CComBSTR cast(const _bstr_t& src)
	{
		return ATL::CComBSTR(static_cast<BSTR>(src));
	}

	static ATL::CComBSTR cast(const __wchar_t* src)
	{
		return ATL::CComBSTR(reinterpret_cast<const wchar_t*>(src));
	}

	static ATL::CComBSTR cast(wchar_t* src)
	{
		return cast(static_cast<const wchar_t*>(src));
	}

	static ATL::CComBSTR cast(const unsigned short* src)
	{
		return ATL::CComBSTR(reinterpret_cast<const wchar_t*>(src));
	}

	static ATL::CComBSTR cast(unsigned short* src)
	{
		return cast(static_cast<const unsigned short*>(src));
	}
};

template <typename Src>
struct string_cast_helper<BSTR, Src> : string_cast_helper<ATL::CComBSTR, Src>
{
};

template <typename Src>
struct string_cast_helper<_bstr_t, Src>
{
	typedef _bstr_t result_type;

	template <typename T>
	static _bstr_t cast(const T& src)
	{
		return static_cast<_bstr_t>(src);
	}

	static _bstr_t cast(const std::string& src)
	{
		return _bstr_t(src.c_str());
	}

	static _bstr_t cast(const std::wstring& src)
	{
		return _bstr_t(src.c_str());
	}

	static _bstr_t cast(const __wchar_t* src)
	{
		return _bstr_t(reinterpret_cast<const wchar_t*>(src));
	}

	static _bstr_t cast(__wchar_t* src)
	{
		return cast(static_cast<const __wchar_t*>(src));
	}

	static _bstr_t cast(const unsigned short* src)
	{
		return _bstr_t(reinterpret_cast<const wchar_t*>(src));
	}

	static _bstr_t cast(unsigned short* src)
	{
		return cast(static_cast<const unsigned short*>(src));
	}
};

#endif // _WIN32

class WStr
{
public:
	explicit WStr(std::wstring str);
	explicit WStr(const std::string& str);

	operator const wchar_t*() const;
	const wchar_t* c_str() const;
	std::wstring str() const;

private:
	std::wstring m_str;
};

std::string AddCr(const std::string& s);
std::wstring AddCr(const std::wstring& s);

} // namespace DjeeDjay
