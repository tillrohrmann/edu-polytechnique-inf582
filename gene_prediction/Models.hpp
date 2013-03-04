/*
 * Models.hpp
 *
 *  Created on: Feb 20, 2013
 *      Author: Till Rohrmann
 */

#ifndef MODELS_HPP_
#define MODELS_HPP_

class HMM;

namespace Models{
	boost::shared_ptr<HMM> createExonModel();
	boost::shared_ptr<HMM> createIntronModel();
}




#endif /* MODELS_HPP_ */
