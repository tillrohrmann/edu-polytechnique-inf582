/*
 * Modules.hpp
 *
 *  Created on: Mar 15, 2013
 *      Author: Till Rohrmann
 */

#ifndef MODULES_HPP_
#define MODULES_HPP_

#include <string>

namespace Modules{
	void learnCompleteModel();
	void learnExonModel();

	void evaluateModel(const std::string & hmmFilename);

	void learnAndEvaluateModel(const std::string& prefix ="");

	void test2StateToyExample();
	void test3StateToyExample();
}




#endif /* MODULES_HPP_ */
