/*
 * nullPtr.hpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#ifndef NULLPTR_HPP_
#define NULLPTR_HPP_

#include <boost/shared_ptr.hpp>

class NullPtr{
public:
	template <typename T>
	operator boost::shared_ptr<T> () { return boost::shared_ptr<T>(); }
};

extern NullPtr nullPtr;




#endif /* NULLPTR_HPP_ */
