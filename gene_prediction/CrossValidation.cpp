/*
 * CrossValidation.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: Till Rohrmann
 */

#include "CrossValidation.hpp"

#include "HMMCompiled.hpp"

boost::shared_ptr<HMMCompiled> CrossValidation::crossValidation(boost::shared_ptr<HMMCompiled> compiled,
		const std::vector<std::vector<std::string> >& trainingSet, double threshold, int tries, int testsetSize){
	boost::shared_ptr<HMMCompiled> result;

	std::list<std::vector<std::string> > set(trainingSet.begin(),trainingSet.end());

	if(compiled->isRandom()){
		double* match = new double[tries];
		boost::shared_ptr<HMMCompiled>* hmms = new boost::shared_ptr<HMMCompiled>[tries];
		double max=0;
		int maxIdx=-1;

		for(int i=0; i< tries; i++){
			match[i] = 0;
			hmms[i] = boost::shared_ptr<HMMCompiled>(new HMMCompiled());
			compiled->copy(hmms[i]);
			hmms[i]->initProbabilities();
		}

		std::list<std::vector<std::string> > testset;
		std::list<std::vector<std::string> >::iterator it  = set.begin();

		for(int n=0; n < trainingSet.size();n+=testsetSize){
			int maxValue = testsetSize>trainingSet.size()-n ? trainingSet.size()-n:testsetSize;

			std::advance(it,maxValue);

			testset.splice(testset.begin(),set,set.begin(),it);

			for(int i =0; i< tries;i++){

				hmms[i]->baumWelch(set,threshold);

				for(std::list<std::vector<std::string> >::const_iterator tests = testset.begin(); tests != testset.end(); ++tests){
					match[i] += std::log(hmms[i]->forward(*tests));
				}
			}

			set.splice(set.end(),testset);
		}

		for(int i =0; i<tries;i++){
			if(max < match[i]){
				max = match[i];
				maxIdx = i;
			}
		}

		result = hmms[maxIdx];

		delete [] match;
		delete [] hmms;

		return result;
	}else{
		result = boost::shared_ptr<HMMCompiled>(new HMMCompiled());
		compiled->copy(result);

		std::list<std::vector<std::string> > tl(trainingSet.begin(),trainingSet.end());
		result->baumWelch(set,threshold);
	}

	return result;
}


