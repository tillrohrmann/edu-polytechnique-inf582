/*
 * HMMTransition.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#include "HMMTransition.hpp"
#include <regex>
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

	std::smatch  sm;

	std::getline(is,line);
	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("Probability:(.*)"))){
		ss.str(sm[0]);
		ss >> probability;
	}else{
		throw InvalidSerializationException("HMMTransition: Probability cannot be deserialized.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("Destination:(.*)"))){
		ss.str(sm[0]);
		ss >> destination;
	}else{
		throw InvalidSerializationException("HMMTransition: Destination cannot be deserialized.");
	}

	std::getline(is,line);

	if(std::regex_match(line,sm,std::regex("Constant:(.*)"))){
		ss.str(sm[0]);
		ss >> constant;
	}else{
		throw InvalidSerializationException("HMMTransition: Constant cannot be deserialized.");
	}

	std::getline(is,line);

	return HMMTransition(probability,destination,constant);
}


