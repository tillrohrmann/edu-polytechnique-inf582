/*
 * main.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "Modules.hpp"
#include <boost/unordered_map.hpp>
#include <iostream>

int main(int argc, char** argv){

	//Modules::learnCompleteModel();
	//Modules::test2StateToyExample();
	//Modules::test3StateToyExample();
	//Modules::learnExonModel();

	Modules::evaluateModel("completeModel.hmm");

	return 0;
}
