// ---------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002  ONERA
//
// This file is part of CERTI-libcerti
//
// CERTI-libcerti is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libcerti is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: audit.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

// $Id: audit.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------
// Constantes pour les fichiers d'audit. Voir aussi local_config.hh. Voir
// la description du format dans AuditFile.hh.

#ifndef _CERTI_AUDIT_HH
#define _CERTI_AUDIT_HH

#include "exception_type.hh"  // Exception Levels

#define AUDIT_MIN_LEVEL 0
#define AUDIT_MAX_LEVEL 10

#define AUDITEVENT_START_AUDIT    128
#define AUDITEVENT_STOP_AUDIT     129
#define AUDITEVENT_START_RTIG     130
#define AUDITEVENT_STOP_RTIG      131

#endif // _CERTI_AUDIT_HH

// $Id: audit.hh,v 3.0 2002/11/21 01:27:51 breholee Exp $

