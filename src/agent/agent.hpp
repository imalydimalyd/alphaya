#pragma once

#include "../game/game.hpp"

#include <istream>
#include <ostream>

namespace AlphaYa
{
	template <typename StateType>
	class Agent
	{
	public:
		static constexpr IndexType players = StateType::players;

		typedef StateType State;
		typedef typename State::Action Action;
		typedef typename State::Data Data;

		virtual Action move(const State &state, std::istream &in, std::ostream &out) = 0;
	};
};