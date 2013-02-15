/*
 * HMMContainer.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMMContainer.hpp"
#include <sstream>

#include <boost/regex.hpp>

#include "Exceptions.hpp"
#include "nullPtr.hpp"
#include "HMM.hpp"
#include "HMMTransition.hpp"
#include "HMMEmission.hpp"
#include "HMMCompiled.hpp"

HMMContainer::HMMContainer(int id,std::string name) : HMMNode(id,name),_startNode(-1),_endNode(-1){
}

HMMContainer::HMMContainer(int id, std::string name,const Transition& transitions, const Emission& emissions):
		HMMNode(id, name, transitions, emissions),_startNode(-1),_endNode(-1){
}

HMMContainer::~HMMContainer(){
	if(_model){
		extractModel();
	}
}

void HMMContainer::addEmission(const HMMEmission& emission){
	throw OperationNotSupportedException("HMMContainer does not support the addEmission operation.");
}

void HMMContainer::removeEmission(const std::string& token){
	throw OperationNotSupportedException("HMMContainer does not support the removeEmission operation.");
}

void HMMContainer::insertModel(const boost::shared_ptr<HMM>& hmm){
	replaceModel(hmm);
}

boost::shared_ptr<HMM> HMMContainer::extractModel(){
	boost::shared_ptr<HMM> oldModel = _model;

	if(oldModel){
		oldModel->removeNode(_startNode);
		oldModel->removeNode(_endNode);

		const boost::unordered_set<int> & endNodes = oldModel->getEndNodes();

		for(boost::unordered_set<int>::const_iterator it = endNodes.begin();
				it != endNodes.end(); ++it){
			oldModel->getNode(*it)->removeTransition(_endNode);
		}
	}

	_model = nullPtr;
	_startNode = -1;
	_endNode = -1;
	return oldModel;
}

boost::shared_ptr<HMM> HMMContainer::replaceModel(const boost::shared_ptr<HMM>& hmm){
	boost::shared_ptr<HMM> oldModel = extractModel();

	_model = hmm;

	std::stringstream ss;

	ss << "Container ID:" << _id << " start node";
	_startNode = _model->createNode(ss.str());

	ss.str("");

	ss<< "Container ID:" << _id << " end node";
	_endNode = _model->createNode(ss.str());

	const boost::unordered_set<int> &startNodes = hmm->getStartNodes();
	ptrHMMNode startNode = _model->getNode(_startNode);
	ptrHMMNode endNode = _model->getNode(_endNode);

	startNode->setSilent(true);
	endNode->setSilent(true);

	auto numberStartNodes = startNodes.size();

	for(boost::unordered_set<int>::const_iterator it = startNodes.begin();
			it != startNodes.end(); ++it){
		startNode->addTransition(HMMTransition(1.0/numberStartNodes,*it,false));
	}

	const boost::unordered_set<int> &endNodes = hmm->getEndNodes();

	auto numberEndNodes = startNodes.size();

	HMMTransition endTransition(1.0/numberEndNodes,_endNode,false);

	for(boost::unordered_set<int>::const_iterator it = endNodes.begin();
			it != endNodes.end(); ++it){
		_model->getNode(*it)->addTransition(endTransition);
	}

	return oldModel;
}

void HMMContainer::serialize(std::ostream& os) const{
	os << "HMMContainer{" << std::endl;

	HMMNode::serialize(os);

	if(_model){
		os << "Model:" << std::endl;
		_model->serialize(os);
	}else{
		os << "No model:" << std::endl;
	}
	os << "StartNode:" << _startNode << std::endl;
	os << "EndNode:" << _endNode << std::endl;

	os << "}" << std::endl;
}

void HMMContainer::deserialize(std::istream& is,boost::shared_ptr<HMMContainer> hmmContainer){
	int startNode;
	int endNode;
	boost::shared_ptr<HMM> model(new HMM());
	int id;
	std::string name;
	std::string line;
	boost::smatch sm;
	std::istringstream ss;

	std::getline(is,line);

	if(!boost::regex_match(line,boost::regex("HMMContainer\\{"))){
		throw InvalidSerializationException("HMMContainer: Invalid initial key word.");
	}

	HMMNode::deserialize(is,boost::static_pointer_cast<HMMNode>(hmmContainer));

	std::getline(is,line);

	if(boost::regex_match(line,boost::regex("Model:"))){
		HMM::deserialize(is,model);
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("StartNode:(.*)"))){
		ss.str(sm[1]);
		ss >> startNode;
		ss.clear();
	}else{
		throw InvalidSerializationException("HMMContainer: Start node cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("EndNode:(.*)"))){
		ss.str(sm[1]);
		ss >> endNode;
		ss.clear();
	}

	std::getline(is,line);


	hmmContainer->_model = model;
	hmmContainer->_startNode = startNode;
	hmmContainer->_endNode = endNode;
}

int HMMContainer::size() const{
	if(_model){
		return _model->size();
	}else{
		return 0;
	}
}

void HMMContainer::buildMapping(HMMCompiled& compiled){
	HMMNode::buildMapping(compiled);

	if(_model){
		for(boost::unordered_map<int,boost::shared_ptr<HMMNode> >::const_iterator it = _model->getNodes().begin();
				it != _model->getNodes().end(); ++it){
			if(_startNode != it->first){
				it->second->buildMapping(compiled);
			}
		}
	}
}

void HMMContainer::buildTransitions(HMMCompiled& compiled, HMM& hmm){
	if(_model){
		for(boost::unordered_map<int,boost::shared_ptr<HMMNode> >::const_iterator it = _model->getNodes().begin();
				it != _model->getNodes().end(); ++it){
			if(it->first == _startNode){
				compiled.addSilentNode(shared_from_this());

				for(boost::unordered_map<int,HMMTransition>::const_iterator tr = it->second->getTransition().begin();
						tr != it->second->getTransition().end(); ++tr){
					HMMTransition transition = tr->second;
					compiled.addTransition(shared_from_this(),_model->getNode(transition._destination),transition._probability);

					if(transition._constant){
						compiled.addConstantTransition(shared_from_this(),_model->getNode(transition._destination));
					}
				}

			} else if(it->first == _endNode){
				compiled.addSilentNode(it->second);

				for(boost::unordered_map<int,HMMTransition>::const_iterator tr = _transitions.begin();
						tr != _transitions.end(); ++tr){
					HMMTransition transition = tr->second;

					compiled.addTransition(it->second,hmm.getNode(transition._destination),transition._probability);

					if(transition._constant){
						compiled.addConstantTransition(it->second,hmm.getNode(transition._destination));
					}
				}
			}else{
				it->second->buildTransitions(compiled,*_model);
			}
		}
	}
}

void HMMContainer::updateValues(HMMCompiled& compiled, HMM& hmm){
	if(_model){
		for(boost::unordered_map<int,boost::shared_ptr<HMMNode> >::const_iterator it = _model->getNodes().begin();
				it != _model->getNodes().end(); ++it){
			if(it->first == _startNode){
				for(boost::unordered_map<int,HMMTransition>::iterator tr = it->second->getTransition().begin();
						tr != it->second->getTransition().end(); ++tr){
					tr->second._probability = compiled.getTransition(shared_from_this(),_model->getNode(tr->first));
				}
			}else if(it->first == _endNode){
				for(boost::unordered_map<int,HMMTransition>::iterator tr = _transitions.begin();
						tr != _transitions.end(); ++tr){
					tr->second._probability = compiled.getTransition(it->second,hmm.getNode(tr->first));
				}
			}else{
				it->second->updateValues(compiled,*_model);
			}
		}
	}
}



