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

class CrossValidation{
public:
	static boost::shared_ptr<HMMCompiled> crossValidation(boost::shared_ptr<HMMCompiled> compiled,const std::vector<std::vector<std::string> >& trainingSet,double threshold,int tries, int testingSize);
	static boost::shared_ptr<HMMCompiled> crossValidationR(boost::shared_ptr<HMMCompiled> compiled,const std::vector<std::vector<std::string> >& trainingSet,double threshold,int tries, int testingSize);
};




#endif /* CROSSVALIDATION_HPP_ */
