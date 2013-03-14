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
		result->addEmission(absorbing1,HMMEmission(-1,endings[i]));
		result->addEmission(absorbing2,HMMEmission(-1,endings[i]));
		result->addEmission(absorbing3,HMMEmission(-1,endings[i]));
		result->addEmission(absorbing4,HMMEmission(-1,endings[i]));
	}

	result->addEmission(stopCodonA1,HMMEmission(1,"A",true));
	result->addEmission(stopCodonA2,HMMEmission(1,"A",true));
	result->addEmission(stopCodonG1,HMMEmission(1,"G",true));
	result->addEmission(stopCodonG2,HMMEmission(1,"G",true));

	for(int i=0; i<4;i++){
		ss1.clear();
		ss2.clear();
		ss3.clear();
		ss1 << "Exon" << endings[i] << "1";
		ss2 << "Exon" << endings[i] << "2";
		ss3 << "Exon" << endings[i] << "3";

		startNodes[i] = result->createNode(ss1.str());
		result->addEmission(startNodes[i],HMMEmission(1,endings[i],true));

		intermediateNodes[i] = result->createNode(ss2.str());
		result->addEmission(intermediateNodes[i],HMMEmission(1,endings[i],true));

		endNodes[i] = result->createNode(ss3.str());
		result->addEmission(endNodes[i],HMMEmission(1,endings[i],true));
	}

	result->addTransition(absorbing1,HMMTransition(1,absorbing2,true));

	for(int i=0; i<4;i++){
		result->addTransition(absorbing2,HMMTransition(-1,startNodes[i]));
	}

	for(int i=0; i<4;i++){
		if(i <3){
			for(int j=0; j<4; j++){
				result->addTransition(startNodes[i],HMMTransition(-1,intermediateNodes[j]));
			}
		}else{
			result->addTransition(startNodes[3],HMMTransition(-1,intermediateNodes[1]));
			result->addTransition(startNodes[3],HMMTransition(-1,intermediateNodes[3]));
			result->addTransition(startNodes[3],HMMTransition(-1,stopCodonA1));
			result->addTransition(startNodes[3],HMMTransition(-1,stopCodonG1));
		}
	}

	for(int i=0; i<4;i++){
		for(int j=0; j<4; j++){
			result->addTransition(intermediateNodes[i],HMMTransition(-1,endNodes[j]));
		}
	}

	result->addTransition(stopCodonA1,HMMTransition(-1,stopCodonA2));
	result->addTransition(stopCodonA1,HMMTransition(-1,stopCodonG2));
	result->addTransition(stopCodonA1,HMMTransition(-1,endNodes[1]));
	result->addTransition(stopCodonA1,HMMTransition(-1,endNodes[3]));

	result->addTransition(stopCodonG1,HMMTransition(-1,stopCodonA2));

	for(int i=1;i<4;i++){
		result->addTransition(stopCodonG1,HMMTransition(-1,endNodes[i]));
	}

	for(int i=0; i<4;i++){
		for(int j =0; j< 4;j++){
			result->addTransition(endNodes[i],HMMTransition(-1,startNodes[j]));
		}

		result->addTransition(endNodes[i],HMMTransition(-1,absorbing3));
	}

	result->addTransition(absorbing3,HMMTransition(-1,absorbing4));

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
		result->addEmission(absorbing1,HMMEmission(-1,endings[i]));
		result->addEmission(absorbing2,HMMEmission(-1,endings[i]));
		result->addEmission(absorbing3,HMMEmission(-1,endings[i]));
		result->addEmission(absorbing4,HMMEmission(-1,endings[i]));
	}

	for(int i=0; i< 3;i++){
		for(int j =0; j < 4;j++){
			ss.clear();
			ss.str(std::string());
			ss << "Intron" << endings[j] << i+1;
			nodes[i][j] = result->createNode(ss.str());
			result->addEmission(nodes[i][j],HMMEmission(1,endings[j],true));
		}
	}

	result->addTransition(absorbing1,HMMTransition(1,absorbing2,true));

	for(int i=0; i < 4;i++){
		result->addTransition(absorbing2,HMMTransition(-1,nodes[0][i]));
	}

	for(int i =0;i<3;i++){
		for(int j=0; j<4; j++){
			for(int k=0;k<4;k++){
				result->addTransition(nodes[i][j],HMMTransition(-1,nodes[(i+1)%3][k]));
			}
		}
	}

	for(int i=0;i<4;i++){
		result->addTransition(nodes[2][i],HMMTransition(-1,absorbing3));
	}

	result->addTransition(absorbing3,HMMTransition(-1,absorbing4));

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

	result->addTransition(n1,HMMTransition(1,n2,true));
	result->addTransition(n2,HMMTransition(1,n3,true));

	result->addEmission(n1,HMMEmission(1,"A",true));
	result->addEmission(n2,HMMEmission(1,"T",true));
	result->addEmission(n3,HMMEmission(1,"G",true));

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
			result->addEmission(stages[i],HMMEmission(-1,emissions[j]));
		}
	}

	for(int i=0;i<14;i++){
		result->addTransition(stages[i],HMMTransition(1,stages[i+1],true));
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
		result->addEmission(stages[i],HMMEmission(1,emissions[i],true));
	}

	for(int i=0; i<5; i++){
		result->addTransition(stages[i],HMMTransition(1,stages[i+1],true));
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
			result->addEmission(stages[i],HMMEmission(-1,emissions[j]));
		}
	}

	for(int i=0;i<8;i++){
		result->addTransition(stages[i],HMMTransition(1,stages[i+1],true));
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
			result->addEmission(stages[i],HMMEmission(-1,emissions[j]));
		}
	}

	for(int i =0; i<9;i++){
		result->addTransition(stages[i],HMMTransition(1,stages[i+1],true));
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
			result->addEmission(stages[i],HMMEmission(-1,emissions[j]));
		}
	}

	for(int i=0; i<14;i++){
		result->addTransition(stages[i],HMMTransition(1,stages[i+1],true));
	}

	result->addTransition(stages[14],HMMTransition(-1,stages[14]));

	result->addStartNode(stages[0],1);
	result->addEndNode(stages[14]);

	return result;
}

