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

/**
 * This class is the high level representation of a HMM. It contains a map
 * of HMM nodes which represent the a state of a HMM. Furthermore, the  starting
 * nodes with their associated probabilities and acceptable ending nodes are
 * stored within this class.
 */
class HMM {
private:
	// each node has a unique integer ID assigned
	boost::unordered_map<int, boost::shared_ptr<HMMNode> > _nodes;
	int _nextID;
	boost::unordered_map<int, double> _startNodes;
	boost::unordered_set<int> _endNodes;

public:
	HMM();

	void clear();

	ptrHMMNode getNode(int id);
	ptrHMMNode getNode(const std::string& name);

	const boost::unordered_map<int, boost::shared_ptr<HMMNode> >& getNodes() const {
		return _nodes;
	}

	const boost::unordered_map<int, double>& getStartNodes() const {
		return _startNodes;
	}
	const boost::unordered_set<int>& getEndNodes() const {
		return _endNodes;
	}

	bool hasNode(int id);

	int createNode(std::string name = "", bool constantTransitions = false,
			bool constantEmissions = false, bool constantEmissionSet = false);

	void removeNode(int id);

	/**
	 * Adds a new transition specified by transition to the node with ID src
	 */
	void addTransition(int src, const HMMTransition& transition);

	/**
	 * Adds a new emission specified by emission to the node with ID src
	 */
	void addEmission(int src, const HMMEmission& emission);

	/**
	 * Adds a set of transitions specified by transitions to the node with ID src
	 */
	void addTransitions(int src, const std::vector<HMMTransition>& transitions);

	/**
	 * Adds a set of emissions specified by emissions to the node with ID src
	 */
	void addEmissions(int src, const std::vector<HMMEmission>& emissions);

	/**
	 * Sets the set of transitions of the node with ID src to be constant
	 */
	void setConstantTransitions(int src, bool constant);

	/**
	 * Sets the set of emissions (including probabilities) of
	 * the node with ID src to be constant
	 */
	void setConstantEmissions(int src, bool constant);

	/**
	 * Sets only the set of emissions (excluding probabilities) of
	 * the node with ID src to be constant
	 */
	void setConstantEmissionSet(int src, bool constant = true);

	void addStartNode(int nodeID, double probability);
	void addStartNode(const std::string& nodeName, double probability);
	void addEndNode(int nodeID);
	void addEndNode(const std::string & nodeName);

	/**
	 * This functions writes the object to os
	 */
	void serialize(std::ostream& os) const;

	/**
	 * This functions reads from is a HMM object and stores it into hmm
	 */
	static void deserialize(std::istream& is, boost::shared_ptr<HMM> hmm);

	int size() const;
	int numTransitions() const;
	int shallowSize() const;

	/**
	 * This function converts the HMM (graph like representation) into a more
	 * computability-friendly version (matrix based representation) which is then
	 * used for the HMM algorithms.
	 */
	void compile(boost::shared_ptr<HMMCompiled> compiled);

	/**
	 * This function writes back the transition and emission probabilities from a
	 * HMMCompiled instance to this HMM. That is necessary after one has learned
	 * the HMM and wants to serialize it afterwards.
	 *
	 * @argument compiled instance from which one extracts the new probabilities
	 */
	void update(boost::shared_ptr<HMMCompiled> compiled);

	/**
	 * Initializes a completely connected HMM model with numberStates nodes where
	 * all nodes have the emissions specified by emissions with random probabilities
	 * and also random transition probabilities.
	 *
	 * @argument numberStates number of states which the new HMM has
	 * @argument emissions set of emissions which all states can emit
	 */
	void initializeRandom(int numberStates,
			boost::unordered_set<std::string>& emissions);

	/**
	 * This function takes another HMM instance and creates a copy of it within this
	 * instance. Furthermore additional connections will be established between
	 * the new and old nodes. Those connection are specified by the map connections.
	 * The key indicates the name of the node and the value for that key is a vector
	 * of new connections which are added to the node.
	 *
	 * @argument hmm new HMM which is supposed to be integrated into the current one
	 * @argument connection map containing new connections. The source is defined by
	 * 		the name of the corresponding node and the value to that key contains the
	 * 		specification (destinations and probabilities) of a set of connections.
	 */
	void integrateHMM(boost::shared_ptr<HMM> hmm,
			const boost::unordered_map<std::string, std::vector<HMMConnection> >& connections);

	/**
	 * This functions sets the probabilities of all connections of the node with ID id
	 * to a random probability.
	 */
	void resetTransitions(int id);

	/**
	 * This functions substitutes the emissions for every node which matches a key of the
	 * map substitution by the defined values in the value associated to key. The key is
	 * interpreted as a regex and the value is map which contains the mapping between old
	 * emission symbol and new emission symbol.
	 *
	 * @argument substitution map containing the substitution information
	 */
	void substituteEmissions(
			const boost::unordered_map<std::string,
					boost::unordered_map<std::string, std::string> >& substitution);
};

#endif /* HMM_HPP_ */
