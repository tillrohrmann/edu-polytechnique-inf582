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
#include <boost/enable_shared_from_this.hpp>

#include <vector>
#include <list>

#include "Analytics.hpp"

class HMMNode;
class HMM;

class HMMCompiled  : public boost::enable_shared_from_this<HMMCompiled>{
private:
	int _numberNodes;
	boost::unordered_map<int,double>* _mapTransitions;
	boost::unordered_map<int,double>* _imapTransitions;
	bool* _constantTransitionNodes;
	bool* _constantEmissionNodes;
	bool* _constantEmissionSetNodes;
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

	void internalBaumWelch(const std::vector<std::vector<std::string> >& trainingset,
		boost::unordered_map<int,double>* cTransitions, boost::unordered_map<std::string,double>* cEmissions,
		double* cInitial, bool initialRun);

public:
	HMMCompiled();
	~HMMCompiled();

	void copy(boost::shared_ptr<HMMCompiled> dst);

	void setTransition(int x, int y, double value) { _mapTransitions[x][y]=value; _imapTransitions[y][x]=value;}//_transitions[x*_numberNodes + y] = value; }
	double getTransition(int x, int y)const { return _mapTransitions[x].count(y) > 0? _mapTransitions[x][y]: 0;}//return _transitions[x*_numberNodes + y]; }
	double getLogTransition(int x, int y)const { return std::log(_mapTransitions[x].count(y)>0? _mapTransitions[x][y]:0);}//return std::log(_transitions[x*_numberNodes + y]); }
	double getTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest);

	double getInitialDistribution(int i) const { return _initialDistribution[i];}
	double getInitialDistribution(boost::shared_ptr<HMMNode> node) const;
	double getLogInitialDistribution(int i)const { return std::log(_initialDistribution[i]);}

	double getEmission(boost::shared_ptr<HMMNode> src,const std::string & token)const;
	double getEmission(int id, const std::string& token) const;
	double getLogEmission(int id, const std::string& token) const;

	bool hasConstantEmissions(int node) const;
	bool hasConstantTransitions(int node) const;
	bool hasConstantEmissionSet(int node) const;

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

	void baumWelch(const std::vector< std::vector<std::string> >& trainingset, double threshold);
	Analytics::AnalyticsResult baumWelch(boost::shared_ptr<HMM> hmm,const  boost::unordered_map<std::string,boost::unordered_map<std::string,std::string> >& substitution,
			const boost::unordered_map<std::string, boost::unordered_map<std::string,std::string > >& inverseSubstitution,
			const std::vector< std::vector<std::string> >& trainingset, const std::vector< std::vector<std::string> >& testset,
			const std::vector<std::vector<std::string> >& annotations,double threshold, bool annotated);
	Analytics::AnalyticsResult baumWelchIterated(boost::shared_ptr<HMM> hmm,const  boost::unordered_map<std::string,boost::unordered_map<std::string,std::string> >& substitution,
				const boost::unordered_map<std::string, boost::unordered_map<std::string,std::string > >& inverseSubstitution,
				const std::vector< std::vector<std::string> >& trainingset, const std::vector< std::vector<std::string> >& testset,
				const std::vector<std::vector<std::string> >& annotations,int numIterations, bool annotated);

	void finishCompilation();
	void initProbabilities();

	void simulate(int n, std::vector<std::string>& sequence, std::vector<int>& states);
	int getState(double* distribution);
	int getState(const boost::unordered_map<int,double>& distribution);
	std::string getRandomEmission(boost::unordered_map<std::string,double>& emissions);

	void ID2Name(const std::vector<int>& ids, std::vector<std::string>& names) const;

	std::string toString() const;
};




#endif /* HMMCOMPILED_HPP_ */
