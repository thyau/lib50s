#ifndef __ERROR_HPP__
#define __ERROR_HPP__

/*
 * Copyright (c) [ORGNAME] 2013
 *
 * Author: Jon Schiavo
 * ALL RIGHTS RESERVED
 *
 * Purpose:
 * Standardized error throwing class and macros
 */

#include <windows.h>
#include <string>
#include <sstream>

/*
 * STRUCTURE: ERROR_DESC
 * A struct to be thrown in the event of an error
 */
typedef struct ERROR_DESC
{
	std::string		file;
	std::string		error;
	long			line;
	HRESULT			hr;

	ERROR_DESC(HRESULT hres, std::string errorText, std::string filename, long lineNum)
		: file(filename), error(errorText), line(lineNum), hr(hres) { }

	std::string getMBString()
	{
		std::stringstream out;
		out<<"Error: "<<this->error<<"\nFile: "<<this->file<<"\nLine: "<<this->line<<"\n\n";

		return out.str();
	}
} ERROR_DESC;

static bool throw_error(HRESULT res, std::string err, std::string filename, long line)
{
	ERROR_DESC e(res, err, filename, line);
	std::string message = e.getMBString();
	message += "Press ABORT to end the program, RETRY to debug, IGNORE to throw the error.";
	
	int result = MessageBoxA(NULL, message.c_str(), "Error.hpp", MB_ABORTRETRYIGNORE | MB_ICONSTOP);

	switch(result)
	{
	case IDABORT:
		exit(-1);
		break;

	case IDRETRY:
		return false;
		break;

	case IDIGNORE:
		throw(e);
		break;
	}

	return true;
}

static void throw_no_dialog(HRESULT res, std::string err, std::string filename, long line)
{
	ERROR_DESC e(res, err, filename, line);
	throw(e);
}

#define THROW(err) { \
	if(!throw_error(NULL, err, __FILE__, __LINE__)) { \
		_asm { int 3 } \
	} \
} \

#define THROW_IF_FAIL(result, err) { \
	if(FAILED((HRESULT)result)) { \
		if(!throw_error(res9001, err, __FILE__, __LINE__)) { \
			_asm { int 3 } \
		}\
	}\
}\

#define THROW_NOMB(err) { \
	throw_no_dialog(NULL, err, __FILE__, __LINE__); \
} \

#endif