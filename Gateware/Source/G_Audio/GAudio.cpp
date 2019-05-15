#ifdef _WIN32
#include"GAudioWin.hpp"

#elif __linux__
#include"GAudioLinux.hpp"
#elif __APPLE__
#include "GAudioMac.hpp"
#endif


using namespace GW;
using namespace AUDIO;


GATEWARE_EXPORT_EXPLICIT GReturn CreateGAudio(GAudio** _outAudio)
{
	return GW::AUDIO::CreateGAudio(_outAudio);
}
GReturn GW::AUDIO::CreateGAudio(GAudio** _outAudio)
{
	GReturn result = FAILURE;
	if (_outAudio == nullptr)
	{
		result = INVALID_ARGUMENT;
		return result;
	}

	 result = PlatformGetAudio(_outAudio);

	return result;
}
GW::AUDIO::GAudio::~GAudio()
{

}
GW::AUDIO::GSound::~GSound()
{

}
GW::AUDIO::GMusic::~GMusic()
{

}
//End of GAudio implementation

