/*
 * HMM.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMM.hpp"
#include <stdexcept>
#include <sstream>

#include <boost/regex.hpp>

#include "HMMSingleNode.hpp"
#include "HMMContainer.hpp"
#include "nullPtr.hpp"
#include "Exceptions.hpp"
#include "HMMTransition.hpp"
#include "HMMEmission.hpp"
#include "HMMCompiled.hpp"

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
	boost::unordered_map<int, ptrHMMNode>::const_iterator iterator = _nodes.begin();

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

void HMM::insertModel(int containerID,const boost::shared_ptr<HMM>& hmm){
	ptrHMMNode node = getNode(containerID);

	if(node){
		node->insertModel(hmm);
	}else{
		throw std::invalid_argument("There is no container with ID:"+containerID);
	}
}

void HMM::serialize(std::ostream& os) const{
	os << "HMM{" << std::endl;
	os << "Nodes:" << shallowSize() << std::endl;
	for(boost::unordered_map<int,ptrHMMNode>::const_iterator it = _nodes.begin();
			it != _nodes.end(); ++it){
		HMMNode::serialize(os,it->second);
	}
	os << "Next ID:" << _nextID << std::endl;
	os << "Start Nodes:" << _startNodes.size() << std::endl;
	for(boost::unordered_set<int>::const_iterator it = _startNodes.begin();
			it != _startNodes.end(); ++it){
		os << *it << std::endl;
	}
	os << "End Nodes:" << _endNodes.size() << std::endl;
	for(boost::unordered_set<int>::const_iterator it = _endNodes.begin();
			it != _endNodes.end(); ++it){
		os << *it << std::endl;
	}
	os << "}" << std::endl;
}

void HMM::deserialize(std::istream& is,boost::shared_ptr<HMM> hmm){
	std::string line;
	boost::smatch sm;
	std::istringstream ss;
	int nextID;

	std::getline(is,line);

	if(!boost::regex_match(line,boost::regex("HMM\\{"))){
		throw InvalidSerializationException("HMM: Invalid initial key word");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Nodes:(.*)"))){
		int numberNodes;
		ss.str(sm[1]);
		ss >> numberNodes;
		ss.clear();

		for(int i =0; i< numberNodes; i++){
			ptrHMMNode node = HMMNode::deserialize(is);
			hmm->_nodes.emplace(node->getID(),node);
		}
	}else{
		throw InvalidSerializationException("HMM: Nodes cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Next ID:(.*)"))){
		ss.str(sm[1]);
		ss >> nextID;
		ss.clear();
	}else{
		throw InvalidSerializationException("HMM: Next id cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Start Nodes:(.*)"))){
		int numberNodes;
		int startNode;
		ss.str(sm[1]);
		ss >> numberNodes;
		ss.clear();

		for(int i=0; i< numberNodes; i++){
			std::getline(is,line);
			ss.str(line);
			ss >> startNode;
			ss.clear();

			hmm->addStartNode(startNode);
		}
	}else{
		throw InvalidSerializationException("HMM: Start nodes cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("End Nodes:(.*)"))){
		int numberNodes;
		int endNode;

		ss.str(sm[1]);
		ss >> numberNodes;
		ss.clear();

		for(int i =0; i< numberNodes;i++){
			std::getline(is,line);
			ss.str(line);
			ss >> endNode;
			ss.clear();

			hmm->addEndNode(endNode);
		}
	}else{
		throw InvalidSerializationException("HMM: End nodes cannot be deserialized.");
	}

	std::getline(is,line);

	hmm->_nextID = nextID;
}

int HMM::size() const{
	int result = 0;

	for(boost::unordered_map<int,ptrHMMNode>::const_iterator it = _nodes.begin();
			it != _nodes.end(); ++it){
		result += it->second->size();
	}

	return result;
}

int HMM::shallowSize() const{
	int result = 0;

	for(boost::unordered_map<int,ptrHMMNode>::const_iterator it = _nodes.begin();
			it != _nodes.end(); ++it){
		result += it->second->shallowSize();
	}

	return result;
}

void HMM::compile(boost::shared_ptr<HMMCompiled> compiled) {
	compiled->initialize(size());

	// Build mapping
	for(boost::unordered_map<int,boost::shared_ptr<HMMNode> >::const_iterator it = _nodes.begin();
			it != _nodes.end(); ++it){
		it->second->buildMapping(*compiled);
	}

	// Incorporate transitions and emissions
	for(boost::unordered_map<int,boost::shared_ptr<HMMNode> >::const_iterator it = _nodes.begin();
			it != _nodes.end(); ++it){
		it->second->buildTransitions(*compiled,*this);
	}
}

void HMM::update(boost::shared_ptr<HMMCompiled> compiled){
	for(boost::unordered_map<int,boost::shared_ptr<HMMNode> >::const_iterator it = _nodes.begin();
			it != _nodes.end(); ++it){
		it->second->updateValues(*compiled,*this);
	}
}