boost::shared_ptr<HMM> buildGeneModel(boost::shared_ptr<HMM> upstreamModel, boost::shared_ptr<HMM> startCodon,
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

	upstream2startcodon.emplace("Upstream15",HMMConnection(-1,"StartCodon1"));
	result->integrateHMM(startCodon,upstream2startcodon);

	result->integrateHMM(exonModel,boost::unordered_map<std::string,HMMConnection>());
	result->integrateHMM(donor,boost::unordered_map<std::string,HMMConnection>());

	donor2intron.emplace("Donor9",HMMConnection(-1,"IntronAbsorbing1"));
	donor2intron.emplace("Donor9",HMMConnection(-1,"IntronAbsorbing2"));
	donor2intron.emplace("Donor9",HMMConnection(-1,"IntronA1"));
	donor2intron.emplace("Donor9",HMMConnection(-1,"IntronC1"));
	donor2intron.emplace("Donor9",HMMConnection(-1,"IntronG1"));
	donor2intron.emplace("Donor9",HMMConnection(-1,"IntronT1"));
	result->integrateHMM(intronModel,donor2intron);

	intron2acceptor.emplace("IntronAbsorbing3",HMMConnection(-1,"Acceptor1"));
	intron2acceptor.emplace("IntronAbsorbing4",HMMConnection(-1,"Acceptor1"));
	intron2acceptor.emplace("IntronA3",HMMConnection(-1,"Acceptor1"));
	intron2acceptor.emplace("IntronC3",HMMConnection(-1,"Acceptor1"));
	intron2acceptor.emplace("IntronG3",HMMConnection(-1,"Acceptor1"));
	intron2acceptor.emplace("IntronT3",HMMConnection(-1,"Acceptor1"));
	result->integrateHMM(acceptor,intron2acceptor);

	result->integrateHMM(downstreamModel,boost::unordered_map<std::string,HMMConnection>());

	downstream2polyAsite.emplace("Downstream10",HMMConnection(-1,"PolyASite1"));
	downstream2polyAsite.emplace("PolyASite6",HMMConnection(-1,"Downstream10"));
	result->integrateHMM(polyASite,downstream2polyAsite);

	exonIn.emplace("Acceptor15",HMMConnection(-1,"ExonAbsorbing1"));
	exonIn.emplace("Acceptor15",HMMConnection(-1,"ExonAbsorbing2"));
	exonIn.emplace("Acceptor15",HMMConnection(-1,"ExonA1"));
	exonIn.emplace("Acceptor15",HMMConnection(-1,"ExonC1"));
	exonIn.emplace("Acceptor15",HMMConnection(-1,"ExonG1"));
	exonIn.emplace("Acceptor15",HMMConnection(-1,"ExonT1"));
	exonIn.emplace("StartCodon3",HMMConnection(-1,"ExonA1"));
	exonIn.emplace("StartCodon3",HMMConnection(-1,"ExonC1"));
	exonIn.emplace("StartCodon3",HMMConnection(-1,"ExonG1"));
	exonIn.emplace("StartCodon3",HMMConnection(-1,"ExonT1"));
	result->integrateHMM(boost::shared_ptr<HMM>(new HMM()),exonIn);

	exonOut.emplace("ExonAbsorbing3",HMMConnection(-1,"Donor1"));
	exonOut.emplace("ExonAbsorbing4",HMMConnection(-1,"Donor1"));
	exonOut.emplace("ExonA3",HMMConnection(-1,"Donor1"));
	exonOut.emplace("ExonC3",HMMConnection(-1,"Donor1"));
	exonOut.emplace("ExonG3",HMMConnection(-1,"Donor1"));
	exonOut.emplace("ExonT3",HMMConnection(-1,"Donor1"));

	exonOut.emplace("ExonStopCodonA2",HMMConnection(-1,"Downstream1"));
	exonOut.emplace("ExonStopCodonG2",HMMConnection(-1,"Downstream1"));
	result->integrateHMM(boost::shared_ptr<HMM>(new HMM()),exonOut);

	return result;
}




