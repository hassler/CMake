#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif
#include "cmMakefile.h"
#include "cmClassFile.h"

#include <fstream>
#include <iostream>


// remove extra spaces and the "\" character from the name
// of the class as it is in the Makefile.in
inline std::string CleanUpName(const char* name)
{
  std::string className = name;
  size_t i =0;
  while(className[i] == ' ')
    {
    i++;
    }
  if(i)
    {
    className = className.substr(i, className.size());
    } 
  size_t pos = className.find('\\');
  if(pos != std::string::npos)
    {
    className = className.substr(0, pos);
    }
  
  pos = className.find(' ');
  if(pos != std::string::npos)
    {
    className = className.substr(0, pos);
    }
  return className;
}

// default is not to be building executables
cmMakefile::cmMakefile()
{
  m_Executables = false;
}


// call print on all the classes in the makefile
void cmMakefile::Print()
{
  for(int i = 0; i < m_Classes.size(); i++)
    m_Classes[i].Print();
}

// Parse the given Makefile.in file into a list of classes.

bool cmMakefile::ReadMakefile(const char* filename)
{
  std::cerr << "reading makefile " << filename << std::endl;
  std::ifstream fin(filename);
  if(!fin)
    {
    std::cerr << "error can not open file " << filename << std::endl;
    return false;
    }
  char inbuffer[2048];
  while ( fin.getline(inbuffer, 2047 ) )
    {
    std::string line = inbuffer;
    cmClassFile file;
    if(line.find("COMPILE_CLASSES") != std::string::npos)
      {
      if(line.find("\\") != std::string::npos)
	{
	this->ReadClasses(fin, false);
	}
      }
#ifdef _WIN32
    else if(line.find("WIN32_CLASSES") != std::string::npos)
      {
      if(line.find("\\") != std::string::npos)
	{
	this->ReadClasses(fin, false);
	}
      }
#else
    else if(line.find("UNIX_CLASSES") != std::string::npos)
      {
      if(line.find("\\") != std::string::npos)
	{
	this->ReadClasses(fin, false);
	}
      }
#endif
    else if(line.find("ABSTRACT_CLASSES") != std::string::npos)
      {
      if(line.find("\\") != std::string::npos)
	{
	this->ReadClasses(fin, true);
	}
      }
    else if(line.find("SUBDIRS") != std::string::npos)
      {
      if(line.find("\\") != std::string::npos)
	{
	this->ReadSubdirs(fin);
	}
      }
    else if(line.find("EXECUTABLES") != std::string::npos)
      {
      if(line.find("\\") != std::string::npos)
	{
	this->ReadClasses(fin, false);
	m_Executables = true;
	}
      }
    else if(line.find("ME") != std::string::npos)
      {
      size_t mestart = line.find("ME");
      size_t start = line.find("=");
      if(start != std::string::npos && start > mestart )
	{
	start++;
	while(line[start] == ' ' && start < line.size())
	  {
	  start++;
	  }
	size_t end = line.size()-1;
	while(line[end] == ' ' && end > start)
	  {
	  end--;
	  }
	this->SetLibraryName(line.substr(start, end).c_str());
	}
      }
    }
  return true;
}
  

// Read a list from the Makefile stream
void cmMakefile::ReadClasses(std::ifstream& fin,
			      bool abstract)
{
  char inbuffer[2048];
  bool done = false;
  while (!done)
    {  
    // read a line from the makefile
    fin.getline(inbuffer, 2047); 
    // convert to a string class
    std::string classname = inbuffer;
    // if the line does not end in \ then we are at the
    // end of the list
    if(classname.find('\\') == std::string::npos)
      {
      done = true;
      }
    // remove extra spaces and \ from the class name
    classname = CleanUpName(classname.c_str());
    
    // if this is not an abstract list then add new class
    // to the list of classes in this makefile
    if(!abstract)
      {
      cmClassFile file;
      file.SetName(classname.c_str(), this->GetCurrentDirectory());
      file.m_AbstractClass = false;
      m_Classes.push_back(file);
      }
    else
      {
      // if this is an abstract list, then look
      // for an existing class and set it to abstract
      for(int i = 0; i < m_Classes.size(); i++)
	{
	if(m_Classes[i].m_ClassName == classname)
	  {
	  m_Classes[i].m_AbstractClass = true;
	  break;
	  }
	}
      }
    }
}


// Read a list of subdirectories from the stream
void cmMakefile::ReadSubdirs(std::ifstream& fin)
{
  char inbuffer[2048];
  bool done = false;

  while (!done)
    {  
    // read a line from the makefile
    fin.getline(inbuffer, 2047); 
    // convert to a string class
    std::string dir = inbuffer;
    // if the line does not end in \ then we are at the
    // end of the list
    if(dir.find('\\') == std::string::npos)
      {
      done = true;
      }
    // remove extra spaces and \ from the class name
    dir = CleanUpName(dir.c_str());
    m_SubDirectories.push_back(dir);
    }
}
