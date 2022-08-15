
#include "../debug.h"
#include "patches.h"

#include "Client62.h"
#include "Titanium.h"
#include "Anniversary.h"
#include "Underfoot.h"
#ifndef LOGINCRYPTO
#include "SoF.h"
#include "SoD.h"
#include "HoT.h"
#endif

void RegisterAllPatches(EQStreamIdentifier &into) {
	Client62::Register(into);
	Titanium::Register(into);
#ifndef LOGINCRYPTO
	SoF::Register(into);
	SoD::Register(into);
	Underfoot::Register(into);
	HoT::Register(into);
#endif
}

void ReloadAllPatches() {
	Client62::Reload();
	Titanium::Reload();
#ifndef LOGINCRYPTO
	SoF::Reload();
	SoD::Reload();
	Underfoot::Reload();
	HoT::Reload();
#endif
}



















