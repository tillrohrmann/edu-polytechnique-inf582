/*
 * Modules.cpp
 *
 *  Created on: Mar 15, 2013
 *      Author: Till Rohrmann
 */

#include "Modules.hpp"

#include <fstream>
#include <string>
#include <boost/shared_ptr.hpp>

#include "HMM.hpp"
#include "HMMTransition.hpp"
#include "HMMEmission.hpp"
#include "HMMConnection.hpp"
#include "HMMCompiled.hpp"
#include "HMMNode.hpp"
#include "CrossValidation.hpp"
#include "GeneDatabase.hpp"
#include "Models.hpp"

void Modules::learnCompleteModel() {
	boost::shared_ptr<HMM> exonModel = Models::createExonModel();
	boost::shared_ptr<HMM> intronModel = Models::createIntronModel();
	boost::shared_ptr<HMM> upstreamModel = Models::createUpstreamModel();
	boost::shared_ptr<HMM> downstreamModel = Models::createDownstreamModel();
	boost::shared_ptr<HMM> acceptorModel = Models::create3SpliceSite();
	boost::shared_ptr<HMM> donorModel = Models::create5SpliceSite();
	boost::shared_ptr<HMM> polyASite = Models::create5PolyASite();
	boost::shared_ptr<HMM> startCodon = Models::createStartCodon();
	std::string databaseFilename = "DNASequences.fasta";
	std::string cdsFilename = "CDS.tbl";
	GeneDatabase database;
	std::vector<std::vector<std::string> > trainingset;

	database.importFile(databaseFilename);
	database.importFile(cdsFilename);

	boost::shared_ptr<HMMCompiled> compiled(new HMMCompiled());

	boost::unordered_map<std::string,
			boost::unordered_map<std::string, std::string> > substitution;
	boost::unordered_map<std::string, std::string> substitutionExon;
	boost::unordered_map<std::string, std::string> substitutionIntron;
	boost::unordered_map<std::string, std::string> substitutionDownstream;
	boost::unordered_map<std::string, std::string> substitutionUpstream;
	boost::unordered_map<std::string, std::string> inverseSubstitution;
	std::string bases[] = { "A", "C", "G", "T" };
	std::string prefixes[] = { "D", "U", "E", "I" };
	std::stringstream ss;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			ss.str("");
			ss.clear();
			ss << prefixes[i] << bases[j];

			inverseSubstitution.emplace(ss.str(), bases[j]);
		}
	}

	for (int i = 0; i < 4; i++) {
		ss.str("");
		ss.clear();
		ss << "E" << bases[i];

		substitutionExon.emplace(bases[i], ss.str());
	}

	for (int i = 0; i < 4; i++) {
		ss.str("");
		ss.clear();
		ss << "I" << bases[i];

		substitutionIntron.emplace(bases[i], ss.str());
	}

	for (int i = 0; i < 4; i++) {
		ss.str("");
		ss.clear();
		ss << "D" << bases[i];

		substitutionDownstream.emplace(bases[i], ss.str());
	}

	for (int i = 0; i < 4; i++) {
		ss.str("");
		ss.clear();
		ss << "U" << bases[i];

		substitutionUpstream.emplace(bases[i], ss.str());
	}

	substitution.emplace("Upstream.*",substitutionUpstream);
	substitution.emplace("StartCodon.*",substitutionExon);
	substitution.emplace("Exon.*",substitutionExon);
	substitution.emplace("Downstream.*",substitutionDownstream);
	substitution.emplace("PolyASite.*",substitutionDownstream);
	substitution.emplace("Intron.*",substitutionIntron);

	for(int i=1; i <=3; i++){
		ss.str("");
		ss.clear();
		ss << "Donor" << i;
		substitution.emplace(ss.str(),substitutionExon);
	}

	for(int i=4; i<=9; i++){
		ss.str("");
		ss.clear();
		ss << "Donor" << i;
		substitution.emplace(ss.str(),substitutionIntron);
	}

	for(int i=1;i<=14;i++){
		ss.str("");
		ss.clear();
		ss << "Acceptor" << i;
		substitution.emplace(ss.str(),substitutionIntron);
	}

	substitution.emplace("Acceptor15",substitutionExon);

	boost::shared_ptr<HMM> model = Models::buildGeneModel(upstreamModel,
			startCodon, exonModel, donorModel, intronModel, acceptorModel,
			downstreamModel, polyASite);

	model->substituteEmissions(substitution);

	model->compile(compiled);

	database.extractAnnotatedSequences(trainingset);

	compiled = CrossValidation::crossValidation(compiled,trainingset,0.01,1,114);

	std::cout << trainingset.size() << std::endl;
}

void Modules::learnExonModel() {
	GeneDatabase database;
	database.importFile("DNASequences.fasta");
	database.importCDS("CDS.tbl");

	std::cout << std::log(0) << std::endl;

	std::cout << -std::log(0) + std::log(0) << std::endl;

	std::cout << database.size() << std::endl;

	boost::shared_ptr<HMMCompiled> chmm(new HMMCompiled());
	boost::shared_ptr<HMM> exon = Models::createExonModel();
	exon->compile(chmm);

	std::vector<std::vector<std::string> > trainingset;

	database.extractExons(trainingset);

	std::vector<std::vector<std::string> > smallset(trainingset.begin(),
			trainingset.end());

	std::cout << trainingset.size() << std::endl;

	chmm = CrossValidation::crossValidation(chmm, smallset, 0.005, 1, 500);

	std::cout << chmm->toString() << std::endl;
}

