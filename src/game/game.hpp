#pragma once

#include <cstdint>
#include <ostream>
#include <type_traits>
#include <vector>

namespace AlphaYa
{
	typedef std::size_t IndexType;
	typedef std::int64_t ScoreType;

	class Action
	{
	public:
		virtual void output(std::ostream &out) const = 0;
	};

	template <IndexType n, typename DataType, typename ActionType, typename = typename std::enable_if<std::is_base_of<Action, ActionType>::value>::type>
	class State
	{
	public:
		typedef ActionType Action;
		typedef DataType Data;
		static constexpr IndexType players = n;
		static constexpr IndexType byte_count=sizeof(DataType);

		union
		{
			DataType content;
			std::uint8_t bytes[byte_count];
		} data;

		DataType &getData()
		{
			return data.content;
		}
		const DataType &getData() const
		{
			return data.content;
		}
		std::uint8_t *getBytes()
		{
			return data.bytes;
		}
		const std::uint8_t *getBytes() const
		{
			return data.bytes;
		}

		virtual IndexType toMove() const = 0;
		virtual std::vector<Action> generateActions() const = 0;
		virtual void move(const Action &action) = 0;
		virtual bool calculateScore(ScoreType scores[players]) const = 0;

		virtual void init(const std::string &state_string) = 0;
		virtual void output(std::ostream &out, const std::string &method) const = 0;
	};
};