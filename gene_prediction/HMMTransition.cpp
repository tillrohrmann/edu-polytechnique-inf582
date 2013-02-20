/*
 * HMMTransition.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#include "HMMTransition.hpp"
#include <boost/regex.hpp>
#include <sstream>
#include "Exceptions.hpp"

HMMTransition::HMMTransition(double probability,int destination, bool constant):
	_probability(probability), _constant(constant), _destination(destination){
}

void HMMTransition::serialize(std::ostream& os) const{
	os << "Transition{" << std::endl;
	os << "Probability:" << _probability << std::endl;
	os << "Destination:" << _destination << std::endl;
	os << "Constant:" << _constant << std::endl;
	os << "}" << std::endl;
}

HMMTransition HMMTransition::deserialize(std::istream& is){
	double probability;
	int destination;
	bool constant;
	std::string line;
	std::istringstream ss;

	boost::smatch  sm;

	std::getline(is,line);
	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Probability:(.*)"))){
		ss.str(sm[1]);
		ss >> probability;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMTransition: Probability cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Destination:(.*)"))){
		ss.str(sm[1]);
		ss >> destination;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMTransition: Destination cannot be deserialized.");
	}

	std::getline(is,line);

	if(boost::regex_match(line,sm,boost::regex("Constant:(.*)"))){
		ss.str(sm[1]);
		ss >> constant;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMTransition: Constant cannot be deserialized.");
	}

	std::getline(is,line);

	return HMMTransition(probability,destination,constant);
}


