/*
 * HMMConnection.hpp
 *
 *  Created on: Mar 12, 2013
 *      Author: Till Rohrmann
 */

#ifndef HMMCONNECTION_HPP_
#define HMMCONNECTION_HPP_

#include <string>

/**
 * This class represents a connection of the HMM. For that purpose it contains
 * the name of the destination node and the probability. This class is used
 * for the integration of an HMM into another and the inter-HMM transitions are
 * represented by HMMConnections. Since one does not know the internal ids of the
 * newly added HMM one uses the nodes' names instead.
 */
struct HMMConnection{
	double _probability;
	std::string _destination;

	HMMConnection(const std::string& destination = "", double probability = -1.0);
};





#endif /* HMMCONNECTION_HPP_ */
