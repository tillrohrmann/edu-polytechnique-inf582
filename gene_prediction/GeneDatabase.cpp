/*
 * GeneDatabase.cpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#include "GeneDatabase.hpp"

#include <fstream>
#include <boost/regex.h>
#include <iostream>
#include <stdlib.h>
#include <sstream>

GeneDatabase::GeneDatabase(){
}

void GeneDatabase::importFile(const std::string& filename){
	std::ifstream is;
	std::string line;
	std::string id="";
	std::vector<std::string> buffer;
	std::stringstream ss;
	std::string token;

	is.open(filename.c_str(),std::ios_base::in);

	if(!is){
		std::cerr << "Could not open file:" << filename << std::endl;
		exit(1);
	}

	while(!is.eof()){
		std::getline(is,line);

		if(line[0] == '>'){
			if(id != ""){
				_entries.emplace(id,DatabaseEntry(id,buffer));
				buffer.clear();
			}

			id = line.substr(1);
		}else{
			for(int i=0; i<line.size();i++){
				ss.clear();
				ss << line[i];
				ss >> token;
				buffer.push_back(token);
			}
		}
	}

	if(id != ""){
		_entries.emplace(id,DatabaseEntry(id,buffer));
		buffer.clear();
	}

	is.close();
}

void GeneDatabase::importCDS(const std::string& filename){
	std::ifstream is;
	std::string line;
	std::string id;
	std::istringstream ss;
	int start;
	int end;

	is.open(filename.c_str(),std::ios_base::in);

	if(!is){
		std::cerr << "Could not open file:" << filename << std::endl;
		exit(1);
	}

	while(!is.eof()){
		std::getline(is,line);
		ss.clear();
		ss.str(line);

		ss >> id;

		if(_entries.count(id)==1){
			DatabaseEntry& entry = _entries[id];

			while(!ss.eof()){
				ss >> start >> end;
				entry.addExon(start,end);
			}
		}
	}
}

void GeneDatabase::extractExons(std::vector<std::vector<std::string> >& result) const{
	for(boost::unordered_map<std::string,DatabaseEntry>::const_iterator it = _entries.begin(); it != _entries.end(); ++it){
		it->second.extractExons(result);
	}
}

void GeneDatabase::extractIntrons(std::vector<std::vector<std::string> >& result) const{
	for(boost::unordered_map<std::string,DatabaseEntry>::const_iterator it = _entries.begin(); it != _entries.end(); ++it){
			it->second.extractIntrons(result);
	}
}

void GeneDatabase::extractUpstream(std::vector<std::vector<std::string> >& result) const{
	for(boost::unordered_map<std::string,DatabaseEntry>::const_iterator it = _entries.begin(); it != _entries.end(); ++it){
			it->second.extractUpstream(result);
	}
}

void GeneDatabase::extractDownstream(std::vector<std::vector<std::string> >& result) const{
	for(boost::unordered_map<std::string,DatabaseEntry>::const_iterator it = _entries.begin(); it != _entries.end(); ++it){
			it->second.extractDownstream(result);
	}
}

void GeneDatabase::extractSequences(std::vector<std::vector<std::string> >& result) const{
	for(boost::unordered_map<std::string,DatabaseEntry>::const_iterator it = _entries.begin(); it != _entries.end(); ++it){
		it->second.extractSequence(result);
	}
}

void GeneDatabase::extractAnnotatedSequences(std::vector<std::vector<std::string> >& result) const{
	for(boost::unordered_map<std::string,DatabaseEntry>::const_iterator it = _entries.begin(); it != _entries.end(); ++it){
		it->second.extractAnnotatedSequence(result);
	}
}

