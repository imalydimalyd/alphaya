#pragma once

#include "agent.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <istream>
#include <memory>
#include <ostream>
#include <random>
#include <unordered_map>
#include <vector>

namespace AlphaYa
{
	template <typename StateType>
	class MCTSAgent : public Agent<StateType>
	{
	public:
		static constexpr IndexType players = StateType::players;

		typedef StateType State;
		typedef typename State::Action Action;
		typedef typename State::Data Data;
		typedef std::mt19937::result_type SeedType;

		typedef float EvalType;

		std::mt19937 rd;
		EvalType c;
		IndexType simulate_count;

		MCTSAgent(SeedType seed, EvalType cc, IndexType s) : rd(seed), c(cc), simulate_count(s) {}

		class Node
		{
		public:
			bool is_final;
			ScoreType count, scores[players];
			State state;
			std::weak_ptr<Node> father;

			class Child
			{
			public:
				Action action;
				std::shared_ptr<Node> node;
				Child(const Action &a) : action(a) {}
			};

			std::vector<Child> children;

			Node(const State &s, std::mt19937 &rd) : state(s)
			{
				is_final = state.calculateScore(scores);
				count = is_final ? 1 : 0;
				children.clear();
				if (!is_final)
				{
					for (IndexType player = 0; player < players; ++player)
					{
						scores[player] = 0;
					}
					const std::vector<Action> actions = state.generateActions();
					children.reserve(actions.size());
					for (const Action &action : actions)
					{
						children.emplace_back(action);
					}
					std::shuffle(children.begin(), children.end(), rd);
				}
			}

			bool best_action(Action &action) const
			{
				IndexType player = state.toMove();
				for (const Child &child : children)
				{
					if (!child.node)
					{
						return false;
					}
				}
				EvalType best = -INFINITY;
				for (const Child &child : children)
				{
					const std::shared_ptr<Node> &node = child.node;
					EvalType eval = ((EvalType)(node->scores[player])) / ((EvalType)(node->count));
					if (best < eval)
					{
						best = eval;
						action = child.action;
					}
				}
				return true;
			}
		};
		std::shared_ptr<Node> root;

		static std::shared_ptr<Node> find_state(std::shared_ptr<Node> node, const State &s)
		{
			const std::uint8_t *bytes = s.getBytes(), *node_bytes = node->state.getBytes();
			if (std::equal(bytes, bytes + State::byte_count, node_bytes, node_bytes + State::byte_count))
			{
				return node;
			}
			for (const typename Node::Child &child : node->children)
			{
				if (child.node)
				{
					std::shared_ptr<Node> ret = find_state(child.node, s);
					if (ret)
					{
						return ret;
					}
				}
			}
			return std::shared_ptr<Node>();
		}

		static std::shared_ptr<Node> explore(std::shared_ptr<Node> node, const EvalType c, std::mt19937 &rd)
		{
			IndexType player = node->state.toMove();
			for (typename Node::Child &child : node->children)
			{
				if (!child.node)
				{
					State next_state = node->state;
					next_state.move(child.action);
					child.node = std::make_shared<Node>(next_state, rd);
					child.node->father = std::weak_ptr<Node>(node);
					return child.node;
				}
			}
			EvalType best = -INFINITY, k = c * std::sqrt(std::log((EvalType)node->count));
			std::shared_ptr<Node> best_node;
			for (const typename Node::Child &child : node->children)
			{
				const std::shared_ptr<Node> &node = child.node;
				EvalType count = (EvalType)(node->count);
				EvalType eval = ((EvalType)(node->scores[player])) / count + k / std::sqrt(count);
				if (best < eval)
				{
					best = eval;
					best_node = node;
				}
			}
			return best_node;
		}

		Action move(const State &state, std::istream &in, std::ostream &out)
		{
			if (root)
			{
				root = find_state(root, state);
			}
			if (!root)
			{
				root = std::make_shared<Node>(state, rd);
			}

			bool has_action = false;
			Action action;
			for (IndexType i = 0;; ++i)
			{
				std::shared_ptr<Node> p = root;
				do
				{
					p = explore(p, c, rd);
				} while (!p->is_final);
				ScoreType *scores = p->scores;
				do
				{
					p = p->father.lock();
					++p->count;
					for (IndexType player = 0; player < players; ++player)
					{
						p->scores[player] += scores[player];
					}
				} while (p != root);
				Action new_action;
				if (root->best_action(new_action) && (!has_action || !(action == new_action)))
				{
					has_action = true;
					action = new_action;
					out << i << ": ";
					action.output(out);
					out << std::endl;
				}
				if (has_action && i >= simulate_count)
				{
					return action;
				}
			}
		}
	};
};