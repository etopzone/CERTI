#!/usr/bin/python
# -*- coding: utf-8 -*-
## ----------------------------------------------------------------------------
## CERTI - HLA RunTime Infrastructure
## Copyright (C) 2002-2018  ISAE-SUPAERO & ONERA
##
## This program is free software ; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public License
## as published by the Free Software Foundation ; either version 2 of
## the License, or (at your option) Any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY ; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this program ; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
## USA
##
## $Id: GenMsgPython.py,v 1.3 2010/06/11 12:43:12 erk Exp $
## ----------------------------------------------------------------------------

"""
The CERTI Message Generator.
Python Backend Generator
"""

import logging
import GenMsgBase


class PythonGenerator(GenMsgBase.CodeGenerator):

    """
    This is a Python generator for C{MessageAST}.
    """

    def generatorName(cls):
        return 'Python'

    generatorName = classmethod(generatorName)

    def __init__(self, MessageAST):
        super(PythonGenerator, self).__init__(MessageAST, '##')


        # Message builtin type to Python type
