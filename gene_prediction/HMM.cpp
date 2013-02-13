/*
 * HMM.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMM.hpp"
#include "nullPtr.hpp"
#include <stdexcept>

HMM::HMM(): _nextID(0){
}

ptrHMMNode HMM::getNode(int id){
	if(_nodes.count(id) > 0){
		return _nodes[id];
	}else{
		return nullPtr;
	}
}

int HMM::createNode(std::string name){
	ptrHMMNode newNode(new HMMSingleNode(_nextID,name));
	_nodes.insert(std::make_pair(_nextID,newNode));
	return _nextID++;
}

int HMM::createContainer(std::string name){
	ptrHMMNode newNode(new HMMContainer(_nextID,name));
	_nodes.insert(std::make_pair(_nextID,newNode));
	return _nextID++;
}

void HMM::removeNode(int id){
	_nodes.erase(id);
}

ptrHMMNode HMM::getNode(const std::string& name){
	std::tr1::unordered_map<int, ptrHMMNode>::const_iterator iterator = _nodes.begin();

	for(;iterator != _nodes.end();++iterator){
		if(iterator->second->getName() == name){
			return iterator->second;
		}
	}

	return nullPtr;
}

bool HMM::hasNode(int id){
	return _nodes.count(id) >0;
}

void HMM::addTransition(int src, const HMMTransition& transition){
	ptrHMMNode node = getNode(src);

	if(node){
		node->addTransition(transition);
	}else{
		throw std::invalid_argument("Could not find node with ID:"+src);
	}
}

void HMM::addEmission(int src, const HMMEmission& emission){
	ptrHMMNode node = getNode(src);

	if(node){
		node->addEmission(emission);
	}else{
		throw std::invalid_argument("Could not find node with ID:"+src);
	}
}

void HMM::addTransitions(int src, const std::vector<HMMTransition> & transitions){
	ptrHMMNode node = getNode(src);

	if(node){
		std::vector<HMMTransition>::const_iterator it = transitions.begin();

		for(;it != transitions.begin(); ++it){
			node->addTransition(*it);
		}
	}else{
		throw std::invalid_argument("Could not find node with ID:"+src);
	}
}

void HMM::addEmissions(int src,const std::vector<HMMEmission>& emissions){
	ptrHMMNode node =getNode(src);

	if(node){
		std::vector<HMMEmission>::const_iterator it = emissions.begin();

		for(; it!= emissions.begin(); ++it){
			node->addEmission(*it);
		}
	}else{
		throw std::invalid_argument("Could not find node with ID:"+src);
	}
}

void HMM::addStartNode(int nodeID){
	if(hasNode(nodeID)){
		_startNodes.insert(nodeID);
	}else{
		throw std::invalid_argument("There is no node with ID:"+nodeID);
	}
}

void HMM::addEndNode(int nodeID){
	if(hasNode(nodeID)){
		_endNodes.insert(nodeID);
	}else{
		throw std::invalid_argument("There is no node with ID:"+nodeID);
	}
}

void HMM::insertModel(int containerID,const std::tr1::shared_ptr<HMM>& hmm){
	ptrHMMNode node = getNode(containerID);

	if(node){
		node->insertModel(hmm);
	}else{
		throw std::invalid_argument("There is no container with ID:"+containerID);
	}
}

void HMM::serialize(std::ostream& os) const{

}

std::tr1::shared_ptr<HMM> HMM::deserialize(std::istream& is){
	return std::tr1::shared_ptr<HMM>(new HMM());
}


