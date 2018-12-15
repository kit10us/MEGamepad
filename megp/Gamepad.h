// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/input/InputDevice.h>
#include <me/game/IGame.h>
#include <Xinput.h>
#include <map>

namespace megp
{
	class Gamepad : public me::input::InputDevice
	{
	public:
		Gamepad( me::game::IGame * gameInstance );
		virtual ~Gamepad();

		std::string GetName() const;

		void Update() override;

		size_t SubSourceCount() const override;

		size_t GetInputCount( size_t subSource ) const override;

		std::string GetInputName( size_t subSource, size_t index ) const override;

		size_t GetInputIndex( size_t subSource, std::string name ) const override;

		me::input::InputType GetInputType( size_t subSource, size_t index ) const override;

		me::input::IData::ptr GetInputData( size_t subSource, size_t index ) const override;

		me::input::IData::ptr GetInputData( size_t subSource, std::string name ) const override;

		void SetInputData( size_t subSource, size_t index, me::input::IData::ptr dataIn ) override;

	private:
		me::game::IGame * m_game;
		std::map< std::string, size_t, unify::CaseInsensitiveLessThanTest > m_nameToIndex;
		std::map< DWORD, XINPUT_STATE > m_states;
		std::map< DWORD, XINPUT_STATE > m_prevStates;
	};
}