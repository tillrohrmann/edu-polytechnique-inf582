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

/**
 * This class represents a HMM in its computability friendly form. For that purpose
 * The transitions and emissions of every node are organized in a vector of unordered_maps.
 * Since the VEIL model is sparse that is to say it contains relatively few transitions
 * per node, it is worth not to store the transitions in a quadratic matrix, but to store
 * only those entries which are different from 0. The same holds for the emissions. The
 * HMM algorithms which are supported are: forward, backward, viterbi, Baum-Welch. All
 * algorithms are computed in the log-space to make long sequences how they appear in
 * DNA-predictions can be handled.
 */
class HMMCompiled: public boost::enable_shared_from_this<HMMCompiled> {
private:
	int _numberNodes;
	// _mapTransitions[i][j] = transition probability from i to j
	boost::unordered_map<int, double>* _mapTransitions;
	// inverse mapping: _imapTransitions[i][j] = transition probability from j to i
	boost::unordered_map<int, double>* _imapTransitions;
	// _constantTransitionNodes[i] = node with index i has constant transitions
	bool* _constantTransitionNodes;
	// same for emissions
	bool* _constantEmissionNodes;
	// same for emission sets
	bool* _constantEmissionSetNodes;
	// _emissions[i][a] = probability that node i emits a symbol a
	boost::unordered_map<std::string, double>* _emissions;
	// set of all possible emissions
	boost::unordered_set<std::string> _supersetEmissions;
	// set of silent states (states which does not emit anything)
	boost::unordered_set<int> _silentStates;
	// order in which the silent states has to be traversed for computing
	// the transitions probabilities
	std::vector<int> _silentStateOrder;
	double* _initialDistribution;

	// mapping between the internal used ids and the nodes
	boost::unordered_map<int, boost::shared_ptr<HMMNode> > _int2Node;
	// inverse mapping
	boost::unordered_map<boost::shared_ptr<HMMNode>, int> _node2Int;

	int _counter;

	boost::random::uniform_01<boost::random::mt19937> _random;

	/**
	 * This function calculates the sum of x and y in the log-space
	 *
	 * @argument x log value of a value to be added
	 * @argument y log value of a value to be added
	 *
	 * @return the sum of exp(x) and exp(y) in the log space
	 */
	double elnsum(double x, double y);

	/**
	 * This function calculates the forward and backward function which is used to predict the
	 * transition and emission probabilities. The contributions are added to cTransitions and
	 * cEmissions.
	 */
	void internalBaumWelch(
			const std::vector<std::vector<std::string> >& trainingset,
			boost::unordered_map<int, double>* cTransitions,
			boost::unordered_map<std::string, double>* cEmissions,
			double* cInitial, bool initialRun);

public:
	HMMCompiled();
	~HMMCompiled();

	void copy(boost::shared_ptr<HMMCompiled> dst);

	void setTransition(int x, int y, double value) {
		_mapTransitions[x][y] = value;
		_imapTransitions[y][x] = value;
	}	//_transitions[x*_numberNodes + y] = value; }
	double getTransition(int x, int y) const {
		return _mapTransitions[x].count(y) > 0 ? _mapTransitions[x][y] : 0;
	}	//return _transitions[x*_numberNodes + y]; }
	double getLogTransition(int x, int y) const {
		return std::log(
				_mapTransitions[x].count(y) > 0 ? _mapTransitions[x][y] : 0);
	}	//return std::log(_transitions[x*_numberNodes + y]); }
	double getTransition(boost::shared_ptr<HMMNode> src,
			boost::shared_ptr<HMMNode> dest);

	double getInitialDistribution(int i) const {
		return _initialDistribution[i];
	}
	double getInitialDistribution(boost::shared_ptr<HMMNode> node) const;
	double getLogInitialDistribution(int i) const {
		return std::log(_initialDistribution[i]);
	}

	double getEmission(boost::shared_ptr<HMMNode> src,
			const std::string & token) const;
	double getEmission(int id, const std::string& token) const;
	double getLogEmission(int id, const std::string& token) const;

	bool hasConstantEmissions(int node) const;
	bool hasConstantTransitions(int node) const;
	bool hasConstantEmissionSet(int node) const;

	void clear();
	void initialize(int numberNodes);

	/**
	 * This functions checks whether there is a random probability value (emission or transition).
	 * If so, then the model is considered to be random and has to be initialized.
	 */
	bool isRandom() const;

	/**
	 * Checks whether the node with id is silent
	 */
	bool isSilent(int id) {
		return _silentStates.count(id) > 0;
	}

	/**
	 * Add new node to the HMMCompiled instance
	 */
	void addMapping(boost::shared_ptr<HMMNode> node);

	void addSilentNode(boost::shared_ptr<HMMNode> node);
	void addTransition(boost::shared_ptr<HMMNode> src,
			boost::shared_ptr<HMMNode> dest, double probability);
	void addEmission(boost::shared_ptr<HMMNode> src, const std::string& token,
			double probability);
	void addInitialDistribution(boost::shared_ptr<HMMNode> node,
			double probability);

