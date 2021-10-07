#pragma once

/**
 * @file
 * @brief Defines StackVector and StackVectorIterator.
 */

#include <array>
#include <cstddef>
#include <vector>

#include <boost/optional.hpp>

namespace csc {
	/**
	 * @brief Class used to iterate through a StackVector.
	 */
	template <typename T> class StackVectorIterator {
	public:
		StackVectorIterator(const boost::optional<T>* arr_ptr) : use_vec{ false } ,arr_ptr{ arr_ptr }{}
		StackVectorIterator(typename std::vector<T>::const_iterator vec_iter) : use_vec{ true },vec_iter{ vec_iter }  {}

		T operator*() const { return use_vec ? *vec_iter : **arr_ptr; }
		bool operator!=(const StackVectorIterator<T>& other) const { return use_vec ? vec_iter != other.vec_iter : arr_ptr != other.arr_ptr; }

		StackVectorIterator<T> operator++(){
			if (use_vec) {
				vec_iter++;
			}
			else {
				arr_ptr++;
			}
			return *this;
		}

	private:
		bool use_vec;
		const boost::optional<T>* arr_ptr;
		typename std::vector<T>::const_iterator vec_iter;
	};

	/**
	 * @brief A vector-like container that will not allocate on the heap until it has >N elements.
	 */
	template <typename T, size_t N> class StackVector {
	public:
		void push(const T item) {
			if (use_vec) {
				assert(_vec.has_value());
				_vec->push_back(item);
				occupied = _vec->size();
			}
			else {
				// Using array
				if (occupied >= N) {
					// Move contents to vec and clear array
					use_vec = true;
					_vec = std::vector<T>{};
					_vec->reserve(occupied + 1);
					for (boost::optional<T>& this_item : _array) {
						_vec->push_back(std::move(*this_item));
					}
					_array.fill(boost::none);
					_vec->push_back(item);
					occupied = _vec->size();
				}
				else {
					_array[occupied] = item;
					occupied++;
				}
			}
		}
		size_t size() const { return occupied; }

		StackVectorIterator<T> begin() const {
			if (use_vec) {
				assert(_vec.has_value());
				return StackVectorIterator<T>{ _vec->begin() };
			}
			else {
				return StackVectorIterator<T>{ _array.data() };
			}
		}
		StackVectorIterator<T> end() const {
			if (use_vec) {
				assert(_vec.has_value());
				return StackVectorIterator<T>{ _vec->end() };
			}
			else {
				return StackVectorIterator<T>{ _array.data() + size() };
			}
		}

		void clear();

	private:
		size_t occupied = 0;

		bool use_vec{ false };
		std::array<boost::optional<T>, N> _array;
		boost::optional<std::vector<T>> _vec;
	};
}
