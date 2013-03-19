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

/**
 * This class represents a DNA sequence and its structure by defining the
 * exon locations.
 */
class DatabaseEntry{
private:
	static boost::random::uniform_01<boost::random::mt19937> _random;

	std::string _id;
	std::vector<std::string> _data;
	/*
	 * Each pair characterizes an exon. The first number is the starting
	 * index and the second number is the ending index. The element at
	 * the ending index belongs also to the exon.
	 */
	std::vector<Pair<int> > _exons;
public:
	DatabaseEntry(const std::string& id, const std::vector<std::string>& data);
	DatabaseEntry();

	std::string toString() const;

	void addExon(int start, int end);

	/**
	 * Fill result with exon sequences extracted from this DNA sequence.
	 */
	void extractExons(std::vector<std::vector<std::string> >& result) const;

	/**
	 * Fill result with intron sequences extracted from this DNA sequence.
	 */
	void extractIntrons(std::vector<std::vector<std::string> >& result) const;

	/**
	 * Fill result with upstream sequence extracted from this DNA sequence.
	 */
	void extractUpstream(std::vector<std::vector<std::string> >& result) const;

	/**
	 * Fill result with downstream sequence extracted from this DNA sequence.
	 */
	void extractDownstream(std::vector<std::vector<std::string> >& result) const;

	/**
	 * Fill the result with the complete sequence
	 */
	void extractSequence(std::vector<std::vector<std::string> >& result) const;

	/**
	 * Fill the result with the complete structure annotated sequence
	 */
	void extractAnnotatedSequence(std::vector<std::vector<std::string> >& result) const;

	/**
	 * Fill annotation with structure information of this DNA sequence. For every
	 * base there is one element in annotation. "E" denoting an exon, "I" denoting
	 * an intron, "U" denoting the upstream model, "D" denoting the downstream model.
	 */
	void getAnnotation(std::vector<std::string>& annotation) const;

	/**
	 * Fill result with the structure annotation
	 */
	void extractAnnotation(std::vector<std::vector<std::string> >& result) const;

	/**
	 * If the initial sequence contains placeholders for a subset of bases (A,C,G,T) those are
	 * randomly instantiated with a base.
	 */
	void instantiateData();
};




#endif /* DATABASEENTRY_HPP_ */
