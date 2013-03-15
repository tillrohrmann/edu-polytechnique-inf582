/*
 * HMMEmission.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#include "HMMEmission.hpp"
#include "Exceptions.hpp"
#include <boost/regex.hpp>
#include <sstream>
#include <ostream>
#include <istream>

HMMEmission::HMMEmission(const std::string& emissionToken,double probability):
	_probabiltiy(probability), _emissionToken(emissionToken){
}

void HMMEmission::serialize(std::ostream& os) const{
	os << "Emission{" << std::endl;
	os << "Probability:" << _probabiltiy << std::endl;
	os << "Token:" << _emissionToken << std::endl;
	os << "}" << std::endl;
}

HMMEmission HMMEmission::deserialize(std::istream& is){
	double probability;
	std::string emissionToken;
	bool constant;
	std::string line;
	boost::smatch sm;
	std::istringstream ss;

	std::getline(is,line);
	std::getline(is,line);
	if(boost::regex_match(line,sm,boost::regex("Probability:(.*)"))){
		ss.str(sm[1]);
		ss >> probability;
		ss.clear();
	}else{
		throw std::invalid_argument("HMMEmission: Probability cannot be deserialized.");
	}

	std::getline(is,line);
	if(boost::regex_match(line,sm,boost::regex("Token:(.*)"))){
		emissionToken = sm[1];
	}else{
		throw std::invalid_argument("HMMEmission: Emission token cannot be deserialized.");
	}

	std::getline(is,line);

	return HMMEmission(emissionToken,probability);
}


