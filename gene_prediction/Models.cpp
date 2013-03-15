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
	}

	for(int i=0;i<14;i++){
		result->addTransition(stages[i],HMMTransition(stages[i+1],1));
		result->setConstantTransitions(stages[i],true);
	}

	result->addStartNode(stages[0],1);
	result->addEndNode(stages[14]);

	return result;
}

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

	result->addStartNode(stages[0],1);
	result->addEndNode(stages[5]);

	return result;
}

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
	}

	for(int i=0;i<8;i++){
		result->addTransition(stages[i],HMMTransition(stages[i+1],1));
		result->setConstantTransitions(stages[i],true);
	}

	result->addStartNode(stages[0],1);
	result->addEndNode(stages[8]);

	return result;
}

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
	}

	for(int i =0; i<9;i++){
		result->addTransition(stages[i],HMMTransition(stages[i+1],1));
		result->setConstantTransitions(stages[i],true);
	}

	result->addTransition(stages[9],HMMTransition(1,stages[9]));

	result->addStartNode(stages[0],1);

	result->addEndNode(stages[9]);

	return result;
}


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

boost::shared_ptr<HMM> Models::buildGeneModel(boost::shared_ptr<HMM> upstreamModel, boost::shared_ptr<HMM> startCodon,
			boost::shared_ptr<HMM> exonModel, boost::shared_ptr<HMM> donor, boost::shared_ptr<HMM> intronModel,
			boost::shared_ptr<HMM> acceptor, boost::shared_ptr<HMM> downstreamModel, boost::shared_ptr<HMM> polyASite){
	boost::shared_ptr<HMM> result(new HMM());
	boost::unordered_map<std::string,HMMConnection> upstream2startcodon;
	boost::unordered_map<std::string,HMMConnection> exonIn;
	boost::unordered_map<std::string,HMMConnection> exonOut;
	boost::unordered_map<std::string,HMMConnection> exon2downstream;
	boost::unordered_map<std::string,HMMConnection> donor2intron;
	boost::unordered_map<std::string,HMMConnection> intron2acceptor;
	boost::unordered_map<std::string,HMMConnection> downstream2polyAsite;

	result->integrateHMM(upstreamModel,boost::unordered_map<std::string,HMMConnection>());

	upstream2startcodon.emplace("Upstream15",HMMConnection("StartCodon1"));
	result->integrateHMM(startCodon,upstream2startcodon);

	result->integrateHMM(exonModel,boost::unordered_map<std::string,HMMConnection>());
	result->integrateHMM(donor,boost::unordered_map<std::string,HMMConnection>());

	donor2intron.emplace("Donor9",HMMConnection("IntronAbsorbing1"));
	donor2intron.emplace("Donor9",HMMConnection("IntronAbsorbing2"));
	donor2intron.emplace("Donor9",HMMConnection("IntronA1"));
	donor2intron.emplace("Donor9",HMMConnection("IntronC1"));
	donor2intron.emplace("Donor9",HMMConnection("IntronG1"));
	donor2intron.emplace("Donor9",HMMConnection("IntronT1"));
	result->integrateHMM(intronModel,donor2intron);

	intron2acceptor.emplace("IntronAbsorbing3",HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronAbsorbing4",HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronA3",HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronC3",HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronG3",HMMConnection("Acceptor1"));
	intron2acceptor.emplace("IntronT3",HMMConnection("Acceptor1"));
	result->integrateHMM(acceptor,intron2acceptor);

	result->integrateHMM(downstreamModel,boost::unordered_map<std::string,HMMConnection>());

	downstream2polyAsite.emplace("Downstream10",HMMConnection("PolyASite1"));
	downstream2polyAsite.emplace("PolyASite6",HMMConnection("Downstream10"));
	result->integrateHMM(polyASite,downstream2polyAsite);

	exonIn.emplace("Acceptor15",HMMConnection("ExonAbsorbing1"));
	exonIn.emplace("Acceptor15",HMMConnection("ExonAbsorbing2"));
	exonIn.emplace("Acceptor15",HMMConnection("ExonA1"));
	exonIn.emplace("Acceptor15",HMMConnection("ExonC1"));
	exonIn.emplace("Acceptor15",HMMConnection("ExonG1"));
	exonIn.emplace("Acceptor15",HMMConnection("ExonT1"));
	exonIn.emplace("StartCodon3",HMMConnection("ExonA1"));
	exonIn.emplace("StartCodon3",HMMConnection("ExonC1"));
	exonIn.emplace("StartCodon3",HMMConnection("ExonG1"));
	exonIn.emplace("StartCodon3",HMMConnection("ExonT1"));
	result->integrateHMM(boost::shared_ptr<HMM>(new HMM()),exonIn);

	exonOut.emplace("ExonAbsorbing3",HMMConnection("Donor1"));
	exonOut.emplace("ExonAbsorbing4",HMMConnection("Donor1"));
	exonOut.emplace("ExonA3",HMMConnection("Donor1"));
	exonOut.emplace("ExonC3",HMMConnection("Donor1"));
	exonOut.emplace("ExonG3",HMMConnection("Donor1"));
	exonOut.emplace("ExonT3",HMMConnection("Donor1"));

	exonOut.emplace("ExonStopCodonA2",HMMConnection("Downstream1"));
	exonOut.emplace("ExonStopCodonG2",HMMConnection("Downstream1"));
	result->integrateHMM(boost::shared_ptr<HMM>(new HMM()),exonOut);

	result->addStartNode("Upstream1",1.0);
	result->addEndNode("Downstream10");
	result->addEndNode("PolyASite6");

	return result;
}




