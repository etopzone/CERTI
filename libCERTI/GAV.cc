// -*- mode:C++ ; tab-width:4 ; c-basic-offset:4 ; indent-tabs-mode:nil -*-
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
// $Id: GAV.cc,v 3.5 2003/02/19 18:07:29 breholee Exp $
// ----------------------------------------------------------------------------

#include "GAV.hh"

namespace certi {

static pdCDebug D("GAV", "(gav) - ");

//*******************************************************************
// Data Integrity
//*******************************************************************
//! Returns buffer size needed for storing message restored by stringToObject.
void
getStringToObjectLength(char *init_string, ULong& size)
{
    ULong counter = 0 ;
    ULong length = strlen(init_string);
    ULong i = 0 ;
    size = 0 ;
    while (i<length) {
        switch (init_string[i]) {
        case '\\':
            i++ ;
            while ((init_string[i]=='?') && (i<length)) {
                counter++ ;
                i++ ;
            }
            if ((counter%2)==0) size += (counter/2);
            else size += 1+((counter-1)/2);
            counter = 0 ;
            break ;
        default:
            size++ ;
            i++ ;
        }
    }
}

//! Restore string network message to the original text object.
/*! stringToObject is used to restore initial object as it was before being
  sent throw the network. Cases are as follows :
  - ? : if alone without \ prefixing caracter, restore to \0
  - \ : this caracter is used to tell that a caracter \ or ? needs replace it
  - other : keep caracter.

  Pattern is "\???\" for case \. If number of ? is odd, it must be
  replaced by a list of ? else it must be replaced by a list of \.
*/
void
stringToObject(char *init_string, char *end_string, ULong size)
{
    ULong counter = 0 ;
    ULong i = 0 ;
    ULong j = 0 ;
    ULong indice = 0 ;
    ULong length = strlen(init_string);

    memset(end_string, '\0', size);

    while (i<length) {
        switch(init_string[i]) {
        case '?':
            // No use to add '\0' since memset does it
            i++ ;
            indice++ ;
            break ;
        case '\\':
            i++ ;
            while ((init_string[i]=='?') && (i<length)) {
                counter++ ;
                i++ ;
            }
            i++ ;
            if ((counter%2)==0) {
                for (j=0 ; j<(counter/2); j++) {
                    end_string[indice]='\\' ;
                    indice++ ;
                }
            }
            else {
                for (j=0 ; j<(1+((counter-1)/2)); j++) {
                    end_string[indice]='?' ;
                    indice++ ;
                }
            }
            counter = 0 ;
            break ;
        default:
            end_string[indice]=init_string[i] ;
            i++ ;
            indice++ ;
        }
    }
}

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

CAttributeHandleValuePairSet::CAttributeHandleValuePairSet(void)
    : _size(0), _head((CAttributeHandleValuePair *)NULL)
{
}

CAttributeHandleValuePairSet::
CAttributeHandleValuePairSet(const AttributeHandleValuePairSet & ahvps) :
    _size(0), _head((CAttributeHandleValuePair *)NULL)
{
    CAttributeHandleValuePair *cahvp ;
    ULong valueLength ;

    for (int i = 0 ; i < ahvps.size(); i++) {
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

    for (int i = 0 ; i < ahs.size(); i++) {
        cahvp = new CAttributeHandleValuePair();
        cahvp->_attrib = ahs.getHandle(i);
        add(cahvp);
    }
}

CAttributeHandleValuePairSet::~CAttributeHandleValuePairSet(void)
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
getWithName(AttributeName valueName) const
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
CAttributeHandleValuePairSet::toAHVPS(void) const
{
    ULong longueur ;
    CAttributeHandleValuePair *cahvp ;

    AttributeHandleValuePairSet *ahvps ;
    ahvps = AttributeSetFactory::create(_size);

    for (int i = 0 ; i<_size ; i++) {
        cahvp = getIeme(i);

        if (cahvp != NULL) {
            if (&cahvp->_value != NULL) {
                // decodage
                getStringToObjectLength(cahvp->_value.value, longueur);
                char *valeur = new char[longueur] ;
                valeur[0] = '\0' ;
                stringToObject(cahvp->_value.value, valeur, longueur);
                memcpy(cahvp->_value.value, valeur, longueur);
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

CParameterHandleValuePairSet::CParameterHandleValuePairSet(void)
    : _size(0), _head((CParameterHandleValuePair *)NULL)

{
}

CParameterHandleValuePairSet::
CParameterHandleValuePairSet(const ParameterHandleValuePairSet & phvps)
    : _size(0), _head((CParameterHandleValuePair *)NULL)

{
    CParameterHandleValuePair *cphvp = NULL ;
    ULong valueLength = 0 ;

    for (ulong i = 0 ; i < phvps.size(); i++) {
        cphvp = new CParameterHandleValuePair();
        cphvp->_param = phvps.getHandle(i);
        phvps.getValue(i, cphvp->_value.value, valueLength);
        cphvp->_value.length = valueLength ;
        add(cphvp);
    }
}

CParameterHandleValuePairSet::~CParameterHandleValuePairSet(void)
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
CParameterHandleValuePairSet::toPHVPS(void) const
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

                // adding new value to PHVPS
                phvps->add(cphvp->_param, cphvp->_value.value, longueur);
            }
            else phvps->add(cphvp->_param, '\0', 0);
        }
    }
    return (ParameterHandleValuePairSet *)phvps ;
}

}

// $Id: GAV.cc,v 3.5 2003/02/19 18:07:29 breholee Exp $
