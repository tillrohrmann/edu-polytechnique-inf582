/*
 * main.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "Modules.hpp"
#include <boost/unordered_map.hpp>
#include <iostream>

int main(int argc, char** argv) {
	// Learn complete model and start result files with "3"
	Modules::learnAndEvaluateModel("3");

	return 0;
}
