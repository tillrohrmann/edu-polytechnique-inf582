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
		double max=-std::numeric_limits<double>::infinity();
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
			std::cout << "Testset:" << n << ":" << maxValue << std::endl;

			std::advance(it,maxValue);

			testset.splice(testset.begin(),set,set.begin(),it);

			for(int i =0; i< tries;i++){
				//std::cout <<"Try:" << i << std::endl;
				boost::shared_ptr<HMMCompiled> hmm(new HMMCompiled());
				//hmms[i]->copy(hmm);
				hmms[i]->baumWelch(set,threshold);

				//std::cout << hmms[i]->toString() << std::endl;

				for(std::list<std::vector<std::string> >::const_iterator tests = testset.begin(); tests != testset.end(); ++tests){
					match[i] += hmms[i]->forward(*tests);
				}
			}

			set.splice(set.end(),testset);
		}

		for(int i =0; i<tries;i++){
			std::cout <<"Match[" << i << "]:" << match[i] << std::endl;
			if(max < match[i]){
				max = match[i];
				maxIdx = i;
			}
		}

		result = hmms[maxIdx];

		//result->baumWelch(set,threshold);

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


boost::shared_ptr<HMMCompiled> CrossValidation::crossValidationR(boost::shared_ptr<HMMCompiled> compiled,
		const std::vector<std::vector<std::string> >& trainingSet, double threshold, int tries, int testsetSize){
	boost::shared_ptr<HMMCompiled> result;

	std::list<std::vector<std::string> > set(trainingSet.begin(),trainingSet.end());

	if(compiled->isRandom()){
		double* match = new double[tries];
		boost::shared_ptr<HMMCompiled>* hmms = new boost::shared_ptr<HMMCompiled>[tries];
		double max=-std::numeric_limits<double>::infinity();
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
				boost::shared_ptr<HMMCompiled> hmm(new HMMCompiled());
				//hmms[i]->copy(hmm);
				hmms[i]->baumWelchR(set,threshold);


				for(std::list<std::vector<std::string> >::const_iterator tests = testset.begin(); tests != testset.end(); ++tests){
					match[i] += hmms[i]->forward(*tests);
				}
			}

			set.splice(set.end(),testset);
		}

		std::cout << "Done calculating" << std::endl;

		for(int i =0; i<tries;i++){
			std::cout <<"Match[" << i << "]:" << match[i] << std::endl;
			if(max < match[i]){
				max = match[i];
				maxIdx = i;
			}
		}

		result = hmms[maxIdx];

		//result->baumWelch(set,threshold);

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



