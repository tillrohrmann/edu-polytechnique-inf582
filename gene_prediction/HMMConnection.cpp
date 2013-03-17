/*
 * HMMConnection.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMMConnection.hpp"

#include <boost/functional/hash_fwd.hpp>


HMMConnection::HMMConnection(const std::string& destination, double probability):
_probability(probability),_destination(destination){
}

