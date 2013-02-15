/*
 * HMMCompiled.cpp
 *
 *  Created on: Feb 14, 2013
 *      Author: Till Rohrmann
 */

#include "HMMCompiled.hpp"

#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include "nullPtr.hpp"

HMMCompiled::HMMCompiled():
	_numberNodes(0),_transitions(NULL),_emissions(NULL),_initialDistribution(NULL),_constantTransitions(NULL),
	_counter(0){
}

HMMCompiled::~HMMCompiled(){
	clear();
}

void HMMCompiled::clear(){
	_numberNodes = 0;

	delete [] _transitions;
	delete [] _emissions;
	delete [] _initialDistribution;
	delete [] _constantTransitions;

	_silentStates.clear();
	_int2Node.clear();
	_node2Int.clear();

	_counter = 0;
}

void HMMCompiled::initialize(int numberNodes){
	_numberNodes = numberNodes;

	_transitions = new double[numberNodes*numberNodes];
	_initialDistribution = new double[numberNodes];
	_emissions = new boost::unordered_map<std::string,double>[numberNodes];
	_constantTransitions = new bool[numberNodes*numberNodes];

	for(int i =0; i< numberNodes*numberNodes;i++){
		_transitions[i] = 0;
		_constantTransitions[i] = false;
	}

	for(int i =0; i< numberNodes;i++){
		_initialDistribution[i] = 0;
	}
}

boost::shared_ptr<HMMNode> HMMCompiled::getNode(int index) const{
	if(_int2Node.count(index) > 0){
		return _int2Node.at(index);
	}else{
		return nullPtr;
	}
}

int HMMCompiled::getIndex(boost::shared_ptr<HMMNode> node) const{
	if(_node2Int.count(node) >0){
		return _node2Int.at(node);
	}else{
		return -1;
	}
}

void HMMCompiled::addMapping(boost::shared_ptr<HMMNode> node){
	if(_node2Int.count(node) == 0){
		_node2Int[node] = _counter;
		_int2Node[_counter++] = node;
	}
}

void HMMCompiled::addSilentNode(boost::shared_ptr<HMMNode> node){
	int index = getIndex(node);

	if(index < 0){
		throw std::invalid_argument("Node could not be found in the mapping.");
	}
	_silentStates.emplace(index);
}

void HMMCompiled::addConstantTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest){
	int srcIndex = getIndex(src);
	int destIndex = getIndex(dest);

	if(srcIndex < 0){
		throw std::invalid_argument("addConstantTransition: Src could not be found in the mapping.");
	}

	if(destIndex < 0){
		throw std::invalid_argument("addConstantTransition: Dest could not be found in the mapping.");
	}

	setConstant(srcIndex,destIndex,true);
}

void HMMCompiled::addTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest, double probability){
	int srcIndex = getIndex(src);
	int destIndex = getIndex(dest);

	if(srcIndex <0 ){
		throw std::invalid_argument("addTransition: Src could not be found in the mapping.");
	}

	if(destIndex <0){
		throw std::invalid_argument("addTransition: Dest could not be found in the mapping.");
	}

	setTransition(srcIndex,destIndex,probability);
}

void HMMCompiled::addEmission(boost::shared_ptr<HMMNode> src, const std::string& token, double probability){
	int index = getIndex(src);

	if(index < 0){
		throw std::invalid_argument("addEmission: Src could not be found in the mapping.");
	}

	_emissions[index].emplace(token,probability);
}

double HMMCompiled::getTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest){
	int srcIndex = getIndex(src);
	int destIndex = getIndex(dest);

	if(srcIndex <0 ){
		throw std::invalid_argument("getTransition: Src could not be found in the mapping.");
	}

	if(destIndex <0){
		throw std::invalid_argument("getTransition: Dest could not be found in the mapping.");
	}

	return getTransition(srcIndex, destIndex);
}

double HMMCompiled::getEmission(boost::shared_ptr<HMMNode> src, const std::string & token){
	int srcIndex = getIndex(src);

	if(srcIndex <0 ){
		throw std::invalid_argument("getEmission: Src could not be found in the mapping.");
	}

	return _emissions[srcIndex].at(token);
}

std::string HMMCompiled::toString() const{
	std::stringstream ss;

	ss << "Transitions:" << std::endl;


	ss << std::setw(5) << " ";
	for(int i =0; i< _numberNodes;i++){
		ss << std::setw(5) <<  i;
	}

	ss << std::endl;

	for(int i=0; i< _numberNodes; i++){
		ss << std::setw(5) <<  i;

		for(int j =0; j< _numberNodes; j++){
			ss << std::setw(5) << std::setprecision(4) << getTransition(i,j);
		}
		ss << std::endl;
	}

	ss << std::endl;

	ss << "Emissions:" << std::endl;

	for(int i=0; i < _numberNodes; i++){
		ss << i  << ":";
		for(boost::unordered_map<std::string,double>::const_iterator it = _emissions[i].begin();
				it != _emissions[i].end(); ++it){
			ss << it->first << "=" << it->second << " ";
		}
		ss << std::endl;
	}


	return ss.str();
}


