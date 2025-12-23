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
	class MCTSAgent : public Agent<StateType>
	{
	public:
		typedef StateType State;
		typedef typename State::Action Action;
		typedef typename State::Data Data;

		//patch
		/*
		State state;
		std::unordered_map<std::string, Action> actions;
		void setState(const State &s)
		{
			state = s;
			actions.clear();
			for (const Action &action : s.generateActions())
			{
				std::ostringstream out;
				action.output(out);
				const std::string description = out.str();
				actions.emplace(description, action);
			}
		}*/
		Action move(const State &state,std::istream &in, std::ostream &out) 
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