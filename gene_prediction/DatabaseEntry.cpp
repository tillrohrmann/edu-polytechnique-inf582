/*
 * DatabaseEntry.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#include "DatabaseEntry.hpp"

#include <sstream>
#include <iostream>
#include <stdlib.h>

boost::random::uniform_01<boost::random::mt19937> DatabaseEntry::_random(boost::random::mt19937(time(NULL)));

DatabaseEntry::DatabaseEntry(const std::string& id, const std::vector<std::string>& data):
	_id(id),_data(data){
	instantiateData();
}

DatabaseEntry::DatabaseEntry(){
	_id = "";
}

std::string DatabaseEntry::toString() const{
	std::stringstream ss;

	ss << "ID:" << _id << std::endl;

	for(int i=0; i< _data.size(); i++){
		ss << _data[i];
	}

	ss<< std::endl;
	ss<< "Exons:";

	for(int i=0; i< _exons.size(); i++){
		ss << _exons[i]._first << " " << _exons[i]._second;
		if(i < _exons.size()-1){
			ss << " ";
		}
	}

	return ss.str();
}

void DatabaseEntry::instantiateData(){
	std::string alphabet[] = {"A","C","G","T"};
	for(int i=0; i< _data.size(); i++){
		if(_data[i] == "N"){
			_data[i] = alphabet[(int)(4*_random())];
		}
		if(_data[i] == "R"){
			_data[i] = alphabet[2*(int)(2*_random())];
		}
		if(_data[i] == "B"){
			_data[i] = alphabet[(int)(3*_random())+1];
		}
		if(_data[i] == "Y"){
			_data[i] = alphabet[(int)(2*_random())*2+1];
		}
		if(_data[i] == "S"){
			_data[i] = alphabet[(int)(2*_random())+1];
		}
		if(_data[i] == "K"){
			_data[i] = alphabet[(int)(2*_random())+2];
		}
	}
}

void DatabaseEntry::addExon(int start,int end){
	_exons.push_back(Pair<int>(start,end));
}

void DatabaseEntry::extractExons(std::vector<std::vector<std::string> >& result) const{
	for(int i =0; i< _exons.size(); i++){
		std::vector<std::string>::const_iterator it = _data.begin();

		std::vector<std::string> exon(it+_exons[i]._first-1,it+_exons[i]._second);
		result.push_back(exon);
	}
}

void DatabaseEntry::extractIntrons(std::vector<std::vector<std::string> >& result) const{
	if(_exons.size() > 0){
		int start = _exons[0]._second;
		for(int i=1; i< _exons.size();i++){
			std::vector<std::string> exon(_data.begin()+start,_data.begin()+_exons[i]._first-1);
			result.push_back(exon);
			start = _exons[i]._second;
		}
	}
}

void DatabaseEntry::extractDownstream(std::vector<std::vector<std::string> > & result) const{
	if(_exons.size() >0){
		std::vector<std::string> exon(_data.begin()+_exons[_exons.size()-1]._second,_data.end());
		result.push_back(exon);
	}
}

void DatabaseEntry::extractUpstream(std::vector<std::vector<std::string> >& result) const{
	if(_exons.size() >0){
		std::vector<std::string> exon(_data.begin(),_data.begin()+_exons[0]._first-1);
		result.push_back(exon);
	}else{
		std::vector<std::string> exon(_data.begin(),_data.end());
		result.push_back(exon);
	}
}

void DatabaseEntry::extractSequence(std::vector<std::vector<std::string> >& result) const{
	std::vector<std::string> sequence(_data);

	result.push_back(sequence);
}

void DatabaseEntry::extractAnnotatedSequence(std::vector<std::vector<std::string> >& result) const{
	std::vector<std::string> annotation;
	getAnnotation(annotation);
	std::stringstream ss;

	std::vector<std::string> sequence;

	for(int i=0; i<_data.size();i++){
		ss.str("");
		ss.clear();
		ss << annotation[i];
		ss << _data[i];

		sequence.push_back(ss.str());
	}

	result.push_back(sequence);
}

void DatabaseEntry::getAnnotation(std::vector<std::string>& result) const{
	if(_exons.size() >0){
		for(int i=0;i<_exons[0]._first-1;i++){
			result.push_back("U");
		}


		for(int i=0; i<_exons.size();i++){
			for(int j=0; j < _exons[i]._second-_exons[i]._first+1;j++){
				result.push_back("E");
			}

			if(i < _exons.size()-1){
				for(int j=0; j< _exons[i+1]._first-_exons[i]._second-1;j++){
					result.push_back("I");
				}
			}
		}

		for(int i=0; i<_data.size()-_exons[_exons.size()-1]._second;i++){
			result.push_back("D");
		}
	}else{
		for(int i=0; i<_data.size();i++){
			result.push_back("U");
		}
	}
}

void DatabaseEntry::extractAnnotation(std::vector<std::vector<std::string> >& result) const{
	std::vector<std::string> annotation;
	getAnnotation(annotation);
	result.push_back(annotation);
}


