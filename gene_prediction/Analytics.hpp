/*
 * Analytics.hpp
 *
 *  Created on: Mar 16, 2013
 *      Author: Till Rohrmann
 */

#ifndef ANALYTICS_HPP_
#define ANALYTICS_HPP_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include <ostream>

class HMMCompiled;

/**
 * The functions in this namespace are used to evaluate a HMM given a test set of
 * DNA sequences and their respective structure.
 */
namespace Analytics {
struct AnalyticsResult {
	double _nucleotidesSensitivity;
	double _nucleotidesSpecificity;
	double _exonSensitivity;
	double _exonSpecificity;

	AnalyticsResult();
};

struct AnalyticsIntermediate {
	int _nucleotidesSensitivity;
	int _nucleotidesSpecificity;
	int _exonSensitivity;
	int _exonSpecificity;
	int _numberExons;
	int _numberNucleotides;
	int _numberCodingNucleotides;
	int _numberExonNucleotides;

	AnalyticsIntermediate();

	void operator+=(const AnalyticsIntermediate& ref);
};

/**
 * Calculates for a given HMM and a set of sequence of DNA sequences as well as its
 * DNA structure the nucleotide sensitivity, specificity, exon sensitivity and exon
 * specificity. These values are returned.
 *
 * Nucleotide sensitivity: #correctly predicted coding bases/#coding bases
 * Nucleotide specificity: #correctly predicted exon bases/#exon bases
 * Exon sensitivity: #exons for which the start and end is correctly predicted/#exons
 * Exon specificity: #correctly predicted exons/#exons
 *
 */
AnalyticsResult analyse(boost::shared_ptr<HMMCompiled> hmm,
		const std::vector<std::vector<std::string> >& sequences,
		const std::vector<std::vector<std::string> >& annotations);

/**
 * Helper function used to count the correctly predicted bases and exons for
 * a predicted dna structure.
 *
 * @argument estimation predicted DNA structure
 * @argument annotation correct DNA structure
 *
 * @return structure which contains the individual counts
 */
AnalyticsIntermediate analyse(const std::vector<std::string>& estimation,
		const std::vector<std::string>& annotation);
double evaluate(boost::shared_ptr<HMMCompiled> hmm,
		const std::vector<std::vector<std::string> >& sequences,
		const std::vector<std::vector<std::string> >& annotations);

/**
 * This function calculates the arithmetic average of all 4 accuracy
 * values: Nucleotide sensitivity, nucleotide specificity, exon sensitivity,
 * exon specificity. Thus it can be used as an overall correctness indicator.
 */
double evaluate(const AnalyticsResult& result);

std::ostream& operator<<(std::ostream& os, const AnalyticsResult& result);
std::ostream& operator<<(std::ostream& os,
		const AnalyticsIntermediate& intermediate);
}

#endif /* ANALYTICS_HPP_ */
