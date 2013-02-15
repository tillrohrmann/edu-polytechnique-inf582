/*
 * HMMSingleNode.hpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMSINGLENODE_HPP_
#define HMMSINGLENODE_HPP_

#include "HMMNode.hpp"

class HMMSingleNode: public HMMNode{
public:
	HMMSingleNode(int id=-1, const std::string& name = "");
	HMMSingleNode(int id, const std::string& name, const Transition& transitions, const Emission& emissions);
	virtual ~HMMSingleNode();
};




#endif /* HMMSINGLENODE_HPP_ */
