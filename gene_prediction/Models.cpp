/*
 * Models.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: Till Rohrmann
 */

#include "Models.hpp"
#include "HMM.hpp"
#include "nullPtr.hpp"
#include "HMMTransition.hpp"
#include "HMMEmission.hpp"
#include "HMMConnection.hpp"

#include <sstream>
#include <boost/regex.hpp>

boost::unordered_map<std::string,std::string> Models::veilMapping = createVeilMapping();

/**
 * For the explanation of the model see the paper
 * "Finding Genes in DNA with a Hidden Markov Model".
 */
boost::shared_ptr<HMM> Models::createExonModel(){
	boost::shared_ptr<HMM> result(new HMM());
	std::string endings[] = {"A", "C", "G", "T"};
	std::stringstream ss1,ss2,ss3;

	int startNodes[4];
	int intermediateNodes[4];
	int endNodes[4];
	int absorbing1 = result->createNode("ExonAbsorbing1");
	int absorbing2 = result->createNode("ExonAbsorbing2");
	int absorbing3 = result->createNode("ExonAbsorbing3");
	int absorbing4 = result->createNode("ExonAbsorbing4");
	int stopCodonA1 = result->createNode("ExonStopCodonA1");
	int stopCodonA2 = result->createNode("ExonStopCodonA2");
	int stopCodonG1 = result->createNode("ExonStopCodonG1");
	int stopCodonG2 = result->createNode("ExonStopCodonG2");

	for(int i=0; i<4; i++){
		result->addEmission(absorbing1,HMMEmission(endings[i],-1));
		result->addEmission(absorbing2,HMMEmission(endings[i],-1));
		result->addEmission(absorbing3,HMMEmission(endings[i],-1));
		result->addEmission(absorbing4,HMMEmission(endings[i],-1));
	}

	result->setConstantEmissionSet(absorbing1);
	result->setConstantEmissionSet(absorbing2);
	result->setConstantEmissionSet(absorbing3);
	result->setConstantEmissionSet(absorbing4);

	result->addEmission(stopCodonA1,HMMEmission("A",1));
	result->setConstantEmissions(stopCodonA1,true);
	result->addEmission(stopCodonA2,HMMEmission("A",1));
	result->setConstantEmissions(stopCodonA2,true);
	result->addEmission(stopCodonG1,HMMEmission("G",1));
	result->setConstantEmissions(stopCodonG1,true);
	result->addEmission(stopCodonG2,HMMEmission("G",1));
	result->setConstantEmissions(stopCodonG2,true);

	for(int i=0; i<4;i++){
		ss1.clear();
		ss1.str(std::string());
		ss2.clear();
		ss2.str(std::string());
		ss3.clear();
		ss3.str(std::string());

		ss1 << "Exon" << endings[i] << "1";
		ss2 << "Exon" << endings[i] << "2";
		ss3 << "Exon" << endings[i] << "3";

		startNodes[i] = result->createNode(ss1.str());
		result->addEmission(startNodes[i],HMMEmission(endings[i],1));
		result->setConstantEmissions(startNodes[i],true);

		intermediateNodes[i] = result->createNode(ss2.str());
		result->addEmission(intermediateNodes[i],HMMEmission(endings[i],1));
		result->setConstantEmissions(intermediateNodes[i],true);

		endNodes[i] = result->createNode(ss3.str());
		result->addEmission(endNodes[i],HMMEmission(endings[i],1));
		result->setConstantEmissions(endNodes[i],true);
	}

	result->addTransition(absorbing1,HMMTransition(absorbing2,1));
	result->setConstantTransitions(absorbing1,true);

	for(int i=0; i<4;i++){
		result->addTransition(absorbing2,HMMTransition(startNodes[i],-1));
	}

	for(int i=0; i<4;i++){
		if(i <3){
			for(int j=0; j<4; j++){
				result->addTransition(startNodes[i],HMMTransition(intermediateNodes[j],-1));
			}
		}else{
			result->addTransition(startNodes[3],HMMTransition(intermediateNodes[1],-1));
			result->addTransition(startNodes[3],HMMTransition(intermediateNodes[3],-1));
			result->addTransition(startNodes[3],HMMTransition(stopCodonA1,-1));
			result->addTransition(startNodes[3],HMMTransition(stopCodonG1,-1));
		}
	}

	for(int i=0; i<4;i++){
		for(int j=0; j<4; j++){
			result->addTransition(intermediateNodes[i],HMMTransition(endNodes[j],-1));
		}
	}

	result->addTransition(stopCodonA1,HMMTransition(stopCodonA2,-1));
	result->addTransition(stopCodonA1,HMMTransition(stopCodonG2,-1));
	result->addTransition(stopCodonA1,HMMTransition(endNodes[1],-1));
	result->addTransition(stopCodonA1,HMMTransition(endNodes[3],-1));

	result->addTransition(stopCodonG1,HMMTransition(stopCodonA2,-1));

	for(int i=1;i<4;i++){
		result->addTransition(stopCodonG1,HMMTransition(endNodes[i],-1));
	}

	for(int i=0; i<4;i++){
		for(int j =0; j< 4;j++){
			result->addTransition(endNodes[i],HMMTransition(startNodes[j],-1));
		}

		result->addTransition(endNodes[i],HMMTransition(absorbing3,-1));
	}

	result->addTransition(absorbing3,HMMTransition(absorbing4,-1));

	result->addStartNode(absorbing1,-1);
	result->addStartNode(absorbing2,-1);
	for(int i=0; i< 4;i++){
		result->addStartNode(startNodes[i],-1);
	}

	result->addEndNode(stopCodonA2);
	result->addEndNode(stopCodonG2);
	result->addEndNode(absorbing3);
	result->addEndNode(absorbing4);

	for(int i =0; i<4;i++){
		result->addEndNode(endNodes[i]);
	}

	return result;
}

