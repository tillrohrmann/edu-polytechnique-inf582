/*
 * Models.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: Till Rohrmann
 */

#include "HMM.hpp"

#include "nullPtr.hpp"

boost::shared_ptr<HMM> createExonModel(){
	boost::shared_ptr<HMM> result(new HMM());

	int startNodes[4];
	int intermediateNodes[4];
	int endNodes;

	for(int i=0; i<4;i++){
		startNodes[i] = result->createNode("Exon1");
	}



	return result;
}

boost::shared_ptr<HMM> createIntronModel(){
	return nullPtr;
}


