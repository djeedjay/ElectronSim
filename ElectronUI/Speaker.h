// (C) Copyright Gert-Jan de Vos 2021.

#pragma once

#include <atlcomcli.h>
#include <mmreg.h>
#include <dsound.h>

namespace DjeeDjay {

class Speaker
{
public:
	Speaker();

	double Amplitude() const;
	void Amplitude(double value);

	void Play(int frequency);
	void Stop();

private:
	CComPtr<IDirectSound> m_pIDirectSound;
	CComPtr<IDirectSoundBuffer> m_pIBuffer;
	DSBUFFERDESC m_dsbdesc;
	WAVEFORMATEX m_wfx;
	double m_amplitude;
};

} // namespace DjeeDjay
