// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <Windows.h>

#include "MEGamepad.h"
#include <megp/Gamepad.h>
#include <me/game/IGame.h>


void DeleterGamepad( megp::Gamepad * gamepad )
{
	delete gamepad;
}

extern "C" __declspec(dllexport) bool MELoader( me::game::IGame * gameInstance, const qxml::Element * element );
__declspec(dllexport) bool MELoader( me::game::IGame * gameInstance, const qxml::Element * element )
{
	gameInstance->GetInputManager()->AddInputDevice( me::input::IInputDevice::ptr( new megp::Gamepad( gameInstance ), DeleterGamepad ) );

	return true;
}

