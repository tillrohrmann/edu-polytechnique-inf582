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
public:
	struct EmissionInfo{
		EmissionInfo(double prob=0, bool constant=false): _prob(prob),_constant(constant){}
		double _prob;
		bool _constant;
	};
private:
	int _numberNodes;
	double* _transitions;
	boost::unordered_map<std::string,EmissionInfo>* _emissions;
	boost::unordered_set<std::string> _supersetEmissions;
	boost::unordered_set<int> _silentStates;
	std::vector<int> _silentStateOrder;
	double* _initialDistribution;
	bool* _constantTransitions;

	boost::unordered_map<int,boost::shared_ptr<HMMNode> > _int2Node;
	boost::unordered_map<boost::shared_ptr<HMMNode>, int > _node2Int;

	int _counter;

	boost::random::uniform_01<boost::random::mt19937> _random;

	double elnsum(double x, double y);
public:
	HMMCompiled();
	~HMMCompiled();

	void copy(boost::shared_ptr<HMMCompiled> dst);

	void setConstant(int x, int y, bool value) { _constantTransitions[x*_numberNodes + y] = value; }
	bool getConstant(int x, int y)const { return _constantTransitions[x*_numberNodes + y]; }

	void setTransition(int x, int y, double value) { _transitions[x*_numberNodes + y] = value; }
	double getTransition(int x, int y)const { return _transitions[x*_numberNodes + y]; }
	double getLogTransition(int x, int y)const { return std::log(_transitions[x*_numberNodes + y]); }
	double getTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest);

	double getInitialDistribution(int i) const { return _initialDistribution[i];}
	double getLogInitialDistribution(int i)const { return std::log(_initialDistribution[i]);}

	double getEmission(boost::shared_ptr<HMMNode> src,const std::string & token)const;
	double getEmission(int id, const std::string& token) const;
	double getLogEmission(int id, const std::string& token) const;

	void clear();
	void initialize(int numberNodes);

	bool isRandom() const;

	bool isSilent(int id) { return _silentStates.count(id) > 0; }

	void addMapping(boost::shared_ptr<HMMNode> node);

	void addSilentNode(boost::shared_ptr<HMMNode> node);
	void addConstantTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest);
	void addTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest, double probability);
	void addEmission(boost::shared_ptr<HMMNode> src, const std::string& token, double probability,bool constant = false);
	void addInitialDistribution(boost::shared_ptr<HMMNode> node, double probability);

	int getIndex(boost::shared_ptr<HMMNode> node) const;
	boost::shared_ptr<HMMNode> getNode(int index) const;

	double forward(const std::vector<std::string>& sequence);
	void viterbi(const std::vector<std::string>& sequence, std::vector<int>& stateSequence);
	double backward(const std::vector<std::string>& sequence);
	void baumWelch(const std::list< std::vector<std::string> >& trainingset, double threshold);

	void finishCompilation();
	void initProbabilities();

	void simulate(int n, std::vector<std::string>& sequence, std::vector<int>& states);
	int getState(double* distribution);
	std::string getEmission(boost::unordered_map<std::string,EmissionInfo>& emissions);

	std::string toString() const;
};




#endif /* HMMCOMPILED_HPP_ */
