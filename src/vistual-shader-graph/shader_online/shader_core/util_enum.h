#pragma once

/**
 * @file
 * @brief Defines a few utility classes to provide some simple functionality around enums.
 */

namespace csc {

	template<typename T>
	class EnumIterator {
	public:
		EnumIterator(T value) : value(value) {}

		inline T operator*() const { return value; }
		inline void operator++() { value = static_cast<T>(static_cast<int>(value) + 1); }
		inline bool operator!=(const EnumIterator<T>& other) const { return value != other.value; }

	private:
		T value;
	};

	template<typename T, T end_value>
	class EnumList {
	public:
		EnumList() {}

		EnumIterator<T> begin() const { return static_cast<T>(0); }
		EnumIterator<T> end() const { return end_value; }
	};

	template <typename T, T...vals>
	class EnumMatcher {
	public:
		EnumMatcher() {}

		bool matches(T) const { return false; }
	};

	template <typename T, T val, T...vals>
	class EnumMatcher<T, val, vals...> {
	public:
		EnumMatcher() {}

		bool matches(const T input) const {
			return (input == val) || sub_matcher.matches(input);
		}

	private:
		EnumMatcher<T, vals...> sub_matcher;
	};
}
