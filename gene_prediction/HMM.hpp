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
class HMMTransition;
class HMMConnection;
class HMMEmission;

typedef boost::shared_ptr<HMMNode> ptrHMMNode;

class HMM{
private:
	boost::unordered_map<int, boost::shared_ptr<HMMNode> > _nodes;
	int _nextID;
	boost::unordered_map<int,double> _startNodes;
	boost::unordered_set<int> _endNodes;

public:
	HMM();

	void clear();

	ptrHMMNode getNode(int id);
	ptrHMMNode getNode(const std::string& name);

	const boost::unordered_map<int,boost::shared_ptr<HMMNode> >& getNodes() const { return _nodes; }

	const boost::unordered_map<int,double>& getStartNodes() const {return _startNodes;}
	const boost::unordered_set<int>& getEndNodes() const {return _endNodes;}

	bool hasNode(int id);

	int createNode(std::string name ="",bool constantTransitions=false, bool constantEmissions=false);

	void removeNode(int id);

	void addTransition(int src, const HMMTransition& transition);
	void addEmission(int src, const HMMEmission& emission);

	void addTransitions(int src, const std::vector<HMMTransition>& transitions);
	void addEmissions(int src, const std::vector<HMMEmission>& emissions);

	void setConstantTransitions(int src, bool constant);
	void setConstantEmissions(int src, bool constant);

	void addStartNode(int nodeID,double probability);
	void addStartNode(const std::string& nodeName, double probability);
	void addEndNode(int nodeID);
	void addEndNode(const std::string & nodeName);

	void serialize(std::ostream& os) const;
	static void deserialize(std::istream& is,boost::shared_ptr<HMM> hmm);

	int size() const;
	int shallowSize() const;

	void compile(boost::shared_ptr<HMMCompiled> compiled);
	void update(boost::shared_ptr<HMMCompiled> compiled);

	void initializeRandom(int numberStates, boost::unordered_set<std::string>& emissions);

	void integrateHMM(boost::shared_ptr<HMM> hmm, const boost::unordered_map<std::string,HMMConnection>& connections);
	void resetTransitions(int id);

	void substituteEmissions(const boost::unordered_map<std::string, boost::unordered_map<std::string, std::string> >& substitution);
};



#endif /* HMM_HPP_ */
