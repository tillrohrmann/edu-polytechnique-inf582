/*
 * GeneDatabase.hpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef GENEDATABASE_HPP_
#define GENEDATABASE_HPP_

#include <boost/unordered_map.hpp>
#include <string>
#include "DatabaseEntry.hpp"

class GeneDatabase{
private:
	boost::unordered_map<std::string,DatabaseEntry> _entries;

public:
	GeneDatabase();

	const boost::unordered_map<std::string,DatabaseEntry>& getEntries() const {return _entries;}
	boost::unordered_map<std::string,DatabaseEntry>& getEntries() { return _entries;}

	void importFile(const std::string& filename);
	void importCDS(const std::string& filename);

	int size() const { return _entries.size(); }

	void extractExons(std::vector<std::vector<std::string> >& result) const;
	void extractIntrons(std::vector<std::vector<std::string> >& result) const;
	void extractUpstream(std::vector<std::vector<std::string> >& result) const;
	void extractDownstream(std::vector<std::vector<std::string> >& result) const;

	void extractSequences(std::vector<std::vector<std::string> >& result) const;
	void extractAnnotatedSequences(std::vector<std::vector<std::string> >& result) const;
	void extractAnnotations(std::vector<std::vector<std::string> >& result) const;

	void extractSequencesAndAnnotations(std::vector<std::vector<std::string> >& sequences, std::vector<std::vector<std::string> >& annotations) const;
	void extractDatabaseEntries(std::vector<DatabaseEntry* >& entries);

	static void separateSet(const std::vector<DatabaseEntry* >& entries,std::vector<std::vector<std::string> >& sequences,
			std::vector<std::vector<std::string> >&	testset, std::vector<std::vector<std::string> >& annotations,bool annotated
			,int start,int end);
};




#endif /* GENEDATABASE_HPP_ */