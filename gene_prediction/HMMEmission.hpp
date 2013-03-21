/*
 * HMMEmission.hpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMEMISSION_HPP_
#define HMMEMISSION_HPP_

#include <string>

/**
 * This class represents an emission of a node. Thus it contains the emitted symbol
 * and its probability.
 */
class HMMEmission{
public:
	double _probabiltiy;
	std::string _emissionToken;

	HMMEmission(const std::string& emissionToken = "", double probability = -1.0);

	void serialize(std::ostream& os) const;

	static HMMEmission deserialize(std::istream& is);
};


#endif /* HMMEMISSION_HPP_ */
