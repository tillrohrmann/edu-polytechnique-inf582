/*
 * Exceptions.hpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#ifndef EXCEPTIONS_HPP_
#define EXCEPTIONS_HPP_

#include <exception>
#include <string>

class OperationNotSupportedException : public std::exception{
private:
	std::string _message;
public:
	OperationNotSupportedException(std::string message) noexcept;
	OperationNotSupportedException(const OperationNotSupportedException& ex) noexcept;
	OperationNotSupportedException& operator=(const OperationNotSupportedException& ex) noexcept;
	const char* what() const noexcept;
};

class InvalidSerializationException : public std::exception{
private:
	std::string _message;
public:
	InvalidSerializationException(std::string message) noexcept;
	InvalidSerializationException(const InvalidSerializationException& ex) noexcept;
	InvalidSerializationException& operator=(const InvalidSerializationException& ex) noexcept;
	const char* what() const noexcept;
};

class HMMException : public std::exception{
private:
	std::string _message;
public:
	HMMException(std::string message) noexcept;
	HMMException(const HMMException& ex) noexcept;
	HMMException& operator=(const HMMException& ex) noexcept;
	const char* what() const noexcept;
};




#endif /* EXCEPTIONS_HPP_ */
