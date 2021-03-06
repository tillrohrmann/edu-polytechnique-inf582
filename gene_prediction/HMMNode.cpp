/*
 * HMMNode.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMMNode.hpp"
#include <assert.h>
#include <sstream>

#include <boost/regex.hpp>

#include "HMMEmission.hpp"
#include "HMMTransition.hpp"
#include "HMMCompiled.hpp"
#include "HMM.hpp"
#include "nullPtr.hpp"

HMMNode::HMMNode(int id,const std::string& name): _id(id),_name(name),_isSilent(false),
_constantEmissions(false),_constantTransitions(false), _constantEmissionSet(false){
}

HMMNode::HMMNode(int id,const std::string& name,const Transition& transitions,const Emission& emissions,
		bool constantTransitions, bool constantEmissions, bool constantEmissionSet) :
		_id(id), _name(name), _isSilent(false),_constantEmissions(constantEmissions),
		_constantTransitions(constantTransitions), _constantEmissionSet(constantEmissionSet){
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

void HMMNode::serialize(std::ostream& os) const{
	os << "Node{" << std::endl;
	os << "ID:" << _id << std::endl;
	os << "Name:" << _name << std::endl;
	os << "IsSilent:" << _isSilent << std::endl;
	os << "ConstantTransitions:" << _constantTransitions << std::endl;
	os << "ConstantEmissions:" << _constantEmissions << std::endl;
	os << "ConstantEmissionSet:" << _constantEmissionSet << std::endl;
	os << "Transitions:" << _transitions.size() << std::endl;

	for(boost::unordered_map<int,HMMTransition>::const_iterator it = _transitions.begin();
			it != _transitions.end(); ++it){
		it->second.serialize(os);
	}
	os << "Emissions:" << _emissions.size() << std::endl;

	for(boost::unordered_map<std::string,HMMEmission>::const_iterator it = _emissions.begin();
			it != _emissions.end(); ++it){
		it->second.serialize(os);
	}

	os << "}" << std::endl;
}

void HMMNode::deserialize(std::istream& is,boost::shared_ptr<HMMNode> hmmNode){
	std::string line;
	std::istringstream ss;
	int id;
	std::string name;
	bool isSilent;
	bool constantTransitions;
	bool constantEmissions;
	bool constantEmissionSet;
	boost::smatch sm;

	std::getline(is,line);

	if(!boost::regex_match(line,boost::regex("Node\\{"))){
		throw std::invalid_argument("HMMNode: Initial keyword misses.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("ID:(.*)"))){
		ss.str(sm[1]);
		ss >> id;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMNode: ID cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Name:(.*)"))){
		name = sm[1];
	}else{
		throw std::invalid_argument("HMMNode: Name cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("IsSilent:(.*)"))){
		ss.str(sm[1]);
		ss >> isSilent;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMNode: IsSilent cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("ConstantTransitions:(.*)"))){
		ss.str(sm[1]);
		ss >> constantTransitions;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMNode: ConstantTransitions cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("ConstantEmissions:(.*)"))){
		ss.str(sm[1]);
		ss >> constantEmissions;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMNode: ConstantEmissions cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("ConstantEmissionSet:(.*)"))){
		ss.str(sm[1]);
		ss >> constantEmissionSet;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMNode: ConstantEmissionSet cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Transitions:(.*)"))){
		int numberTransitions;
		ss.str(sm[1]);
		ss >> numberTransitions;
		ss.clear();

		for(int i =0; i< numberTransitions; i++){
			hmmNode->addTransition(HMMTransition::deserialize(is));
		}
	}else{
		throw std::invalid_argument("HMMNode: Transitions cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Emissions:(.*)"))){
		int numberEmissions;
		ss.str(sm[1]);
		ss >> numberEmissions;
		ss.clear();

		for(int i =0; i< numberEmissions; i++){
			hmmNode->addEmission(HMMEmission::deserialize(is));
		}
	}else{
		throw std::invalid_argument("HMMNode: Emissions cannot be deserialized.");
	}

	std::getline(is,line);

	hmmNode->_id =id;
	hmmNode->_name = name;
	hmmNode->_isSilent = isSilent;
	hmmNode->_constantTransitions = constantTransitions;
	hmmNode->_constantEmissions = constantEmissions;
	hmmNode->_constantEmissionSet = constantEmissionSet;
}

boost::shared_ptr<HMMNode> HMMNode::deserialize(std::istream& is){
	boost::shared_ptr<HMMNode> node(new HMMNode());
	HMMNode::deserialize(is,node);
	return node;
}

void HMMNode::serialize(std::ostream& os, boost::shared_ptr<HMMNode> node){
	node->serialize(os);
}

void HMMNode::buildMapping(HMMCompiled& compiled){
	compiled.addMapping(shared_from_this());
}

void HMMNode::buildTransitions(HMMCompiled& compiled, HMM& hmm){
	if(_isSilent){
		compiled.addSilentNode(shared_from_this());
	}

	for(Transition::const_iterator it = _transitions.begin(); it != _transitions.end(); ++it){
		HMMTransition transition = it->second;

		compiled.addTransition(shared_from_this(),hmm.getNode(transition._destination),transition._probability);
	}

	for(Emission::const_iterator it = _emissions.begin(); it != _emissions.end(); ++it){
		HMMEmission emission = it->second;

		compiled.addEmission(shared_from_this(),emission._emissionToken,emission._probabiltiy);
	}
}

void HMMNode::updateValues(HMMCompiled& compiled, HMM& hmm){
	for(boost::unordered_map<int,HMMTransition>::iterator it = _transitions.begin();
			it != _transitions.end(); ++it){
		it->second._probability = compiled.getTransition(shared_from_this(),hmm.getNode(it->first));
	}

	for(boost::unordered_map<std::string, HMMEmission>::iterator it = _emissions.begin();
			it != _emissions.end(); ++it){
		it->second._probabiltiy = compiled.getEmission(shared_from_this(),it->second._emissionToken);
	}
}

void HMMNode::substituteEmissions(const boost::unordered_map<std::string, std::string>& substitution){
	boost::unordered_map<std::string, HMMEmission> newValues;
	for(boost::unordered_map<std::string, HMMEmission>::const_iterator it = _emissions.begin(); it != _emissions.end();){
		if(substitution.count(it->first) > 0){
			newValues.emplace(substitution.at(it->first),HMMEmission(substitution.at(it->first),it->second._probabiltiy));
			it = _emissions.erase(it);
		}else{
			++it;
		}
	}

	_emissions.insert(newValues.begin(),newValues.end());
}
