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

HMMEmission::HMMEmission(double probability,const std::string& emissionToken, bool constant):
	_probabiltiy(probability), _emissionToken(emissionToken), _constant(constant){
}

void HMMEmission::serialize(std::ostream& os) const{
	os << "Emission{" << std::endl;
	os << "Probability:" << _probabiltiy << std::endl;
	os << "Token:" << _emissionToken << std::endl;
	os << "Constant:" << _constant << std::endl;
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
		throw InvalidSerializationException("HMMEmission: Probability cannot be deserialized.");
	}

	std::getline(is,line);
	if(boost::regex_match(line,sm,boost::regex("Token:(.*)"))){
		emissionToken = sm[1];
	}else{
		throw InvalidSerializationException("HMMEmission: Emission token cannot be deserialized.");
	}

	std::getline(is,line);
	if(boost::regex_match(line,sm,boost::regex("Constant:(.*)"))){
		ss.str(sm[1]);
		ss >> constant;
		ss.clear();
	}else{
		throw InvalidSerializationException("HMMEmission: Constant cannot be deserialized.");
	}

	std::getline(is,line);

	return HMMEmission(probability,emissionToken,constant);
}


