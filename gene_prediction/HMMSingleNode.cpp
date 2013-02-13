/*
 * HMMSingleNode.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#include "HMMSingleNode.hpp"

HMMSingleNode::HMMSingleNode(int id, std::string name): HMMNode(id,name){
}

HMMSingleNode::HMMSingleNode(int id, std::string name, const Transition& transitions, const Emission& emissions)
	:HMMNode(id,name,transitions,emissions){
}

HMMSingleNode::~HMMSingleNode(){
}
