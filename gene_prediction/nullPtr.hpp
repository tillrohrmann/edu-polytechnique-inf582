/*
 * nullPtr.hpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#ifndef NULLPTR_HPP_
#define NULLPTR_HPP_

#include <tr1/memory>

class NullPtr{
public:
	template <typename T>
	operator std::tr1::shared_ptr<T> () { return std::tr1::shared_ptr<T>(); }
};

extern NullPtr nullPtr;




#endif /* NULLPTR_HPP_ */
