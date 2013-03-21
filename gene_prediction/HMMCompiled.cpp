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
#include "Pair.hpp"
#include "HMMNode.hpp"
#include "HMM.hpp"

HMMCompiled::HMMCompiled() :
		_numberNodes(0), _mapTransitions(NULL), _imapTransitions(NULL), _constantTransitionNodes(
				NULL), _constantEmissionNodes(NULL), _constantEmissionSetNodes(
				NULL), _emissions(NULL), _initialDistribution(NULL), _counter(
				0), _random(boost::random::mt19937(time(NULL))) {
}

HMMCompiled::~HMMCompiled() {
	clear();
}

void HMMCompiled::clear() {
	_numberNodes = 0;

	delete[] _mapTransitions;
	delete[] _imapTransitions;
	delete[] _emissions;
	delete[] _initialDistribution;
	delete[] _constantTransitionNodes;
	delete[] _constantEmissionNodes;
	delete[] _constantEmissionSetNodes;

	_silentStates.clear();
	_int2Node.clear();
	_node2Int.clear();

	_counter = 0;
}

bool HMMCompiled::isRandom() const {
	for (int i = 0; i < _numberNodes; i++) {
		for (boost::unordered_map<int, double>::const_iterator jt =
				_mapTransitions[i].begin(); jt != _mapTransitions[i].end();
				++jt) {
			if (jt->second < 0)
				return true;
		}
	}

	for (int i = 0; i < _numberNodes; i++) {
		if (_initialDistribution[i] < 0) {
			return true;
		}

		for (boost::unordered_map<std::string, double>::const_iterator it =
				_emissions[i].begin(); it != _emissions[i].end(); ++it) {
			if (it->second < 0) {
				return true;
			}
		}
	}

	return false;
}

void HMMCompiled::initialize(int numberNodes) {
	clear();
	_numberNodes = numberNodes;

	_mapTransitions = new boost::unordered_map<int, double>[numberNodes];
	_imapTransitions = new boost::unordered_map<int, double>[numberNodes];
	_initialDistribution = new double[numberNodes];
	_emissions = new boost::unordered_map<std::string, double>[numberNodes];
	_constantEmissionNodes = new bool[numberNodes];
	_constantTransitionNodes = new bool[numberNodes];
	_constantEmissionSetNodes = new bool[numberNodes];

	for (int i = 0; i < numberNodes; i++) {
		_initialDistribution[i] = 0;
		_constantEmissionNodes[i] = false;
		_constantTransitionNodes[i] = false;
		_constantEmissionSetNodes[i] = false;
	}
}

boost::shared_ptr<HMMNode> HMMCompiled::getNode(int index) const {
	if (_int2Node.count(index) > 0) {
		return _int2Node.at(index);
	} else {
		return nullPtr;
	}
}

int HMMCompiled::getIndex(boost::shared_ptr<HMMNode> node) const {
	if (_node2Int.count(node) > 0) {
		return _node2Int.at(node);
	} else {
		return -1;
	}
}

bool HMMCompiled::hasConstantTransitions(int node) const {
	if (std::abs(node) >= _numberNodes) {
		throw std::invalid_argument("Node index out of range");
	}

	return _constantTransitionNodes[node];
}

bool HMMCompiled::hasConstantEmissions(int node) const {
	if (std::abs(node) >= _numberNodes) {
		throw std::invalid_argument("Node index out of range");
	}

	return _constantEmissionNodes[node];
}

bool HMMCompiled::hasConstantEmissionSet(int node) const {
	if (std::abs(node) >= _numberNodes) {
		throw std::invalid_argument("Node index out of range");
	}

	return _constantEmissionSetNodes[node];
}

/**
 * This functions adds the node node to this HMMCompiled and
 * thus creates a new entry in the mapping.
 */
void HMMCompiled::addMapping(boost::shared_ptr<HMMNode> node) {
	if (_node2Int.count(node) == 0) {
		_constantTransitionNodes[_counter] = node->constantTransitions();
		_constantEmissionNodes[_counter] = node->constantEmissions();
		_constantEmissionSetNodes[_counter] = node->constantEmissionSet();

		_node2Int[node] = _counter;
		_int2Node[_counter++] = node;
	}
}

void HMMCompiled::addInitialDistribution(boost::shared_ptr<HMMNode> node,
		double probability) {
	int index = getIndex(node);

	_initialDistribution[index] = probability;
}

void HMMCompiled::addSilentNode(boost::shared_ptr<HMMNode> node) {
	int index = getIndex(node);

	if (index < 0) {
		throw std::invalid_argument("Node could not be found in the mapping.");
	}
	_silentStates.emplace(index);
}

void HMMCompiled::addTransition(boost::shared_ptr<HMMNode> src,
		boost::shared_ptr<HMMNode> dest, double probability) {
	int srcIndex = getIndex(src);
	int destIndex = getIndex(dest);

	if (srcIndex < 0) {
		throw std::invalid_argument(
				"addTransition: Src could not be found in the mapping.");
	}

	if (destIndex < 0) {
		throw std::invalid_argument(
				"addTransition: Dest could not be found in the mapping.");
	}

	setTransition(srcIndex, destIndex, probability);
}

void HMMCompiled::addEmission(boost::shared_ptr<HMMNode> src,
		const std::string& token, double probability) {
	int index = getIndex(src);

	if (index < 0) {
		throw std::invalid_argument(
				"addEmission: Src could not be found in the mapping.");
	}

	_emissions[index].emplace(token, probability);
}

double HMMCompiled::getInitialDistribution(
		boost::shared_ptr<HMMNode> node) const {
	int index = getIndex(node);

	if (index < 0) {
		throw std::invalid_argument(
				"getInitialDistribution: Src could not be found in the mapping.");
	}

	return getInitialDistribution(index);
}

