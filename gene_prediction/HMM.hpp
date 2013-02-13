/*
 * HMM.hpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMM_HPP_
#define HMM_HPP_

#include "HMMSingleNode.hpp"
#include "HMMContainer.hpp"
#include <tr1/memory>
#include <vector>
#include <tr1/unordered_set>
#include <ostream>
#include <istream>

typedef std::tr1::shared_ptr<HMMNode> ptrHMMNode;

class HMM{
private:
	std::tr1::unordered_map<int, ptrHMMNode > _nodes;
	int _nextID;
	std::tr1::unordered_set<int> _startNodes;
	std::tr1::unordered_set<int> _endNodes;
public:
	HMM();

	ptrHMMNode getNode(int id);
	ptrHMMNode getNode(const std::string& name);

	const std::tr1::unordered_set<int>& getStartNodes() const {return _startNodes;}
	const std::tr1::unordered_set<int>& getEndNodes() const {return _endNodes;}

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

	void insertModel(int containerID, const std::tr1::shared_ptr<HMM>& hmm);

	void serialize(std::ostream& os) const;
	static std::tr1::shared_ptr<HMM> deserialize(std::istream& is);

};



#endif /* HMM_HPP_ */
