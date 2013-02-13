/*
 * HMMContainer.hpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMCONTAINER_HPP_
#define HMMCONTAINER_HPP_

#include "HMMNode.hpp"

class HMMContainer : public HMMNode{
private:
	std::tr1::shared_ptr<HMM> _model;
	int _startNode;
	int _endNode;
public:
	HMMContainer(int id, std::string name = "");
	HMMContainer(int id,std::string name, const Transition& transitions, const Emission& emissions);
	virtual ~HMMContainer();

	virtual void addTransition(const HMMTransition& transition);
	virtual void removeTransition(int destination);

	virtual void addEmission(const HMMEmission& emission);
	virtual void removeEmission(const std::string& token);

	virtual void insertModel(const std::tr1::shared_ptr<HMM>& hmm);
	virtual std::tr1::shared_ptr<HMM> replaceModel(const std::tr1::shared_ptr<HMM>& hmm);

	virtual void serialize(std::ostream& os) const;

	static std::tr1::shared_ptr<HMMContainer> deserialize(std::istream& is);
};


#endif /* HMMCONTAINER_HPP_ */
