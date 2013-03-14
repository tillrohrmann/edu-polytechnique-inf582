/*
 * HMMConnection.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMMConnection.hpp"


HMMConnection::HMMConnection(double probability, const std::string& destination, bool constant):
_probability(probability),_destination(destination), _constant(constant){
}


