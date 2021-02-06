// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

namespace DjeeDjay {

template <typename T>
struct Identity
{
	typedef T type;

	T operator()(T value) const
	{
		return value;
	}
};

template <typename T>
using IdentityType = typename Identity<T>::type;

} // namespace DjeeDjay
