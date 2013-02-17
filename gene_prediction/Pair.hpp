/*
 * Pair.hpp
 *
 *  Created on: Feb 16, 2013
 *      Author: Till Rohrmann
 */

#ifndef PAIR_HPP_
#define PAIR_HPP_

#include <string>
#include <sstream>

template<typename T>

class Pair{
public:
	T _first;
	T _second;

	Pair(T first,T second){ _first = first; _second = second; }

	bool operator<(Pair<T> const & rhs) const{
		if(_first < rhs._first){
			return true;
		}else if(_first > rhs._first){
			return false;
		}else{
			return _second < rhs._second;
		}
	}

	bool operator>(Pair<T> const & rhs) const{
		if(_first > rhs._first){
			return true;
		}else if(_first < rhs._first){
			return false;
		}else{
			return _second > rhs._second;
		}
	}

	std::string toString() const{
		std::stringstream ss;

		ss<<"(" << _first << "," << _second << ")";

		return ss.str();
	}
};



#endif /* PAIR_HPP_ */
