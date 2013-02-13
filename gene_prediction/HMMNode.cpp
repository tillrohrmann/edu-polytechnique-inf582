/*
 * HMMNode.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMMNode.hpp"
#include <assert.h>
#include "Exceptions.hpp"
#include <sstream>
#include <regex>

HMMNode::HMMNode(int id, std::string name): _id(id),_name(name),_isSilent(false){
}

HMMNode::HMMNode(int id,std::string name,const Transition& transitions,const Emission& emissions) :
		_id(id), _name(name), _isSilent(false){
	_transitions.insert(transitions.begin(),transitions.end());
	_emissions.insert(emissions.begin(),emissions.end());
}

HMMNode::~HMMNode(){

}

void HMMNode::addTransition(const HMMTransition& transition){
	_transitions[transition._destination] = transition;
}

void HMMNode::removeTransition(int destination){
	_transitions.erase(destination);
}

void HMMNode::addEmission(const HMMEmission& emission){
	_emissions[emission._emissionToken] = emission;
}

void HMMNode::removeEmission(const std::string& token){
	_emissions.erase(token);
}

void HMMNode::insertModel(const std::tr1::shared_ptr<HMM>& hmm){
	throw OperationNotSupportedException("The operation insertModel is not supported for that type of HMMNode.");
}

std::tr1::shared_ptr<HMM> HMMNode::replaceModel(const std::tr1::shared_ptr<HMM>& hmm){
	throw OperationNotSupportedException("The operation replaceModel is not supported for that type of HMMNode.");
}

void HMMNode::serialize(std::ostream& os) const{
	os << "Node{" << std::endl;
	os << "ID:" << _id << std::endl;
	os << "Name:" << _name << std::endl;
	os << "IsSilent:" << _isSilent << std::endl;
	os << "Transitions:" << _transitions.size() << std::endl;

	for(std::tr1::unordered_map<int,HMMTransition>::const_iterator it = _transitions.begin();
			it != _transitions.end(); ++it){
		it->second.serialize(os);
	}
	os << "Emissions:" << _emissions.size() << std::endl;

	for(std::tr1::unordered_map<std::string,HMMEmission>::const_iterator it = _emissions.begin();
			it != _emissions.end(); ++it){
		it->second.serialize(os);
	}

	os << "}" << std::endl;
}

std::tr1::shared_ptr<HMMNode> HMMNode::deserialize(std::istream& is){
	std::string line;
	std::istringstream ss;
	int id;
	std::string name;
	bool isSilent;
	std::smatch sm;

	std::getline(is,line);

	if(!std::regex_match(line,std::regex("Node{"))){
		throw InvalidSerializationException("HMMNode: Initial keyword misses.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("ID:(.*)"))){
		ss.str(sm[0]);
		ss >> id;
	}else{
		throw InvalidSerializationException("HMMNode: ID cannot be deserialized.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("Name:(.*)"))){
		name = sm[0];
	}else{
		throw InvalidSerializationException("HMMNode: Name cannot be deserialized.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("IsSilent:(.*)"))){
		ss.str(sm[0]);
		ss >> isSilent;
	}else{
		throw InvalidSerializationException("HMMNode: IsSilent cannot be deserialized.");
	}

	std::tr1::shared_ptr<HMMNode> newNode(new HMMNode(id,name));
	newNode->_isSilent = isSilent;

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("Transitions:(.*)"))){
		int numberTransitions;
		ss.str(sm[0]);
		ss >> numberTransitions;

		for(int i =0; i< numberTransitions; i++){
			newNode->addTransition(HMMTransition::deserialize(is));
		}
	}else{
		throw InvalidSerializationException("HMMNode: Transitions cannot be deserialized.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("Emissions:(.*)"))){
		int numberEmissions;
		ss.str(sm[0]);
		ss >> numberEmissions;

		for(int i =0; i< numberEmissions; i++){
			newNode->addEmission(HMMEmission::deserialize(is));
		}
	}else{
		throw InvalidSerializationException("HMMNode: Emissions cannot be deserialized.");
	}

	std::getline(is,line);

	return newNode;
}
