// (C) Copyright Gert-Jan de Vos 2021.

#define NOMINMAX
#include "DjeeDjay/Win32/ComApi.h"
#include "Speaker.h"

#pragma comment(lib, "dsound.lib")

namespace DjeeDjay {

Speaker::Speaker() :
	m_dsbdesc(),
	m_wfx(),
	m_amplitude(0.05)
{
	Win32::ThrowFailed(DirectSoundCreate(nullptr, &m_pIDirectSound, nullptr));
	Win32::ThrowFailed(m_pIDirectSound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_NORMAL));
		
	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
	m_wfx.nChannels = 1;
	m_wfx.wBitsPerSample = 16;
	m_wfx.nSamplesPerSec = 44100;
	m_wfx.nBlockAlign = m_wfx.nChannels * m_wfx.wBitsPerSample / 8;
	m_wfx.nAvgBytesPerSec = m_wfx.nSamplesPerSec * m_wfx.nBlockAlign;
	m_wfx.cbSize = 0;

	m_dsbdesc.dwSize = sizeof(m_dsbdesc);
	m_dsbdesc.dwFlags = DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLFREQUENCY | DSBCAPS_GLOBALFOCUS;
	m_dsbdesc.lpwfxFormat = &m_wfx;
}

double Speaker::Amplitude() const
{
	return m_amplitude;
}

void Speaker::Amplitude(double value)
{
	m_amplitude = value;
}

void Speaker::Play(int frequency)
{
	Stop();

	frequency = std::min(frequency, 32767);
	frequency = std::max(frequency, 37);

	// 1/frequency = period of wave, in seconds
	// SamplingRate / frequency = samples per wave-period
	int halfPeriod = std::max<int>(m_wfx.nSamplesPerSec * m_wfx.nChannels / (2 * frequency), 1);
	// The above line introduces roundoff error, which at higher
	// frequencies is significant (>30% at the 32kHz,
	// easily above 1% in general, not good). We will fix this below.

	m_dsbdesc.dwBufferBytes = 2 * halfPeriod * m_wfx.wBitsPerSample / 8;
	Win32::ThrowFailed(m_pIDirectSound->CreateSoundBuffer(&m_dsbdesc, &m_pIBuffer, nullptr));

	// Frequency adjustment to correct for the roundoff error above.
	DWORD playFrequency;
	Win32::ThrowFailed(m_pIBuffer->GetFrequency(&playFrequency));

	// When we set the half_period above, we rounded down, so if
	// we play the buffer as is, it will sound higher frequency
	// than it ought to be.
	// To compensate, we should play the buffer at a slower speed.
	// The slowdown factor is precisely the rounded-down period
	// divided by the true period:
	//   half_period / [ SamplingRate * Channels / (2*frequency) ]
	// = 2*frequency*half_period / (SamplingRate * Channels)
	//
	// The adjusted frequency needs to be multiplied by this factor:
	//   play_freq *= 2*frequency*half_period / (SamplingRate * Channels)
	// To do this computation (in a way that works on 32-bit machines),
	// we cannot multiply the numerator directly, since that may
	// cause rounding problems (44100 * 2*44100*1 ~ 3.9 billion which
	// is uncomfortable close to the upper limit of 4.3 billion).
	// Therefore, we use MulDiv to safely (and efficiently) avoid any
	// problems.
	playFrequency = MulDiv(playFrequency, 2 * frequency * halfPeriod, m_wfx.nSamplesPerSec * m_wfx.nChannels);
	Win32::ThrowFailed(m_pIBuffer->SetFrequency(playFrequency));

	void* pWrite;
	DWORD dwLen;
	Win32::ThrowFailed(m_pIBuffer->Lock(0, m_dsbdesc.dwBufferBytes, &pWrite, &dwLen, nullptr, nullptr, DSBLOCK_ENTIREBUFFER));
	auto p = static_cast<int16_t*>(pWrite);

	int16_t amplitude = static_cast<int16_t>(m_amplitude * ((1 << (m_wfx.wBitsPerSample - 1)) - 1));
	for (int i = 0; i < halfPeriod; ++i)
	{
		for (int j = 0; j < m_wfx.nChannels; ++j)
			*p++ = -amplitude;
	}
	for (int i = 0; i < halfPeriod; ++i)
	{
		for (int j = 0; j < m_wfx.nChannels; ++j)
			*p++ = +amplitude;
	}

	Win32::ThrowFailed(m_pIBuffer->Unlock(pWrite, dwLen, nullptr, 0));
	Win32::ThrowFailed(m_pIBuffer->SetCurrentPosition(0));
	Win32::ThrowFailed(m_pIBuffer->Play(0, 0, DSBPLAY_LOOPING));
}

void Speaker::Stop()
{
	if (m_pIBuffer)
	{
		m_pIBuffer->Stop();
		m_pIBuffer = nullptr;
	}
}

} // namespace DjeeDjay
