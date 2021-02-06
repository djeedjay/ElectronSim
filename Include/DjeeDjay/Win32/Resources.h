// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <string>

namespace DjeeDjay {
namespace Win32 {

struct ModuleVersion
{
	int ver1, ver2, ver3, ver4;
};

struct FileVersion
{
	int major, minor, patch, revision;
};

std::ostream& operator<<(std::ostream& os, const FileVersion& v);
bool operator==(const FileVersion& a, const FileVersion& b);
bool operator!=(const FileVersion& a, const FileVersion& b);
bool operator<(const FileVersion& a, const FileVersion& b);
bool operator>(const FileVersion& a, const FileVersion& b);
bool operator<=(const FileVersion& a, const FileVersion& b);
bool operator>=(const FileVersion& a, const FileVersion& b);

std::wstring LoadString(HINSTANCE hInstance, unsigned id);
std::wstring LoadString(unsigned id);

} // namespace Win32
} // namespace DjeeDjay
