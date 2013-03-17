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

namespace Analytics{
	struct AnalyticsResult{
		double _nucleotidesSensitivity;
		double _nucleotidesSpecificity;
		double _exonSensitivity;
		double _exonSpecificity;

		AnalyticsResult();
	};

	struct AnalyticsIntermediate{
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

	AnalyticsResult analyse(boost::shared_ptr<HMMCompiled> hmm,const std::vector<std::vector<std::string> >& sequences, const std::vector<std::vector<std::string> >& annotations);
	AnalyticsIntermediate analyse(const std::vector<std::string>& estimation, const std::vector<std::string>& annotation);
	double evaluate(boost::shared_ptr<HMMCompiled> hmm,const std::vector<std::vector<std::string> >& sequences, const std::vector<std::vector<std::string> >& annotations);
	double evaluate(const AnalyticsResult& result);

	std::ostream& operator<<(std::ostream& os, const AnalyticsResult& result);
	std::ostream& operator<<(std::ostream& os, const AnalyticsIntermediate& intermediate);
}



#endif /* ANALYTICS_HPP_ */