	/**
	 * Get internal index of node
	 */
	int getIndex(boost::shared_ptr<HMMNode> node) const;
	/**
	 * Get node associated to the internal index index
	 */
	boost::shared_ptr<HMMNode> getNode(int index) const;

	double forward(const std::vector<std::string>& sequence);
	void viterbi(const std::vector<std::string>& sequence,
			std::vector<int>& stateSequence);
	double backward(const std::vector<std::string>& sequence);

	/**
	 * This function learns for the current model the transition and emission probabilities.
	 * As input it takes the training set and a threshold value which defines when to stop
	 * the iteration procedure. That is to say, if the maximum probability change is smaller
	 * than threshold, than it stops.
	 */
	void baumWelch(const std::vector<std::vector<std::string> >& trainingset,
			double threshold);

	/**
	 * This functions performs at its core the Baum-Welch algorithm to learn the probabilities of
	 * a training set, but at the same time it uses a different termination criterium. For each
	 * iteration it uses all sequence from the training set to learn the probabilities. Then the
	 * learned model is used to predict the structure of the testing set and the accuracy of that
	 * prediction is used as the termination criterium. If the difference of the current accuracy
	 * value minus the previous one is lower than the negative threshold value, then it terminates.
	 *
	 * @argument hmm HMM from which this was compiled
	 * @argument substitution emission substitution set if the models is learned by annotated sequences
	 * @argument inverseSubstitution inverse substitution set for the emissions
	 * @argument trainingset
	 * @argument testset
	 * @argument annotations structure informations of the testset sequences
	 * @argument threshold threshold value for the termination criterium
	 * @argument annotated says whether the training set is annotated or not
	 *
	 * @return best analytics result
	 */
	Analytics::AnalyticsResult baumWelch(boost::shared_ptr<HMM> hmm,
			const boost::unordered_map<std::string,
					boost::unordered_map<std::string, std::string> >& substitution,
			const boost::unordered_map<std::string,
					boost::unordered_map<std::string, std::string> >& inverseSubstitution,
			const std::vector<std::vector<std::string> >& trainingset,
			const std::vector<std::vector<std::string> >& testset,
			const std::vector<std::vector<std::string> >& annotations,
			double threshold, bool annotated);

	/**
	 * This functions is similar to the previous one, only that the Baum-Welch algorithm is performed
	 * a numIterations before it terminates.
	 * @argument hmm HMM from which this was compiled
	 * @argument substitution emission substitution set if the models is learned by annotated sequences
	 * @argument inverseSubstitution inverse substitution set for the emissions
	 * @argument trainingset
	 * @argument testset
	 * @argument annotations structure informations of the testset sequences
	 * @argument numIterations number of learning iterations
	 * @argument annotated says whether the training set is annotated or not
	 *
	 * @return best analytics result
	 */
	Analytics::AnalyticsResult baumWelchIterated(boost::shared_ptr<HMM> hmm,
			const boost::unordered_map<std::string,
					boost::unordered_map<std::string, std::string> >& substitution,
			const boost::unordered_map<std::string,
					boost::unordered_map<std::string, std::string> >& inverseSubstitution,
			const std::vector<std::vector<std::string> >& trainingset,
			const std::vector<std::vector<std::string> >& testset,
			const std::vector<std::vector<std::string> >& annotations,
			int numIterations, bool annotated);

	/**
	 * This function calculates the traversing order of the silent states.
	 */
	void finishCompilation();
	/**
	 * This function inits all random probabilities with a uniformly distributed value.
	 */
	void initProbabilities();

	/**
	 * This function simulates n steps of the HMM and stores its output into sequence
	 * and the sequence of states into states.
	 *
	 * @argument n length of simulation sequence
	 * @argument sequence output sequence
	 * @argument states state sequence of the simulation
	 */
	void simulate(int n, std::vector<std::string>& sequence,
			std::vector<int>& states);

	/**
	 * Draws randomly a state from the distribution given by distribution. For that
	 * purpose it draws a uniformly value r from [0,1) and looks for the partial sum
	 * s_i = sum_{j=0}^{i} distribution[j] such that s_i <= r < s_(i+1).
	 */
	int getState(double* distribution);

	/**
	 * Does the same as the previous function. It just takes the distribution in a different
	 * form.
	 */
	int getState(const boost::unordered_map<int, double>& distribution);

	/**
	 * This function draws randomly emission from the emission distribution emissions. It
	 * works the same way as getState.
	 */
	std::string getRandomEmission(
			boost::unordered_map<std::string, double>& emissions);

	/**
	 * This function gets a sequence of state ids and translates those into a sequence
	 * of state names such that each id is replaced by the name of the associated node.
	 */
	void ID2Name(const std::vector<int>& ids,
			std::vector<std::string>& names) const;

	std::string toString() const;
};

#endif /* HMMCOMPILED_HPP_ */