double HMMCompiled::getTransition(boost::shared_ptr<HMMNode> src,
		boost::shared_ptr<HMMNode> dest) {
	int srcIndex = getIndex(src);
	int destIndex = getIndex(dest);

	if (srcIndex < 0) {
		throw std::invalid_argument(
				"getTransition: Src could not be found in the mapping.");
	}

	if (destIndex < 0) {
		throw std::invalid_argument(
				"getTransition: Dest could not be found in the mapping.");
	}

	return getTransition(srcIndex, destIndex);
}

double HMMCompiled::getEmission(boost::shared_ptr<HMMNode> src,
		const std::string & token) const {
	int srcIndex = getIndex(src);

	if (srcIndex < 0) {
		throw std::invalid_argument(
				"getEmission: Src could not be found in the mapping.");
	}

	return _emissions[srcIndex].at(token);
}

double HMMCompiled::getEmission(int id, const std::string &token) const {
	if (_emissions[id].count(token) > 0) {
		return _emissions[id].at(token);
	} else {
		return 0;
	}
}

double HMMCompiled::getLogEmission(int id, const std::string &token) const {
	return std::log(getEmission(id, token));
}

std::string HMMCompiled::toString() const {
	std::stringstream ss;

	ss << "Transitions:" << std::endl;

	for (int i = 0; i < _numberNodes; i++) {
		ss << _int2Node.at(i)->getName() << "=> ";

		for (boost::unordered_map<int, double>::const_iterator jt =
				_mapTransitions[i].begin(); jt != _mapTransitions[i].end();
				++jt) {
			ss << _int2Node.at(jt->first)->getName() << ":" << jt->second
					<< " ";
		}
		ss << std::endl;
	}

	ss << std::endl;

	ss << "Emissions:" << std::endl;

	for (int i = 0; i < _numberNodes; i++) {
		ss << _int2Node.at(i)->getName() << ":";
		for (boost::unordered_map<std::string, double>::const_iterator it =
				_emissions[i].begin(); it != _emissions[i].end(); ++it) {
			ss << it->first << "=" << it->second << " ";
		}
		ss << std::endl;
	}

	ss << std::endl;

	ss << "Initial distribution:" << std::endl;

	for (int i = 0; i < _numberNodes; i++) {
		ss << _int2Node.at(i)->getName() << ":" << _initialDistribution[i]
				<< std::endl;
	}

	return ss.str();
}

/**
 * This function calculates the traversal order of silent states for the HMM algorithm.
 * If no such order can be found, it throws an exception.
 */
void HMMCompiled::finishCompilation() {
	boost::unordered_map<int, std::vector<int> > deps;
	boost::unordered_map<int,
			boost::heap::fibonacci_heap<Pair<int>,
					boost::heap::compare<std::greater<Pair<int> > > >::handle_type> mapping;
	boost::unordered_map<int, int> incomingEdges;
	boost::heap::fibonacci_heap<Pair<int>,
			boost::heap::compare<std::greater<Pair<int> > > >::handle_type handle;
	boost::heap::fibonacci_heap<Pair<int>,
			boost::heap::compare<std::greater<Pair<int> > > > heap;

	for (boost::unordered_set<int>::const_iterator it = _silentStates.begin();
			it != _silentStates.end(); ++it) {
		incomingEdges[*it] = 0;
	}

	//calculate number incoming edges for every node
	for (boost::unordered_set<int>::const_iterator it = _silentStates.begin();
			it != _silentStates.end(); ++it) {
		std::vector<int> children;

		for (boost::unordered_set<int>::const_iterator jt =
				_silentStates.begin(); jt != _silentStates.end(); ++jt) {
			if (getTransition(*it, *jt) != 0) {
				if (*it == *jt) {
					throw std::invalid_argument(
							"HMM contains silent state with a self transition.");
				}
				children.push_back(*jt);

				incomingEdges[*jt]++;
			}
		}

		deps.emplace(*it, children);
	}

	// order states according to their incoming degree in increasing order
	for (boost::unordered_set<int>::const_iterator it = _silentStates.begin();
			it != _silentStates.end(); ++it) {
		handle = heap.push(Pair<int>(incomingEdges[*it], *it));
		mapping.emplace(*it, handle);
	}

	// pop successively the states from the heap. The idea is that the transition probabilities
	// of a silent state can be calculated if all the probabilities of the incoming edges have
	// been calculated. To make a long story short, we have to find a ordering of a DAG such that
	// all nodes which have a transition into this node are ordered before the respective node.
	while (!heap.empty()) {
		Pair<int> pair = heap.top();
		heap.pop();

		// Graph contains a circle --> no order possible
		if (pair._first != 0) {
			throw std::invalid_argument("HMM contains silent states cycle.");
		}

		_silentStateOrder.push_back(pair._second);
		std::vector<int>& children = deps.at(pair._second);
		for (std::vector<int>::const_iterator it = children.begin();
				it != children.end(); ++it) {
			handle = mapping.at(*it);
			(*handle)._first--;

			heap.increase(handle);
		}
	}

	for (int i = 0; i < _numberNodes; i++) {
		for (boost::unordered_map<std::string, double>::const_iterator it =
				_emissions[i].begin(); it != _emissions[i].end(); ++it) {
			_supersetEmissions.insert(it->first);
		}
	}
}

/**
 * ln(x+y) = elnsum(ln(x),ln(y))
 */
double HMMCompiled::elnsum(double x, double y) {
	if (x == -std::numeric_limits<double>::infinity()) {
		return y;
	} else if (y == -std::numeric_limits<double>::infinity()) {
		return x;
	} else if (x > y) {
		return x + std::log(1 + std::exp(y - x));
	} else {
		return y + std::log(1 + std::exp(x - y));
	}
}

