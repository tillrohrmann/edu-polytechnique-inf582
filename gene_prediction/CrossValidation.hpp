/*
 * CrossValidation.hpp
 *
 *  Created on: Feb 19, 2013
 *      Author: Till Rohrmann
 */

#ifndef CROSSVALIDATION_HPP_
#define CROSSVALIDATION_HPP_

#include <boost/shared_ptr.hpp>

#include <vector>

class HMMCompiled;
class DatabaseEntry;
class HMM;

class CrossValidation {
public:
	/**
	 * This function learns a HMM from a given training set. Furthermore, it selects the best fitting model,
	 * in the case of a probabilistic initialization by the mean of cross validation. The procedure is the
	 * following: It removes from the training set a certain number of elements which are after the learning
	 * used for the cross validation. Then for every initialization, the model is learned and then evaluated.
	 * Then the next number of test set elements are chosen from the training set and the process is repeated
	 * with the now already trained HMMs to further improve them. This is done until all elements of the original
	 * training set have exactly once be used for the cross validation. Due to the fact that in latter turns the
	 * models are tested on sequences which they already learned, the prediction quality is considerably better.
	 * But this changes nothing for the selection criteria, which is the probability, that the test set
	 * sequences have been emitted by the learned model. At the end, the model with the highest probability
	 * is chosen and returned.
	 *
	 * @argument compiled is the compiled reference HMM
	 * @argument trainingSet
	 * @argument threshold if all probability changes in the HMM are smaller than the threshold,
	 * 	then the learning process is stopped
	 * @argument tries if the HMM contains random probabilities, the model is tries times randomly
	 * 	instantiated. For each instantiation, the model is learned and the best fitting model returned
	 * @argument testingSize defines the size of testing set which is extracted in each turn from the training set
	 *
	 * @return learned HMM
	 */
	static boost::shared_ptr<HMMCompiled> crossValidation(
			boost::shared_ptr<HMMCompiled> compiled,
			const std::vector<std::vector<std::string> >& trainingSet,
			double threshold, int tries, int testingSize);

	/**
	 * This function learns the probabilities of a given HMM with respect to the provided training data set
	 * and evaluates its accuracy. For that purpose, the initial training set is separated into a training
	 * set and a testing set. The model is learned using the training set and the testing set is used
	 * to evaluate its accuracy. Given a initial training set of n sequences and a testing set size of m,
	 * the algorithm makes ceil(n/m) runs, each time choosing m different sequences as the testing set. In
	 * each turn, the HMM will be reset to its initial probabilities. If the HMM contains random probabilities,
	 * they are initialized once in the beginning and reused for every new testing set. Furthermore it is possible
	 * to learn several randomly initialized HMM and select the best one. The selection criteria is the average
	 * accuracy of the prediction of the testing set.
	 *
	 * @argument prefix string wich is appended to the file name of the result files containing the learned HMM
	 * 	for every testing set
	 * @argument hmm contains the initial HMM (structure and probabilities)
	 * @argument entries is a vector of DatabaseEntry which contain the DNA sequences and their annotations
	 * @argument testsetSize the size of the testing set
	 * @argument tries the number of different random initialization which shall be tried in the case of
	 * 	a HMM with random transition or emission probabilities
	 * @argument errorEvalution a string specifying the termination criteria for the learning algorithm.
	 * 	The current options are:
	 * 		"Threshold" with threshold used as a threshold for the maximum probability change in order to
	 * 		stop the learning.
	 * 		"Evaluation" with threshold used as the maximum worsening of the average accuracy value
	 * 		between 2 iterations, otherwise the algorithm stops
	 * 		"Iteration" with threshold used as the maximum number of iterations before stopping
	 * @argument annotated defines whether sequences annotated by their structure information or the plain
	 * 	sequences shall be used for the learning algorithm
	 */
	static void modelLearning(const std::string& prefix,
			boost::shared_ptr<HMM> hmm,
			const std::vector<DatabaseEntry*>& entries, int testsetSize,
			int tries, const std::string& errorEvaluation, double threshold,
			bool annotated = true);
};

#endif /* CROSSVALIDATION_HPP_ */
