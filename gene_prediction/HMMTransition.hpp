/*
 * HMMTransition.hpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMTRANSITION_HPP_
#define HMMTRANSITION_HPP_

#include <ostream>
#include <istream>

/**
 * This class represents a transition of the HMM. Thus it contains the
 * destination id and the transition probability.
 */
class HMMTransition{
public:
	double _probability;
	int _destination;

	HMMTransition( int destination = -1, double probability=-1.0);

	void serialize(std::ostream& os) const;

	static HMMTransition deserialize(std::istream& is);
};



#endif /* HMMTRANSITION_HPP_ */
