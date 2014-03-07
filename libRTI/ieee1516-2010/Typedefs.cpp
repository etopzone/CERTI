// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002-2014  ONERA
//
// This file is part of CERTI-libRTI
//
// CERTI-libRTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libRTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// ----------------------------------------------------------------------------
#include <RTI/Typedefs.h>

namespace rti1516e
{
FederateRestoreStatus::FederateRestoreStatus(FederateHandle const & thePreHandle,
        FederateHandle const & thePostHandle,
        RestoreStatus theStatus) :
                         preRestoreHandle(thePostHandle),
                         postRestoreHandle(thePostHandle),
                         status(theStatus)
{
}

FederationExecutionInformation::FederationExecutionInformation(
        std::wstring const & theFederationExecutionName,
        std::wstring const & theLogicalTimeImplementationName) :
                federationExecutionName(theFederationExecutionName),
                logicalTimeImplementationName(theLogicalTimeImplementationName)
{
}

SupplementalReflectInfo::SupplementalReflectInfo() :
        hasProducingFederate(true), hasSentRegions(false)
{
}

SupplementalReflectInfo::SupplementalReflectInfo (
         FederateHandle const & theFederateHandle) :
                 hasProducingFederate(true), hasSentRegions(false), producingFederate(theFederateHandle)
    {

    }
SupplementalReflectInfo::SupplementalReflectInfo (
         RegionHandleSet const & theRegionHandleSet) :
        hasProducingFederate(false), hasSentRegions(true), sentRegions(theRegionHandleSet)
    {

    }
SupplementalReflectInfo::SupplementalReflectInfo (
         FederateHandle const & theFederateHandle,
         RegionHandleSet const & theRegionHandleSet) :
        hasProducingFederate(true), hasSentRegions(true),
        producingFederate(theFederateHandle), sentRegions(theRegionHandleSet)
    {
    }
SupplementalReceiveInfo::SupplementalReceiveInfo() :
        hasProducingFederate(true), hasSentRegions(false)
{
}

SupplementalReceiveInfo::SupplementalReceiveInfo (
         FederateHandle const & theFederateHandle) :
                 hasProducingFederate(true), hasSentRegions(false), producingFederate(theFederateHandle)
    {

    }
SupplementalReceiveInfo::SupplementalReceiveInfo (
         RegionHandleSet const & theRegionHandleSet) :
        hasProducingFederate(false), hasSentRegions(true), sentRegions(theRegionHandleSet)
    {

    }
SupplementalReceiveInfo::SupplementalReceiveInfo (
         FederateHandle const & theFederateHandle,
         RegionHandleSet const & theRegionHandleSet) :
        hasProducingFederate(true), hasSentRegions(true),
        producingFederate(theFederateHandle), sentRegions(theRegionHandleSet)
    {
    }

SupplementalRemoveInfo::SupplementalRemoveInfo() :
        hasProducingFederate(false)
{
}

SupplementalRemoveInfo::SupplementalRemoveInfo(FederateHandle const & theFederateHandle) :
        hasProducingFederate(true), producingFederate(theFederateHandle)
{
}
} // end namespace rti1516e
