#include "Effect.h"

#include "Chorus.h"
#include "Compressor.h"
#include "Distortion.h"
#include "Echo.h"
#include "Flanger.h"
#include "Gargle.h"
#include "Equalizer.h"
#include "Reverb.h"

std::shared_ptr<Effect> Effect::create(EffectType type)
{
#define EFF_TYPE(t) case EffectType_##t: return std::make_shared<Effect##t>()
	switch (type)
	{
		EFF_TYPE(Chorus);
		EFF_TYPE(Compressor);
		EFF_TYPE(Distortion);
		EFF_TYPE(Echo);
		EFF_TYPE(Flanger);
		EFF_TYPE(Gargle);
		EFF_TYPE(Equalizer);
		EFF_TYPE(Reverb);
	}
#undef EFF_TYPE

	return nullptr;
}
