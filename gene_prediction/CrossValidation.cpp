/*
 * CrossValidation.cpp
 *
 *  Created on: Feb 19, 2013
 *      Author: Till Rohrmann
 */

#include "CrossValidation.hpp"

#include <fstream>

#include "HMMCompiled.hpp"
#include "HMM.hpp"
#include "Analytics.hpp"
#include "Models.hpp"
#include "GeneDatabase.hpp"

boost::shared_ptr<HMMCompiled> CrossValidation::crossValidation(boost::shared_ptr<HMMCompiled> compiled,
		const std::vector<std::vector<std::string> >& trainingSet, double threshold, int tries, int testsetSize){
	boost::shared_ptr<HMMCompiled> result;

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
			std::cout << hmms[i]->toString() << std::endl;
		}

		for(int n=0; n < trainingSet.size();n+=testsetSize){
			int end = std::min((int)trainingSet.size(),n+testsetSize);
			std::cout << "Testset:" << n << ":" << end << std::endl;

			std::vector<std::vector<std::string> > testset(trainingSet.begin()+n,trainingSet.begin()+end);
			std::vector<std::vector<std::string> > set(trainingSet.begin(),trainingSet.begin()+n);
			set.insert(set.end(),trainingSet.begin()+end,trainingSet.end());

			for(int i =0; i< tries;i++){
				//std::cout <<"Try:" << i << std::endl;
				//boost::shared_ptr<HMMCompiled> hmm(new HMMCompiled());
				//hmms[i]->copy(hmm);
				hmms[i]->baumWelch(set,threshold);

				std::cout << hmms[i]->toString() << std::endl;

				for(std::vector<std::vector<std::string> >::const_iterator tests = testset.begin(); tests != testset.end(); ++tests){
					double prob = hmms[i]->forward(*tests);

					if(prob != -std::numeric_limits<double>::infinity())
						match[i] += prob;
				}
			}
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

		result->baumWelch(trainingSet,threshold);
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
		result->baumWelchR(set,threshold);
	}

	return result;
}

void CrossValidation::modelLearning(boost::shared_ptr<HMM> hmm, const std::vector<DatabaseEntry*>& entries,
		int testsetSize, int tries, const std::string& errorEvaluation, double threshold){
	int numberTestsets = std::ceil((double)entries.size()/testsetSize);
	int numberSequences = entries.size();

	double matchingScore[numberTestsets][tries];
	boost::shared_ptr<HMMCompiled> chmms[numberTestsets][tries];
	Analytics::AnalyticsResult analyticsResult[numberTestsets][tries];
	boost::shared_ptr<HMMCompiled> chmm;
	boost::unordered_map<std::string, boost::unordered_map<std::string,std::string> > substitution;
	boost::unordered_map<std::string, boost::unordered_map<std::string,std::string> > inverseSubstitution;
	std::vector<std::vector<std::string> > sequences;
	std::vector<std::vector<std::string> > annotations;
	std::vector<std::vector<std::string> > testset;
	std::stringstream ss;
	std::ofstream os;

	Models::createAnnotatedSubstitution(substitution);
	Models::createInverseAnnotatedSubstitution(inverseSubstitution);

	hmm->substituteEmissions(substitution);
	hmm->compile(chmm);

	assert(chmm->isRandom());

	for(int i = 0; i < numberTestsets; i++){
		for (int j=0; j< tries;j++){
			matchingScore[i][j] = -std::numeric_limits<double>::infinity();
			chmms[i][j] = boost::shared_ptr<HMMCompiled>(new HMMCompiled());

			chmm->copy(chmms[i][j]);
			chmms[i][j]->initProbabilities();
		}
	}

	//TODO: shuffle entries

	for(int start=0, i=0; start < numberSequences; start+=testsetSize,i++){
		int end = std::min(numberSequences,start+testsetSize);

		GeneDatabase::separateSet(entries,sequences,testset,annotations,true,start,end);

		for(int j=0; j< tries; j++){
			if(errorEvaluation == "Threshold"){
				chmms[i][j]->baumWelch(sequences,threshold);
				hmm->update(chmms[i][j]);
				hmm->substituteEmissions(inverseSubstitution);
				hmm->compile(chmms[i][j]);

				analyticsResult[i][j] = Analytics::analyse(chmms[i][j],testset,annotations);
				matchingScore[i][j] = Analytics::evaluate(analyticsResult[i][j]);

			}else if(errorEvaluation == "Evaluation"){
				chmms[i][j]->baumWelch(hmm,)
			}
		}

		int maxIdx = 0;
		int maxValue = matchingScore[i][0];

		for(int j =1; j< tries;j++){
			if(maxValue < matchingScore[i][j]){
				maxIdx= j;
				maxValue = matchingScore[i][j];
			}
		}

		chmm = chmms[i][maxIdx];

		Analytics::AnalyticsResult result = analyticsResult[i][maxIdx];

		std::cout << "Testset:" << (i+1) << std::endl;
		std::cout << result << std::endl << std::endl;

		ss.str(std::string());
		ss.clear();
		ss << "modelTestset" << (i+1) << ".hmm";

		os.open(ss.str().c_str(),std::ios_base::out);

		hmm->update(chmm);
		hmm->serialize(os);
		os.close();
	}
}



