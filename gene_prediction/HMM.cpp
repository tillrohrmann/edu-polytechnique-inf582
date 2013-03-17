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

#include "HMMNode.hpp"
#include "nullPtr.hpp"
#include "Exceptions.hpp"
#include "HMMTransition.hpp"
#include "HMMEmission.hpp"
#include "HMMCompiled.hpp"
#include "HMMConnection.hpp"

HMM::HMM(): _nextID(0){
}

ptrHMMNode HMM::getNode(int id){
	if(_nodes.count(id) > 0){
		return _nodes[id];
	}else{
		return nullPtr;
	}
}

int HMM::createNode(std::string name,bool constantTransitions, bool constantEmissions, bool constantEmissionSet){
	ptrHMMNode newNode(new HMMNode(_nextID,name));
	_nodes.insert(std::make_pair(_nextID,newNode));

	setConstantEmissions(_nextID,constantEmissions);
	setConstantTransitions(_nextID,constantTransitions);
	setConstantEmissionSet(_nextID,constantEmissionSet);

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

void HMM::setConstantEmissions(int src, bool constant){
	ptrHMMNode node = getNode(src);

	if(node){
		node->constantEmissions() = constant;
		node->constantEmissionSet() = constant;
	}else{
		throw std::invalid_argument("Could not find node with ID:"+src);
	}
}

void HMM::setConstantTransitions(int src, bool constant){
	ptrHMMNode node = getNode(src);

	if(node){
		node->constantTransitions() = constant;
	}else{
		throw std::invalid_argument("Could not find node with ID:"+src);
	}
}

void HMM::setConstantEmissionSet(int src, bool constant){
	ptrHMMNode node = getNode(src);

	if(node){
		node->constantEmissionSet() = constant;
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

void HMM::addStartNode(int nodeID, double probability){
	if(hasNode(nodeID)){
		_startNodes.emplace(nodeID,probability);
	}else{
		throw std::invalid_argument("There is no node with ID:"+nodeID);
	}
}

void HMM::addStartNode(const std::string& nodeName, double probability){
	boost::shared_ptr<HMMNode> node = getNode(nodeName);

	if(node){
		_startNodes.emplace(node->getID(),probability);
	}else{
		throw std::invalid_argument("There is no node with name:"+nodeName);
	}
}

void HMM::addEndNode(int nodeID){
	if(hasNode(nodeID)){
		_endNodes.insert(nodeID);
	}else{
		throw std::invalid_argument("There is no node with ID:"+nodeID);
	}
}

void HMM::addEndNode(const std::string& nodeName){
	boost::shared_ptr<HMMNode> node = getNode(nodeName);

	if(node){
		_endNodes.insert(node->getID());
	}else{
		throw std::invalid_argument("There is no node with name:"+nodeName);
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
	for(boost::unordered_map<int,double>::const_iterator it = _startNodes.begin();
			it != _startNodes.end(); ++it){
		os << it->first << ":" << it->second << std::endl;
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
	double probability;

	std::getline(is,line);

	if(!boost::regex_match(line,boost::regex("HMM\\{"))){
		throw std::invalid_argument("HMM: Invalid initial key word");
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
		throw std::invalid_argument("HMM: Nodes cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Next ID:(.*)"))){
		ss.str(sm[1]);
		ss >> nextID;
		ss.clear();
	}else{
		throw std::invalid_argument("HMM: Next id cannot be deserialized.");
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

			boost::regex_match(line,sm,boost::regex("([^:]*):([^:]*)"));

			ss.str(sm[1]);
			ss >> startNode;
			ss.clear();
			ss.str(sm[2]);
			ss >> probability;
			ss.clear();

			hmm->addStartNode(startNode,probability);
		}
	}else{
		throw std::invalid_argument("HMM: Start nodes cannot be deserialized.");
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
		throw std::invalid_argument("HMM: End nodes cannot be deserialized.");
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

	// add initial distribution
	for(boost::unordered_map<int,double>::const_iterator it = _startNodes.begin();
			it != _startNodes.end(); ++it){
		compiled->addInitialDistribution(_nodes.at(it->first),it->second);
	}

	compiled->finishCompilation();
}

void HMM::update(boost::shared_ptr<HMMCompiled> compiled){
	for(boost::unordered_map<int,boost::shared_ptr<HMMNode> >::const_iterator it = _nodes.begin();
			it != _nodes.end(); ++it){
		it->second->updateValues(*compiled,*this);
	}

	for(boost::unordered_map<int,double>::iterator it = _startNodes.begin();
			it != _startNodes.end(); ++it){
		it->second = compiled->getInitialDistribution(getNode(it->first));
	}
}

void HMM::clear(){
	_nodes.clear();
	_startNodes.clear();
	_endNodes.clear();
	_nextID = 0;
}

void HMM::initializeRandom(int numberStates, boost::unordered_set<std::string>& emissions){
	std::stringstream ss;

	clear();

	for(int i=0; i< numberStates; i++){
		ss.clear();
		ss.str(std::string());
		ss << "Node" << i;
		this->createNode(ss.str());
		ss.clear();
	}

	for(int i=0; i<numberStates;i++){
		for(int j =0; j<numberStates;j++){
			addTransition(i,HMMTransition(j));
		}

		for(boost::unordered_set<std::string>::const_iterator it = emissions.begin();
				it != emissions.end(); ++it){
			addEmission(i,HMMEmission(*it));
		}
	}

	addStartNode(0,1);
}

void HMM::integrateHMM(boost::shared_ptr<HMM> hmm, const boost::unordered_map<std::string,std::vector<HMMConnection> >& connections){
	boost::unordered_map<int,int> translation;
	boost::unordered_set<int> nodes2ResetTransitions;
	int id;

	boost::unordered_map<int,boost::shared_ptr<HMMNode> >::const_iterator it = hmm->getNodes().begin();

	for(; it != hmm->getNodes().end(); ++it){
		id = createNode(it->second->getName(),it->second->constantTransitions(),it->second->constantEmissions(),
				it->second->constantEmissionSet());

		translation.emplace(it->first,id);
		boost::unordered_map<std::string,HMMEmission>::const_iterator jt = it->second->getEmission().begin();

		for(; jt != it->second->getEmission().end(); ++jt){
			addEmission(id,jt->second);
		}
	}

	it = hmm->getNodes().begin();

	for(;it != hmm->getNodes().end(); ++it){
		boost::unordered_map<int,HMMTransition>::const_iterator jt = it->second->getTransition().begin();

		for(; jt != it->second->getTransition().end(); ++jt){
			addTransition(translation.at(it->first),HMMTransition(translation.at(jt->first),jt->second._probability));
		}
	}

	for(boost::unordered_map<std::string,std::vector<HMMConnection> >::const_iterator it = connections.begin(); it != connections.end(); ++it){
		for(std::vector<HMMConnection>::const_iterator jt = it->second.begin(); jt != it->second.end(); ++jt){
			int src = getNode(it->first)->getID();
			int dest = getNode(jt->_destination)->getID();

			addTransition(src,HMMTransition(dest,jt->_probability));

			if(jt->_probability < 0){
				nodes2ResetTransitions.emplace(src);
			}
		}
	}

	for(boost::unordered_set<int>::const_iterator it = nodes2ResetTransitions.begin(); it != nodes2ResetTransitions.end(); ++it){
		resetTransitions(*it);
		setConstantTransitions(*it,false);
	}
}

void HMM::resetTransitions(int id){
	boost::shared_ptr<HMMNode> node = getNode(id);

	for(boost::unordered_map<int,HMMTransition>::iterator it =node->getTransition().begin(); it != node->getTransition().end(); ++it){
		it->second._probability = -1;
	}
}

void HMM::substituteEmissions(const boost::unordered_map<std::string,boost::unordered_map<std::string,std::string> >& substitution){
	for(boost::unordered_map<int,boost::shared_ptr<HMMNode> >::iterator it = _nodes.begin(); it != _nodes.end(); ++it){
		for(boost::unordered_map<std::string,boost::unordered_map<std::string,std::string> >::const_iterator jt = substitution.begin(); jt != substitution.end(); ++jt){
			if(boost::regex_match(it->second->getName(),boost::regex(jt->first))){
				it->second->substituteEmissions(jt->second);

				break;
			}
		}
	}
}


