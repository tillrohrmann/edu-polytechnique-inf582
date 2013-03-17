/*
 * DatabaseEntry.hpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef DATABASEENTRY_HPP_
#define DATABASEENTRY_HPP_

#include <string>
#include <vector>
#include <boost/random.hpp>

#include "Pair.hpp"

class DatabaseEntry{
private:
	static boost::random::uniform_01<boost::random::mt19937> _random;

	std::string _id;
	std::vector<std::string> _data;
	std::vector<Pair<int> > _exons;
public:
	DatabaseEntry(const std::string& id, const std::vector<std::string>& data);
	DatabaseEntry();

	std::string toString() const;

	void addExon(int start, int end);

	void extractExons(std::vector<std::vector<std::string> >& result) const;
	void extractIntrons(std::vector<std::vector<std::string> >& result) const;
	void extractUpstream(std::vector<std::vector<std::string> >& result) const;
	void extractDownstream(std::vector<std::vector<std::string> >& result) const;

	void extractSequence(std::vector<std::vector<std::string> >& result) const;
	void extractAnnotatedSequence(std::vector<std::vector<std::string> >& result) const;

	void getAnnotation(std::vector<std::string>& annotation) const;
	void extractAnnotation(std::vector<std::vector<std::string> >& result) const;

	void instantiateData();
};




#endif /* DATABASEENTRY_HPP_ */
