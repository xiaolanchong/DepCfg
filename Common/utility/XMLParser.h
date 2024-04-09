// XMLParser.h: interface for the XMLParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLPARSER_H__F16338B6_5040_4237_B216_6E4DC10D3689__INCLUDED_)
#define AFX_XMLPARSER_H__F16338B6_5040_4237_B216_6E4DC10D3689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <atlbase.h>
#include <msxml.h>
#include <string>

#include "Exception.h"

MACRO_EXCEPTION( XMLException, CommonException );

MACRO_EXCEPTION( NameException, XMLException );
MACRO_EXCEPTION( AtributeException, XMLException );

MACRO_EXCEPTION( StringException, AtributeException );
MACRO_EXCEPTION( BoolException, AtributeException );
MACRO_EXCEPTION( NumericException, AtributeException );

struct XMLElem
{ 
	CComPtr<IXMLDOMElement> elem;
	  CComPtr<IXMLDOMNodeList> nlist; int pos; long clen;
	//
	XMLElem() : elem(0), nlist(0), pos(-1), clen(0) {}
	XMLElem(int _clen) : elem(0), nlist(0), pos(-1), clen(_clen) {}
	XMLElem(CComPtr<IXMLDOMElement> _elem) : elem(_elem), nlist(0), pos(-1), clen(0) {get();}
	XMLElem(CComPtr<IXMLDOMNodeList> _nlist) : elem(0), nlist(_nlist), pos(0), clen(0) {get();}
	void get()
	{ 
		if (pos!=-1)
		{ 
			elem=0;
		    CComPtr<IXMLDOMNode> inode;
		    nlist->get_item(pos,&inode);
		    if (inode==0) return;
		    DOMNodeType type; inode->get_nodeType(&type);
		    if (type!=NODE_ELEMENT) return;
		    CComQIPtr<IXMLDOMElement> e(inode);
		    elem=e;
		 }
	   clen=0; if (elem!=0)
	    { CComPtr<IXMLDOMNodeList> iNodeList;
	      elem->get_childNodes(&iNodeList);
	      iNodeList->get_length(&clen);  
	    }
	  }
  //
  std::wstring name() const
  { if (!elem) throw NameException(std::string());
    CComBSTR bn; elem->get_tagName(&bn);
    return std::wstring(bn);
  }
  
  std::wstring attr(const std::wstring& name) const
  { if (!elem) throw XMLException(std::string());
    CComBSTR bname(name.c_str());
    CComVariant val(VT_EMPTY);
    elem->getAttribute(bname,&val);
    if (val.vt==VT_BSTR) return val.bstrVal;
	throw StringException(std::string());
    return L"";
  }
  
  bool attrBool(const std::wstring& name) const
  { std::wstring a = attr(name);
    if (a==L"true" || a==L"TRUE") return true;
    else if (a==L"false" || a==L"FALSE") return false;
    throw BoolException(std::string());
  }
  
  int attrInt(const std::wstring& name) const
  { std::wstring a = attr(name);
    int i, res=swscanf(a.c_str(),L"%i",&i);
    if (res==1) return i; 
	throw NumericException(std::string());
  }
  
  std::wstring val() const
  { if (!elem) return L"";
    CComVariant val(VT_EMPTY);
    elem->get_nodeTypedValue(&val);
    if (val.vt==VT_BSTR) return val.bstrVal;
    return L"";
  }
  
  XMLElem subnode(const std::wstring name) const
  { if (!elem) return XMLElem();
    for (XMLElem c=begin(); c!=end(); c++)
    { if (c.name()==name) return c;
    }
    return XMLElem();
  }
  
  std::wstring subval(const std::wstring name) const
  { if (!elem) return L"";
    XMLElem c=subnode(name);
    return c.val();
  }
  
  XMLElem begin() const
  { if (!elem) return XMLElem();
    CComPtr<IXMLDOMNodeList> iNodeList;
    elem->get_childNodes(&iNodeList);
    return XMLElem(iNodeList);
  }
  
  XMLElem end() const
  { return XMLElem(clen);
  }
  
  XMLElem operator++(int)
  { if (pos!=-1) {pos++; get();}
    return *this;
  }
  
  bool operator!=(const XMLElem &e) const
  { return pos!=e.clen;
  }
};

#endif // !defined(AFX_XMLPARSER_H__F16338B6_5040_4237_B216_6E4DC10D3689__INCLUDED_)
