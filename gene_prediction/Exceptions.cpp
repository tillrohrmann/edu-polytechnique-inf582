/*
 * Exceptions.cpp
 *
 *  Created on: Feb 13, 2013
 *      Author: Till Rohrmann
 */

#include "Exceptions.hpp"

OperationNotSupportedException::OperationNotSupportedException(std::string message) noexcept
: exception(), _message(message){
}

OperationNotSupportedException::OperationNotSupportedException(const OperationNotSupportedException & ex) noexcept
: exception(ex), _message(ex._message){
}

OperationNotSupportedException& OperationNotSupportedException::operator =(const OperationNotSupportedException& ex) noexcept {
	exception::operator=(ex);
	_message = ex._message;

	return (*this);
}

const char* OperationNotSupportedException::what() const noexcept{
	return _message.c_str();
}


InvalidSerializationException::InvalidSerializationException(std::string message) noexcept
: exception(), _message(message){
}

InvalidSerializationException::InvalidSerializationException(const InvalidSerializationException & ex) noexcept
: exception(ex), _message(ex._message){
}

InvalidSerializationException& InvalidSerializationException::operator =(const InvalidSerializationException& ex) noexcept {
	exception::operator=(ex);
	_message = ex._message;

	return (*this);
}

const char* InvalidSerializationException::what() const noexcept{
	return _message.c_str();
}


