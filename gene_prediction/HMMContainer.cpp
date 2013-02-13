/*
 * HMMContainer.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMMContainer.hpp"
#include "Exceptions.hpp"
#include "nullPtr.hpp"
#include "HMM.hpp"
#include <sstream>
#include <regex>

HMMContainer::HMMContainer(int id,std::string name) : HMMNode(id,name),_startNode(-1),_endNode(-1){
}

HMMContainer::HMMContainer(int id, std::string name,const Transition& transitions, const Emission& emissions):
		HMMNode(id, name, transitions, emissions),_startNode(-1),_endNode(-1){
}

HMMContainer::~HMMContainer(){
}

void HMMContainer::addTransition(const HMMTransition& transition){
	throw OperationNotSupportedException("HMMContainer does not support the addTransition operation.");
}

void HMMContainer::removeTransition(int destination){
	throw OperationNotSupportedException("HMMContainer does not support the removeTransition operation.");
}

void HMMContainer::addEmission(const HMMEmission& emission){
	throw OperationNotSupportedException("HMMContainer does not support the addEmission operation.");
}

void HMMContainer::removeEmission(const std::string& token){
	throw OperationNotSupportedException("HMMContainer does not support the removeEmission operation.");
}

void HMMContainer::insertModel(const std::tr1::shared_ptr<HMM>& hmm){
	replaceModel(hmm);
}

std::tr1::shared_ptr<HMM> HMMContainer::replaceModel(const std::tr1::shared_ptr<HMM>& hmm){
	std::tr1::shared_ptr<HMM> oldModel = _model;

	if(oldModel){
		oldModel->removeNode(_startNode);
		oldModel->removeNode(_endNode);

		const std::tr1::unordered_set<int> & endNodes = oldModel->getEndNodes();

		for(std::tr1::unordered_set<int>::const_iterator it = endNodes.begin();
				it != endNodes.end(); ++it){
			oldModel->getNode(*it)->removeTransition(_endNode);
		}
	}

	_model = hmm;

	std::stringstream ss;

	ss << "Container ID:" << _id << " start node";
	_startNode = _model->createNode(ss.str());

	ss<< "Container ID:" << _id << " start node";
	_endNode = _model->createNode(ss.str());

	const std::tr1::unordered_set<int> &startNodes = hmm->getStartNodes();
	ptrHMMNode startNode = _model->getNode(_startNode);
	ptrHMMNode endNode = _model->getNode(_endNode);

	startNode->setSilent(true);
	endNode->setSilent(true);

	auto numberStartNodes = startNodes.size();

	for(std::tr1::unordered_set<int>::const_iterator it = startNodes.begin();
			it != startNodes.end(); ++it){
		startNode->addTransition(HMMTransition(1.0/numberStartNodes,*it,false));
	}

	const std::tr1::unordered_set<int> &endNodes = hmm->getEndNodes();

	auto numberEndNodes = startNodes.size();

	HMMTransition endTransition(1.0/numberEndNodes,_endNode,false);

	for(std::tr1::unordered_set<int>::const_iterator it = endNodes.begin();
			it != endNodes.end(); ++it){
		_model->getNode(*it)->addTransition(endTransition);
	}

	return oldModel;
}

void HMMContainer::serialize(std::ostream& os) const{
	os << "HMMContainer{" << std::endl;

	os << "ID:" << _id << std::endl;
	os << "Name:" << _name << std::endl;
	os << "Transitions:" << _transitions.size() << std::endl;

	for(std::tr1::unordered_map<int,HMMTransition>::const_iterator it = _transitions.begin();
			it != _transitions.end(); ++it){
		it->second.serialize(os);
	}

	if(_model){
		os << "Model:" << std::endl;
		_model->serialize(os);
	}else{
		os << "No model" << std::endl;
	}
	os << "StartNode:" << _startNode << std::endl;
	os << "EndNode:" << _endNode << std::endl;

	os << "}" << std::endl;
}

std::tr1::shared_ptr<HMMContainer> HMMContainer::deserialize(std::istream& is){
	int startNode;
	int endNode;
	std::tr1::shared_ptr<HMM> model;
	int id;
	std::string name;
	std::string line;
	std::smatch sm;
	std::istringstream ss;

	std::getline(is,line);

	if(!std::regex_match(line,std::regex("HMMContainer{"))){
		throw InvalidSerializationException("HMMContainer: Invalid initial key word.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("ID:(.*)"))){
		ss.str(sm[0]);
		ss >> id;
	}else{
		throw InvalidSerializationException("HMMContainer: ID cannot be deserialized.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("Name:(.*)"))){
		name = sm[0];
	}else{
		throw InvalidSerializationException("HMMContainer: Name cannot be deserialized.");
	}

	std::tr1::shared_ptr<HMMContainer> newContainer(new HMMContainer(id,name));

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("Transitions:(.*)"))){
		int numberTransitions;
		ss.str(sm[0]);
		ss >> numberTransitions;

		for(int i =0; i< numberTransitions; i++){
			newContainer->addTransition(HMMTransition::deserialize(is));
		}
	}else{
		throw InvalidSerializationException("HMMContainer: Transitions cannot be deserialized.");
	}

	std::getline(is,line);

	if(std::regex_match(line,std::regex("Model"))){
		model = HMM::deserialize(is);
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("StartNode:(.*)"))){
		ss.str(sm[0]);
		ss >> startNode;
	}else{
		throw InvalidSerializationException("HMMContainer: Start node cannot be deserialized.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("EndNode:(.*)"))){
		ss.str(sm[0]);
		ss >> endNode;
	}

	std::getline(is,line);

	newContainer->_model = model;
	newContainer->_startNode = startNode;
	newContainer->_endNode = endNode;

	return newContainer;
}




