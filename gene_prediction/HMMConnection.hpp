/*
 * HMMConnection.hpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMCONNECTION_HPP_
#define HMMCONNECTION_HPP_

#include <string>

class HMMConnection{
public:
	double _probability;
	bool _constant;
	std::string _destination;

	HMMConnection(double probability=-1.0, const std::string& destination = "", bool constant=false);
};





#endif /* HMMCONNECTION_HPP_ */
