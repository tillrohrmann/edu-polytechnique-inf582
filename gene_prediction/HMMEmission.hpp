/*
 * HMMEmission.hpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMEMISSION_HPP_
#define HMMEMISSION_HPP_

#include <string>

class HMMEmission{
public:
	double _probabiltiy;
	std::string _emissionToken;
	bool _constant;

	HMMEmission(double probability = -1.0,const std::string& emissionToken = "", bool constant = false);

	void serialize(std::ostream& os) const;

	static HMMEmission deserialize(std::istream& is);
};


#endif /* HMMEMISSION_HPP_ */
