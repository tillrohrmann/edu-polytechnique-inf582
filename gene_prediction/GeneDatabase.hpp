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

/**
 * Container class for DatabasesEntries/DNASequences
 */
class GeneDatabase {
private:
	boost::unordered_map<std::string, DatabaseEntry> _entries;
public:
	GeneDatabase();

	const boost::unordered_map<std::string, DatabaseEntry>& getEntries() const {
		return _entries;
	}
	boost::unordered_map<std::string, DatabaseEntry>& getEntries() {
		return _entries;
	}

	/**
	 * Read a fasta file and import the contained DNA sequences
	 */
	void importFile(const std::string& filename);

	/**
	 * Read a CDS file and assign the contained exon information to the respective
	 * DNA sequence
	 */
	void importCDS(const std::string& filename);

	int size() const {
		return _entries.size();
	}

	void extractExons(std::vector<std::vector<std::string> >& result) const;
	void extractIntrons(std::vector<std::vector<std::string> >& result) const;
	void extractUpstream(std::vector<std::vector<std::string> >& result) const;
	void extractDownstream(
			std::vector<std::vector<std::string> >& result) const;

	void extractSequences(std::vector<std::vector<std::string> >& result) const;
	void extractAnnotatedSequences(
			std::vector<std::vector<std::string> >& result) const;
	void extractAnnotations(
			std::vector<std::vector<std::string> >& result) const;

	void extractSequencesAndAnnotations(
			std::vector<std::vector<std::string> >& sequences,
			std::vector<std::vector<std::string> >& annotations) const;
	void extractDatabaseEntries(std::vector<DatabaseEntry*>& entries);

	/**
	 * This function takes a vector of DatabaseEntry and extracts from them the
	 * DNA sequences and the structure annotation. It then separates those data
	 * according to the indices start and end into a testing set and a training set.
	 * The start, end indices define the range of the testing set.
	 *
	 * @argument entries vector of DatabaseEntries which are used to get the data
	 * @argument sequences vector in which the training set is stored
	 * @argument testset vector in which the testing set is stored
	 * @argument annotations vector in which the structure annotations of the testing set are stored
	 * @argument annotated boolean value which says whether the annotated or not-annotated version
	 * 		of the DNA sequence shall be extracted for the training set
	 * @argument start starting index for the testing set
	 * @argument end ending index for the testing set (exclusive)
	 */
	static void separateSet(const std::vector<DatabaseEntry*>& entries,
			std::vector<std::vector<std::string> >& sequences,
			std::vector<std::vector<std::string> >& testset,
			std::vector<std::vector<std::string> >& annotations, bool annotated,
			int start, int end);
};

#endif /* GENEDATABASE_HPP_ */
