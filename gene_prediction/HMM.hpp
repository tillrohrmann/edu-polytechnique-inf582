/*
 * HMM.hpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMM_HPP_
#define HMM_HPP_

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>

class HMMNode;
class HMMCompiled;
class HMMSingleNode;
class HMMContainer;
class HMMTransition;
class HMMEmission;

typedef boost::shared_ptr<HMMNode> ptrHMMNode;

class HMM{
private:
	boost::unordered_map<int, boost::shared_ptr<HMMNode> > _nodes;
	int _nextID;
	boost::unordered_set<int> _startNodes;
	boost::unordered_set<int> _endNodes;

public:
	HMM();

	ptrHMMNode getNode(int id);
	ptrHMMNode getNode(const std::string& name);

	const boost::unordered_map<int,boost::shared_ptr<HMMNode> >& getNodes() const { return _nodes; }

	const boost::unordered_set<int>& getStartNodes() const {return _startNodes;}
	const boost::unordered_set<int>& getEndNodes() const {return _endNodes;}

	bool hasNode(int id);

	int createNode(std::string name ="");
	int createContainer(std::string name = "");

	void removeNode(int id);

	void addTransition(int src, const HMMTransition& transition);
	void addEmission(int src, const HMMEmission& emission);

	void addTransitions(int src, const std::vector<HMMTransition>& transitions);
	void addEmissions(int src, const std::vector<HMMEmission>& emissions);

	void addStartNode(int nodeID);
	void addEndNode(int nodeID);

	void insertModel(int containerID, const boost::shared_ptr<HMM>& hmm);

	void serialize(std::ostream& os) const;
	static void deserialize(std::istream& is,boost::shared_ptr<HMM> hmm);

	int size() const;
	int shallowSize() const;

	void compile(boost::shared_ptr<HMMCompiled> compiled);
	void update(boost::shared_ptr<HMMCompiled> compiled);

};



#endif /* HMM_HPP_ */
