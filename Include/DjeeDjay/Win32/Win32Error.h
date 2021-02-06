// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <stdexcept>
#include <string>

namespace DjeeDjay {
namespace Win32 {

class Win32Error : public std::runtime_error
{
public:
	Win32Error(unsigned long rc, const char* msg);
	Win32Error(unsigned long rc, const std::string& msg);

	unsigned long GetError() const;
	virtual const char* what() const noexcept override;

private:
	unsigned long m_rc;
	mutable std::string m_what;
};

std::string GetErrorMessage(unsigned long rc);
std::string GetErrorMessage(long rc);

void ThrowLastError(const char* msg);
void ThrowLastError(const std::string& msg);

} // namespace Win32
} // namespace DjeeDjay
