/*
 * HMMCompiled.hpp
 *
 *  Created on: Feb 14, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMCOMPILED_HPP_
#define HMMCOMPILED_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/random.hpp>

#include <vector>
#include <list>

class HMMNode;

class HMMCompiled{
private:
	int _numberNodes;
	double* _transitions;
	bool* _constantTransitionNodes;
	bool* _constantEmissionNodes;
	boost::unordered_map<std::string,double>* _emissions;
	boost::unordered_set<std::string> _supersetEmissions;
	boost::unordered_set<int> _silentStates;
	std::vector<int> _silentStateOrder;
	double* _initialDistribution;

	boost::unordered_map<int,boost::shared_ptr<HMMNode> > _int2Node;
	boost::unordered_map<boost::shared_ptr<HMMNode>, int > _node2Int;

	int _counter;

	boost::random::uniform_01<boost::random::mt19937> _random;

	double elnsum(double x, double y);
public:
	HMMCompiled();
	~HMMCompiled();

	void copy(boost::shared_ptr<HMMCompiled> dst);

	void setTransition(int x, int y, double value) { _transitions[x*_numberNodes + y] = value; }
	double getTransition(int x, int y)const { return _transitions[x*_numberNodes + y]; }
	double getLogTransition(int x, int y)const { return std::log(_transitions[x*_numberNodes + y]); }
	double getTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest);

	double getInitialDistribution(int i) const { return _initialDistribution[i];}
	double getLogInitialDistribution(int i)const { return std::log(_initialDistribution[i]);}

	double getEmission(boost::shared_ptr<HMMNode> src,const std::string & token)const;
	double getEmission(int id, const std::string& token) const;
	double getLogEmission(int id, const std::string& token) const;

	bool hasConstantEmissions(int node) const;
	bool hasConstantTransitions(int node) const;

	void clear();
	void initialize(int numberNodes);

	bool isRandom() const;

	bool isSilent(int id) { return _silentStates.count(id) > 0; }

	void addMapping(boost::shared_ptr<HMMNode> node);

	void addSilentNode(boost::shared_ptr<HMMNode> node);
	void addTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest, double probability);
	void addEmission(boost::shared_ptr<HMMNode> src, const std::string& token, double probability);
	void addInitialDistribution(boost::shared_ptr<HMMNode> node, double probability);

	int getIndex(boost::shared_ptr<HMMNode> node) const;
	boost::shared_ptr<HMMNode> getNode(int index) const;

	double forward(const std::vector<std::string>& sequence);
	void viterbi(const std::vector<std::string>& sequence, std::vector<int>& stateSequence);
	double backward(const std::vector<std::string>& sequence);

	void baumWelch(const std::list< std::vector<std::string> >& trainingset, double threshold);

	double forwardR(const std::vector<std::string>& sequence);
	double backwardR(const std::vector<std::string>& sequence);
	void baumWelchR(const std::list< std::vector<std::string> >& trainingset, double threshold);

	void finishCompilation();
	void initProbabilities();

	void simulate(int n, std::vector<std::string>& sequence, std::vector<int>& states);
	int getState(double* distribution);
	std::string getRandomEmission(boost::unordered_map<std::string,double>& emissions);

	std::string toString() const;
};




#endif /* HMMCOMPILED_HPP_ */
