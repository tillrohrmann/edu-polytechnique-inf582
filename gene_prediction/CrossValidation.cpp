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

boost::shared_ptr<HMMCompiled> CrossValidation::crossValidation(
		boost::shared_ptr<HMMCompiled> compiled,
		const std::vector<std::vector<std::string> >& trainingSet,
		double threshold, int tries, int testsetSize) {
	boost::shared_ptr<HMMCompiled> result;

	// if the hmm contains random probabilities
	if (compiled->isRandom()) {
		double* match = new double[tries];
		boost::shared_ptr<HMMCompiled>* hmms =
				new boost::shared_ptr<HMMCompiled>[tries];
		double max = -std::numeric_limits<double>::infinity();
		int maxIdx = -1;

		// intialise all tries
		for (int i = 0; i < tries; i++) {
			match[i] = 0;
			hmms[i] = boost::shared_ptr<HMMCompiled>(new HMMCompiled());
			compiled->copy(hmms[i]);
			hmms[i]->initProbabilities();
		}

		for (int n = 0; n < trainingSet.size(); n += testsetSize) {
			int end = std::min((int) trainingSet.size(), n + testsetSize);
			std::cout << "Testset:" << n << ":" << end << std::endl;

			// get training and test set from the original set
			std::vector<std::vector<std::string> > testset(
					trainingSet.begin() + n, trainingSet.begin() + end);
			std::vector<std::vector<std::string> > set(trainingSet.begin(),
					trainingSet.begin() + n);
			set.insert(set.end(), trainingSet.begin() + end, trainingSet.end());

			for (int i = 0; i < tries; i++) {
				hmms[i]->baumWelch(set, threshold);

				std::cout << hmms[i]->toString() << std::endl;

				// evaluate accuracy
				for (std::vector<std::vector<std::string> >::const_iterator tests =
						testset.begin(); tests != testset.end(); ++tests) {
					double prob = hmms[i]->forward(*tests);

					if (prob != -std::numeric_limits<double>::infinity())
						match[i] += prob;
				}
			}
		}

		// choose best fitting instance
		for (int i = 0; i < tries; i++) {
			std::cout << "Match[" << i << "]:" << match[i] << std::endl;
			if (max < match[i]) {
				max = match[i];
				maxIdx = i;
			}
		}

		result = hmms[maxIdx];

		delete[] match;
		delete[] hmms;

		return result;
	} else {
		result = boost::shared_ptr<HMMCompiled>(new HMMCompiled());
		compiled->copy(result);

		result->baumWelch(trainingSet, threshold);
	}

	return result;
}

void CrossValidation::modelLearning(const std::string& prefix,
		boost::shared_ptr<HMM> hmm, const std::vector<DatabaseEntry*>& entries,
		int testsetSize, int tries, const std::string& errorEvaluation,
		double threshold, bool annotated) {
	int numberTestsets = std::ceil((double) entries.size() / testsetSize);
	int numberSequences = entries.size();

	double matchingScore[numberTestsets][tries];
	boost::shared_ptr<HMMCompiled> chmms[numberTestsets][tries];
	Analytics::AnalyticsResult analyticsResult[numberTestsets][tries];
	boost::shared_ptr<HMMCompiled> chmm(new HMMCompiled());
	boost::unordered_map<std::string,
			boost::unordered_map<std::string, std::string> > substitution;
	boost::unordered_map<std::string,
			boost::unordered_map<std::string, std::string> > inverseSubstitution;
	std::vector<std::vector<std::string> > sequences;
	std::vector<std::vector<std::string> > annotations;
	std::vector<std::vector<std::string> > testset;
	std::stringstream ss;
	std::ofstream os;

	Models::createAnnotatedSubstitution(substitution);
	Models::createInverseAnnotatedSubstitution(inverseSubstitution);

	// change emissions if the sequences are annotated because the model was intended
	// to work with not annotated sequences.
	if (annotated)
		hmm->substituteEmissions(substitution);

	// Translate the HMM to a version which is more suited for computations
	hmm->compile(chmm);

	assert(chmm->isRandom());

	// initialize for every try a HMM
	for (int j = 0; j < tries; j++) {
		matchingScore[0][j] = -std::numeric_limits<double>::infinity();
		chmms[0][j] = boost::shared_ptr<HMMCompiled>(new HMMCompiled());
		chmm->copy(chmms[0][j]);
		chmms[0][j]->initProbabilities();
	}

	// but for the same try use the same initial HMM for every testset possible
	for (int i = 1; i < numberTestsets; i++) {
		for (int j = 0; j < tries; j++) {
			matchingScore[i][j] = -std::numeric_limits<double>::infinity();
			chmms[i][j] = boost::shared_ptr<HMMCompiled>(new HMMCompiled());

			chmms[0][j]->copy(chmms[i][j]);
		}
	}

	for (int start = 0, i = 0; start < numberSequences;
			start += testsetSize, i++) {
		int end = std::min(numberSequences, start + testsetSize);

		sequences.clear();
		testset.clear();
		annotations.clear();
		// get the training set, test set and the corresponding structure information
		GeneDatabase::separateSet(entries, sequences, testset, annotations,
				annotated, start, end);

		for (int j = 0; j < tries; j++) {
			if (errorEvaluation == "Threshold") {
				chmms[i][j]->baumWelch(sequences, threshold);

				if (annotated) {
					// for the analysis it is necessary to have a HMM
					// which emits the plain bases (A,C,G,T) --> apply
					// the inverse substitution. Unfortunately we sill
					// have to take the detour using the class HMM because
					// HMMCompiled supports no substitution.
					hmm->update(chmms[i][j]);
					hmm->substituteEmissions(inverseSubstitution);
					hmm->compile(chmms[i][j]);
					hmm->substituteEmissions(substitution);
				}

				analyticsResult[i][j] = Analytics::analyse(chmms[i][j], testset,
						annotations);
				matchingScore[i][j] = Analytics::evaluate(
						analyticsResult[i][j]);

			} else if (errorEvaluation == "Evaluation") {
				analyticsResult[i][j] = chmms[i][j]->baumWelch(hmm,
						substitution, inverseSubstitution, sequences, testset,
						annotations, threshold, annotated);
				matchingScore[i][j] = Analytics::evaluate(
						analyticsResult[i][j]);

			} else if (errorEvaluation == "Iteration") {
				analyticsResult[i][j] = chmms[i][j]->baumWelchIterated(hmm,
						substitution, inverseSubstitution, sequences, testset,
						annotations, (int) threshold, annotated);
				matchingScore[i][j] = Analytics::evaluate(
						analyticsResult[i][j]);
			}
		}

		int maxIdx = 0;
		int maxValue = matchingScore[i][0];

		for (int j = 1; j < tries; j++) {
			if (maxValue < matchingScore[i][j]) {
				maxIdx = j;
				maxValue = matchingScore[i][j];
			}
		}

		chmm = chmms[i][maxIdx];

		Analytics::AnalyticsResult result = analyticsResult[i][maxIdx];

		std::cout << "Testset:" << (i + 1) << std::endl;
		std::cout << result << std::endl << std::endl;

		ss.str(std::string());
		ss.clear();
		ss << prefix << "modelTestset" << (i + 1) << ".hmm";

		os.open(ss.str().c_str(), std::ios_base::out);

		if (annotated)
			hmm->substituteEmissions(inverseSubstitution);
		hmm->update(chmm);
		// save the best learned HMM for later usages
		hmm->serialize(os);

		if (annotated)
			hmm->substituteEmissions(substitution);

		os.close();
	}
}

