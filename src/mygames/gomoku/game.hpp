#pragma once

#include "../../game/game.hpp"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace AlphaYa
{
	namespace Gomoku
	{
		// The height of board, should be at least 1
		constexpr IndexType GOMOKU_HEIGHT = 15;
		// The width of board, should be in [5, 16]
		constexpr IndexType GOMOKU_WIDTH = 15;
		/*
		All data of game state
		It is recommended to use multiple bitboards to represent the board,
		and to represent integers in unary numeral system
		(0b0, 0b1, 0b11, 0b111, ...).
		See: https://www.chessprogramming.org/Bitboard_Board-Definition
		*/
		class GomokuData
		{
		public:
			std::uint16_t bitboard0[GOMOKU_HEIGHT];
			std::uint8_t side;
			std::uint16_t bitboard1[GOMOKU_HEIGHT];
		};

		/*
		Game action
		*/
		class GomokuAction : public Action
		{
		public:
			std::uint8_t position;
			GomokuAction(std::uint8_t p = 0) : position(p) {}

			/*
			Output action as a short string to out, which does not contain endl
			*/
			void output(std::ostream &out) const
			{
				out << (char)('a' + (position & 15)) << (1 + (position >> 4));
			}

			/*
			Check if two actions are equal
			*/
			bool operator==(const GomokuAction &o) const
			{
				return position == o.position;
			}
		};

		/*
		Game state
		Template arguments are: <# of players, data type, action type>.
		*/
		class GomokuState : public State<2, GomokuData, GomokuAction>
		{
		public:
			/*
			Returns id of the current player
			*/
			IndexType toMove() const
			{
				const GomokuData &data = getData();
				return data.side;
			}

			/*
			Returns actions that the current can make
			*/
			std::vector<GomokuAction> generateActions() const
			{
				const GomokuData &data = getData();
				std::vector<GomokuAction> actions;
				actions.reserve(GOMOKU_WIDTH * GOMOKU_HEIGHT);
				for (IndexType i = 0; i < GOMOKU_HEIGHT; ++i)
				{
					const std::uint16_t bitboard_i = data.bitboard0[i] | data.bitboard1[i];
					for (IndexType j = 0; j < GOMOKU_WIDTH; ++j)
					{
						if (!(bitboard_i >> j & 1))
						{
							actions.emplace_back(i << 4 | j);
						}
					}
				}
				return actions;
			}

			/*
			Modifies the data according to action
			*/
			void move(const GomokuAction &action)
			{
				GomokuData &data = getData();
				const std::uint16_t mask = (((std::uint16_t)1) << (action.position & 15));
				data.side ? (data.bitboard1[action.position >> 4] |= mask) : (data.bitboard0[action.position >> 4] |= mask);
				data.side ^= 1;
			}

			static bool hasFive(const std::uint16_t bitboard[GOMOKU_HEIGHT])
			{
				std::uint16_t a[4] = {0, 0, 0, 0}, b[4] = {0, 0, 0, 0}, c[4] = {0, 0, 0, 0};
				for (IndexType i = 0;;)
				{
					const std::uint16_t x = bitboard[i];
					if (x & (a[0] | b[0] | c[0]))
					{
						return true;
					}

					std::uint16_t mask = 31;
					for (IndexType j = 4; j < GOMOKU_WIDTH; ++j)
					{
						if ((x & mask) == mask)
						{
							return true;
						}
						mask <<= 1;
					}

					++i;
					if (i >= GOMOKU_HEIGHT)
					{
						return false;
					}
					a[0] = ((a[1] & x) >> 1);
					a[1] = ((a[2] & x) >> 1);
					a[2] = ((a[3] & x) >> 1);
					a[3] = (x >> 1);
					b[0] = (b[1] & x);
					b[1] = (b[2] & x);
					b[2] = (b[3] & x);
					b[3] = x;
					c[0] = ((c[1] & x) << 1);
					c[1] = ((c[2] & x) << 1);
					c[2] = ((c[3] & x) << 1);
					c[3] = (x << 1);
				}
			}

			/*
			Calculate score if game over
			If game is over, write the score of each player into scores, and return true.
			If game is not over, do not modify scores, and return false.
			*/
			bool calculateScore(ScoreType scores[2]) const
			{
				const GomokuData &data = getData();
				if (hasFive(data.bitboard0))
				{
					scores[0] = 1;
					scores[1] = -1;
					return true;
				}
				if (hasFive(data.bitboard1))
				{
					scores[0] = -1;
					scores[1] = 1;
					return true;
				}
				for (IndexType i = 0; i < GOMOKU_HEIGHT; ++i)
				{
					const std::uint16_t bitboard_i = data.bitboard0[i] | data.bitboard1[i];
					if (bitboard_i != (((std::uint16_t)1) << GOMOKU_WIDTH) - 1)
					{
						return false;
					}
				}
				scores[0] = 0;
				scores[1] = 0;
				return true;
			}

			/*
			Init the game state from ANY string
			*/
			void init(const std::string &state_string)
			{
				GomokuData &data = getData();
				for (IndexType i = 0; i < GOMOKU_HEIGHT; ++i)
				{
					data.bitboard0[i] = 0;
				}
				data.side = 0;
				for (IndexType j = 0; j < GOMOKU_HEIGHT; ++j)
				{
					data.bitboard1[j] = 0;
				}

				std::string argument;
				for (std::istringstream ssin(state_string);;)
				{
					ssin >> argument;
					if (ssin.fail())
					{
						break;
					}
					if (argument == "XM")
					{
						data.side = 0;
						continue;
					}
					if (argument == "OM")
					{
						data.side = 1;
						continue;
					}
					if (argument == "X" || argument == "O" || argument == "+")
					{
						std::string a;
						std::uint16_t b;
						ssin >> a >> b;
						if (ssin.fail() || a.length() != 1 || a[0] < 'a' || a[0] >= 'a' + ((char)GOMOKU_WIDTH) || !b || b > GOMOKU_HEIGHT)
						{
							break;
						}
						--b;
						std::uint16_t mask = (((std::uint16_t)1) << (a[0] - 'a'));
						if (argument == "X")
						{
							data.bitboard0[b] |= mask;
						}
						else
						{
							data.bitboard0[b] &= (~mask);
						}
						if (argument == "O")
						{
							data.bitboard1[b] |= mask;
						}
						else
						{
							data.bitboard1[b] &= (~mask);
						}
						continue;
					}
				}
			}

			/*
			Output the game state to out using method
			The possible methods are:
			terminal: output pretty-printed board
			(other value): output a short string, which does not contain endl, and can be read by init(std::string)
			*/
			void output(std::ostream &out, const std::string &method) const
			{
				const GomokuData &data = getData();
				if (method == "terminal")
				{
					for (IndexType i = GOMOKU_HEIGHT - 1; ~i; --i)
					{
						out << "\033[0;37;90m" << std::setw(2) << (i + 1) << "\033[0m \033[30;43;103;48;5;214m ";
						for (IndexType j = 0; j < GOMOKU_WIDTH; ++j)
						{
							if (data.bitboard0[i] >> j & 1)
							{
								out << "X ";
							}
							else if (data.bitboard1[i] >> j & 1)
							{
								out << "\033[37;97;38;5;231;38;2;255;255;255mO\033[30m ";
							}
							else
							{
								out << "\033[38;5;208;38;2;255;135;0m+\033[30m ";
							}
						}
						out << "\033[0m" << std::endl;
					}
					out << "\033[0;37;90m    ";
					for (IndexType j = 0; j < GOMOKU_WIDTH; ++j)
					{
						out << (char)('a' + j) << " ";
					}
					out << "\033[0m" << std::endl;
					return;
				}
				out << (data.side ? "OM " : "XM ");
				for (IndexType i = 0; i < GOMOKU_HEIGHT; ++i)
				{
					std::uint16_t bitboard1_i = data.bitboard1[i];
					std::uint16_t bitboard_i = bitboard1_i | data.bitboard0[i];
					for (IndexType j = 0; j < GOMOKU_WIDTH; ++j)
					{
						if (bitboard_i >> j & 1)
						{
							out << ((bitboard1_i >> j & 1) ? "O " : "X ") << (char)('a' + j) << " " << (1 + i) << " ";
						}
					}
				}
			}
		};
	};
};