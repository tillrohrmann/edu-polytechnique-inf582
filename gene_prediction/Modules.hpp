/*
 * Modules.hpp
 *
 *  Created on: Mar 15, 2013
 *      Author: Till Rohrmann
 */

#ifndef MODULES_HPP_
#define MODULES_HPP_

#include <string>

/**
 * This class encapsulates different tasks which the program supports
 */
namespace Modules {
/**
 * This function generates the complete VEIL model and uses the
 * annotated DNA sequences found in the file "DNASequences.fasta"
 * to learn the HMM by cross validation. The learned model is then
 * written to "completeModel.hmm"
 */
void learnCompleteModel();

/**
 * This function learns only the Exon model of VEIL. For that purpose
 * it extracts all the Exons found in DNASequences.fasta and uses cross
 * validation to learn the HMM. The result is then printed to stdout.
 */
void learnExonModel();

/**
 * This function takes as argument a filename which contains a HMM. The HMM is
 * then used to predict the DNA structure of the DNA sequences found in
 * "DNASequences.fasta". The results are then printed to stdout.
 *
 * @argument hmmFilename string to file containing the HMM
 */
void evaluateModel(const std::string & hmmFilename);

/**
 * This function learns the complete VEIL model from the set of DNA sequences
 * found in "DNASequences.fasta". For that purpose it splits the set of
 * sequences up into a learning and a testing set. The testing set is used after
 * the learning to evaluate the performance of the learned model.
 *
 * @argument prefix string which is appended to the hmm result files
 */
void learnAndEvaluateModel(const std::string& prefix = "");

/**
 * This function runs a 2 state toy example to see whether the learning
 * algorithm can correctly learn the HMM. For that purpose, a HMM is defined
 * and used to generate sequences for the training set. Then a random 2 state
 * model is generated and the produced sequences are used to train it. The
 * result is then printed to stdout.
 */
void test2StateToyExample();

/**
 * This function is used to see whether the learning algorithm can learn
 * a 3 state toy example. For this purpose it defines a 3 state HMM which
 * is then used to generate training sequences. These training sequences
 * are the input for the learning algorithm of a random 3 state HMM. The
 * final result is stored in "test.hmm" and printed to stdout.
 */
void test3StateToyExample();
}

#endif /* MODULES_HPP_ */
