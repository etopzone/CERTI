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
// $Id: GAV.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
// ---------------------------------------------------------------------------

#include <config.h>

#include "RTItypes.hh"
#include "PrettyDebug.hh"

#include <assert.h>

namespace certi {

  static pdCDebug D("GAV", "(gav)      - ");

//-----------------------------------------------------------------------
// CAttributeHandleValuePair
//-----------------------------------------------------------------------

CAttributeHandleValuePair::CAttributeHandleValuePair()
{
  _order = RECEIVE;
  _transport = RELIABLE;
  _isPublished = RTI_FALSE;
  _isSubscribed = RTI_FALSE;
  _attrib = MAX_ATTRIBUTES_PER_CLASS;
  _value.name[0] = '\0';
  _value.value[0] = '\0';
  _value.type[0] = '\0';
  _value.lenght = 0;
  _next =(CAttributeHandleValuePair *)NULL;
}

CAttributeHandleValuePair::~CAttributeHandleValuePair()
{
}

void CAttributeHandleValuePair::AfficheAttribute()
{
  printf("   Attrib: %2d %-9s %-11s Pub=%-3s Sub=%-3s \"%s,%s,%s\"\n",
	 _attrib,
	(_order==1)?"RECEIVE":"TIMESTAMP",
	(_transport==1)?"RELIABLE":"BEST_EFFORT",
	(_isPublished)?"Yes":"No",
	(_isSubscribed)?"Yes":"No",
	 _value.name, _value.value, _value.type);
}

//-----------------------------------------------------------------------
// CParameterHandleValuePair
//-----------------------------------------------------------------------

CParameterHandleValuePair::CParameterHandleValuePair()
{
  _param = MAX_PARAMETERS_PER_CLASS;
  _value.name[0] = '\0';
  _value.value[0] = '\0';
  _value.type[0] = '\0';
  _value.lenght = 0;
  _next =(CParameterHandleValuePair *)NULL;
}

CParameterHandleValuePair::~CParameterHandleValuePair()
{
}

void CParameterHandleValuePair::AfficheParameter()
{
  printf("   Param: %2d                                        \"%s,%s,%s\"\n",
	 _param,_value.name, _value.value, _value.type);
}

//-----------------------------------------------------------------------
// CAttributeHandleValuePairSet
//-----------------------------------------------------------------------

CAttributeHandleValuePairSet::CAttributeHandleValuePairSet()
{
  _size = 0;
  _head =(CAttributeHandleValuePair *)NULL;
}

void CAttributeHandleValuePairSet::add(CAttributeHandleValuePair *att)
{
  CAttributeHandleValuePair *newNode = att;
  newNode->_next = _head;
  _head = newNode;
  _size++;
  assert(_size > 0);
}

CAttributeHandleValuePair *CAttributeHandleValuePairSet::
getWithHandle(AttributeHandle attrib) const
{
  CAttributeHandleValuePair *tmp = _head;

  for(int i=0; i<_size; i++)
    {
       if(tmp->_attrib == attrib)
	return tmp;
      else
	tmp = tmp->_next;
    }
  D.Out(pdError,"CAttributeHandleValuePairSet::getWithHandle : Not Found.");
  return NULL;
}

CAttributeHandleValuePair *CAttributeHandleValuePairSet::
getWithName(AttributeName valueName) const
{
  CAttributeHandleValuePair *tmp = _head;
  for(int i=0; i<_size; i++)
    {
      if(strcmp(tmp->_value.name, valueName) == 0)
	return tmp;
      else
	tmp = tmp->_next;
    }
  D.Out(pdError, 
	 "CAttributeHandleValuePairSet::getWithName : Not Found.");
  return NULL;
}

CAttributeHandleValuePair *CAttributeHandleValuePairSet::
getIeme(UShort pos) const
{
  CAttributeHandleValuePair *tmp = _head;
  assert(pos < _size);
  for(int i=0; i<pos; i++)
    {
      tmp = tmp->_next;
    }
  return tmp;
}

void CAttributeHandleValuePairSet::del(AttributeHandle attrib)
{
  CAttributeHandleValuePair *tmp = _head;
  if(tmp->_attrib == attrib)
    {
      _head = _head->_next;
      delete tmp;
      _size--;
      // assert(_size >= 0);  FIXME: était toujours positif(unsigned)
    }
  else
    {
      CAttributeHandleValuePair *tmpsuiv = tmp->_next;
      for(int i=1; i<_size; i++)
	{
	  if(tmpsuiv->_attrib == attrib)
	    {
	      tmp->_next = tmpsuiv->_next;
	      delete tmpsuiv;
	      _size--;
	      // assert(_size >= 0);  FIXME: était toujours positif(unsigned)
	    }
	  else
	    {
	      tmp = tmp->_next;
	      tmpsuiv = tmpsuiv->_next;
	    }
	}
    }
}

void CAttributeHandleValuePairSet::empty()
{
  CAttributeHandleValuePair *tmp = _head;
  while(_head != NULL)
    {
      _head = _head->_next;
      delete tmp;
      tmp = _head;
      _size--;
      // assert(_size >= 0);  FIXME: était toujours positif(unsigned)
    }
  assert(_size == 0);
}

//-----------------------------------------------------------------------
// CParameterHandleValuePairSet
//-----------------------------------------------------------------------

CParameterHandleValuePairSet::CParameterHandleValuePairSet()
{
  _size = 0;
  _head =(CParameterHandleValuePair *)NULL;
}

void CParameterHandleValuePairSet::add(CParameterHandleValuePair *par)
{
  CParameterHandleValuePair *newNode = par;
  newNode->_next = _head;
  _head = newNode;
  _size++;
  assert(_size > 0);
}

CParameterHandleValuePair *
CParameterHandleValuePairSet::getWithHandle(ParameterHandle param) const
{
  CParameterHandleValuePair *tmp = _head;
  for(int i=0; i<_size; i++)
    {
      if(tmp->_param == param)
	return tmp;
      else
	tmp = tmp->_next;
    }
  D.Out(pdError,"CParamterHandleValuePairSet::getWithHandle : Not Found.");
  return NULL;
}

CParameterHandleValuePair *
CParameterHandleValuePairSet::getWithName(ParameterName valueName) const
{
  CParameterHandleValuePair *tmp = _head;
  for(int i=0; i<_size; i++)
    {
      if(strcmp(tmp->_value.name, valueName) == 0)
	return tmp;
      else
	tmp = tmp->_next;
    }
  D.Out(pdError, 
	 "CParamterHandleValuePairSet::getWithName : Not Found.");
  return NULL;
}


CParameterHandleValuePair *
CParameterHandleValuePairSet::getIeme(UShort pos) const
{
  CParameterHandleValuePair *tmp = _head;
  assert(pos < _size);
  for(int i=0; i<pos; i++)
    {
      tmp = tmp->_next;
    }
  return tmp;
}

void CParameterHandleValuePairSet::del(ParameterHandle param)
{
  CParameterHandleValuePair *tmp = _head;
  if(tmp->_param == param)
    {
      _head = _head->_next;
      delete tmp;
      _size--;
      // assert(_size >= 0); FIXME: était toujours positif(unsigned)
    }
  else
    {
      CParameterHandleValuePair *tmpsuiv = tmp->_next;
      for(int i=1; i<_size; i++)
	{
	  if(tmpsuiv->_param == param)
	    {
	      tmp->_next = tmpsuiv->_next;
	      delete tmpsuiv;
	      _size--;
	      // assert(_size >= 0); FIXME: était toujours positif(unsigned)
	    }
	  else
	    {
	      tmp = tmp->_next;
	      tmpsuiv = tmpsuiv->_next;
	    }
	}
    }
}

void CParameterHandleValuePairSet::empty()
{
  CParameterHandleValuePair *tmp = _head;
  while(_head != NULL)
    {
      _head = _head->_next;
      delete tmp;
      tmp = _head;
      _size--;
      // assert(_size >= 0);  FIXME: était toujours positif(unsigned)
    }
  assert(_size == 0);
}

// //-----------------------------------------------------------------------
// // CObject
// //-----------------------------------------------------------------------

// CObject::CObject()
// {
//   _objID = 0;
//   _objClass = 0;
//   _objName = new char [MAX_BYTES_PER_VALUE];
//   _isPublished = RTI_FALSE;
//   _isSubscribed = RTI_FALSE;
//   _father = ROOT_OBJECT_CLASS_HANDLE;
//   _prof = 0;
//   _next =(CObject *)NULL;
//   _prev =(CObject *)NULL;
// }

// CObject::~CObject()
// {
//   delete _objName;
// }

// void CObject::AfficheObject()
// {
//   printf("Object: %3d id=%-2d Fath=%-3d Pub=%-3s Sub=%-3s \"%s\"\n",
// 	  _objClass,
// 	  _objID,
// 	  _father,
// 	(_isPublished)?"Yes":"No",
// 	(_isSubscribed)?"Yes":"No",
// 	  _objName);
//   if(_sonSet._size != 0)
//     printf("   Sons:");
//   for(int j=0; j<_sonSet._size; j++)
//     {
//       CSon *tmpson = _sonSet.getIeme(j);
//       tmpson->AfficheSon();
//     }
//   printf("\n");
//   for(int k=0; k<_attribSet._size; k++)
//     {
//       CAttributeHandleValuePair *tmpattrib = _attribSet.getIeme(k);
//       tmpattrib->AfficheAttribute();
//     }
// }

// void CObject::publish(Boolean booleen, CRootObj *theRootObj)
// {
//   for(int i=0; i<_sonSet._size; i++)
//     {
//       CObject *tmpobj = theRootObj->_obj.getWithHandle((_sonSet.getIeme(i))->_handle);
//       tmpobj->publish(booleen, theRootObj);
//     }
//   _isPublished = booleen;
// }

// void CObject::subscribe(Boolean booleen, CRootObj *theRootObj)
// {
//   for(int i=0; i<_sonSet._size; i++)
//     {
//       CObject *tmpobj = theRootObj->_obj.getWithHandle((_sonSet.getIeme(i))->_handle);
//       tmpobj->subscribe(booleen, theRootObj);
//     }
//   _isSubscribed = booleen;
// }

// //-----------------------------------------------------------------------
// // CInteraction
// //-----------------------------------------------------------------------

// CInteraction::CInteraction()
// {
//   _intClass = 0;
//   _intName = new char [MAX_BYTES_PER_VALUE];
//   _order = RECEIVE;
//   _transport = RELIABLE;
//   _isPublished = RTI_FALSE;
//   _isSubscribed = RTI_FALSE;
//   _father = ROOT_INTERACTION_CLASS_HANDLE;
//   _prof = 0;
//   _next =(CInteraction *)NULL;
//   _prev =(CInteraction *)NULL;
// }

// CInteraction::~CInteraction()
// {
//   delete _intName;
// }

// void CInteraction::AfficheInteraction()
// {
//   printf("Inter: %3d %-9s %-11s F=%-3d P=%-3s S=%-3s \"%s\"\n",
// 	  _intClass,
// 	(_order==1)?"RECEIVE":"TIMESTAMP",
// 	(_transport==1)?"RELIABLE":"BEST_EFFORT",
// 	  _father,
// 	(_isPublished)?"Yes":"No",
// 	(_isSubscribed)?"Yes":"No",
// 	  _intName);
//   if(_sonSet._size != 0)
//     printf("   Sons:");
//   for(int n=0; n<_sonSet._size; n++)
//     {
//       CSon *tmpson = _sonSet.getIeme(n);
//       tmpson->AfficheSon();
//     }
//   printf("\n");
//   for(int p=0; p<_paramSet._size; p++)
//     {
//       CParameterHandleValuePair *tmpparam = _paramSet.getIeme(p);
//       tmpparam->AfficheParameter();
//     }
// }

// void CInteraction::publish(Boolean booleen, CRootObj *theRootObj)
// {
//   for(int i=0; i<_sonSet._size; i++)
//     {
//       CInteraction *tmpint = theRootObj->_int.getWithHandle((_sonSet.getIeme(i))->_handle);
//       tmpint->publish(booleen, theRootObj);
//     }
//   _isPublished = booleen;
// }

// void CInteraction::subscribe(Boolean booleen, CRootObj *theRootObj)
// {
//   for(int i=0; i<_sonSet._size; i++)
//     {
//       CInteraction *tmpint = theRootObj->_int.getWithHandle((_sonSet.getIeme(i))->_handle);
//       tmpint->subscribe(booleen, theRootObj);
//     }
//   _isSubscribed = booleen;
// }

// void CInteraction::transport(TransportType type, CRootObj *theRootObj)
// {
//   for(int i=0; i<_sonSet._size; i++)
//     {
//       CInteraction *tmpint = theRootObj->_int.getWithHandle((_sonSet.getIeme(i))->_handle);
//       tmpint->transport(type, theRootObj);
//     }
//   _transport = type;
// }

// void CInteraction::order(OrderType type, CRootObj *theRootObj)
// {
//   for(int i=0; i<_sonSet._size; i++)
//     {
//       CInteraction *tmpint = theRootObj->_int.getWithHandle((_sonSet.getIeme(i))->_handle);
// 		tmpint->order(type, theRootObj);
//     }
//   _order = type;
// }

// //-----------------------------------------------------------------------
// // CObjectSet
// //-----------------------------------------------------------------------

// CObjectSet::CObjectSet()
// {
//   _size = 0;
//   _head =(CObject *)NULL;
// }

// void CObjectSet::add(CObject *oClass)
// {
//   CObject *newNode = oClass;
//   newNode->_next = _head;
//   newNode->_prev =(CObject *)NULL;
//   _head = newNode;
//   _size++;
//   assert(_size > 0);
// }

// CObject *CObjectSet::getWithObj(ULong objID) const
// {
//   CObject *tmp = _head;
//   for(int i=0; i<_size; i++)
//     {
//       if(tmp->_objID == objID)
// 	return tmp;
//       else
// 	tmp = tmp->_next;
//     }
//   D.Out(pdError, "CObjectSet::getWithObj : Not Found.");
//   return NULL;
// }

// CObject *CObjectSet::getWithHandle(ObjectClassHandle objClass) const
// {
//   CObject *tmp = _head;
//   for(int i=0; i<_size; i++)
//     {
//       if(tmp->_objClass == objClass)
// 	return tmp;
//       else
// 	tmp = tmp->_next;
//     }
//   D.Out(pdError, "CObjectSet::getWithHandle : Not Found.");
//   return NULL;
// }

// CObject *CObjectSet::getWithName(ObjectClassName objName) const
// {
//   CObject *tmp = _head;
//   for(int i=0; i<_size; i++)
//     {
//       if(strcmp(tmp->_objName, objName) == 0)
// 	return tmp;
//       else
// 	tmp = tmp->_next;
//     }
//   D.Out(pdError, "CObjectSet::getWithName : Not Found.");
//   return NULL;
// }

// CObject *CObjectSet::getWithNameAndID(ObjectClassName objName, ObjectHandle id) const
// {
//   CObject *tmp = _head;
//   for(int i=0; i<_size; i++)
//     {
//       if((strcmp(tmp->_objName, objName) == 0) &&(tmp->_objID == id))
// 	return tmp;
//       else
// 	tmp = tmp->_next;
//     }
//   D.Out(pdError, "CObjectSet::getWithNameAndID : Not Found.");
//   return NULL;
// }

// CObject *CObjectSet::getIeme(UShort pos) const
// {
//   CObject *tmp = _head;
//   assert(pos < _size);
//   for(int i=0; i<pos; i++)
//     {
//       tmp = tmp->_next;
//     }
//   return tmp;
// }

// void CObjectSet::delWithObj(ULong objID)
// {
//   CObject *tmp = _head;
//   if(tmp->_objID == objID)
//     {
//       _head = _head->_next;
//       if(_head != NULL)
// 	_head->_prev =(CObject *)NULL;
//       delete tmp;
//       _size--;
//       assert(_size >= 0);
//     }
//   else
//     {
//       CObject *tmpsuiv = tmp->_next;
//       for(int i=1; i<_size; i++)
// 	{
// 	  if(tmpsuiv->_objID == objID)
// 	    {
// 	      tmp->_next = tmpsuiv->_next;
// 	      if(tmpsuiv->_next != NULL)
// 		tmp->_next->_prev = tmp;
// 	      delete tmpsuiv;
// 	      _size--;
// 	      assert(_size >= 0);
// 	    }
// 	  else
// 	    {
// 	      tmp = tmp->_next;
// 	      tmpsuiv = tmpsuiv->_next;
// 	    }
// 	}
//     }
// }

// void CObjectSet::delWithHandle(ObjectClassHandle objClass)
// {
//   CObject *tmp = _head;
//   if(tmp->_objClass == objClass)
//     {
//       _head = _head->_next;
//       if(_head != NULL)
// 	_head->_prev =(CObject *)NULL;
//       delete tmp;
//       _size--;
//       assert(_size >= 0);
//     }
//   else
//     {
//       CObject *tmpsuiv = tmp->_next;
//       for(int i=1; i<_size; i++)
// 	{
// 	  if(tmpsuiv->_objClass == objClass)
// 	    {
// 	      tmp->_next = tmpsuiv->_next;
// 	      if(tmpsuiv->_next != NULL)
// 		tmp->_next->_prev = tmp;
// 	      delete tmpsuiv;
// 	      _size--;
// 	      assert(_size >= 0);
// 	    }
// 	  else
// 	    {
// 	      tmp = tmp->_next;
// 	      tmpsuiv = tmpsuiv->_next;
// 	    }
// 	}
//     }
// }

// //-----------------------------------------------------------------------
// // CInteractionSet
// //-----------------------------------------------------------------------

// CInteractionSet::CInteractionSet()
// {
//   _size = 0;
//   _head =(CInteraction *)NULL;
// }

// void CInteractionSet::add(CInteraction *iClass)
// {
//   CInteraction *newNode = iClass;
//   newNode->_next = _head;
//   newNode->_prev =(CInteraction *)NULL;
//   _head = newNode;
//   _size++;
//   assert(_size > 0);
// }

// CInteraction *CInteractionSet::getWithHandle(InteractionClassHandle intClass) const
// {
//   CInteraction *tmp = _head;
//   for(int i=0; i<_size; i++)
//     {
//       if(tmp->_intClass == intClass)
// 	return tmp;
//       else
// 	tmp = tmp->_next;
//     }
//   D.Out(pdError, "CInteractionSet::getWithHandle : Not Found.");
//   return NULL;
// }

// CInteraction *CInteractionSet::getWithName(InteractionClassName intName) const
// {
//   CInteraction *tmp = _head;
//   for(int i=0; i<_size; i++)
//     {
//       if(strcmp(tmp->_intName, intName) == 0)
// 	return tmp;
//       else
// 	tmp = tmp->_next;
//     }
//   D.Out(pdError, "CInteractionSet::getWithName : Not Found.");
//   return NULL;
// }

// CInteraction *CInteractionSet::getIeme(UShort pos) const
// {
//   CInteraction *tmp = _head;
//   assert(pos < _size);
//   for(int i=0; i<pos; i++)
//     {
//       tmp = tmp->_next;
//     }
//   return tmp;
// }

// void CInteractionSet::del(InteractionClassHandle intClass)
// {
//   CInteraction *tmp = _head;
//   if(tmp->_intClass == intClass)
//     {
//       _head = _head->_next;
//       if(_head != NULL)
// 	_head->_prev =(CInteraction *)NULL;
//       delete tmp;
//       _size--;
//       assert(_size >= 0);
//     }
//   else
//     {
//       CInteraction *tmpsuiv = tmp->_next;
//       for(int i=1; i<_size; i++)
// 	{
// 	  if(tmpsuiv->_intClass == intClass)
// 	    {
// 	      tmp->_next = tmpsuiv->_next;
// 	      if(tmpsuiv->_next != NULL)
// 		tmp->_next->_prev = tmp;
// 	      delete tmpsuiv;
// 	      _size--;
// 	      assert(_size >= 0);
// 	    }
// 	  else
// 	    {
// 	      tmp = tmp->_next;
// 	      tmpsuiv = tmpsuiv->_next;
// 	    }
// 	}
//     }
// }


// //-----------------------------------------------------------------------
// // CRootObj
// //-----------------------------------------------------------------------

// CRootObj::CRootObj()
// {
// }


// void CRootObj::display()
// {
//   for(int i=0; i<_obj._size; i++)
//     {
//       CObject *tmpobj = _obj.getIeme(i);
//       printf("\n--------------------------------------------------------------------------------\n");
//       tmpobj->AfficheObject();
//     }
//   printf("\n--------------------------------------------------------------------------------\n");
  
//   for(int m=0; m<_int._size; m++)
//     {
//       CInteraction *tmpint = _int.getIeme(m);
//       printf("\n--------------------------------------------------------------------------------\n");
//       tmpint->AfficheInteraction();
//     }
//   printf("\n--------------------------------------------------------------------------------\n");
// }

// CRootObj& CRootObj::operator =(const CRootObj& theObj)
// {
//   // Gestion du CObjectSet
//   for(int i=0; i<theObj._obj._size; i++)
//     {
//       CObject *tmpobj = theObj._obj.getIeme(i);
//       CObject *insertObj = new CObject;
//       // Gestion du CAttributeHandleValuePairSet
//       for(int j=0; j<tmpobj->_attribSet._size; j++)
// 	{
// 	  CAttributeHandleValuePair *tmpattrib = tmpobj->_attribSet.getIeme(j);
// 	  CAttributeHandleValuePair *insertAttrib = new CAttributeHandleValuePair;
// 	  insertAttrib->_order = tmpattrib->_order;
// 	  insertAttrib->_transport	= tmpattrib->_transport;
// 	  insertAttrib->_isPublished = tmpattrib->_isPublished;
// 	  insertAttrib->_isSubscribed = tmpattrib->_isSubscribed;
// 	  insertAttrib->_attrib = tmpattrib->_attrib;
// 	  strcpy(insertAttrib->_value.name, tmpattrib->_value.name);
// 	  strcpy(insertAttrib->_value.value, tmpattrib->_value.value);
// 	  strcpy(insertAttrib->_value.type, tmpattrib->_value.type);
// 	  insertAttrib->_next = NULL;
// 	  insertObj->_attribSet.add(insertAttrib);
// 	}
//       // Gestion du CSonSet
//       for(int k=0; k<tmpobj->_sonSet._size; k++)
// 	{
// 	  CSon *tmpson = tmpobj->_sonSet.getIeme(k);
// 	  CSon *insertSon = new CSon;
// 	  insertSon->_handle = tmpson->_handle;
// 	  insertSon->_next = NULL;
// 	  insertObj->_sonSet.add(insertSon);
// 	}
//       // Gestion des autres attributs
//       insertObj->_objID = tmpobj->_objID;
//       insertObj->_objClass = tmpobj->_objClass;
//       strcpy(insertObj->_objName, tmpobj->_objName);
//       insertObj->_isPublished = tmpobj->_isPublished;
//       insertObj->_isSubscribed = tmpobj->_isSubscribed;
//       insertObj->_father = tmpobj->_father;
//       insertObj->_next = NULL;
//       insertObj->_prev = NULL;
//       _obj.add(insertObj);
//     }
  
//   // Gestion du CInteractionSet
//   for(int m=0; m<theObj._int._size; m++)
//     {
//       CInteraction *tmpint = theObj._int.getIeme(m);
//       CInteraction *insertInt = new CInteraction;
//       // Gestion du CParameterHandleValuePairSet
//       for(int n=0; n<tmpint->_paramSet._size; n++)
// 	{
// 	  CParameterHandleValuePair *tmpparam = tmpint->_paramSet.getIeme(n);
// 	  CParameterHandleValuePair *insertParam = new CParameterHandleValuePair;
// 	  insertParam->_param = tmpparam->_param;
// 	  strcpy(insertParam->_value.name, tmpparam->_value.name);
// 	  strcpy(insertParam->_value.value, tmpparam->_value.value);
// 	  strcpy(insertParam->_value.type, tmpparam->_value.type);
// 	  insertParam->_next = NULL;
// 	  insertInt->_paramSet.add(insertParam);
// 	}
//       // Gestion du CSonSet
//       for(int p=0; p<tmpint->_sonSet._size; p++)
// 	{
// 	  CSon *tmpson = tmpint->_sonSet.getIeme(p);
// 	  CSon *insertSon = new CSon;
// 	  insertSon->_handle = tmpson->_handle;
// 	  insertSon->_next = NULL;
// 	  insertInt->_sonSet.add(insertSon);
// 	}
//       // Gestion des autres attributs
//       insertInt->_intClass = tmpint->_intClass;
//       strcpy(insertInt->_intName, tmpint->_intName);
//       insertInt->_order = tmpint->_order;
//       insertInt->_transport = tmpint->_transport;
//       insertInt->_isPublished = tmpint->_isPublished;
//       insertInt->_isSubscribed = tmpint->_isSubscribed;
//       insertInt->_father = tmpint->_father;
//       insertInt->_prof = tmpint->_prof;
//       insertInt->_next = NULL;
//       insertInt->_prev = NULL;
//       _int.add(insertInt);
//     }
  
//   return *this;
// }

}

// $Id: GAV.cc,v 3.0 2002/11/21 01:27:51 breholee Exp $
