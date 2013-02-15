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
	boost::shared_ptr<HMM> _model;
	int _startNode;
	int _endNode;
public:
	HMMContainer(int id=-1, std::string name = "");
	HMMContainer(int id,std::string name, const Transition& transitions, const Emission& emissions);
	virtual ~HMMContainer();

	virtual int size() const;

	virtual void addEmission(const HMMEmission& emission);
	virtual void removeEmission(const std::string& token);

	virtual void insertModel(const boost::shared_ptr<HMM>& hmm);
	virtual boost::shared_ptr<HMM> replaceModel(const boost::shared_ptr<HMM>& hmm);

	virtual boost::shared_ptr<HMM> extractModel();

	virtual void buildMapping(HMMCompiled& compiled);
	virtual void buildTransitions(HMMCompiled& compiled, HMM& hmm);

	virtual void updateValues(HMMCompiled& compiled, HMM& hmm);

	virtual void serialize(std::ostream& os) const;

	static void deserialize(std::istream& is,boost::shared_ptr<HMMContainer> hmmContainer);

};


#endif /* HMMCONTAINER_HPP_ */