double HMMCompiled::forward(const std::vector<std::string>& sequence) {
	double* prev = new double[_numberNodes];
	double* cur = new double[_numberNodes];
	double *temp;
	double result = -std::numeric_limits<double>::infinity();

	std::vector<std::string>::const_iterator it = sequence.begin();

	for (int i = 0; i < _numberNodes; i++) {
		cur[i] = getLogInitialDistribution(i) + getLogEmission(i, *it);
	}

	++it;

	for (; it != sequence.end(); ++it) {
		temp = prev;
		prev = cur;
		cur = temp;

		for (int i = 0; i < _numberNodes; i++) {
			// cur[i] = ln(0)
			cur[i] = -std::numeric_limits<double>::infinity();
			if (!isSilent(i)) {
				// forward(i,t) = sum_{j=1}^{N} forward(j,t-1)*transition(j,i)*emission(i,sequence(i))
				for (boost::unordered_map<int, double>::const_iterator jt =
						_imapTransitions[i].begin();
						jt != _imapTransitions[i].end(); ++jt) {
					cur[i] = elnsum(cur[i],
							prev[jt->first] + std::log(jt->second));
				}

				cur[i] = cur[i] + getLogEmission(i, *it);
			}
		}

		// silent states
		for (std::vector<int>::const_iterator order = _silentStateOrder.begin();
				order != _silentStateOrder.end(); ++order) {
			int node = *order;

			// forward(i,t) = sum_{j=1}^{N} forward(j,t)*transition(j,i)
			for (boost::unordered_map<int, double>::const_iterator jt =
					_imapTransitions[node].begin();
					jt != _imapTransitions[node].end(); ++jt) {
				cur[node] = elnsum(cur[node],
						cur[jt->first] + std::log(jt->second));
			}
		}
	}

	// probability that the sequence was emitted by this model
	for (int i = 0; i < _numberNodes; i++) {
		result = elnsum(result, cur[i]);
	}

	delete[] prev;
	delete[] cur;

	// log-space
	return result;
}

double HMMCompiled::backward(const std::vector<std::string>& sequence) {
	double* prev = new double[_numberNodes];
	double* cur = new double[_numberNodes];
	double *temp;
	double result = -std::numeric_limits<double>::infinity();

	for (int i = 0; i < _numberNodes; i++) {
		cur[i] = 0;
	}

	for (int k = sequence.size() - 1; k > 0; k--) {
		temp = prev;
		prev = cur;
		cur = temp;

		for (int i = 0; i < _numberNodes; i++) {
			cur[i] = -std::numeric_limits<double>::infinity();
			if (!isSilent(i)) {
				// backward(i,t-1) = sum_{j=1}^{N} backward(j,t)*emission(j,sequence(t))*transition(i,j)
				for (boost::unordered_map<int, double>::const_iterator jt =
						_mapTransitions[i].begin();
						jt != _mapTransitions[i].end(); ++jt) {
					cur[i] = elnsum(cur[i],
							prev[jt->first]
									+ getLogEmission(jt->first, sequence[k])
									+ std::log(jt->second));
				}
			}
		}

		// silent states
		for (int i = _silentStateOrder.size() - 1; i >= 0; i--) {
			int node = _silentStateOrder[i];
			// backward(i,t) = sum_{j=1}^{N} backward(j,t)*transition(i,j)
			for (boost::unordered_map<int, double>::const_iterator jt =
					_mapTransitions[node].begin();
					jt != _mapTransitions[node].end(); ++jt) {
				cur[node] = elnsum(cur[node],
						cur[jt->first] + std::log(jt->second));
			}
		}
	}

	temp = prev;
	prev = cur;
	cur = temp;

	// multiply the emission probability of the first symbol
	for (int i = 0; i < _numberNodes; i++) {
		cur[i] = -std::numeric_limits<double>::infinity();
		if (!isSilent(i)) {
			cur[i] = getLogEmission(i, sequence[0]) + prev[i];
		}
	}

	for (int i = _silentStateOrder.size() - 1; i >= 0; i--) {
		int node = _silentStateOrder[i];
		for (boost::unordered_map<int, double>::const_iterator jt =
				_mapTransitions[node].begin();
				jt != _mapTransitions[node].end(); ++jt) {
			cur[node] = elnsum(cur[node], jt->second + cur[jt->first]);
		}
	}

	// probability that this sequence was emitted by this model
	for (int i = 0; i < _numberNodes; i++) {
		result = elnsum(result, cur[i] + getLogInitialDistribution(i));
	}

	delete[] prev;
	delete[] cur;

	return result;
}

void HMMCompiled::viterbi(const std::vector<std::string>& sequence,
		std::vector<int>& stateSequence) {
	double* prev = new double[_numberNodes];
	double* cur = new double[_numberNodes];
	int* backtrack = new int[_numberNodes * (sequence.size() - 1)];
	double *temp;
	double maxProb;
	int maxPred;
	int counter = 0;

	std::vector<std::string>::const_iterator it = sequence.begin();

	for (int i = 0; i < _numberNodes; i++) {
		cur[i] = getLogInitialDistribution(i) + getLogEmission(i, *it);
	}

	for (++it; it != sequence.end(); ++it, counter++) {
		temp = prev;
		prev = cur;
		cur = temp;

		for (int i = 0; i < _numberNodes; i++) {
			cur[i] = -std::numeric_limits<double>::infinity();
			if (!isSilent(i)) {
				maxProb = -std::numeric_limits<double>::infinity();
				maxPred = -1;

				for (boost::unordered_map<int, double>::const_iterator jt =
						_imapTransitions[i].begin();
						jt != _imapTransitions[i].end(); ++jt) {
					if (maxProb < prev[jt->first] + std::log(jt->second)) {
						maxProb = prev[jt->first] + std::log(jt->second);
						maxPred = jt->first;
					}
				}

				cur[i] = maxProb + getLogEmission(i, *it);
				// store best predecessor for i
				backtrack[counter * _numberNodes + i] = maxPred;
			}
		}

		for (std::vector<int>::const_iterator jt = _silentStateOrder.begin();
				jt != _silentStateOrder.end(); ++jt) {
			maxProb = -std::numeric_limits<double>::infinity();
			maxPred = -1;

			for (boost::unordered_map<int, double>::const_iterator kt =
					_imapTransitions[*jt].begin();
					kt != _imapTransitions[*jt].end(); ++kt) {
				if (maxProb < cur[kt->first] + std::log(kt->second)) {
					maxProb = cur[kt->first] + std::log(kt->second);
					maxPred = kt->first;
				}
			}

			cur[*jt] = maxProb;
			backtrack[counter * _numberNodes + *jt] = maxPred;
		}
	}

	maxProb = -std::numeric_limits<double>::infinity();
	maxPred = -1;

	for (int i = 0; i < _numberNodes; i++) {
		if (maxProb < cur[i]) {
			maxProb = cur[i];
			maxPred = i;
		}
	}

	stateSequence.push_back(maxPred);

	// backtrack sequence
	for (int i = counter - 1; i >= 0; i--) {
		do {
			stateSequence.push_back(backtrack[i * _numberNodes + maxPred]);
			maxPred = backtrack[i * _numberNodes + maxPred];
		} while (isSilent(maxPred));
	}

	// reverse found backtracked sequence
	std::reverse(stateSequence.begin(), stateSequence.end());

	delete[] prev;
	delete[] cur;
	delete[] backtrack;
}