/**
 * For the explanation of the model see the paper
 * "Finding Genes in DNA with a Hidden Markov Model".
 */
boost::shared_ptr<HMM> Models::createIntronModel(){
	boost::shared_ptr<HMM> result(new HMM());
	std::string endings[] = {"A","C","G","T"};
	std::stringstream ss;

	int absorbing1 = result->createNode("IntronAbsorbing1");
	int absorbing2 = result->createNode("IntronAbsorbing2");
	int absorbing3 = result->createNode("IntronAbsorbing3");
	int absorbing4 = result->createNode("IntronAbsorbing4");

	int nodes[3][4];

	for(int i=0; i<4;i++){
		result->addEmission(absorbing1,HMMEmission(endings[i],-1));
		result->addEmission(absorbing2,HMMEmission(endings[i],-1));
		result->addEmission(absorbing3,HMMEmission(endings[i],-1));
		result->addEmission(absorbing4,HMMEmission(endings[i],-1));
	}

	result->setConstantEmissionSet(absorbing1);
	result->setConstantEmissionSet(absorbing2);
	result->setConstantEmissionSet(absorbing3);
	result->setConstantEmissionSet(absorbing4);

	for(int i=0; i< 3;i++){
		for(int j =0; j < 4;j++){
			ss.clear();
			ss.str(std::string());
			ss << "Intron" << endings[j] << i+1;
			nodes[i][j] = result->createNode(ss.str());
			result->addEmission(nodes[i][j],HMMEmission(endings[j],1));
			result->setConstantEmissions(nodes[i][j],true);
		}
	}

	result->addTransition(absorbing1,HMMTransition(absorbing2,1));
	result->setConstantTransitions(absorbing1,true);

	for(int i=0; i < 4;i++){
		result->addTransition(absorbing2,HMMTransition(nodes[0][i],-1));
	}

	for(int i =0;i<3;i++){
		for(int j=0; j<4; j++){
			for(int k=0;k<4;k++){
				result->addTransition(nodes[i][j],HMMTransition(nodes[(i+1)%3][k],-1));
			}
		}
	}

	for(int i=0;i<4;i++){
		result->addTransition(nodes[2][i],HMMTransition(absorbing3,-1));
	}

	result->addTransition(absorbing3,HMMTransition(absorbing4,-1));

	result->addStartNode(absorbing1,-1);
	result->addStartNode(absorbing2,-1);

	for(int i=0;i<4;i++){
		result->addStartNode(nodes[0][i],-1);
	}

	result->addEndNode(absorbing3);
	result->addEndNode(absorbing4);

	for(int i=0; i< 4; i++){
		result->addEndNode(nodes[2][i]);
	}

	return result;
}

