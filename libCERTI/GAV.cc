// ----------------------------------------------------------------------------
// CERTI - HLA RunTime Infrastructure
// Copyright (C) 2002, 2003  ONERA
//
// This file is part of CERTI-libCERTI
//
// CERTI-libCERTI is free software ; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation ; either version 2 of
// the License, or (at your option) any later version.
//
// CERTI-libCERTI is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY ; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program ; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
//
// $Id: GAV.cc,v 3.10 2005/03/13 22:40:49 breholee Exp $
// ----------------------------------------------------------------------------

#include <config.h>
#include "GAV.hh"

#include "PrettyDebug.hh"
#include "converter.hh"

#include <algorithm>
#include <assert.h>

namespace certi {

static pdCDebug D("GAV", "(gav) - ");

// ----------------------------------------------------------------------------
// CAttributeHandleValuePair
// ----------------------------------------------------------------------------

CAttributeHandleValuePair::CAttributeHandleValuePair()
{
    _order = RECEIVE ;
    _transport = RELIABLE ;
    _isPublished = RTI_FALSE ;
    _isSubscribed = RTI_FALSE ;
    _attrib = MAX_ATTRIBUTES_PER_CLASS ;
    _value.name[0] = '\0' ;
    _value.value[0] = '\0' ;
    _value.type[0] = '\0' ;
    _value.length = 0 ;
    _next = (CAttributeHandleValuePair *)NULL ;
}

CAttributeHandleValuePair::~CAttributeHandleValuePair()
{
}

void CAttributeHandleValuePair::AfficheAttribute()
{
    printf(" Attrib: %2ld %-9s %-11s Pub=%-3s Sub=%-3s \"%s, %s, %s\"\n",
           _attrib,
           (_order==1)?"RECEIVE":"TIMESTAMP",
           (_transport==1)?"RELIABLE":"BEST_EFFORT",
           (_isPublished)?"Yes":"No",
           (_isSubscribed)?"Yes":"No",
           _value.name, _value.value, _value.type);
}

// ----------------------------------------------------------------------------
// CParameterHandleValuePair
// ----------------------------------------------------------------------------

CParameterHandleValuePair::CParameterHandleValuePair()
{
    _param = MAX_PARAMETERS_PER_CLASS ;
    _value.name[0] = '\0' ;
    _value.value[0] = '\0' ;
    _value.type[0] = '\0' ;
    _value.length = 0 ;
    _next = (CParameterHandleValuePair *)NULL ;
}

CParameterHandleValuePair::~CParameterHandleValuePair()
{
}

void CParameterHandleValuePair::AfficheParameter()
{
    printf(" Param: %2ld \"%s, %s, %s\"\n",
           _param, _value.name, _value.value, _value.type);
}

// ----------------------------------------------------------------------------
// CAttributeHandleValuePairSet
// ----------------------------------------------------------------------------

CAttributeHandleValuePairSet::CAttributeHandleValuePairSet()
    : _size(0), _head((CAttributeHandleValuePair *)NULL)
{
}

CAttributeHandleValuePairSet::
CAttributeHandleValuePairSet(const AttributeHandleValuePairSet & ahvps) :
    _size(0), _head((CAttributeHandleValuePair *)NULL)
{
    CAttributeHandleValuePair *cahvp ;
    ULong valueLength ;

    for (unsigned int i = 0 ; i < ahvps.size(); ++i) {
        cahvp = new CAttributeHandleValuePair();
        cahvp->_attrib = ahvps.getHandle(i);
        ahvps.getValue(i, cahvp->_value.value, valueLength);
        cahvp->_value.length = valueLength ;
        add(cahvp);
    }
}

CAttributeHandleValuePairSet::
CAttributeHandleValuePairSet(const AttributeHandleSet & ahs)
    : _size(0), _head((CAttributeHandleValuePair *)NULL)
{
    CAttributeHandleValuePair *cahvp ;

    for (unsigned int i = 0 ; i < ahs.size(); ++i) {
        cahvp = new CAttributeHandleValuePair();
        cahvp->_attrib = ahs.getHandle(i);
        add(cahvp);
    }
}

CAttributeHandleValuePairSet::~CAttributeHandleValuePairSet()
{
    empty();
}

void CAttributeHandleValuePairSet::add(CAttributeHandleValuePair *att)
{
    CAttributeHandleValuePair *newNode = att ;
    newNode->_next = _head ;
    _head = newNode ;
    _size++ ;
    assert(_size > 0);
}

CAttributeHandleValuePair *CAttributeHandleValuePairSet::
getWithHandle(AttributeHandle attrib) const
{
    CAttributeHandleValuePair *tmp = _head ;

    for (int i=0 ; i<_size ; i++)
        {
            if (tmp->_attrib == attrib)
                return tmp ;
            else
                tmp = tmp->_next ;
        }
    D.Out(pdError, "CAttributeHandleValuePairSet::getWithHandle : Not Found.");
    return NULL ;
}

CAttributeHandleValuePair *CAttributeHandleValuePairSet::
getWithName(const char *valueName) const
{
    CAttributeHandleValuePair *tmp = _head ;
    for (int i=0 ; i<_size ; i++)
        {
            if (strcmp(tmp->_value.name, valueName) == 0)
                return tmp ;
            else
                tmp = tmp->_next ;
        }
    D.Out(pdError,
          "CAttributeHandleValuePairSet::getWithName : Not Found.");
    return NULL ;
}

CAttributeHandleValuePair *CAttributeHandleValuePairSet::
getIeme(UShort pos) const
{
    CAttributeHandleValuePair *tmp = _head ;
    assert(pos < _size);
    for (int i=0 ; i<pos ; i++)
        {
            tmp = tmp->_next ;
        }
    return tmp ;
}

void CAttributeHandleValuePairSet::del(AttributeHandle attrib)
{
    CAttributeHandleValuePair *tmp = _head ;
    if (tmp->_attrib == attrib) {
        _head = _head->_next ;
        delete tmp ;
        _size-- ;
        // assert(_size >= 0); FIXME: était toujours positif (unsigned)
    }
    else
        {
            CAttributeHandleValuePair *tmpsuiv = tmp->_next ;
            for (int i=1 ; i<_size ; i++)
                {
                    if (tmpsuiv->_attrib == attrib)
                        {
                            tmp->_next = tmpsuiv->_next ;
                            delete tmpsuiv ;
                            _size-- ;
                            // assert(_size >= 0); FIXME: était toujours positif (unsigned)
                        }
                    else
                        {
                            tmp = tmp->_next ;
                            tmpsuiv = tmpsuiv->_next ;
                        }
                }
        }
}

void CAttributeHandleValuePairSet::empty()
{
    CAttributeHandleValuePair *tmp = _head ;
    while (_head != NULL)
        {
            _head = _head->_next ;
            delete tmp ;
            tmp = _head ;
            _size-- ;
            // assert(_size >= 0); FIXME: était toujours positif (unsigned)
        }
    assert(_size == 0);
}

AttributeHandleValuePairSet*
CAttributeHandleValuePairSet::toAHVPS() const
{
    AttributeHandleValuePairSet *ahvps ;
    ahvps = AttributeSetFactory::create(_size);

    for (int i = 0 ; i<_size ; i++) {
        CAttributeHandleValuePair *cahvp = getIeme(i);

        if (cahvp != NULL) {
            if (&cahvp->_value != NULL) {
                ULong longueur;
                
                // decodage
                getStringToObjectLength(cahvp->_value.value, longueur);

                char *valeur = new char[longueur] ;
                valeur[0] = '\0' ;
                stringToObject(cahvp->_value.value, valeur, longueur);
                memcpy(cahvp->_value.value, valeur, longueur);
                delete valeur;
                /* phvps->add(cahvp->_param,
                   cahvp->_value.value,
                   (strlen(cahvp->_value.value)*sizeof(char))); */

                ahvps->add(cahvp->_attrib, cahvp->_value.value, longueur);
            }
            else ahvps->add(cahvp->_attrib, '\0', 0);
        }
    }

    return ahvps ;
}

// ----------------------------------------------------------------------------
// CParameterHandleValuePairSet
// ----------------------------------------------------------------------------

CParameterHandleValuePairSet::CParameterHandleValuePairSet()
    : _size(0), _head((CParameterHandleValuePair *) NULL)

{
}

CParameterHandleValuePairSet::
CParameterHandleValuePairSet(const ParameterHandleValuePairSet & phvps)
    : _size(0), _head((CParameterHandleValuePair *) NULL)

{
    CParameterHandleValuePair *cphvp = NULL ;
    ULong valueLength = 0 ;

    for (unsigned int i = 0 ; i < phvps.size(); i++) {
        cphvp = new CParameterHandleValuePair();
        cphvp->_param = phvps.getHandle(i);
        phvps.getValue(i, cphvp->_value.value, valueLength);
        cphvp->_value.length = valueLength ;
        add(cphvp);
    }
}

CParameterHandleValuePairSet::~CParameterHandleValuePairSet()
{
    empty();
}

void CParameterHandleValuePairSet::add(CParameterHandleValuePair *par)
{
    CParameterHandleValuePair *newNode = par ;
    newNode->_next = _head ;
    _head = newNode ;
    _size++ ;
    assert(_size > 0);
}

CParameterHandleValuePair *
CParameterHandleValuePairSet::getWithHandle(ParameterHandle param) const
{
    CParameterHandleValuePair *tmp = _head ;
    for (int i=0 ; i<_size ; i++)
        {
            if (tmp->_param == param)
                return tmp ;
            else
                tmp = tmp->_next ;
        }
    D.Out(pdError, "CParamterHandleValuePairSet::getWithHandle : Not Found.");
    return NULL ;
}

CParameterHandleValuePair *
CParameterHandleValuePairSet::getWithName(ParameterName valueName) const
{
    CParameterHandleValuePair *tmp = _head ;
    for (int i=0 ; i<_size ; i++)
        {
            if (strcmp(tmp->_value.name, valueName) == 0)
                return tmp ;
            else
                tmp = tmp->_next ;
        }
    D.Out(pdError,
          "CParamterHandleValuePairSet::getWithName : Not Found.");
    return NULL ;
}


CParameterHandleValuePair *
CParameterHandleValuePairSet::getIeme(UShort pos) const
{
    CParameterHandleValuePair *tmp = _head ;
    assert(pos < _size);
    for (int i=0 ; i<pos ; i++)
        {
            tmp = tmp->_next ;
        }
    return tmp ;
}

void CParameterHandleValuePairSet::del(ParameterHandle param)
{
    CParameterHandleValuePair *tmp = _head ;
    if (tmp->_param == param)
        {
            _head = _head->_next ;
            delete tmp ;
            _size-- ;
            // assert(_size >= 0); FIXME: était toujours positif (unsigned)
        }
    else
        {
            CParameterHandleValuePair *tmpsuiv = tmp->_next ;
            for (int i=1 ; i<_size ; i++)
                {
                    if (tmpsuiv->_param == param)
                        {
                            tmp->_next = tmpsuiv->_next ;
                            delete tmpsuiv ;
                            _size-- ;
                            // assert(_size >= 0); FIXME: était toujours positif (unsigned)
                        }
                    else
                        {
                            tmp = tmp->_next ;
                            tmpsuiv = tmpsuiv->_next ;
                        }
                }
        }
}

void CParameterHandleValuePairSet::empty()
{
    CParameterHandleValuePair *tmp = _head ;
    while (_head != NULL)
        {
            _head = _head->_next ;
            delete tmp ;
            tmp = _head ;
            _size-- ;
            // assert(_size >= 0); FIXME: était toujours positif (unsigned)
        }
    assert(_size == 0);
}

ParameterHandleValuePairSet*
CParameterHandleValuePairSet::toPHVPS() const
{
    ULong longueur ;
    CParameterHandleValuePair *cphvp ;

    ParameterHandleValuePairSetImp *phvps ;
    phvps = new ParameterHandleValuePairSetImp(_size);

    for (int i = 0 ; i<_size ; i++) {
        cphvp = getIeme(i);

        if (cphvp != NULL) {
            if (&cphvp->_value != NULL) {

                // decoding string to object
                getStringToObjectLength(cphvp->_value.value, longueur);
                char *valeur = new char[longueur] ;
                valeur[0] = '\0' ;
                stringToObject(cphvp->_value.value, valeur, longueur);
                memcpy(cphvp->_value.value, valeur, longueur);
                delete valeur;

                // adding new value to PHVPS
                phvps->add(cphvp->_param, cphvp->_value.value, longueur);
            }
            else phvps->add(cphvp->_param, '\0', 0);
        }
    }
    return (ParameterHandleValuePairSet *)phvps ;
}

}

// $Id: GAV.cc,v 3.10 2005/03/13 22:40:49 breholee Exp $
