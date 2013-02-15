/*
 * HMMCompiled.hpp
 *
 *  Created on: Feb 14, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMCOMPILED_HPP_
#define HMMCOMPILED_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

class HMMNode;

class HMMCompiled{
private:
	int _numberNodes;
	double* _transitions;
	boost::unordered_map<std::string,double>* _emissions;
	boost::unordered_set<int> _silentStates;
	double* _initialDistribution;
	bool* _constantTransitions;

	boost::unordered_map<int,boost::shared_ptr<HMMNode> > _int2Node;
	boost::unordered_map<boost::shared_ptr<HMMNode>, int > _node2Int;

	int _counter;
public:
	HMMCompiled();
	~HMMCompiled();

	void setConstant(int x, int y, bool value) { _constantTransitions[x*_numberNodes + y] = value; }
	bool getConstant(int x, int y)const { return _constantTransitions[x*_numberNodes + y]; }

	void setTransition(int x, int y, double value) { _transitions[x*_numberNodes + y] = value; }
	double getTransition(int x, int y)const { return _transitions[x*_numberNodes + y]; }
	double getTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest);

	double getEmission(boost::shared_ptr<HMMNode> src,const std::string & token);

	void clear();
	void initialize(int numberNodes);

	void addMapping(boost::shared_ptr<HMMNode> node);

	void addSilentNode(boost::shared_ptr<HMMNode> node);
	void addConstantTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest);
	void addTransition(boost::shared_ptr<HMMNode> src, boost::shared_ptr<HMMNode> dest, double probability);
	void addEmission(boost::shared_ptr<HMMNode> src, const std::string& token, double probability);

	int getIndex(boost::shared_ptr<HMMNode> node) const;
	boost::shared_ptr<HMMNode> getNode(int index) const;

	std::string toString() const;
};




#endif /* HMMCOMPILED_HPP_ */
