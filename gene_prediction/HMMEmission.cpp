/*
 * HMMEmission.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#include "HMMEmission.hpp"
#include "Exceptions.hpp"
#include <regex>
#include <sstream>

HMMEmission::HMMEmission(double probability, std::string emissionToken, bool constant):
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
	std::smatch sm;
	std::istringstream ss;

	std::getline(is,line);
	std::getline(is,line);
	if(std::regex_match(line,sm,std::regex("Probability:(.*)"))){
		ss.str(sm[0]);
		ss >> probability;
	}else{
		throw InvalidSerializationException("HMMEmission: Probability cannot be deserialized.");
	}

	std::getline(is,line);
	if(std::regex_match(line,sm,std::regex("Token:(.*)"))){
		emissionToken = sm[0];
	}else{
		throw InvalidSerializationException("HMMEmission: Emission token cannot be deserialized.");
	}

	std::getline(is,line);
	if(std::regex_match(line,sm,std::regex("Constant:(.*)"))){
		ss.str(sm[0]);
		ss >> constant;
	}else{
		throw InvalidSerializationException("HMMEmission: Constant cannot be deserialized.");
	}

	std::getline(is,line);

	return HMMEmission(probability,emissionToken,constant);
}


