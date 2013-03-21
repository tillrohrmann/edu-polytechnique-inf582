/*
 * HMMNode.hpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMNODE_HPP_
#define HMMNODE_HPP_

#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

class HMMTransition;
class HMMEmission;
class HMM;
class HMMCompiled;

typedef boost::unordered_map<int,HMMTransition> Transition;
typedef boost::unordered_map<std::string, HMMEmission> Emission;

/**
 * This class represents a HMM node which contains the transitions to other nodes
 * and the emissions with the associated probabilities.
 */
class HMMNode : public boost::enable_shared_from_this<HMMNode> {
protected:
	int _id;
	Transition _transitions;
	Emission _emissions;
	std::string _name;
	bool _isSilent;
	bool _constantEmissions;
	bool _constantTransitions;
	bool _constantEmissionSet;
public:
	HMMNode(int id=-1,const std::string& name = "");
	HMMNode(int id,const std::string& name, const Transition& transitions,const Emission& emissions,
			bool constantTransitions, bool constantEmissions, bool constantEmissionSet);
	virtual ~HMMNode();

	int getID() const{ return _id; }
	Transition& getTransition() { return _transitions; }
	const Transition& getTransition() const { return _transitions;}
	Emission& getEmission() { return _emissions; }
	const Emission& getEmission() const { return _emissions;}
	std::string getName() const{ return _name; }

	bool constantTransitions() const { return _constantTransitions;}
	bool constantEmissions() const { return _constantEmissions; }
	bool constantEmissionSet() const {return _constantEmissionSet;}

	bool& constantTransitions() { return _constantTransitions;}
	bool& constantEmissions() { return _constantEmissions;}
	bool& constantEmissionSet() {return _constantEmissionSet;}

	virtual int size() const { return 1; }
	int shallowSize() const { return 1; }

	void setSilent(bool silent) { _isSilent = silent; }
	bool getSilent() const { return _isSilent; }

	virtual void addTransition(const HMMTransition& transition);
	virtual void removeTransition(int destination);

	virtual void addEmission(const HMMEmission& emission);
	virtual void removeEmission(const std::string& token);

	virtual void serialize(std::ostream& os) const;

	/**
	 * This function inserts this node into the HMMCompiled instance compiled.
	 */
	virtual void buildMapping(HMMCompiled & compiled);
	/**
	 * This function inserts the transitions of this node into the HMMCompiled
	 * instance compiled.
	 */
	virtual void buildTransitions(HMMCompiled& compiled, HMM& hmm);

	/**
	 * This function is used to write back the possible newly learned values
	 * in the HMMCompiled form to the HMM form.
	 */
	virtual void updateValues(HMMCompiled &compiled, HMM& hmm);

	static void deserialize(std::istream& is,boost::shared_ptr<HMMNode> hmmNode);

	static boost::shared_ptr<HMMNode> deserialize(std::istream& is);
	static void serialize(std::ostream& os, boost::shared_ptr<HMMNode> hmmNode);

	/**
	 * This functions replaces all emissions by the emissions defined in substitution:
	 * newEmission = substitution[oldEmission].
	 */
	void substituteEmissions(const boost::unordered_map<std::string, std::string>& substitution);
};


#endif /* HMMNODE_HPP_ */
