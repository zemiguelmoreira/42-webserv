/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdias-mo <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/18 17:12:42 by nmoreira          #+#    #+#             */
/*   Updated: 2024/02/05 19:30:33 by jdias-mo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Location.hpp"

Location::Location() : redirectionCode(0), autoindex(false) {
	// std::cout << "teste" << std::endl;
	return;
}

Location::~Location() {
	return ;
}

Location::Location(const Location &src) {
	*this = src;
	return;
}

Location &Location::operator=(const Location &opRight) {
	if (this != &opRight)
	{
		this->path = opRight.path;
		this->allow = opRight.allow;
		this->redirectionCode = opRight.redirectionCode;
		this->redirectionPath = opRight.redirectionPath;
		this->rootPath = opRight.rootPath;
		this->index = opRight.index;
		this->autoindex = opRight.autoindex;
	}
	return *this;
}

bool Location::checkLocation() const {
	if (this->get_path() == "")
	{
		std::cerr << "Error: location without a path!" << std::endl;
		return false;
	}
	// std::cout << this->get_allow().size() << std::endl;
	// if (this->get_rootPath() != "" && !this->get_allow().size())
	// {
	// 	std::cout << "Error: location without allows!" << std::endl;
	// 	return false;
	// }
	// if (this->get_rootPath() == "" && this->get_redirectionPath() == "")
	// {
	// 	std::cout << "Error: location without root or return!" << std::endl;
	// 	return false;
	// }

	return true;
}

std::string Location::get_path() const {
	return this->path;
}

std::vector<std::string> Location::get_allow() const {
	return this->allow;
}

unsigned int Location::get_redirectionCode() const {
	return this->redirectionCode;
}

std::string Location::get_redirectionPath() const {
	return this->redirectionPath;
}

std::string Location::get_rootPath() const {
	return this->rootPath;
}

std::string Location::get_index() const {
	return this->index;
}

bool Location::get_autoindex(void) const {
	return this->autoindex;
}

void Location::setPath(const std::string &location) {
	this->path = location;
	return ;
}

void Location::setAllow(const std::string &allowStr) {
	this->allow.push_back(allowStr);
	return ;
}

void Location::setRedirectionCode(const unsigned int code) {
	this->redirectionCode = code;
	return;
}

void Location::setRedirectionPath(const std::string rePath) {
	this->redirectionPath = rePath;
	return;
}

void Location::setRootPath(const std::string root) {
	this->rootPath = root;
	return;
}

void Location::setIndex(const std::string ind) {
	this->index = ind;
	return;
}

void Location::setAutoindex(const bool aindex) {
	this->autoindex = aindex;
	return ;
}
