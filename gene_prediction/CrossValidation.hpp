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

class CrossValidation{
public:
	static boost::shared_ptr<HMMCompiled> crossValidation(boost::shared_ptr<HMMCompiled> compiled,const std::vector<std::vector<std::string> >& trainingSet,double threshold,int tries, int testingSize);

	static void modelLearning(const std::string& prefix, boost::shared_ptr<HMM> hmm, const std::vector<DatabaseEntry*>& entries,
			int testsetSize, int tries, const std::string& errorEvaluation, double threshold, bool annotated= true);
};




#endif /* CROSSVALIDATION_HPP_ */