/**
 * For the explanation of the model see the paper
 * "Finding Genes in DNA with a Hidden Markov Model".
 */
boost::shared_ptr<HMM> Models::createStartCodon(){
	boost::shared_ptr<HMM> result(new HMM());

	int n1 = result->createNode("StartCodon1");
	int n2 = result->createNode("StartCodon2");
	int n3 = result->createNode("StartCodon3");

	result->addTransition(n1,HMMTransition(n2,1));
	result->setConstantTransitions(n1,true);
	result->addTransition(n2,HMMTransition(n3,1));
	result->setConstantTransitions(n2,true);

	result->addEmission(n1,HMMEmission("A",1));
	result->setConstantEmissions(n1,true);
	result->addEmission(n2,HMMEmission("T",1));
	result->setConstantEmissions(n2,true);
	result->addEmission(n3,HMMEmission("G",1));
	result->setConstantEmissions(n3,true);

	result->addStartNode(n1,1);
	result->addEndNode(n3);

	return result;
}

/**
 * For the explanation of the model see the paper
 * "Finding Genes in DNA with a Hidden Markov Model".
 */
boost::shared_ptr<HMM> Models::create3SpliceSite(){
	//acceptor, 15 stages
	boost::shared_ptr<HMM> result(new HMM());
	std::stringstream ss;
	std::string emissions[] = {"A","C","G","T"};

	int stages[15];

	for(int i=0; i< 15;i++){
		ss.clear();
		ss.str(std::string());
		ss<< "Acceptor" << i+1;
		stages[i] = result->createNode(ss.str());

		for(int j =0; j< 4; j++){
			result->addEmission(stages[i],HMMEmission(emissions[j],-1));
		}

		result->setConstantEmissionSet(stages[i]);
	}

	for(int i=0;i<14;i++){
		result->addTransition(stages[i],HMMTransition(stages[i+1],1));
		result->setConstantTransitions(stages[i],true);
	}

	result->addStartNode(stages[0],1);
	result->addEndNode(stages[14]);

	return result;
}

/**
 * For the explanation of the model see the paper
 * "Finding Genes in DNA with a Hidden Markov Model".
 */
boost::shared_ptr<HMM> Models::create5PolyASite(){
	boost::shared_ptr<HMM> result(new HMM());
	std::stringstream ss;
	std::string emissions[] = {"A","A","A","T","A","A"};
	int stages[6];

	for(int i =0; i< 6; i++){
		ss.clear();
		ss.str(std::string());
		ss << "PolyASite" << i+1;
		stages[i] = result->createNode(ss.str());
		result->addEmission(stages[i],HMMEmission(emissions[i],1));
		result->setConstantEmissions(stages[i],true);
	}

	for(int i=0; i<5; i++){
		result->addTransition(stages[i],HMMTransition(stages[i+1],1));
		result->setConstantTransitions(stages[i],true);
	}

	result->setConstantTransitions(stages[5],true);

	result->addStartNode(stages[0],1);
	result->addEndNode(stages[5]);

	return result;
}

/**
 * For the explanation of the model see the paper
 * "Finding Genes in DNA with a Hidden Markov Model".
 */
boost::shared_ptr<HMM> Models::create5SpliceSite(){
	//donor, 9 stages
	boost::shared_ptr<HMM> result(new HMM());
	int stages[9];
	std::stringstream ss;
	std::string emissions[] ={"A","C","G","T"};

	for(int i=0; i< 9; i++){
		ss.clear();
		ss.str(std::string());
		ss << "Donor" << i+1;
		stages[i] = result->createNode(ss.str());

		for(int j=0;j<4;j++){
			result->addEmission(stages[i],HMMEmission(emissions[j],-1));
		}

		result->setConstantEmissionSet(stages[i]);
	}

	for(int i=0;i<8;i++){
		result->addTransition(stages[i],HMMTransition(stages[i+1],1));
		result->setConstantTransitions(stages[i],true);
	}

	result->addStartNode(stages[0],1);
	result->addEndNode(stages[8]);

	return result;
}

/**
 * For the explanation of the model see the paper
 * "Finding Genes in DNA with a Hidden Markov Model".
 */
