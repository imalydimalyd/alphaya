#pragma once

#include "agent.hpp"

#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace AlphaYa
{
	template <typename StateType>
	class InputAgent : public Agent<StateType>
	{
	public:
		static constexpr IndexType players = StateType::players;

		typedef StateType State;
		typedef typename State::Action Action;
		typedef typename State::Data Data;

		Action move(const State &state, std::istream &in, std::ostream &out)
		{
			std::unordered_map<std::string, Action> actions;
			for (const Action &action : state.generateActions())
			{
				std::ostringstream out;
				action.output(out);
				const std::string description = out.str();
				actions.emplace(description, action);
			}
			for (std::string input;;)
			{
				out << "Please input your move: ";
				std::getline(in, input);
				typename std::unordered_map<std::string, Action>::const_iterator it = actions.find(input);
				if (it == actions.end())
				{
					out << "No such move" << std::endl;
					continue;
				}
				return it->second;
			}
		}
	};
};