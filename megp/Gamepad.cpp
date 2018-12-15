// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <Windows.h>

#include <megp/Gamepad.h>

#pragma comment( lib, "Xinput9_1_0.lib" )

using namespace megp;
using namespace me;
using namespace input;

std::vector< std::string > g_ButtonNames =
{
	"DPadUp",
	"DPadDown",
	"DPadLeft",
	"DPadRight",
	"Start",
	"Back",
	"LeftThumb",
	"Righthumb",
	"LeftShoulder",
	"RightShoulder",
	"A",
	"B",
	"X",
	"Y"
};

std::vector< WORD > g_ButtonCodes =
{
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y
};

std::vector< std::string > g_TriggerNames =
{
	"LeftTrigger",
	"RightTrigger"
};

std::vector< std::string > g_StickNames =
{
	"LeftStick",
	"RightStick"
};

Gamepad::Gamepad( me::game::IGame * gameInstance )
	: m_game( gameInstance )
{
	size_t index = 0;
	for ( auto && name : g_ButtonNames )
	{
		m_nameToIndex[ name ] = index++;
	}
	for ( auto && name : g_TriggerNames )
	{
		m_nameToIndex[ name ] = index++;
	}
	for ( auto && name : g_StickNames )
	{
		m_nameToIndex[ name ] = index++;
	}
}

Gamepad::~Gamepad()
{
}

std::string Gamepad::GetName() const
{
	return "Gamepad";
}

void Gamepad::Update()
{
	m_prevStates = m_states;
	m_states.clear();
	for ( DWORD i = 0; i < XUSER_MAX_COUNT; ++i )
	{
		XINPUT_STATE state{};
		if ( XInputGetState( i, &state ) == ERROR_SUCCESS )
		{	  
			m_states[i] = state;
		}
	}
}

size_t Gamepad::SubSourceCount() const
{
	return m_states.size();
}

size_t Gamepad::GetInputCount( size_t subSource ) const
{
	return g_ButtonNames.size() + g_TriggerNames.size() + g_StickNames.size();
}

std::string Gamepad::GetInputName( size_t subSource, size_t index ) const
{
	if ( index < (g_ButtonNames.size()) )
	{
		return g_ButtonNames[ index ];
	}
	else if ( index < (g_ButtonNames.size() + g_TriggerNames.size()) )
	{
		return g_TriggerNames[ index - g_ButtonNames.size() ];
	}
	else if ( index < (g_ButtonNames.size() + g_TriggerNames.size() + g_StickNames.size()) )
	{
		return g_StickNames[ index - g_ButtonNames.size() - g_TriggerNames.size() ];
	}
	else
	{
		return std::string();
	}
}

size_t Gamepad::GetInputIndex( size_t subSource, std::string name ) const
{
	auto && itr = m_nameToIndex.find( name );
	if ( itr == m_nameToIndex.end() )
	{
		return MAXSIZE_T;
	}
	else
	{
		return itr->second;
	}
}

me::input::InputType Gamepad::GetInputType( size_t subSource, size_t index ) const
{
	if ( index < (g_ButtonNames.size()) )
	{
		return input::InputType::Button;
	}
	else if ( index < (g_ButtonNames.size() + g_TriggerNames.size()) )
	{
		return input::InputType::Trigger;
	}
	else if ( index < (g_ButtonNames.size() + g_TriggerNames.size() + g_StickNames.size()) )
	{
		return input::InputType::Stick;
	}
	else
	{
		return input::InputType::Invalid;
	}
}

input::IData::ptr Gamepad::GetInputData( size_t subSource, size_t index ) const
{
	if ( m_game->IsUpdateLocked( false ) || ! m_game->GetOS()->GetHasFocus() ) return nullptr;

	if ( index < (g_ButtonNames.size()) )
	{	
		auto itr = m_states.find( subSource );
		if ( itr == m_states.end() )
		{
			return input::IData::ptr();
		}

		const XINPUT_STATE * prevState = 0;
		auto itrPrevious = m_prevStates.find( subSource );
		if ( itrPrevious != m_prevStates.end() )
		{
			prevState = &itrPrevious->second;
		}

		const XINPUT_STATE * state = &itr->second;
		WORD button = g_ButtonCodes[ index ];

		input::ButtonData * data = new input::ButtonData();
		data->down = ((state->Gamepad.wButtons & button) == button);
		data->pressed = (prevState && ((state->Gamepad.wButtons & button) == 0) && (prevState->Gamepad.wButtons & button) == button);
		return input::IData::ptr( data );
	}
	else if ( index < (g_ButtonNames.size() + g_TriggerNames.size()) )
	{
		auto itr = m_states.find( subSource );
		if ( itr == m_states.end() )
		{
			return input::IData::ptr();
		}

		auto TriggerValue = []( auto v ) { return (v <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? 0.0f : ( v / 255.f ); };

		const XINPUT_STATE * state = &itr->second;
		input::TriggerData * data = new input::TriggerData();
	
		int triggerIndex = index - g_ButtonNames.size();
		switch ( triggerIndex )
		{
		case 0:
			data->value = TriggerValue( state->Gamepad.bLeftTrigger );
			break;
		case 1:
			data->value = TriggerValue( state->Gamepad.bRightTrigger );
			break;
		}
		return input::IData::ptr( data );
	}
	else if ( index < (g_ButtonNames.size() + g_TriggerNames.size() + g_StickNames.size()) )
	{
		auto itr = m_states.find( subSource );
		if ( itr == m_states.end() )
		{
			return input::IData::ptr();
		}

		auto StickValue = []( auto v ) { return (v < 0) ? (v / 32768.0f) : (v / 32767.0f); };

		const XINPUT_STATE * state = &itr->second;
		input::StickData * data = new input::StickData();

		int stickIndex = index - g_ButtonNames.size() - g_TriggerNames.size();
		switch ( stickIndex )
		{
		case 0:
			data->axis.x = StickValue( state->Gamepad.sThumbLX );
			data->axis.y = StickValue( state->Gamepad.sThumbLY );
			break;
		case 1:
			data->axis.x = StickValue( state->Gamepad.sThumbRX );
			data->axis.y = StickValue( state->Gamepad.sThumbRY );
			break;
		}
		return input::IData::ptr( data );
	}
	else
	{
		return input::IData::ptr();
	}
}

me::input::IData::ptr Gamepad::GetInputData( size_t subSource, std::string name ) const
{
	size_t index = GetInputIndex( subSource, name );
	return GetInputData( subSource, index );
}

void Gamepad::SetInputData( size_t subSource, size_t index, me::input::IData::ptr dataIn )
{
	throw std::exception( "Error! Attempted to SetInputData on Gamepad, which is read-only!" );
}
