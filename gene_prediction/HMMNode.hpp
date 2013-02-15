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

class HMMNode : public boost::enable_shared_from_this<HMMNode> {
protected:
	int _id;
	Transition _transitions;
	Emission _emissions;
	std::string _name;
	bool _isSilent;
public:
	HMMNode(int id=-1,const std::string& name = "");
	HMMNode(int id,const std::string& name, const Transition& transitions,const Emission& emissions);
	virtual ~HMMNode();

	int getID() const{ return _id; }
	Transition& getTransition() { return _transitions; }
	Emission& getEmission() { return _emissions; }
	std::string getName() const{ return _name; }

	virtual int size() const { return 1; }
	int shallowSize() const { return 1; }

	void setSilent(bool silent) { _isSilent = silent; }
	bool getSilent() const { return _isSilent; }

	virtual void addTransition(const HMMTransition& transition);
	virtual void removeTransition(int destination);

	virtual void addEmission(const HMMEmission& emission);
	virtual void removeEmission(const std::string& token);

	virtual void insertModel(const boost::shared_ptr<HMM>& hmm);
	virtual boost::shared_ptr<HMM> replaceModel(const boost::shared_ptr<HMM>& hmm);

	virtual void serialize(std::ostream& os) const;

	virtual void buildMapping(HMMCompiled & compiled);
	virtual void buildTransitions(HMMCompiled& compiled, HMM& hmm);

	virtual void updateValues(HMMCompiled &compiled, HMM& hmm);

	static void deserialize(std::istream& is,boost::shared_ptr<HMMNode> hmmNode);

	static boost::shared_ptr<HMMNode> deserialize(std::istream& is);
	static void serialize(std::ostream& os, boost::shared_ptr<HMMNode> hmmNode);
};


#endif /* HMMNODE_HPP_ */
