/*
 * Models.hpp
 *
 *  Created on: Feb 20, 2013
 *      Author: Till Rohrmann
 */

#ifndef MODELS_HPP_
#define MODELS_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

class HMM;

namespace Models{
	boost::unordered_map<std::string,std::string> createVeilMapping();

	extern boost::unordered_map<std::string,std::string> veilMapping;

	boost::shared_ptr<HMM> createExonModel();
	boost::shared_ptr<HMM> createIntronModel();
	boost::shared_ptr<HMM> createUpstreamModel();
	boost::shared_ptr<HMM> createStartCodon();
	boost::shared_ptr<HMM> create5SpliceSite();
	boost::shared_ptr<HMM> create3SpliceSite();
	boost::shared_ptr<HMM> createDownstreamModel();
	boost::shared_ptr<HMM> create5PolyASite();

	boost::shared_ptr<HMM> createVeilModel();

	void createAnnotatedSubstitution(boost::unordered_map<std::string, boost::unordered_map<std::string,std::string> >& result);
	void createInverseAnnotatedSubstitution(boost::unordered_map<std::string, boost::unordered_map<std::string, std::string> >& result);

	boost::shared_ptr<HMM> buildGeneModel(boost::shared_ptr<HMM> upstreamModel, boost::shared_ptr<HMM> startCodon,
			boost::shared_ptr<HMM> exonModel, boost::shared_ptr<HMM> donor, boost::shared_ptr<HMM> intronModel,
			boost::shared_ptr<HMM> acceptor, boost::shared_ptr<HMM> downstreamModel, boost::shared_ptr<HMM> polyASite);

	void VeilAnnotation(const std::vector<std::string>& states, std::vector<std::string>& output);
}




#endif /* MODELS_HPP_ */
