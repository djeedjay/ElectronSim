// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

namespace DjeeDjay {

class NonCopyable
{
protected:
	NonCopyable() = default;
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};

} // namespace DjeeDjay