void HMMCompiled::internalBaumWelch(
		const std::vector<std::vector<std::string> >& trainingset,
		boost::unordered_map<int, double>* cTransitions,
		boost::unordered_map<std::string, double>* cEmissions, double* cInitial,
		bool initialRun) {

	for (std::vector<std::vector<std::string> >::const_iterator it =
			trainingset.begin(); it != trainingset.end(); ++it) {
		double * forward = new double[_numberNodes * (it->size())];
		double * backward = new double[_numberNodes * (it->size())];
		double probWord = -std::numeric_limits<double>::infinity();
		boost::unordered_map<std::string, double> temp;

		if (initialRun)
			_supersetEmissions.insert(it->at(0));

		//calculate forward function
		for (int i = 0; i < _numberNodes; i++) {
			forward[i] = getLogInitialDistribution(i)
					+ getLogEmission(i, it->at(0));
		}

		for (int c = 1; c < it->size(); c++) {
			// collect all possible outputs in _supersetEmissions for the later smoothing
			if (initialRun)
				_supersetEmissions.insert(it->at(c));

			for (int i = 0; i < _numberNodes; i++) {
				forward[_numberNodes * c + i] =
						-std::numeric_limits<double>::infinity();
				if (!isSilent(i)) {
					// forward(i,t) = sum_{j=1}^{N} forward(j,t-1)*transition(j,i)*emission(i,sequence(t))
					for (boost::unordered_map<int, double>::const_iterator jt =
							_imapTransitions[i].begin();
							jt != _imapTransitions[i].end(); ++jt) {
						forward[_numberNodes * c + i] = elnsum(
								forward[_numberNodes * c + i],
								forward[_numberNodes * (c - 1) + jt->first]
										+ std::log(jt->second));
					}

					forward[_numberNodes * c + i] =
							forward[_numberNodes * c + i]
									+ getLogEmission(i, it->at(c));
				}
			}

			for (int i = 0; i < _silentStateOrder.size(); i++) {
				int node = _silentStateOrder[i];

				for (boost::unordered_map<int, double>::const_iterator jt =
						_imapTransitions[node].begin();
						jt != _imapTransitions[node].end(); ++jt) {
					forward[_numberNodes * c + node] = elnsum(
							forward[_numberNodes * c + node],
							forward[_numberNodes * c + jt->first]
									+ std::log(jt->second));
				}
			}
		}

		// probability of this sequence being emitted by this model
		for (int i = 0; i < _numberNodes; i++) {
			probWord = elnsum(probWord,
					forward[i + _numberNodes * (it->size() - 1)]);
		}

		if (probWord == -std::numeric_limits<double>::infinity()) {
			if (initialRun) {
				std::cerr << "Data not representable by model" << std::endl;

				for (int i = 0; i < it->size(); i++) {
					std::cerr << it->at(i);
				}
				std::cerr << std::endl;

				for (int t = 0; t < it->size(); t++) {
					double s = -std::numeric_limits<double>::infinity();
					for (int i = 0; i < _numberNodes; i++) {
						s = elnsum(s, forward[i + t * _numberNodes]);
					}

					if (s == -std::numeric_limits<double>::infinity()) {
						std::cerr << "Break:" << t << std::endl;
						for (int i = std::max(0, t - 5); i <= t; i++) {
							std::cerr << it->at(i);
						}

						std::cerr << std::endl;
						break;
					}
				}
			}

			delete[] forward;
			delete[] backward;
			continue;
		}

		//calculate backward function
		for (int i = 0; i < _numberNodes; i++) {
			backward[i + _numberNodes * (it->size() - 1)] = 0;
		}

		for (int c = it->size() - 2; c >= 0; c--) {
			for (int i = 0; i < _numberNodes; i++) {
				backward[i + _numberNodes * c] =
						-std::numeric_limits<double>::infinity();
				if (!isSilent(i)) {
					// backward(i,t-1) = sum_{j=1}^{N} backward(j,t)*transition(i,j)*emission(j,sequence(t))
					for (boost::unordered_map<int, double>::const_iterator jt =
							_mapTransitions[i].begin();
							jt != _mapTransitions[i].end(); ++jt) {
						backward[i + _numberNodes * c] = elnsum(
								backward[i + _numberNodes * c],
								backward[jt->first + _numberNodes * (c + 1)]
										+ std::log(jt->second)
										+ getLogEmission(jt->first,
												it->at(c + 1)));
					}
				}
			}

			for (int i = _silentStateOrder.size() - 1; i >= 0; i--) {
				int node = _silentStateOrder[i];

				for (boost::unordered_map<int, double>::const_iterator jt =
						_mapTransitions[i].begin();
						jt != _mapTransitions[i].end(); ++jt) {
					backward[node + _numberNodes * c] = elnsum(
							backward[node + _numberNodes * c],
							backward[jt->first + _numberNodes * c]
									+ std::log(jt->second));
				}
			}
		}

		// calculate contributions
		// transitions
		for (int i = 0; i < _numberNodes; i++) {
			for (boost::unordered_map<int, double>::const_iterator jt =
					_mapTransitions[i].begin(); jt != _mapTransitions[i].end();
					++jt) {
				double numerator = -std::numeric_limits<double>::infinity();

				if (isSilent(jt->first)) {
					for (int t = 0; t < it->size() - 1; t++) {
						numerator =
								elnsum(numerator,
										forward[t * _numberNodes + i]
												+ backward[t * _numberNodes
														+ jt->first]);
					}
				} else {
					//cTransitions[i][j] = sum_{t=1}^{L} forward(i,t)*backward(j,t+1)*transition(i,j)*
					// emission(j,sequence(t+1))/Pr(sequence)
					for (int t = 0; t < it->size() - 1; t++) {
						numerator = elnsum(numerator,
								forward[t * _numberNodes + i]
										+ backward[(t + 1) * _numberNodes
												+ jt->first]
										+ getLogEmission(jt->first,
												it->at(t + 1)));
					}
				}
				cTransitions[i][jt->first] += std::exp(
						numerator + std::log(jt->second) - probWord);
			}
		}

		// emissions
		for (int i = 0; i < _numberNodes; i++) {
			if (!isSilent(i)) {
				for (boost::unordered_set<std::string>::const_iterator jt =
						_supersetEmissions.begin();
						jt != _supersetEmissions.end(); ++jt) {
					temp[*jt] = -std::numeric_limits<double>::infinity();
				}

				// cEmission[i][symbol] = sum_{t=1}^{L} forward(i,t)*backward(i,t)/Pr(sequence)*
				//	1(sequence(t)==symbol)
				for (int t = 0; t < it->size(); t++) {
					temp[it->at(t)] = elnsum(temp[it->at(t)],
							forward[t * _numberNodes + i]
									+ backward[t * _numberNodes + i]);
				}

				for (boost::unordered_map<std::string, double>::const_iterator jt =
						temp.begin(); jt != temp.end(); ++jt) {
					double prob = std::exp(jt->second - probWord);

					if (prob > 0)
						cEmissions[i][jt->first] += prob;
				}
			}
		}

		// initial distribution
		for (int i = 0; i < _numberNodes; i++) {
			cInitial[i] += std::exp(forward[i] + backward[i] - probWord);
		}

		delete[] forward;
		delete[] backward;
	}

}

