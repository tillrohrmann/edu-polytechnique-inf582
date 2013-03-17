/*
 * HMMConnection.hpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMCONNECTION_HPP_
#define HMMCONNECTION_HPP_

#include <string>

struct HMMConnection{
	double _probability;
	std::string _destination;

	HMMConnection(const std::string& destination = "", double probability = -1.0);
};





#endif /* HMMCONNECTION_HPP_ */
