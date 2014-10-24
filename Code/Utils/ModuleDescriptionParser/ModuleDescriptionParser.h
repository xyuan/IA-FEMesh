/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/ModuleDescriptionParser/ModuleDescriptionParser.h $
  Date:      $Date: 2007/04/09 02:03:22 $
  Version:   $Revision: 1.1 $

==========================================================================*/
#ifndef __ModuleDescriptionParser_h
#define __ModuleDescriptionParser_h

#include <string>
#include <vector>

#include "ModuleDescriptionParserWin32Header.h"

class ModuleDescription;
class ModuleParameterGroup;


class ModuleDescriptionParser_EXPORT ModuleDescriptionParser
{
public:
  ModuleDescriptionParser() {};
  ~ModuleDescriptionParser() {};

  int Parse( const std::string& xml, ModuleDescription& description);
};

#endif