void Modules::test2StateToyExample() {
	boost::shared_ptr<HMM> toy(new HMM());
	boost::shared_ptr<HMMCompiled> ctoy(new HMMCompiled());
	boost::shared_ptr<HMM> ltoy(new HMM());
	boost::shared_ptr<HMMCompiled> lctoy(new HMMCompiled());
	std::vector<int> toystates;
	std::vector<std::string> toyoutput;
	std::vector<std::vector<std::string> > toytrainingSet;
	boost::unordered_set<std::string> emission_set;
	emission_set.insert("A");
	emission_set.insert("G");
	emission_set.insert("C");
	emission_set.insert("T");

	int node0 = toy->createNode("A");
	int node1 = toy->createNode("B");

	toy->addTransition(node0, HMMTransition(node1, 1));
	toy->addTransition(node1, HMMTransition(node1, 1));

	toy->addEmission(node0, HMMEmission("A", 1));
	toy->addEmission(node1, HMMEmission("C", 1));

	toy->addStartNode(node0, 1.0);

	toy->compile(ctoy);

	for (int i = 0; i < 250; i++) {
		toyoutput.clear();
		ctoy->simulate(20, toyoutput, toystates);

		toytrainingSet.push_back(toyoutput);
	}

	ltoy->initializeRandom(2, emission_set);
	ltoy->compile(lctoy);

	lctoy = CrossValidation::crossValidation(lctoy, toytrainingSet, 0.001, 10,
			50);

	std::cout << lctoy->toString() << std::endl;
}

void Modules::test3StateToyExample() {

	std::string filename = "test.hmm";
	boost::shared_ptr<HMM> hmm(new HMM());
	boost::shared_ptr<HMM> learned(new HMM());
	boost::shared_ptr<HMMCompiled> compiled(new HMMCompiled());
	boost::shared_ptr<HMMCompiled> cLearned(new HMMCompiled());
	boost::shared_ptr<HMMCompiled> result;
	boost::unordered_set<std::string> emission_set;
	emission_set.insert("A");
	emission_set.insert("G");
	emission_set.insert("C");
	emission_set.insert("T");

	int a = hmm->createNode("A");
	int b = hmm->createNode("B");
	int c = hmm->createNode("C");

	hmm->addTransition(a, HMMTransition(b, 1));
	hmm->addTransition(b, HMMTransition(c, 0.75));
	hmm->addTransition(b, HMMTransition(b, 0.25));
	hmm->addTransition(c, HMMTransition(c, 1));

	hmm->addStartNode(a, 1.0);

	hmm->addEmission(a, HMMEmission("A", 0.5));
	hmm->addEmission(a, HMMEmission("G", 0.5));
	hmm->addEmission(b, HMMEmission("A", 0.25));
	hmm->addEmission(b, HMMEmission("G", 0.25));
	hmm->addEmission(b, HMMEmission("T", 0.5));
	hmm->addEmission(c, HMMEmission("A", 0.25));
	hmm->addEmission(c, HMMEmission("C", 0.25));
	hmm->addEmission(c, HMMEmission("G", 0.25));
	hmm->addEmission(c, HMMEmission("T", 0.25));

	hmm->compile(compiled);

	std::cout << compiled->toString() << std::endl;

	std::vector<int> states;
	std::vector<std::string> output;
	std::vector<std::vector<std::string> > trainingSet;
	for (int i = 0; i < 500; i++) {
		output.clear();
		compiled->simulate(30, output, states);
		trainingSet.push_back(output);

		//		std::cout << i << ":";
		//		for(int j=0; j<output.size(); j++){
		//			std::cout << output[j];
		//		}
		//		std::cout << std::endl;
	}

	learned->initializeRandom(3, emission_set);

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
	std::cout << std::exp(compiled->forward(sequence)) << std::endl;
	std::cout << compiled->forwardR(sequence) << std::endl;

	states.clear();

	compiled->viterbi(sequence, states);

	for (int i = 0; i < states.size(); i++) {
		std::cout << compiled->getNode(states[i])->getName();
	}

	std::cout << std::endl;

	learned->compile(cLearned);

	std::cout << "Crossvalidation" << std::endl;
	result = CrossValidation::crossValidation(cLearned, trainingSet, 0.001, 5,
			100);
	std::cout << "finished cv" << std::endl;

	std::cout << result->toString() << std::endl;

	learned->update(result);

	std::fstream file;

	file.open(filename.c_str(), std::ios_base::out);

	learned->serialize(file);

	file.close();

	file.close();

	boost::shared_ptr<HMM> newHMM(new HMM());

	file.open(filename.c_str(), std::ios_base::in);

	HMM::deserialize(file, newHMM);

	file.close();

	newHMM->serialize(std::cout);

}