void HMMCompiled::baumWelch(
		const std::vector<std::vector<std::string> >& trainingset,
		double threshold) {
	boost::unordered_map<int, double>* cTransitions = new boost::unordered_map<
			int, double>[_numberNodes];
	boost::unordered_map<std::string, double>* cEmissions =
			new boost::unordered_map<std::string, double>[_numberNodes];
	double* cInitial = new double[_numberNodes];
	double maxDiff, maxDiffTransition, maxDiffEmission, maxDiffInitial;
	double prob;
	bool initialRun = true;

	if (trainingset.size() == 0) {
		std::cerr << "Training set is empty." << std::endl;
		return;
	}

	do {
		maxDiff = 0;
		maxDiffTransition = 0;
		maxDiffEmission = 0;
		maxDiffInitial = 0;
		for (int i = 0; i < _numberNodes; i++) {
			cTransitions[i].clear();
			cEmissions[i].clear();
		}

		for (int i = 0; i < _numberNodes; i++) {
			cInitial[i] = 0;
		}

		internalBaumWelch(trainingset, cTransitions, cEmissions, cInitial,
				initialRun);

		//smoothing of transitions by pseudo counts
		for (int i = 0; i < _numberNodes; i++) {
			for (boost::unordered_map<int, double>::iterator jt =
					cTransitions[i].begin(); jt != cTransitions[i].end();
					++jt) {
				jt->second += 1;
			}
		}

		// smoothing of emissions by pseudo counts
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantEmissionSet(i)) {
				for (boost::unordered_set<std::string>::const_iterator it =
						_supersetEmissions.begin();
						it != _supersetEmissions.end(); ++it) {
					cEmissions[i][*it] += 1;
				}
			} else {
				for (boost::unordered_map<std::string, double>::iterator it =
						_emissions[i].begin(); it != _emissions[i].end();
						++it) {
					cEmissions[i][it->first] += 1;
				}
			}
		}

		// new emission probabilities
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantEmissions(i)) {
				double sum = 0;

				if (!hasConstantEmissionSet(i)) {
					for (boost::unordered_map<std::string, double>::const_iterator it =
							cEmissions[i].begin(); it != cEmissions[i].end();
							++it) {
						sum += it->second;
					}

					for (boost::unordered_map<std::string, double>::const_iterator it =
							cEmissions[i].begin(); it != cEmissions[i].end();
							++it) {
						prob = it->second / sum;
						double eProb = getEmission(i, it->first);
						if (maxDiffEmission < std::abs(eProb - prob)) {
							maxDiffEmission = std::abs(eProb - prob);
						}

						_emissions[i][it->first] = prob;
					}
				} else {
					for (boost::unordered_map<std::string, double>::const_iterator it =
							_emissions[i].begin(); it != _emissions[i].end();
							++it) {
						sum += cEmissions[i].at(it->first);
					}

					for (boost::unordered_map<std::string, double>::iterator it =
							_emissions[i].begin(); it != _emissions[i].end();
							++it) {
						prob = cEmissions[i].at(it->first) / sum;
						double eProb = it->second;
						if (maxDiffEmission < std::abs(eProb - prob)) {
							maxDiffEmission = std::abs(eProb - prob);
						}

						it->second = prob;
					}

				}
			}
		}

		// new transition probabilities
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantTransitions(i)) {
				double sum = 0;

				for (boost::unordered_map<int, double>::const_iterator jt =
						cTransitions[i].begin(); jt != cTransitions[i].end();
						++jt) {
					sum += jt->second;
				}

				for (boost::unordered_map<int, double>::iterator jt =
						_mapTransitions[i].begin();
						jt != _mapTransitions[i].end(); ++jt) {
					double prob = 0;

					if (sum != 0) {
						prob = cTransitions[i][jt->first] / sum;
					}

					if (maxDiffTransition < std::abs(jt->second - prob)) {
						maxDiffTransition = std::abs(jt->second - prob);
					}
					jt->second = prob;
					_imapTransitions[jt->first][i] = prob;

				}
			}
		}

		// new initial distribution
		double sum = 0;
		for (int i = 0; i < _numberNodes; i++) {
			sum += cInitial[i];
		}

		for (int i = 0; i < _numberNodes; i++) {
			prob = cInitial[i] / sum;
			if (maxDiffInitial < std::abs(_initialDistribution[i] - prob)) {
				maxDiffInitial = std::abs(_initialDistribution[i] - prob);
			}
			_initialDistribution[i] = prob;
		}

		initialRun = false;

		maxDiff = std::max(maxDiffInitial,
				std::max(maxDiffTransition, maxDiffEmission));

		std::cout << "MaxDiff:" << maxDiff << " Initial:" << maxDiffInitial
				<< " Transition:" << maxDiffTransition << " Emission:"
				<< maxDiffEmission << std::endl;

	} while (maxDiff > threshold);

	delete[] cTransitions;
	delete[] cEmissions;
	delete[] cInitial;
}

