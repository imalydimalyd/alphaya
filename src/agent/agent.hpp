#pragma once

#include "../game/game.hpp"

#include <istream>
#include <ostream>
#include <type_traits>

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

		//patch
		//virtual void setState(const State &state) = 0;
		virtual Action move(const State &state,std::istream &in, std::ostream &out)  = 0;
	};
};