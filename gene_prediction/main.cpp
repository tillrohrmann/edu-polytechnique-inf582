/*
 * main.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMM.hpp"
#include "HMMTransition.hpp"
#include "HMMCompiled.hpp"
#include <fstream>
#include <string>
#include <boost/shared_ptr.hpp>

int main(int argc, char** argv){
	std::string filename = "test.hmm";
	HMM hmm;
	boost::shared_ptr<HMM> hmmExon(new HMM());
	boost::shared_ptr<HMMCompiled> compiled(new HMMCompiled());

	int exon = hmm.createContainer("Exon");
	int intron = hmm.createNode("Intron");
	int intergenic = hmm.createNode("Intergenic");

	hmm.addTransition(intron,HMMTransition(1,exon,false));
	hmm.addTransition(intergenic,HMMTransition(0.5,intergenic,false));
	hmm.addTransition(intergenic,HMMTransition(0.5,exon,false));
	hmm.addTransition(exon,HMMTransition(0.5,intergenic,false));
	hmm.addTransition(exon,HMMTransition(0.5,exon,false));

	int a = hmmExon->createNode("A");
	int b = hmmExon->createNode("B");
	int c = hmmExon->createNode("C");

	hmmExon->addTransition(a,HMMTransition(1,b,false));
	hmmExon->addTransition(b,HMMTransition(1,c,false));
	hmmExon->addStartNode(a);
	hmmExon->addEndNode(c);

	hmm.insertModel(exon,hmmExon);

	hmm.compile(compiled);
	std::cout << compiled->toString() << std::endl;

	hmm.update(compiled);

//	std::fstream file;
//
//	file.open(filename.c_str(),std::ios_base::out);
//
//	hmm.serialize(file);
//
//	file.close();
//
//	boost::shared_ptr<HMM> newHMM(new HMM());
//
//	file.open(filename.c_str(),std::ios_base::in);
//
//	HMM::deserialize(file,newHMM);
//
//	file.close();
//
//	newHMM->serialize(std::cout);

	return 0;
}
