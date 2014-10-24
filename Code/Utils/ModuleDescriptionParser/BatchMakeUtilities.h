/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://www.na-mic.org/svn/Slicer3/trunk/Libs/ModuleDescriptionParser/ModuleDescription.h $
  Date:      $Date: 2007/04/09 02:03:22 $
  Version:   $Revision: 1.1 $

==========================================================================*/

#ifndef __BatchMakeUtilities_h
#define __BatchMakeUtilities_h

#include "ModuleDescriptionParserWin32Header.h"

#include "ModuleDescription.h"

ModuleDescriptionParser_EXPORT std::string GenerateBatchMakeWrapper(const ModuleDescription &module);

#endif
