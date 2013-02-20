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
#include <ctime>
#include <limits>

#include <boost/heap/fibonacci_heap.hpp>

#include "nullPtr.hpp"
#include "Exceptions.hpp"
#include "Pair.hpp"

HMMCompiled::HMMCompiled():
	_numberNodes(0),_transitions(NULL),_emissions(NULL),_initialDistribution(NULL),_constantTransitions(NULL),
	_counter(0),_random(boost::random::mt19937(time(NULL))){
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

bool HMMCompiled::isRandom() const{
	for(int i =0; i<_numberNodes*_numberNodes;i++){
		if(_transitions[i] < 0){
			return true;
		}
	}

	for(int i =0; i<_numberNodes;i++){
		if(_initialDistribution[i] <0){
			return true;
		}

		for(boost::unordered_map<std::string,EmissionInfo>::const_iterator it = _emissions[i].begin();
				it != _emissions[i].end(); ++it){
			if(it->second._prob <0){
				return true;
			}
		}
	}

	return false;
}

void HMMCompiled::initialize(int numberNodes){
	_numberNodes = numberNodes;

	_transitions = new double[numberNodes*numberNodes];
	_initialDistribution = new double[numberNodes];
	_emissions = new boost::unordered_map<std::string,EmissionInfo>[numberNodes];
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

void HMMCompiled::addInitialDistribution(boost::shared_ptr<HMMNode> node, double probability){
	int index = getIndex(node);

	_initialDistribution[index] = probability;
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

void HMMCompiled::addEmission(boost::shared_ptr<HMMNode> src, const std::string& token, double probability, bool constant){
	int index = getIndex(src);

	if(index < 0){
		throw std::invalid_argument("addEmission: Src could not be found in the mapping.");
	}

	_emissions[index].emplace(token,EmissionInfo(probability,constant));
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

double HMMCompiled::getEmission(boost::shared_ptr<HMMNode> src, const std::string & token) const{
	int srcIndex = getIndex(src);

	if(srcIndex <0 ){
		throw std::invalid_argument("getEmission: Src could not be found in the mapping.");
	}

	return _emissions[srcIndex].at(token)._prob;
}

double HMMCompiled::getEmission(int id, const std::string &token) const{
	if(_emissions[id].count(token) >0){
		return _emissions[id].at(token)._prob;
	}else{
		return 0;
	}
}

std::string HMMCompiled::toString() const{
	std::stringstream ss;

	ss << "Transitions:" << std::endl;


	ss << std::setw(7) << "  ";
	for(int i =0; i< _numberNodes;i++){
		ss << std::setw(7) <<  i;
	}

	ss << std::endl;

	for(int i=0; i< _numberNodes; i++){
		ss << std::setw(7) <<  i << " ";

		for(int j =0; j< _numberNodes; j++){
			ss << std::setw(7) << std::setprecision(4) << getTransition(i,j);
		}
		ss << std::endl;
	}

	ss << std::endl;

	ss << "Emissions:" << std::endl;

	for(int i=0; i < _numberNodes; i++){
		ss << i  << ":";
		for(boost::unordered_map<std::string,EmissionInfo>::const_iterator it = _emissions[i].begin();
				it != _emissions[i].end(); ++it){
			ss << it->first << "=" << it->second._prob << " ";
		}
		ss << std::endl;
	}

	ss << std::endl;

	ss << "Initial distribution:" << std::endl;

	for(int i =0; i< _numberNodes; i++){
		ss << i << ":" << _initialDistribution[i] << std::endl;
	}

	return ss.str();
}

/**
 * This function calculates the traversal order of silent states for the HMM algorithm.
 * If no such order can be found, it throws an exception.
 */
void HMMCompiled::finishCompilation(){
	boost::unordered_map<int, std::vector<int> > deps;
	boost::unordered_map<int, boost::heap::fibonacci_heap<Pair<int>,boost::heap::compare<std::greater<Pair<int> > > >::handle_type > mapping;
	boost::unordered_map<int,int> incomingEdges;
	boost::heap::fibonacci_heap<Pair<int>, boost::heap::compare<std::greater<Pair<int> > > >::handle_type handle;
	boost::heap::fibonacci_heap<Pair<int>, boost::heap::compare<std::greater<Pair<int> > > > heap;

	for(boost::unordered_set<int>::const_iterator it = _silentStates.begin();
			it!= _silentStates.end(); ++it){
		incomingEdges[*it] = 0;
	}

	for(boost::unordered_set<int>::const_iterator it = _silentStates.begin();
			it != _silentStates.end(); ++it){
		std::vector<int> children;

		for(boost::unordered_set<int>::const_iterator jt = _silentStates.begin();
				jt != _silentStates.end(); ++jt){
			if(getTransition(*it,*jt) != 0){
				if(*it == *jt){
					throw std::invalid_argument("HMM contains silent state with a self transition.");
				}
				children.push_back(*jt);

				incomingEdges[*jt]++;
			}
		}

		deps.emplace(*it,children);
	}

	for(boost::unordered_set<int>::const_iterator it = _silentStates.begin();
			it != _silentStates.end(); ++it){
		handle = heap.push(Pair<int>(incomingEdges[*it],*it));
		mapping.emplace(*it,handle);
	}

	while(!heap.empty()){
		Pair<int> pair = heap.top();
		heap.pop();

		if(pair._first != 0){
			throw std::invalid_argument("HMM contains silent states cycle.");
		}

		_silentStateOrder.push_back(pair._second);
		std::vector<int>& children = deps.at(pair._second);
		for(std::vector<int>::const_iterator it = children.begin();
				it != children.end(); ++it){
			handle = mapping.at(*it);
			(*handle)._first--;

			heap.increase(handle);
		}
	}

	for(int i=0; i< _numberNodes; i++){
		for(boost::unordered_map<std::string,EmissionInfo>::const_iterator it = _emissions[i].begin(); it!= _emissions[i].end(); ++it){
				_supersetEmissions.insert(it->first);
		}
	}
}

double HMMCompiled::forward(const std::vector<std::string>& sequence){
	double* prev = new double[_numberNodes];
	double* cur = new double[_numberNodes];
	double *temp;
	double result =0;

	std::vector<std::string>::const_iterator it = sequence.begin();

	for(int i=0; i< _numberNodes; i++){
		cur[i] = _initialDistribution[i]*getEmission(i,*it);
	}

	++it;

	for(; it != sequence.end(); ++it){
		temp = prev;
		prev = cur;
		cur = temp;

		for(int i =0; i< _numberNodes;i++){
			cur[i] = 0;
			if(!isSilent(i)){
				for(int j =0; j< _numberNodes; j++){
					cur[i] += prev[j]*getTransition(j,i)*getEmission(i,*it);
				}
			}
		}

		for(std::vector<int>::const_iterator order = _silentStateOrder.begin();
				order != _silentStateOrder.end(); ++order){
			int node = *order;
			for(int j =0; j< _numberNodes; j++){
				cur[node] += cur[j]*getTransition(j,node);
			}
		}
	}

	for(int i =0; i< _numberNodes; i++){
		result += cur[i];
	}

	delete [] prev;
	delete [] cur;

	return result;
}

double HMMCompiled::backward(const std::vector<std::string>& sequence){
	double* prev = new double[_numberNodes];
	double* cur = new double[_numberNodes];
	double *temp;
	double result = 0;

	for(int i =0; i < _numberNodes; i++){
		cur[i] = 1;
	}

	for(int k = sequence.size()-1; k>0; k--){
		temp = prev;
		prev = cur;
		cur = temp;

		for(int i =0; i<_numberNodes; i++){
			cur[i] = 0;
			if(!isSilent(i)){
				for(int j =0; j< _numberNodes; j++){
					cur[i] += prev[j]*getEmission(j,sequence[k])*getTransition(i,j);
				}
			}
		}

		for(int i =_silentStateOrder.size()-1;i>=0;i--){
			int node = _silentStateOrder[i];
			for(int j=0; j< _numberNodes;j++){
				cur[node] += cur[j]*getTransition(node,j);
			}
		}
	}

	temp = prev;
	prev = cur;
	cur = temp;

	for(int i =0; i< _numberNodes;i++){
		cur[i] = 0;
		if(!isSilent(i)){
			cur[i]= getEmission(i,sequence[0])*prev[i];
		}
	}

	for(int i= _silentStateOrder.size()-1; i>=0;i--){
		int node = _silentStateOrder[i];
		for(int j =0; j< _numberNodes; j++){
			cur[i] += getTransition(node,j)*cur[j];
		}
	}

	for(int i =0; i< _numberNodes; i++){
		result += cur[i]*_initialDistribution[i];
	}

	delete [] prev;
	delete [] cur;

	return result;
}

void HMMCompiled::viterbi(const std::vector<std::string>& sequence, std::vector<int>& stateSequence){
	double* prev = new double[_numberNodes];
	double* cur = new double[_numberNodes];
	int* backtrack = new int[_numberNodes*(sequence.size()-1)];
	double *temp;
	double maxProb;
	int maxPred;
	int counter =0;

	std::vector<std::string>::const_iterator it = sequence.begin();

	for(int i=0; i< _numberNodes; i++){
		cur[i] = _initialDistribution[i]*getEmission(i,*it);
	}

	for(++it;it != sequence.end(); ++it,counter++){
		temp = prev;
		prev = cur;
		cur = temp;

		for(int i=0; i< _numberNodes;i++){
			cur[i] =0;
			if(!isSilent(i)){
				maxProb = -1;
				maxPred = -1;
				for(int j=0; j< _numberNodes;j++){
					if(maxProb < prev[j]*getTransition(j,i)*getEmission(i,*it)){
						maxProb = prev[j]*getTransition(j,i)*getEmission(i,*it);
						maxPred = j;
					}
				}

				cur[i] = maxProb;
				backtrack[counter*_numberNodes+i] = maxPred;
			}
		}

		for(std::vector<int>::const_iterator jt = _silentStateOrder.begin(); jt != _silentStateOrder.end(); ++jt){
			maxProb = -1;
			maxPred = -1;
			for(int j =0; j< _numberNodes; j++){
				if(maxProb < cur[j]*getTransition(j,*jt)){
					maxProb = cur[j]*getTransition(j,*jt);
					maxPred = j;
				}
			}

			cur[*jt] = maxProb;
			backtrack[counter*_numberNodes+*jt] = maxPred;
		}
	}

	maxProb = -1;
	maxPred = -1;

	for(int i =0; i< _numberNodes; i++){
		if(maxProb < cur[i]){
			maxProb = cur[i];
			maxPred = i;
		}
	}

	stateSequence.push_back(maxPred);

	for(int i = counter-1; i >=0;i--){
		do{
			stateSequence.push_back(backtrack[i*_numberNodes+maxPred]);
			maxPred = backtrack[i*_numberNodes+maxPred];
		}while(isSilent(maxPred));
	}

	std::reverse(stateSequence.begin(), stateSequence.end());

	delete [] prev;
	delete [] cur;
	delete [] backtrack;
}

void HMMCompiled::baumWelch(const std::list<std::vector<std::string> >& trainingset, double threshold){
	double* cTransitions = new double[_numberNodes*_numberNodes];
	boost::unordered_map<std::string,double>* cEmissions = new boost::unordered_map<std::string,double>[_numberNodes];
	double* cInitial = new double[_numberNodes];
	double maxDiff;
	double prob;
	bool initialRun = true;

	do{
		maxDiff = 0;
		for(int i = 0; i<_numberNodes*_numberNodes; i++){
			cTransitions[i] = 0;
		}

		for(int i =0; i< _numberNodes;i++){
			cInitial[i] = 0;
		}

		cEmissions->clear();

		for(std::list<std::vector<std::string> >::const_iterator it = trainingset.begin(); it != trainingset.end(); ++it){
			double * forward = new double[_numberNodes*(it->size())];
			double * backward = new double[_numberNodes*(it->size())];
			double probWord = 0;

			for(int i=0; i<_numberNodes*(it->size()); i++){
				forward[i] = 0;
				backward[i] = 0;
			}

			if(initialRun)
				_supersetEmissions.insert(it->at(0));

			//calculate forward function
			for(int i=0; i< _numberNodes;i++){
				forward[i] = _initialDistribution[i]*getEmission(i,it->at(0));
			}



			for(int c =1; c < it->size(); c++){
				if(initialRun)
					_supersetEmissions.insert(it->at(c));

				for(int i=0; i< _numberNodes; i++){
					forward[_numberNodes*c+i] = 0;
					if(!isSilent(i)){
						for(int j =0; j< _numberNodes;j++){
							forward[_numberNodes*c+i] += forward[_numberNodes*(c-1)+j]*getEmission(i,it->at(c))*getTransition(j,i);
						}
					}
				}

				for(int i =0; i< _silentStateOrder.size(); i++){
					int node = _silentStateOrder[i];

					for(int j =0; j< _numberNodes; j++){
						forward[_numberNodes*c+node] += forward[_numberNodes*c+j]*getTransition(j,node);
					}
				}
			}

			for(int i = 0; i < _numberNodes; i++){
				probWord += forward[_numberNodes*(it->size()-1)+i];
			}

			//calculate backward function
			for(int i=0; i<_numberNodes;i++){
				backward[i+_numberNodes*(it->size()-1)] = 1;
			}

			for(int c = it->size()-2; c>=0; c--){
				for(int i =0; i< _numberNodes; i++){
					backward[i+_numberNodes*c] = 0;
					if(!isSilent(i)){
						for(int j =0; j< _numberNodes; j++){
							backward[i+_numberNodes*c] += backward[j+_numberNodes*(c+1)]*getTransition(i,j)*getEmission(j,it->at(c+1));
						}
					}
				}

				for(int i = _silentStateOrder.size()-1; i>=0; i--){
					int node = _silentStateOrder[i];
					for(int j =0; j< _numberNodes; j++){
						backward[node+_numberNodes*c] += backward[j+_numberNodes*c]*getTransition(node,j);
					}
				}
			}

			// calculate contributions
			// transitions
			for(int i =0; i<_numberNodes;i++){
				for(int j = 0; j < _numberNodes; j++){
					double temp =0;
					if(isSilent(j)){
						for(int t = 0; t < it->size(); t++){
							temp += forward[t*_numberNodes +i]*backward[t*_numberNodes + j];
						}
					}else{
						for(int t = 0; t < it->size()-1; t++){
							temp += forward[t*_numberNodes + i]*backward[(t+1)*_numberNodes + j]*getEmission(j,it->at(t+1));
						}
					}

					cTransitions[i*_numberNodes + j] += getTransition(i,j)*temp/probWord;
				}
			}

			// emissions
			for(int i =0; i< _numberNodes; i++){
				if(!isSilent(i)){
					for(int t = 0; t < it->size(); t++){
						cEmissions[i][it->at(t)] += forward[t*_numberNodes+i]*backward[t*_numberNodes+i]/probWord;
					}
				}
			}

			// initial distribution
			for(int i =0; i<_numberNodes;i++){
				cInitial[i] += forward[i]*backward[i]/probWord;
			}

			delete [] forward;
			delete [] backward;
		}

		// smoothing by pseudo counts
		for(int i =0; i< _numberNodes; i++){
			for(boost::unordered_set<std::string>::const_iterator it = _supersetEmissions.begin(); it != _supersetEmissions.end();++it){
				cEmissions[i][*it] += 1;
			}
		}

		// new emission probabilities
		for(int i =0; i<_numberNodes; i++){
			double sum =0;
			double constantRest = 0;

			for(boost::unordered_map<std::string,EmissionInfo>::const_iterator it= _emissions[i].begin(); it != _emissions[i].end(); ++it){
				if(it->second._constant)
					constantRest += it->second._prob;
			}

			for(boost::unordered_map<std::string,double>::const_iterator it = cEmissions[i].begin(); it != cEmissions[i].end(); ++it){
				sum += it->second;
			}

			for(boost::unordered_map<std::string,double>::const_iterator it = cEmissions[i].begin(); it != cEmissions[i].end(); ++it){
				prob = it->second/sum;

				if(maxDiff < std::abs(_emissions[i].at(it->first)._prob-prob)){
					maxDiff = std::abs(_emissions[i].at(it->first)._prob-prob);
				}

				if(!_emissions[i].at(it->first)._constant)
					_emissions[i][it->first]._prob = (1-constantRest)*prob;
			}
		}

		// new transition probabilities
		for(int i=0; i<_numberNodes;i++){
			double sum =0;
			double constantRest = 0;

			for(int j =0; j< _numberNodes;j++){
				if(_constantTransitions[i*_numberNodes + j]){
					constantRest += _transitions[i*_numberNodes +j];
				}else{
					sum += cTransitions[i*_numberNodes + j];
				}
			}

			for(int j =0; j<_numberNodes; j++){
				if(!_constantTransitions[i*_numberNodes+j]){
					prob = (1-constantRest)*cTransitions[i*_numberNodes+j]/sum;
					if(maxDiff < std::abs(_transitions[i*_numberNodes+j]-prob)){
						maxDiff = std::abs(_transitions[i*_numberNodes+j]-prob);
					}
					_transitions[i*_numberNodes + j] = prob;
				}
			}
		}

		// new initial distribution
		double sum = 0;
		for(int i =0; i < _numberNodes; i++){
			sum += cInitial[i];
		}

		for(int i=0; i< _numberNodes; i++){
			prob = cInitial[i]/sum;
			if(maxDiff < std::abs(_initialDistribution[i]-prob)){
				maxDiff = std::abs(_initialDistribution[i]-prob);
			}
			_initialDistribution[i] = prob;
		}

		initialRun = false;
	}while(maxDiff > threshold);

	delete [] cTransitions;
	delete [] cEmissions;
	delete [] cInitial;
}

void HMMCompiled::initProbabilities(){
	//initial probabilities
	double constant = 0;
	double sum = 0;
	for(int i =0; i< _numberNodes;i++){
		if(_initialDistribution[i] >= 0){
			constant += _initialDistribution[i];
		}else{
			do{
				_initialDistribution[i] = -_random();
			}while(_initialDistribution[i] == 0);
			sum += _initialDistribution[i];
		}
	}

	for(int i =0; i <_numberNodes;i++){
		if(_initialDistribution[i] < 0){
			_initialDistribution[i] *= (1-constant)/sum;
		}
	}

	//transition probabilities
	for(int i =0; i < _numberNodes; i++){
		sum =0;
		constant = 0;

		for(int j =0; j<_numberNodes;j++){
			if(_transitions[i*_numberNodes+j] >= 0){
				constant += _transitions[i*_numberNodes + j];
			}else{
				do{
					_transitions[i*_numberNodes + j] = -_random();
				}while(_transitions[i*_numberNodes +j] == 0);
				sum += _transitions[i*_numberNodes +j];
			}
		}

		for(int j =0; j<_numberNodes;j++){
			if(_transitions[i*_numberNodes +j] <0){
				_transitions[i*_numberNodes +j] *= (1-constant)/sum;
			}
		}
	}

	//emission probabilities
	for(int i =0; i<_numberNodes; i++){
		sum =0;
		constant = 0;

		for(boost::unordered_map<std::string,EmissionInfo>::iterator it = _emissions[i].begin(); it!= _emissions[i].end(); ++it){
			if(it->second._prob >= 0){
				constant += it->second._prob;
			}else{
				do{
					it->second._prob = -_random();
				}while(it->second._prob == 0);

				sum += it->second._prob;
			}
		}

		for(boost::unordered_map<std::string,EmissionInfo>::iterator it = _emissions[i].begin(); it!=_emissions[i].end(); ++it){
			if(it->second._prob < 0){
				it->second._prob *= (1-constant)/sum;
			}
		}
	}
}

void HMMCompiled::simulate(int length, std::vector<std::string>& sequence, std::vector<int>& states){
	int state = getState(_initialDistribution);
	int counter = 0;
	states.push_back(state);

	if(!isSilent(state)){
		std::string emission = getEmission(_emissions[state]);
		sequence.push_back(emission);
		counter++;
	}

	while( counter < length){
		state = getState(_transitions+(state*_numberNodes));

		states.push_back(state);
		if(!isSilent(state)){
			std::string emission = getEmission(_emissions[state]);
			sequence.push_back(emission);
			counter++;
		}
	}
}

int HMMCompiled::getState(double* distribution){
	double rvalue = _random();
	double sum =0;
	int result = 0;

	while(result < _numberNodes && sum+distribution[result] < rvalue){
		sum += distribution[result];
		result++;
	}

	return result;
}

std::string HMMCompiled::getEmission(boost::unordered_map<std::string,EmissionInfo>& emissions){
	double rvalue = _random();
	double sum =0;


	boost::unordered_map<std::string,EmissionInfo>::const_iterator it = emissions.begin();

	while(it != emissions.end() && sum+it->second._prob < rvalue){
		sum += it->second._prob;
		++it;
	}

	return it->first;
}

void HMMCompiled::copy(boost::shared_ptr<HMMCompiled> dst){
	dst->clear();
	dst->_numberNodes = _numberNodes;

	dst->_transitions = new double[_numberNodes*_numberNodes];
	std::memcpy(dst->_transitions,_transitions,sizeof(double)*_numberNodes*_numberNodes);

	dst->_emissions = new boost::unordered_map<std::string,EmissionInfo>[_numberNodes];
	for(int i =0; i< _numberNodes; i++){
		dst->_emissions[i] = _emissions[i];
	}

	dst->_supersetEmissions = _supersetEmissions;
	dst->_silentStateOrder = _silentStateOrder;
	dst->_silentStates = _silentStates;

	dst->_initialDistribution = new double[_numberNodes];
	std::memcpy(dst->_initialDistribution,_initialDistribution,sizeof(double)*_numberNodes);

	dst->_constantTransitions = new bool[_numberNodes*_numberNodes];
	std::memcpy(dst->_constantTransitions,_constantTransitions,sizeof(bool)*_numberNodes*_numberNodes);

	dst->_int2Node = _int2Node;
	dst->_node2Int = _node2Int;

	dst->_counter = _counter;

	dst->_random.base().seed((unsigned int)(_random()*std::numeric_limits<unsigned int>::max())+time(NULL));
}