Analytics::AnalyticsResult HMMCompiled::baumWelch(boost::shared_ptr<HMM> hmm,
		const boost::unordered_map<std::string,
				boost::unordered_map<std::string, std::string> >& substitution,
		const boost::unordered_map<std::string,
				boost::unordered_map<std::string, std::string> >& inverseSubstitution,
		const std::vector<std::vector<std::string> >& trainingset,
		const std::vector<std::vector<std::string> >& testset,
		const std::vector<std::vector<std::string> >& annotations,
		double threshold, bool annotated) {

	boost::unordered_map<int, double>* cTransitions = new boost::unordered_map<
			int, double>[_numberNodes];
	boost::unordered_map<std::string, double>* cEmissions =
			new boost::unordered_map<std::string, double>[_numberNodes];
	double* cInitial = new double[_numberNodes];
	bool initialRun = true;
	double prob;
	double diff;
	double oldValue = -std::numeric_limits<double>::infinity();
	double currentValue = 0;
	Analytics::AnalyticsResult oldAnalytics;
	Analytics::AnalyticsResult currentAnalytics;
	boost::shared_ptr<HMMCompiled> oldHMM(new HMMCompiled());
	boost::shared_ptr<HMMCompiled> chmm(new HMMCompiled);

	if (trainingset.size() == 0) {
		std::cerr << "Training set is empty." << std::endl;
		return Analytics::AnalyticsResult();
	}

	do {
		// keep the best HMM (with respect to the analytics result/accuracy) found so far
		chmm->copy(oldHMM);
		oldValue = currentValue;
		oldAnalytics = currentAnalytics;

		for (int i = 0; i < _numberNodes; i++) {
			cTransitions[i].clear();
			cEmissions[i].clear();
		}

		for (int i = 0; i < _numberNodes; i++) {
			cInitial[i] = 0;
		}

		internalBaumWelch(trainingset, cTransitions, cEmissions, cInitial,
				initialRun);

		//smoothing of transitions
		for (int i = 0; i < _numberNodes; i++) {
			for (boost::unordered_map<int, double>::iterator jt =
					cTransitions[i].begin(); jt != cTransitions[i].end();
					++jt) {
				jt->second += 1;
			}
		}

		// smoothing by pseudo counts
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantEmissionSet(i)) {
				for (boost::unordered_set<std::string>::const_iterator it =
						_supersetEmissions.begin();
						it != _supersetEmissions.end(); ++it) {
					cEmissions[i][*it] += 1;
				}
			} else {
				for (boost::unordered_map<std::string, double>::iterator it =
						_emissions[i].begin(); it != _emissions[i].end();
						++it) {
					cEmissions[i][it->first] += 1;
				}
			}
		}

		// new emission probabilities
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantEmissions(i)) {
				double sum = 0;

				if (!hasConstantEmissionSet(i)) {
					for (boost::unordered_map<std::string, double>::const_iterator it =
							cEmissions[i].begin(); it != cEmissions[i].end();
							++it) {
						sum += it->second;
					}

					for (boost::unordered_map<std::string, double>::const_iterator it =
							cEmissions[i].begin(); it != cEmissions[i].end();
							++it) {
						prob = it->second / sum;
						double eProb = getEmission(i, it->first);
						_emissions[i][it->first] = prob;
					}
				} else {
					for (boost::unordered_map<std::string, double>::const_iterator it =
							_emissions[i].begin(); it != _emissions[i].end();
							++it) {
						sum += cEmissions[i].at(it->first);
					}

					for (boost::unordered_map<std::string, double>::iterator it =
							_emissions[i].begin(); it != _emissions[i].end();
							++it) {
						prob = cEmissions[i].at(it->first) / sum;
						double eProb = it->second;
						it->second = prob;
					}

				}
			}
		}

		// new transition probabilities
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantTransitions(i)) {
				double sum = 0;

				for (boost::unordered_map<int, double>::const_iterator jt =
						cTransitions[i].begin(); jt != cTransitions[i].end();
						++jt) {
					sum += jt->second;
				}

				for (boost::unordered_map<int, double>::iterator jt =
						_mapTransitions[i].begin();
						jt != _mapTransitions[i].end(); ++jt) {
					double prob = 0;

					if (sum != 0) {
						prob = cTransitions[i][jt->first] / sum;
					}

					jt->second = prob;
					_imapTransitions[jt->first][i] = prob;

				}
			}
		}

		// new initial distribution
		double sum = 0;
		for (int i = 0; i < _numberNodes; i++) {
			sum += cInitial[i];
		}

		for (int i = 0; i < _numberNodes; i++) {
			prob = cInitial[i] / sum;
			_initialDistribution[i] = prob;
		}

		initialRun = false;

		// it the training set is annotated, then the HMMCompiled has to be translated
		// to use the non-annotated emission symbols again for the analysis.
		if (annotated) {
			hmm->update(shared_from_this());
			hmm->substituteEmissions(inverseSubstitution);
			hmm->compile(chmm);
			hmm->substituteEmissions(substitution);
		} else {
			copy(chmm);
		}

		currentAnalytics = Analytics::analyse(chmm, testset, annotations);
		currentValue = Analytics::evaluate(currentAnalytics);

		diff = currentValue - oldValue;

	} while (diff > threshold);

	delete[] cTransitions;
	delete[] cEmissions;
	delete[] cInitial;

	oldHMM->copy(shared_from_this());

	return oldAnalytics;
}

