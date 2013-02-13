/*
 * HMMNode.hpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMNODE_HPP_
#define HMMNODE_HPP_

#include <tr1/unordered_map>
#include "HMMTransition.hpp"
#include "HMMEmission.hpp"
#include <tr1/memory>
#include <ostream>
#include <istream>

class HMM;

typedef std::tr1::unordered_map<int,HMMTransition> Transition;
typedef std::tr1::unordered_map<std::string, HMMEmission> Emission;

class HMMNode{
protected:
	int _id;
	Transition _transitions;
	Emission _emissions;
	std::string _name;
	bool _isSilent;
public:
	HMMNode(int id,std::string name = "");
	HMMNode(int id,std::string name, const Transition& transitions,const Emission& emissions);
	virtual ~HMMNode();

	int getID() const{ return _id; }
	Transition& getTransition() { return _transitions; }
	Emission& getEmission() { return _emissions; }
	std::string getName() const{ return _name; }

	void setSilent(bool silent) { _isSilent = silent; }
	bool getSilent() const { return _isSilent; }

	virtual void addTransition(const HMMTransition& transition);
	virtual void removeTransition(int destination);

	virtual void addEmission(const HMMEmission& emission);
	virtual void removeEmission(const std::string& token);

	virtual void insertModel(const std::tr1::shared_ptr<HMM>& hmm);
	virtual std::tr1::shared_ptr<HMM> replaceModel(const std::tr1::shared_ptr<HMM>& hmm);

	virtual void serialize(std::ostream& os) const;

	static std::tr1::shared_ptr<HMMNode> deserialize(std::istream& is);
};


#endif /* HMMNODE_HPP_ */
