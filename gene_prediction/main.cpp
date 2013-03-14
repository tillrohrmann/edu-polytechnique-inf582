/*
 * main.cpp
 *
 *  Created on: Feb 12, 2013
 *      Author: Till Rohrmann
 */

#include "HMM.hpp"
#include "HMMTransition.hpp"
#include "HMMEmission.hpp"
#include "HMMCompiled.hpp"
#include "HMMNode.hpp"
#include "CrossValidation.hpp"
#include <fstream>
#include <string>
#include <boost/shared_ptr.hpp>
#include "GeneDatabase.hpp"
#include "Models.hpp"
#include "HMMConnection.hpp"

int main(int argc, char** argv){

//	GeneDatabase database;
//	database.importFile("DNASequences.fasta");
//	database.importCDS("CDS.tbl");
//
//	std::cout << std::log(0) << std::endl;
//
//	std::cout << -std::log(0) + std::log(0) << std::endl;
//
//	std::cout << database.size() << std::endl;
//
//	boost::shared_ptr<HMMCompiled> chmm(new HMMCompiled());
//	boost::shared_ptr<HMM> exon = Models::createExonModel();
//	exon->compile(chmm);
//
//	std::vector<std::vector<std::string> > trainingset;
//
//	database.extractExons(trainingset);
//
//	std::vector<std::vector<std::string> > smallset(trainingset.begin(),trainingset.begin()+500);
//
//	std::cout << trainingset.size() << std::endl;
//
//	CrossValidation::crossValidation(chmm,smallset,0.001,1,100);
//
//	std::cout << chmm->toString() << std::endl;



	std::string filename = "test.hmm";
	boost::shared_ptr<HMM> hmm(new HMM());
	boost::shared_ptr<HMM> learned(new HMM());
	boost::shared_ptr<HMMCompiled> compiled(new HMMCompiled());
	boost::shared_ptr<HMMCompiled> cLearned(new HMMCompiled());
	boost::shared_ptr<HMMCompiled> result;

	int a = hmm->createNode("A");
	int b = hmm->createNode("B");
	int c = hmm->createNode("C");

	hmm->addTransition(a,HMMTransition(1.0,b,false));
	hmm->addTransition(b,HMMTransition(0.75,c,false));
	hmm->addTransition(b,HMMTransition(0.25,b,false));
	hmm->addTransition(c,HMMTransition(1.0,c,false));

	hmm->addStartNode(a,1.0);

	hmm->addEmission(a,HMMEmission(0.5,"A",false));
	hmm->addEmission(a,HMMEmission(0.5,"G",false));
	hmm->addEmission(b,HMMEmission(0.25,"A",false));
	hmm->addEmission(b,HMMEmission(0.25,"G",false));
	hmm->addEmission(b,HMMEmission(0.5,"T",false));
	hmm->addEmission(c,HMMEmission(0.25,"A",false));
	hmm->addEmission(c,HMMEmission(0.25,"C",false));
	hmm->addEmission(c,HMMEmission(0.25,"G",false));
	hmm->addEmission(c,HMMEmission(0.25,"T",false));

	hmm->compile(compiled);

	std::cout << compiled->toString() << std::endl;

	std::vector<int> states;
	std::vector<std::string> output;
	std::vector<std::vector<std::string> > trainingSet;
	for(int i=0; i < 500; i++){
		output.clear();
		compiled->simulate(20,output,states);
		trainingSet.push_back(output);

//		std::cout << i << ":";
//		for(int j=0; j<output.size(); j++){
//			std::cout << output[j];
//		}
//		std::cout << std::endl;
	}

	boost::unordered_set<std::string> emission_set;
	emission_set.insert("A");
	emission_set.insert("G");
	emission_set.insert("C");
	emission_set.insert("T");

	learned->initializeRandom(3,emission_set);

	std::vector<std::string> sequence;
	sequence.push_back("A");
	sequence.push_back("A");
	sequence.push_back("T");
	sequence.push_back("T");
	sequence.push_back("A");
	sequence.push_back("C");
	sequence.push_back("G");
	sequence.push_back("T");
	sequence.push_back("A");
	sequence.push_back("C");
	sequence.push_back("G");
	sequence.push_back("T");
	sequence.push_back("A");
	sequence.push_back("C");
	sequence.push_back("G");
	std::cout << compiled->forward(sequence) << std::endl;
	std::cout << compiled->backward(sequence) << std::endl;

	states.clear();

	compiled->viterbi(sequence,states);

	for(int i =0; i< states.size(); i++){
		std::cout << compiled->getNode(states[i])->getName();
	}

	std::cout << std::endl;

	learned->compile(cLearned);

	std::cout << "Crossvalidation" << std::endl;
	result = CrossValidation::crossValidation(cLearned,trainingSet,0.001,5,100);
	std::cout << "finished cv" << std::endl;

	std::cout << result->toString() << std::endl;

	learned->update(result);

	std::fstream file;

	file.open(filename.c_str(),std::ios_base::out);

	learned->serialize(file);

	file.close();

	file.close();

	boost::shared_ptr<HMM> newHMM(new HMM());

	file.open(filename.c_str(),std::ios_base::in);

	HMM::deserialize(file,newHMM);

	file.close();

	newHMM->serialize(std::cout);

	return 0;
}
