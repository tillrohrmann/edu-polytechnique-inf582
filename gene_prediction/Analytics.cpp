/*
 * Analytics.cpp
 *
 *  Created on: Mar 16, 2013
 *      Author: Till Rohrmann
 */

#include "Analytics.hpp"
#include "HMMCompiled.hpp"
#include "Models.hpp"

Analytics::AnalyticsResult::AnalyticsResult() :
		_nucleotidesSensitivity(0), _nucleotidesSpecificity(0), _exonSensitivity(
				0), _exonSpecificity(0) {
}

Analytics::AnalyticsIntermediate::AnalyticsIntermediate() :
		_nucleotidesSensitivity(0), _nucleotidesSpecificity(0), _exonSensitivity(
				0), _exonSpecificity(0), _numberExons(0), _numberNucleotides(0), _numberCodingNucleotides(
				0), _numberExonNucleotides(0) {
}

void Analytics::AnalyticsIntermediate::operator+=(
		const Analytics::AnalyticsIntermediate& ref) {
	this->_nucleotidesSensitivity += ref._nucleotidesSensitivity;
	this->_nucleotidesSpecificity += ref._nucleotidesSpecificity;
	this->_exonSensitivity += ref._exonSensitivity;
	this->_exonSpecificity += ref._exonSpecificity;
	this->_numberExons += ref._numberExons;
	this->_numberNucleotides += ref._numberNucleotides;
	this->_numberCodingNucleotides += ref._numberCodingNucleotides;
	this->_numberExonNucleotides += ref._numberExonNucleotides;
}

std::ostream& Analytics::operator <<(std::ostream& os,
		const Analytics::AnalyticsResult& result) {
	os << "Nucleotide sensitivity:" << result._nucleotidesSensitivity
			<< " Nucleotide specificity:" << result._nucleotidesSpecificity
			<< " Exon sensitivity:" << result._exonSensitivity
			<< " Exon specificity:" << result._exonSpecificity;
	return os;
}

std::ostream& Analytics::operator<<(std::ostream& os,
		const Analytics::AnalyticsIntermediate& intermediate) {
	os << "Number nucleotides:" << intermediate._numberNucleotides
			<< " Coding nucleotides:" << intermediate._nucleotidesSensitivity
			<< " Number coding nucleotides:"
			<< intermediate._numberCodingNucleotides << " Exon nucleotides:"
			<< intermediate._nucleotidesSpecificity
			<< " Number exon nucleotides:"
			<< intermediate._numberExonNucleotides << " Exon sensitivity:"
			<< intermediate._exonSensitivity << " Exon specificity:"
			<< intermediate._exonSpecificity << " Number exons:"
			<< intermediate._numberExons;
	return os;
}

double Analytics::evaluate(boost::shared_ptr<HMMCompiled> hmm,
		const std::vector<std::vector<std::string> >& sequences,
		const std::vector<std::vector<std::string> >& annotations) {
	AnalyticsResult result = analyse(hmm, sequences, annotations);

	return (result._exonSensitivity + result._exonSpecificity
			+ result._nucleotidesSensitivity + result._nucleotidesSpecificity)
			/ 4;
}

double Analytics::evaluate(const Analytics::AnalyticsResult& result) {
	return (result._exonSensitivity + result._exonSpecificity
			+ result._nucleotidesSensitivity + result._nucleotidesSpecificity)
			/ 4;
}

Analytics::AnalyticsIntermediate Analytics::analyse(
		const std::vector<std::string>& estimation,
		const std::vector<std::string>& annotation) {
	Analytics::AnalyticsIntermediate result;
	bool isExon = false;
	bool exactlyMatched = true;
	bool matched = false;
	bool exonStart = false;
	result._numberNucleotides = estimation.size();

	assert(estimation.size() == annotation.size());

	for (int i = 0; i < estimation.size(); i++) {
		// A coding region is either an intron or an exon
		// Nucleotide Sensitivity
		if (annotation[i] == "E" || annotation[i] == "I") {
			result._numberCodingNucleotides++;

			if (estimation[i] == annotation[i]) {
				result._nucleotidesSensitivity++;
			}
		}

		// Nucleotide Specificity
		if (annotation[i] == "E") {
			result._numberExonNucleotides++;

			if (estimation[i] == annotation[i]) {
				result._nucleotidesSpecificity++;
			}
		}

		// Exon Sensitivity
		if (annotation[i] == "E") {
			if (!isExon) {
				isExon = true;
				exonStart = exactlyMatched = annotation[i] == estimation[i];
				result._numberExons++;
			} else if (annotation[i] != estimation[i]) {
				exactlyMatched = false;
			}
		} else {
			if (isExon) {
				isExon = false;
				if (exactlyMatched)
					result._exonSpecificity++;
				if (matched && exonStart) {
					result._exonSensitivity++;
				}
			}
		}
		// matched stores the matching result of the preceding element
		matched = annotation[i] == estimation[i];
	}

	return result;
}

Analytics::AnalyticsResult Analytics::analyse(
		boost::shared_ptr<HMMCompiled> hmm,
		const std::vector<std::vector<std::string> >& sequences,
		const std::vector<std::vector<std::string> >& annotations) {

	AnalyticsResult result;
	AnalyticsIntermediate intermediate;
	AnalyticsIntermediate sum;
	int counter = 0;

	for (std::vector<std::vector<std::string> >::const_iterator at =
			annotations.begin(), it = sequences.begin(); it != sequences.end();
			++it, ++at) {
		std::vector<int> states;
		std::vector<std::string> namedStates;
		std::vector<std::string> annotation;
		// calculate the most likely state sequence
		hmm->viterbi(*it, states);
		// replace state id numbers by their names
		hmm->ID2Name(states, namedStates);

		// translate sequence of state name into DNA structure
		Models::VeilAnnotation(namedStates, annotation);

		// calculate the statistics
		intermediate = analyse(annotation, *at);

		sum += intermediate;

		counter++;
	}

	result._exonSensitivity = ((double) sum._exonSensitivity)
			/ sum._numberExons;
	result._exonSpecificity = ((double) sum._exonSpecificity)
			/ sum._numberExons;
	result._nucleotidesSensitivity = ((double) sum._nucleotidesSensitivity)
			/ sum._numberCodingNucleotides;
	result._nucleotidesSpecificity = ((double) sum._nucleotidesSpecificity)
			/ sum._numberExonNucleotides;

	return result;

}