Analytics::AnalyticsResult HMMCompiled::baumWelchIterated(
		boost::shared_ptr<HMM> hmm,
		const boost::unordered_map<std::string,
				boost::unordered_map<std::string, std::string> >& substitution,
		const boost::unordered_map<std::string,
				boost::unordered_map<std::string, std::string> >& inverseSubstitution,
		const std::vector<std::vector<std::string> >& trainingset,
		const std::vector<std::vector<std::string> >& testset,
		const std::vector<std::vector<std::string> >& annotations,
		int numIterations, bool annotated) {

	boost::unordered_map<int, double>* cTransitions = new boost::unordered_map<
			int, double>[_numberNodes];
	boost::unordered_map<std::string, double>* cEmissions =
			new boost::unordered_map<std::string, double>[_numberNodes];
	double* cInitial = new double[_numberNodes];
	bool initialRun = true;
	double prob;
	double diff;
	double oldValue = -std::numeric_limits<double>::infinity();
	double currentValue = 0;
	Analytics::AnalyticsResult oldAnalytics;
	Analytics::AnalyticsResult currentAnalytics;
	boost::shared_ptr<HMMCompiled> oldHMM(new HMMCompiled());
	boost::shared_ptr<HMMCompiled> chmm(new HMMCompiled);

	if (trainingset.size() == 0) {
		std::cerr << "Training set is empty." << std::endl;
		return Analytics::AnalyticsResult();
	}

	for (int k = 0; k < numIterations; k++) {

		for (int i = 0; i < _numberNodes; i++) {
			cTransitions[i].clear();
			cEmissions[i].clear();
		}

		for (int i = 0; i < _numberNodes; i++) {
			cInitial[i] = 0;
		}

		internalBaumWelch(trainingset, cTransitions, cEmissions, cInitial,
				initialRun);

		//smoothing of transitions
		for (int i = 0; i < _numberNodes; i++) {
			for (boost::unordered_map<int, double>::iterator jt =
					cTransitions[i].begin(); jt != cTransitions[i].end();
					++jt) {
				jt->second += 1;
			}
		}

		// smoothing by pseudo counts
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantEmissionSet(i)) {
				for (boost::unordered_set<std::string>::const_iterator it =
						_supersetEmissions.begin();
						it != _supersetEmissions.end(); ++it) {
					cEmissions[i][*it] += 1;
				}
			} else {
				for (boost::unordered_map<std::string, double>::iterator it =
						_emissions[i].begin(); it != _emissions[i].end();
						++it) {
					cEmissions[i][it->first] += 1;
				}
			}
		}

		// new emission probabilities
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantEmissions(i)) {
				double sum = 0;

				if (!hasConstantEmissionSet(i)) {
					for (boost::unordered_map<std::string, double>::const_iterator it =
							cEmissions[i].begin(); it != cEmissions[i].end();
							++it) {
						sum += it->second;
					}

					for (boost::unordered_map<std::string, double>::const_iterator it =
							cEmissions[i].begin(); it != cEmissions[i].end();
							++it) {
						prob = it->second / sum;
						double eProb = getEmission(i, it->first);
						_emissions[i][it->first] = prob;
					}
				} else {
					for (boost::unordered_map<std::string, double>::const_iterator it =
							_emissions[i].begin(); it != _emissions[i].end();
							++it) {
						sum += cEmissions[i].at(it->first);
					}

					for (boost::unordered_map<std::string, double>::iterator it =
							_emissions[i].begin(); it != _emissions[i].end();
							++it) {
						prob = cEmissions[i].at(it->first) / sum;
						double eProb = it->second;
						it->second = prob;
					}

				}
			}
		}

		// new transition probabilities
		for (int i = 0; i < _numberNodes; i++) {
			if (!hasConstantTransitions(i)) {
				double sum = 0;

				for (boost::unordered_map<int, double>::const_iterator jt =
						cTransitions[i].begin(); jt != cTransitions[i].end();
						++jt) {
					sum += jt->second;
				}

				for (boost::unordered_map<int, double>::iterator jt =
						_mapTransitions[i].begin();
						jt != _mapTransitions[i].end(); ++jt) {
					double prob = 0;

					if (sum != 0) {
						prob = cTransitions[i][jt->first] / sum;
					}

					jt->second = prob;
					_imapTransitions[jt->first][i] = prob;

				}
			}
		}

		// new initial distribution
		double sum = 0;
		for (int i = 0; i < _numberNodes; i++) {
			sum += cInitial[i];
		}

		for (int i = 0; i < _numberNodes; i++) {
			prob = cInitial[i] / sum;
			_initialDistribution[i] = prob;
		}

		initialRun = false;

		if (annotated) {
			hmm->update(shared_from_this());
			hmm->substituteEmissions(inverseSubstitution);
			hmm->compile(chmm);
			hmm->substituteEmissions(substitution);
		} else {
			copy(chmm);
		}

		currentAnalytics = Analytics::analyse(chmm, testset, annotations);
		currentValue = Analytics::evaluate(currentAnalytics);

		if (currentValue > oldValue) {
			oldValue = currentValue;
			oldAnalytics = currentAnalytics;
			chmm->copy(oldHMM);
		}

		std::cout << "Iteration:" << k << " value:" << currentValue
				<< std::endl;
		std::cout << currentAnalytics << std::endl;
	}

	delete[] cTransitions;
	delete[] cEmissions;
	delete[] cInitial;

	oldHMM->copy(shared_from_this());

	return oldAnalytics;
}