boost::shared_ptr<HMM> Models::createDownstreamModel(){
	//10 stages long
	boost::shared_ptr<HMM> result(new HMM());
	std::stringstream ss;
	int stages[10];
	std::string emissions[] ={"A","C","G","T"};

	for(int i=0; i< 10; i++){
		ss.clear();
		ss.str(std::string());
		ss << "Downstream" << i+1;
		stages[i] = result->createNode(ss.str());

		for(int j=0; j< 4; j++){
			result->addEmission(stages[i],HMMEmission(emissions[j],-1));
		}

		result->setConstantEmissionSet(stages[i]);
	}

	for(int i =0; i<9;i++){
		result->addTransition(stages[i],HMMTransition(stages[i+1],1));
		result->setConstantTransitions(stages[i],true);
	}

	result->addTransition(stages[9],HMMTransition(stages[9],1));

	result->addStartNode(stages[0],1);

	result->addEndNode(stages[9]);

	return result;
}

/**
 * For the explanation of the model see the paper
 * "Finding Genes in DNA with a Hidden Markov Model".
 */
boost::shared_ptr<HMM> Models::createUpstreamModel(){
	//15 stages long
	boost::shared_ptr<HMM> result(new HMM());
	std::stringstream ss;
	int stages[15];
	std::string emissions[] = {"A","C","G","T"};

	for(int i=0; i< 15;i++){
		ss.clear();
		ss.str(std::string());
		ss << "Upstream" << i+1;
		stages[i] = result->createNode(ss.str());

		for(int j =0; j<4;j++){
			result->addEmission(stages[i],HMMEmission(emissions[j],-1));
		}

		result->setConstantEmissionSet(stages[i]);
	}

	for(int i=0; i<14;i++){
		result->addTransition(stages[i],HMMTransition(stages[i+1],1));
		result->setConstantTransitions(stages[i],true);
	}

	result->addTransition(stages[14],HMMTransition(stages[14],-1));

	result->addStartNode(stages[0],1);
	result->addEndNode(stages[14]);

	return result;
}

/**
 * Construction of the complete VEIL model from the individual parts.
 */
