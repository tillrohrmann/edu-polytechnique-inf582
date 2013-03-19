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

/**
 * Convenience functions for the construction of the VEIL model and emission
 * substitutions.
 */
namespace Models {

/**
 * This function creates the mapping from the states names of the
 * VEIL model to the structure of the DNA (exons, introns, downstream
 * model, upstream model).
 */
boost::unordered_map<std::string, std::string> createVeilMapping();

/**
 * Storage for the VEIL to DNA mapping
 */
extern boost::unordered_map<std::string, std::string> veilMapping;

boost::shared_ptr<HMM> createExonModel();
boost::shared_ptr<HMM> createIntronModel();
boost::shared_ptr<HMM> createUpstreamModel();
boost::shared_ptr<HMM> createStartCodon();
boost::shared_ptr<HMM> create5SpliceSite();
boost::shared_ptr<HMM> create3SpliceSite();
boost::shared_ptr<HMM> createDownstreamModel();
boost::shared_ptr<HMM> create5PolyASite();

boost::shared_ptr<HMM> createVeilModel();

/**
 * This functions create the substitution for annotated sequences. That is
 * to say, e.g. for exons
 * 	A -> EA
 * 	C -> EC
 * 	G -> EG
 * 	T -> ET
 */
void createAnnotatedSubstitution(
		boost::unordered_map<std::string,
				boost::unordered_map<std::string, std::string> >& result);

/**
 * This functions create the emission substitution to reverse the effect of
 * createAnnotatedSubstitution.
 */
void createInverseAnnotatedSubstitution(
		boost::unordered_map<std::string,
				boost::unordered_map<std::string, std::string> >& result);

/**
 * This function takes the individual models of the VEIL model and connects
 * them properly.
 */
boost::shared_ptr<HMM> buildGeneModel(boost::shared_ptr<HMM> upstreamModel,
		boost::shared_ptr<HMM> startCodon, boost::shared_ptr<HMM> exonModel,
		boost::shared_ptr<HMM> donor, boost::shared_ptr<HMM> intronModel,
		boost::shared_ptr<HMM> acceptor, boost::shared_ptr<HMM> downstreamModel,
		boost::shared_ptr<HMM> polyASite);

/**
 * This functions takes a sequence of state names of the VEIL model and
 * translates them to the DNA structure. That is to say that it replaces
 * those states representing an exon by "E", those representing an intron
 * by "I", the downstream model by "D" and the upstream model by "U".
 *
 * @argument states input sequence of state names
 * @argument output output for the translated sequence
 *
 */
void VeilAnnotation(const std::vector<std::string>& states,
		std::vector<std::string>& output);
}

#endif /* MODELS_HPP_ */