void HMMCompiled::initProbabilities() {
	//initial probabilities
	double constant = 0;
	double sum = 0;
	for (int i = 0; i < _numberNodes; i++) {
		if (_initialDistribution[i] >= 0) {
			constant += _initialDistribution[i];
		} else {
			do {
				_initialDistribution[i] = -_random();
			} while (_initialDistribution[i] == 0);
			sum += _initialDistribution[i];
		}
	}

	for (int i = 0; i < _numberNodes; i++) {
		if (_initialDistribution[i] < 0) {
			_initialDistribution[i] *= (1 - constant) / sum;
		}
	}

	//transition probabilities
	for (int i = 0; i < _numberNodes; i++) {
		sum = 0;
		constant = 0;

		for (boost::unordered_map<int, double>::iterator jt =
				_mapTransitions[i].begin(); jt != _mapTransitions[i].end();
				++jt) {
			if (jt->second >= 0) {
				constant += jt->second;
			} else {
				do {
					jt->second = -_random();
				} while (jt->second == 0);
				sum += jt->second;
			}
		}

		for (boost::unordered_map<int, double>::iterator jt =
				_mapTransitions[i].begin(); jt != _mapTransitions[i].end();
				++jt) {
			if (jt->second < 0) {
				jt->second *= (1 - constant) / sum;

				_imapTransitions[jt->first][i] = jt->second;
			}
		}

	}

	//emission probabilities
	for (int i = 0; i < _numberNodes; i++) {
		sum = 0;
		constant = 0;

		for (boost::unordered_map<std::string, double>::iterator it =
				_emissions[i].begin(); it != _emissions[i].end(); ++it) {
			if (it->second >= 0) {
				constant += it->second;
			} else {
				do {
					it->second = -_random();
				} while (it->second == 0);

				sum += it->second;
			}
		}

		for (boost::unordered_map<std::string, double>::iterator it =
				_emissions[i].begin(); it != _emissions[i].end(); ++it) {
			if (it->second < 0) {
				it->second *= (1 - constant) / sum;
			}
		}
	}
}

void HMMCompiled::simulate(int length, std::vector<std::string>& sequence,
		std::vector<int>& states) {
	int state = getState(_initialDistribution);
	int counter = 0;
	states.push_back(state);

	if (!isSilent(state)) {
		std::string emission = getRandomEmission(_emissions[state]);
		sequence.push_back(emission);
		counter++;
	}

	while (counter < length) {
		state = getState(_mapTransitions[state]);

		states.push_back(state);
		if (!isSilent(state)) {
			std::string emission = getRandomEmission(_emissions[state]);
			sequence.push_back(emission);
			counter++;
		}
	}
}

int HMMCompiled::getState(
		const boost::unordered_map<int, double>& distribution) {
	double rvalue = _random();
	double sum = 0;
	int result = 0;

	boost::unordered_map<int, double>::const_iterator it = distribution.begin();

	while (it != distribution.end() && sum + it->second < rvalue) {
		sum += it->second;
		++it;
	}

	return it->first;
}

int HMMCompiled::getState(double* distribution) {
	double rvalue = _random();
	double sum = 0;
	int result = 0;

	while (result < _numberNodes && sum + distribution[result] < rvalue) {
		sum += distribution[result];
		result++;
	}

	return result;
}

std::string HMMCompiled::getRandomEmission(
		boost::unordered_map<std::string, double>& emissions) {
	double rvalue = _random();
	double sum = 0;

	boost::unordered_map<std::string, double>::const_iterator it =
			emissions.begin();

	while (it != emissions.end() && sum + it->second < rvalue) {
		sum += it->second;
		++it;
	}

	return it->first;
}

void HMMCompiled::copy(boost::shared_ptr<HMMCompiled> dst) {
	dst->clear();
	dst->_numberNodes = _numberNodes;

	dst->_mapTransitions = new boost::unordered_map<int, double>[_numberNodes];

	for (int i = 0; i < _numberNodes; i++) {
		dst->_mapTransitions[i] = _mapTransitions[i];
	}

	dst->_imapTransitions = new boost::unordered_map<int, double>[_numberNodes];

	for (int i = 0; i < _numberNodes; i++) {
		dst->_imapTransitions[i] = _imapTransitions[i];
	}

	dst->_emissions =
			new boost::unordered_map<std::string, double>[_numberNodes];
	for (int i = 0; i < _numberNodes; i++) {
		dst->_emissions[i] = _emissions[i];
	}

	dst->_supersetEmissions = _supersetEmissions;
	dst->_silentStateOrder = _silentStateOrder;
	dst->_silentStates = _silentStates;

	dst->_initialDistribution = new double[_numberNodes];
	std::memcpy(dst->_initialDistribution, _initialDistribution,
			sizeof(double) * _numberNodes);

	dst->_constantTransitionNodes = new bool[_numberNodes];
	std::memcpy(dst->_constantTransitionNodes, _constantTransitionNodes,
			sizeof(bool) * _numberNodes);

	dst->_constantEmissionNodes = new bool[_numberNodes];
	std::memcpy(dst->_constantEmissionNodes, _constantEmissionNodes,
			sizeof(bool) * _numberNodes);

	dst->_constantEmissionSetNodes = new bool[_numberNodes];
	std::memcpy(dst->_constantEmissionSetNodes, _constantEmissionSetNodes,
			sizeof(bool) * _numberNodes);

	dst->_int2Node = _int2Node;
	dst->_node2Int = _node2Int;

	dst->_counter = _counter;

	dst->_random.base().seed(
			(unsigned int) (_random() * std::numeric_limits<unsigned int>::max())
					+ time(NULL));
}

void HMMCompiled::ID2Name(const std::vector<int>& ids,
		std::vector<std::string>& names) const {
	for (std::vector<int>::const_iterator it = ids.begin(); it != ids.end();
			++it) {
		names.push_back(_int2Node.at(*it)->getName());
	}
}