boost::shared_ptr<HMM> Models::buildGeneModel(boost::shared_ptr<HMM> upstreamModel, boost::shared_ptr<HMM> startCodon,
			boost::shared_ptr<HMM> exonModel, boost::shared_ptr<HMM> donor, boost::shared_ptr<HMM> intronModel,
			boost::shared_ptr<HMM> acceptor, boost::shared_ptr<HMM> downstreamModel, boost::shared_ptr<HMM> polyASite){
	boost::shared_ptr<HMM> result(new HMM());
	boost::unordered_map<std::string,std::vector<HMMConnection> > upstream2startcodon;
	boost::unordered_map<std::string,std::vector<HMMConnection> > exonIn;
	boost::unordered_map<std::string,std::vector<HMMConnection> > exonOut;
	boost::unordered_map<std::string,std::vector<HMMConnection> > exon2downstream;
	boost::unordered_map<std::string,std::vector<HMMConnection> > donor2intron;
	boost::unordered_map<std::string,std::vector<HMMConnection> > intron2acceptor;
	boost::unordered_map<std::string,std::vector<HMMConnection> > downstream2polyAsite;
	std::vector<HMMConnection> upstream15;
	std::vector<HMMConnection> donor9;
	std::vector<HMMConnection> intronAbsorbing3;
	std::vector<HMMConnection> intronAbsorbing4;
	std::vector<HMMConnection> intronA3;
	std::vector<HMMConnection> intronC3;
	std::vector<HMMConnection> intronG3;
	std::vector<HMMConnection> intronT3;
	std::vector<HMMConnection> downstream10;
	std::vector<HMMConnection> polyASite6;
	std::vector<HMMConnection> acceptor15;
	std::vector<HMMConnection> startCodon3;
	std::vector<HMMConnection> exonAbsorbing3;
	std::vector<HMMConnection> exonAbsorbing4;
	std::vector<HMMConnection> exonA3;
	std::vector<HMMConnection> exonC3;
	std::vector<HMMConnection> exonG3;
	std::vector<HMMConnection> exonT3;
	std::vector<HMMConnection> exonStopCodonA2;
	std::vector<HMMConnection> exonStopCodonG2;

	result->integrateHMM(upstreamModel,boost::unordered_map<std::string,std::vector<HMMConnection> >());

	upstream15.push_back(HMMConnection("StartCodon1"));
	upstream2startcodon.emplace("Upstream15",upstream15);
	result->integrateHMM(startCodon,upstream2startcodon);

	result->integrateHMM(exonModel,boost::unordered_map<std::string,std::vector<HMMConnection> >());
	result->integrateHMM(donor,boost::unordered_map<std::string,std::vector<HMMConnection> >());

	donor9.push_back(HMMConnection("IntronAbsorbing1"));
	donor9.push_back(HMMConnection("IntronAbsorbing2"));
	donor9.push_back(HMMConnection("IntronA1"));
	donor9.push_back(HMMConnection("IntronC1"));
	donor9.push_back(HMMConnection("IntronG1"));
	donor9.push_back(HMMConnection("IntronT1"));
	donor2intron.emplace("Donor9",donor9);
	result->integrateHMM(intronModel,donor2intron);

	intronAbsorbing3.push_back(HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronAbsorbing3",intronAbsorbing3);

	intronAbsorbing4.push_back(HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronAbsorbing4",intronAbsorbing4);

	intronA3.push_back(HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronA3",intronA3);

	intronC3.push_back(HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronC3",intronC3);

	intronG3.push_back(HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronG3",intronG3);

	intronT3.push_back(HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronT3",intronT3);
	result->integrateHMM(acceptor,intron2acceptor);

	result->integrateHMM(downstreamModel,boost::unordered_map<std::string,std::vector<HMMConnection> >());

	downstream10.push_back(HMMConnection("PolyASite1"));
	downstream2polyAsite.emplace("Downstream10",downstream10);

	polyASite6.push_back(HMMConnection("Downstream10",1));
	downstream2polyAsite.emplace("PolyASite6",polyASite6);
	result->integrateHMM(polyASite,downstream2polyAsite);

	acceptor15.push_back(HMMConnection("ExonAbsorbing1"));
	acceptor15.push_back(HMMConnection("ExonAbsorbing2"));
	acceptor15.push_back(HMMConnection("ExonA1"));
	acceptor15.push_back(HMMConnection("ExonC1"));
	acceptor15.push_back(HMMConnection("ExonG1"));
	acceptor15.push_back(HMMConnection("ExonT1"));
	exonIn.emplace("Acceptor15",acceptor15);

	startCodon3.push_back(HMMConnection("ExonA1"));
	startCodon3.push_back(HMMConnection("ExonC1"));
	startCodon3.push_back(HMMConnection("ExonG1"));
	startCodon3.push_back(HMMConnection("ExonT1"));

	startCodon3.push_back(HMMConnection("IntronA1"));
	startCodon3.push_back(HMMConnection("IntronC1"));
	startCodon3.push_back(HMMConnection("IntronG1"));
	startCodon3.push_back(HMMConnection("IntronT1"));
	startCodon3.push_back(HMMConnection("Donor1"));
	startCodon3.push_back(HMMConnection("ExonAbsorbing3"));
	startCodon3.push_back(HMMConnection("ExonAbsorbing4"));
	exonIn.emplace("StartCodon3",startCodon3);
	result->integrateHMM(boost::shared_ptr<HMM>(new HMM()),exonIn);

	exonAbsorbing3.push_back(HMMConnection("Donor1"));
	exonOut.emplace("ExonAbsorbing3",exonAbsorbing3);

	exonAbsorbing4.push_back(HMMConnection("Donor1"));
	exonOut.emplace("ExonAbsorbing4",exonAbsorbing4);

	exonA3.push_back(HMMConnection("Donor1"));
	exonOut.emplace("ExonA3",exonA3);

	exonC3.push_back(HMMConnection("Donor1"));
	exonOut.emplace("ExonC3",exonC3);

	exonG3.push_back(HMMConnection("Donor1"));
	exonOut.emplace("ExonG3",exonG3);

	exonT3.push_back(HMMConnection("Donor1"));
	exonOut.emplace("ExonT3",exonT3);

	exonStopCodonA2.push_back(HMMConnection("Downstream1"));
	exonOut.emplace("ExonStopCodonA2",exonStopCodonA2);

	exonStopCodonG2.push_back(HMMConnection("Downstream1"));
	exonOut.emplace("ExonStopCodonG2",exonStopCodonG2);
	result->integrateHMM(boost::shared_ptr<HMM>(new HMM()),exonOut);

	for(int i=0; i< 15;i++){
		std::stringstream ss;

		ss << "Upstream" << (i+1);
		result->addStartNode(ss.str(),-1);
	}

	result->addStartNode("StartCodon1",-1.0);
	result->addEndNode("Downstream10");
	result->addEndNode("PolyASite6");

	return result;
}

void Models::VeilAnnotation(const std::vector<std::string>& states, std::vector<std::string>& output){

	for(std::vector<std::string>::const_iterator it = states.begin(); it != states.end(); ++it){
		// veilMapping contains the mapping between state names and the DNA structure
		// (exons, introns, upstream model, downstream model)
		for(boost::unordered_map<std::string,std::string>::const_iterator jt = veilMapping.begin(); jt != veilMapping.end(); ++jt){
			if(boost::regex_match(*it,boost::regex(jt->first))){
				output.push_back(jt->second);
				break;
			}
		}
	}
}

/**
 * The result is a map from an regex to a string denoting the respective
 * DNA structure element.
 */
boost::unordered_map<std::string,std::string> Models::createVeilMapping(){
	boost::unordered_map<std::string, std::string> result;
	std::stringstream ss;

	result.emplace("Exon.*","E");
	result.emplace("Intron.*","I");
	result.emplace("Upstream.*","U");
	result.emplace("Downstream.*","D");
	result.emplace("StartCodon.*","E");
	result.emplace("PolyASite.*","D");

	for(int i=0; i<3;i++){
		ss.str(std::string());
		ss.clear();
		ss << "Donor" << (i+1);

		result.emplace(ss.str(),"E");
	}

	for(int i=3; i< 9;i++){
		ss.str(std::string());
		ss.clear();
		ss << "Donor" << (i+1);

		result.emplace(ss.str(),"I");
	}

	for(int i=0;i < 14;i++){
		ss.str(std::string());
		ss.clear();
		ss<<"Acceptor" << (i+1);
		result.emplace(ss.str(),"I");

	}

	result.emplace("Acceptor15","E");

	return result;
}

boost::shared_ptr<HMM> Models::createVeilModel(){
	boost::shared_ptr<HMM> exonModel = Models::createExonModel();
	boost::shared_ptr<HMM> intronModel = Models::createIntronModel();
	boost::shared_ptr<HMM> upstreamModel = Models::createUpstreamModel();
	boost::shared_ptr<HMM> downstreamModel = Models::createDownstreamModel();
	boost::shared_ptr<HMM> acceptorModel = Models::create3SpliceSite();
	boost::shared_ptr<HMM> donorModel = Models::create5SpliceSite();
	boost::shared_ptr<HMM> polyASite = Models::create5PolyASite();
	boost::shared_ptr<HMM> startCodon = Models::createStartCodon();

	return buildGeneModel(upstreamModel,startCodon,exonModel,donorModel,intronModel,acceptorModel,downstreamModel,polyASite);
}

/**
 * The result is stored in the argument substitution. Substitution is a map
 * containing as key an regex specifying the nodes on which the substitution
 * is performed. The substition is the respective value element associated to
 * the regex. The value itself is a map containing as keys the emission which
 * shall be substituted by the value element.
 */
void Models::createAnnotatedSubstitution(boost::unordered_map<std::string, boost::unordered_map<std::string, std::string> >& substitution){
	boost::unordered_map<std::string, std::string> substitutionExon;
	boost::unordered_map<std::string, std::string> substitutionIntron;
	boost::unordered_map<std::string, std::string> substitutionDownstream;
	boost::unordered_map<std::string, std::string> substitutionUpstream;
	std::string bases[] = { "A", "C", "G", "T" };
	std::string prefixes[] = { "D", "U", "E", "I" };
	std::stringstream ss;

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
}

void Models::createInverseAnnotatedSubstitution(boost::unordered_map<std::string, boost::unordered_map<std::string,std::string> >& inverseSubstitution){
	boost::unordered_map<std::string, std::string> iSubstitution;
	std::string bases[] = { "A", "C", "G", "T" };
	std::string prefixes[] = { "D", "U", "E", "I" };
	std::stringstream ss;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			ss.str("");
			ss.clear();
			ss << prefixes[i] << bases[j];

			iSubstitution.emplace(ss.str(), bases[j]);
		}
	}

	inverseSubstitution.emplace(".*",iSubstitution);
}


