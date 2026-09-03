//////////////////////////////////////////////////////////////////////////////
/// \file XML.cpp
/// \author excel96
/// \date 2003.7.25
///
/// \todo
/// \bug
/// \warning
//////////////////////////////////////////////////////////////////////////////

//#include "SFCPCH.h"
#include "client_PCH.h"

#include "SXML.h"
#include "PacketAssert.h"
using namespace std;

//#pragma warning (push, 1)
//	#include <xercesc/sax/SAXParseException.hpp>
//	#include <xercesc/sax/SAXException.hpp>
//	#include <xercesc/framework/MemBufInputSource.hpp>
//	#include <stdio.h>
//	#include <stdarg.h>
//	#include <time.h>
//	#include <fstream>
//#pragma warning (pop)

static const char chXMLTagOpen		= '<';
static const char chXMLTagClose	= '>';
static const char chXMLTagPre	= '/';
static const char chXMLEscape = '\\';	// 값 필드 이스케이프용
static const char chXMLTrimToken[] = " </>\r\t\n=\"\'";
static const char chXMLAttrToken[] = "\"\'";

#pragma warning(disable:4100)

static const char* XML_ERROR_FILENAME = "__XMLError.log";

//////////////////////////////////////////////////////////////////////////////
/// \brief 
/// \param str 
/// \return string 
//////////////////////////////////////////////////////////////////////////////
string XMLUtil::WideCharToString(const wchar_t * wstr, int wstrlen)
{
	if ( wstrlen == -1 )
	{
		wstrlen = (int)wcslen(wstr);
	}

	// WideCharToMultiByte로 한글 테스트
	//	int WideCharToMultiByte(
	//		UINT CodePage,            // 코드 페이지
	//		DWORD dwFlags,            // 성능 및 매핑 플래그
	//		LPCWSTR lpWideCharStr,    // 와이드 문자열
	//		int cchWideChar,          // 문자열 내 문자 수
	//		LPSTR lpMultiByteStr,     // 새 문자열을 담을 버퍼
	//		int cbMultiByte,          // 버퍼 크기
	//		LPCSTR lpDefaultChar,     // 매핑 불가능한 문자에 대한 기본값
	//		LPBOOL lpUsedDefaultChar  // 기본 문자가 사용됐는지 여부
	//	);
	//
	//lpDefaultChar
	//	[in] 와이드 문자가 지정된 코드 페이지로 표현될 수 없을 때 사용할 문자를 가리킨다.
	//  이 매개변수가 NULL이면 시스템 기본값이 사용된다.
	//  lpDefaultChar와 lpUsedDefaultChar가 둘 다 NULL이면 함수가 더 빠르게 동작한다.
	//	dwFlags에 언급된 코드 페이지의 경우 lpDefaultChar는 반드시 NULL이어야 하며,
	//  그렇지 않으면 함수가 ERROR_INVALID_PARAMETER로 실패한다.

	//	lpUsedDefaultChar
	//	[in] 기본 문자가 사용됐는지를 나타내는 플래그를 가리킨다.
	//  원본 문자열의 와이드 문자 중 하나 이상이 지정된 코드 페이지로 표현될 수 없으면
	//  플래그가 TRUE로 설정된다. 그렇지 않으면 FALSE로 설정된다.
	//  이 매개변수는 NULL일 수 있다. lpDefaultChar와 lpUsedDefaultChar가 둘 다 NULL이면 함수가 더 빠르게 동작한다.
	//	dwFlags에 언급된 코드 페이지의 경우 lpUsedDefaultChar는 반드시 NULL이어야 하며,
	//  그렇지 않으면 함수가 ERROR_INVALID_PARAMETER로 실패한다.


	char szTemp[5120];

	string strBuffer;
	strBuffer.reserve( wstrlen * 2 + 1 );
	int nCopied = WideCharToMultiByte(
		CP_OEMCP,
		WC_COMPOSITECHECK,
		wstr,									// 와이드 문자열
		wstrlen,								// 와이드 문자열 길이
		szTemp, //const_cast<LPSTR>(strBuffer.data()),	// mbcs 문자열(유니코드)
		5120,//(int)strBuffer.capacity(),					// mbcs 문자열 길이
		NULL,
		NULL );
	szTemp[nCopied] = NULL;


	strBuffer = szTemp;

	return strBuffer;
}

string XMLUtil::trim(const string& str)
{
	if (str.size() == 0) return "";

	static const char * WhiteSpaces = " \t\n\r";
	size_t begin = str.find_first_not_of(WhiteSpaces);
	size_t end = str.find_last_not_of(WhiteSpaces);

	if (begin == string::npos)
	{
		if (end == string::npos) return "";
		else begin = 0;
	}
	else if (end == string::npos)
	{
		end = str.size();
	}

	return str.substr(begin , end - begin + 1);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 
/// \param fmt 
/// \param ... 
//////////////////////////////////////////////////////////////////////////////
void XMLUtil::filelog(char* fmt, ...)
{
//	class ofstream file(XML_ERROR_FILENAME, ios::out | ios::app);
//	if (file.is_open())
//	{
//		va_list valist;
//		va_start(valist, fmt);
//		char message_buffer[30000] = {0, };
//		vsprintf(message_buffer, fmt, valist);
////		int nchars = _vsnprintf(message_buffer, 30000, fmt, valist);
////		if (nchars == -1 || nchars > 30000)
////		{
////			filelog(NULL, "filelog buffer overflow!");
////			throw ("filelog() : more buffer size needed for log");
////		}
//		va_end(valist);
//
//		time_t now = time(0);
//		char time_buffer[256] = {0, };
//		sprintf(time_buffer, "%s : ", ctime(&now));
//
//		file.write(time_buffer, (streamsize)strlen(time_buffer));
//		file.write(message_buffer, (streamsize)strlen(message_buffer));
//		file.write("\n", (streamsize)strlen("\n"));
//	}
}

//////////////////////////////////////////////////////////////////////////////
//
//	XMLAttribute
//
//////////////////////////////////////////////////////////////////////////////

XMLAttribute::XMLAttribute( IN const string &name, IN const string &value )
: m_Name( name ), m_Value( value )
{
}

XMLAttribute::~XMLAttribute()
{
}

OUT const char*
XMLAttribute::GetName() const
{
	return m_Name.c_str();
}

OUT const char*
XMLAttribute::ToString() const
{
	return m_Value.c_str();
}

OUT const int
XMLAttribute::ToInt() const
{
	return atoi( m_Value.c_str() );
}

OUT const DWORD
XMLAttribute::ToHex() const
{
	return strtol( m_Value.c_str(), NULL, 16 );
}

OUT const bool
XMLAttribute::ToBool() const
{
	return ( m_Value == "true" ) ? true : false;
}


//////////////////////////////////////////////////////////////////////////////
//
//	XMLTree
//
//////////////////////////////////////////////////////////////////////////////

XMLTree::XMLTree()
: m_pParent( NULL )
{
}

XMLTree::XMLTree( IN const string& name )
: m_pParent( NULL ), m_Name( name )
{
}

XMLTree::~XMLTree()
{
	Release();
}

OUT const string&
XMLTree::GetName() const
{
	return m_Name;
}

void
XMLTree::SetName( IN const string& name )
{
	m_Name = name;
}

OUT const string&
XMLTree::GetText() const
{
	return m_Text;
}

void
XMLTree::SetText( IN const string& text )
{
	m_Text = text;
}

OUT const XMLTree*
XMLTree::GetParent() const
{
	return m_pParent;
}

void
XMLTree::SetParent( IN XMLTree* pParent )
{
	m_pParent = pParent;
}

void XMLTree::AddAttribute( IN const string& name, IN const string& value )
{
	ATTRIBUTES_MAP::iterator itr = m_AttributesMap.find( name );
	
	if ( itr == m_AttributesMap.end() )
	{
		XMLAttribute *pAttr = new XMLAttribute( name, value );
		
		m_AttributesVector.push_back( pAttr );
		m_AttributesMap.insert( ATTRIBUTES_MAP::value_type( name, pAttr ) );
	}
	else
	{
		assert(false);
	}
}

OUT const XMLAttribute *
XMLTree::GetAttribute( IN const string& name ) const
{
	if(m_AttributesMap.empty() == true)
		return NULL;

	ATTRIBUTES_MAP::const_iterator itr = m_AttributesMap.find( name );

	if( itr == m_AttributesMap.end() )
		return NULL;

	return itr->second;
}

OUT XMLTree*
XMLTree::AddChild( IN const string& name )
{
	CHILDREN_MAP::iterator itr = m_ChildrenMap.find( name );

	XMLTree *pTree = new XMLTree( name );
	m_ChildrenVector.push_back( pTree );
	pTree->SetParent( this );

	if ( itr == m_ChildrenMap.end() )
	{
		m_ChildrenMap.insert( CHILDREN_MAP::value_type( name, pTree ) );
	}

	return pTree;
}

OUT XMLTree*
XMLTree::AddChild( IN XMLTree *pChild )
{
	CHILDREN_MAP::iterator itr = m_ChildrenMap.find( pChild->GetName() );

	if ( itr == m_ChildrenMap.end() )
	{
		m_ChildrenVector.push_back( pChild );
		m_ChildrenMap.insert( CHILDREN_MAP::value_type( pChild->GetName(), pChild ) );
		pChild->SetParent( this );
		
		return pChild;
	}
	else
	{
		assert(false);
	}
	
	return NULL;
}
// 2004, 7, 10 sobeit 추가 시작 -
OUT XMLTree*
XMLTree::AddChildOnlyVector( IN XMLTree *pChild )
{

	m_ChildrenVector.push_back( pChild );

	// 
	CHILDREN_MAP::iterator itr = m_ChildrenMap.find( pChild->GetName() );
	if ( itr == m_ChildrenMap.end() )
	{
		m_ChildrenMap.insert( CHILDREN_MAP::value_type( pChild->GetName(), pChild ) );
		pChild->SetParent( this );
		
		return pChild;
	}
	return pChild;
}
// 2004, 7, 10 sobeit 추가 끝

OUT const XMLTree*
XMLTree::GetChild( IN const string& name ) const
{
	CHILDREN_MAP::const_iterator itr = m_ChildrenMap.find( name );

	if ( itr == m_ChildrenMap.end() )
		return NULL;

	return itr->second;
}

OUT const XMLTree*
XMLTree::GetChild( IN size_t index ) const
{
	return ( ( index < m_ChildrenVector.size() ) ? m_ChildrenVector[index] : NULL );
}
// 2004, 7, 13 sobeit 추가 시작
OUT const XMLTree*
XMLTree::GetChildByAttr( IN size_t index , IN const string& name) const
{
	const XMLAttribute * TempAttr;
	int MaxChildCount = GetChildCount();
	for(int i = 0; i<MaxChildCount; i++)
	{
		TempAttr = m_ChildrenVector[i]->GetAttribute(name);
		if(NULL != TempAttr)
		{
			if(TempAttr->ToInt() == index)
				return m_ChildrenVector[i];
		}
	}
	return NULL;
}
// 2004, 7, 13 sobeit 추가 끝
OUT const size_t
XMLTree::GetChildCount() const
{
	return m_ChildrenVector.size();
}

void XMLTree::Release()
{
	ATTRIBUTES_VECTOR::iterator itr = m_AttributesVector.begin();
	ATTRIBUTES_VECTOR::iterator endItr = m_AttributesVector.end();

	while(itr != endItr)
	{
		delete *itr;
		itr++;
	}

	m_AttributesMap.clear();
	m_AttributesVector.clear();

	CHILDREN_VECTOR::iterator itr2 = m_ChildrenVector.begin();
	CHILDREN_VECTOR::iterator endItr2 = m_ChildrenVector.end();

	while(itr2 != endItr2)
	{
		delete *itr2;
		itr2++;
	}

	m_ChildrenMap.clear();
	m_ChildrenVector.clear();
}

void
XMLTree::Save( const char* pFilename )
{
	std::ofstream file( pFilename, ios::out | ios::trunc );

	if ( !file.is_open() ) return;

	file << "<?xml version=\"1.0\" encoding=\"euc-kr\"?>" << endl;
	
	Save(file, 0);
}

void
XMLTree::Save(std::ofstream& file, size_t indent )
{
	for ( size_t i = 0; i < indent; i++ )
		file << "\t";

	file << "<" << m_Name;

	ATTRIBUTES_VECTOR::iterator itr = m_AttributesVector.begin();
	ATTRIBUTES_VECTOR::iterator endItr = m_AttributesVector.end();

	while( itr != endItr)
	{
		file << " " << (*itr)->GetName() << "='" << (*itr)->ToString() << "'";

		itr++;
	}

	if (m_ChildrenVector.empty() == true && GetText().empty() == true)
	{
		file << "/>" << endl;
	}
	else
	{
		file << ">" << GetText();

		if(m_ChildrenVector.empty() == true)
		{
			file << "</" << m_Name << ">" << endl;
		}
		else
		{
			file << endl;

			CHILDREN_VECTOR::iterator itr = m_ChildrenVector.begin();
			CHILDREN_VECTOR::iterator endItr = m_ChildrenVector.end();

			while( itr != endItr)
			{
				(*itr)->Save( file, indent + 1 );

				itr++;
			}

			for ( size_t i = 0; i < indent; i++ )
				file << "\t";

			file << "</" << m_Name << ">" << endl;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//	XMLTreeGenerator
//
//////////////////////////////////////////////////////////////////////////////

//XMLTreeGenerator::XMLTreeGenerator( IN XMLTree* pTree )
//: m_pRoot(pTree), m_pBuffer(NULL)
//{
//}
//
//XMLTreeGenerator::~XMLTreeGenerator()
//{
//}
//
//void
//XMLTreeGenerator::startElement( IN const XMLCh* const uri, 
//									IN const XMLCh* const localname, 
//									IN const XMLCh* const qname, 
//									IN const Attributes& attrs)
//{
//	string name = XMLUtil::trim( XMLUtil::WideCharToString( localname ) );
//
//	if ( name.empty() ) return;
//
//	XMLTree* pTree = NULL;
//
//	if ( m_pBuffer == NULL )
//	{
//		m_pRoot->SetName( name );
//		
//		pTree = m_pRoot;
//	}
//	else
//	{
//		pTree = m_pBuffer->AddChild( name );
//	}
//
//	for ( unsigned int i = 0; i < attrs.getLength(); i++ ) 
//	{
//		pTree->AddAttribute(
//			XMLUtil::trim( XMLUtil::WideCharToString( attrs.getLocalName( i ) ) ),
//			XMLUtil::trim( XMLUtil::WideCharToString( attrs.getValue( i ) ) ) );
//	}
//
//	m_pBuffer = pTree;
//}
//
//void
//XMLTreeGenerator::endElement(	IN const XMLCh* const uri, 
//									IN const XMLCh* const localname, 
//									IN const XMLCh* const qname )
//{
//	assert( m_pBuffer != NULL );
//
//	m_pBuffer = const_cast<XMLTree *>(m_pBuffer->GetParent());
//}
//
//void
//XMLTreeGenerator::characters( IN const XMLCh* const chars, 
//								IN const unsigned int length )
//{
//	assert( m_pBuffer != NULL );
//
//	string text = XMLUtil::trim( XMLUtil::WideCharToString( chars ) );
//	m_pBuffer->SetText( m_pBuffer->GetText() + text );
//}
//
////////////////////////////////////////////////////////////////////////////////
///// \brief 
////////////////////////////////////////////////////////////////////////////////
//void XMLTreeGenerator::resetDocument()
//{
//	m_pRoot->Release();
//}
//
////////////////////////////////////////////////////////////////////////////////
///// \brief 
///// \param e 
////////////////////////////////////////////////////////////////////////////////
//void XMLTreeGenerator::warning(const SAXParseException& e)
//{
//	XMLUtil::filelog(
//		"\nWarning at FILE %s, LINE %d, CHAR %d, \nMessage: %s\n",
//		XMLUtil::WideCharToString(e.getSystemId()).c_str(), 
//		e.getLineNumber(),
//		e.getColumnNumber(),
//		XMLUtil::WideCharToString(e.getMessage()));
//
//	assert(false);
//}
//
////////////////////////////////////////////////////////////////////////////////
///// \brief 
///// \param e 
////////////////////////////////////////////////////////////////////////////////
//void XMLTreeGenerator::error(const SAXParseException& e)
//{
//	XMLUtil::filelog(
//		"\nError at FILE %s, LINE %d, CHAR %d, \nMessage: %s\n",
//		XMLUtil::WideCharToString(e.getSystemId()).c_str(), 
//		e.getLineNumber(),
//		e.getColumnNumber(),
//		XMLUtil::WideCharToString(e.getMessage()));
//
//	assert(false);
//}
//
////////////////////////////////////////////////////////////////////////////////
///// \brief 
///// \param e 
////////////////////////////////////////////////////////////////////////////////
//void XMLTreeGenerator::fatalError(const SAXParseException& e)
//{
//	const char *pStr = XMLUtil::WideCharToString(e.getSystemId()).c_str();
//	int l = e.getLineNumber();
//	int c = e.getColumnNumber();
//	const char *pMessage = XMLUtil::WideCharToString(e.getMessage()).c_str();
//	
//	XMLUtil::filelog(
//		"\nFatal error at FILE %s, LINE %d, CHAR %d, \nMessage: %s\n",
//		XMLUtil::WideCharToString(e.getSystemId()).c_str(), 
//		e.getLineNumber(),
//		e.getColumnNumber(),
//		XMLUtil::WideCharToString(e.getMessage()).c_str());
//
//	assert(false);
//}


//////////////////////////////////////////////////////////////////////////////
/// \brief 
/// \param pTree 
//////////////////////////////////////////////////////////////////////////////
XMLParser::XMLParser()
{
//	try // XML4C2 시스템 초기화
//	{
//		XMLPlatformUtils::Initialize();
//	}
//	catch (const XMLException& toCatch)
//	{
//		XMLUtil::filelog(
//			"Error during initialization! Message:%s\n",
//			XMLUtil::WideCharToString(toCatch.getMessage()));
//	}

//	m_pHandler = new XMLTreeGenerator(pTree);
}

//////////////////////////////////////////////////////////////////////////////
/// \brief 
//////////////////////////////////////////////////////////////////////////////
XMLParser::~XMLParser()
{
//	if (m_pHandler != NULL) delete m_pHandler;

	// 그리고 종료 메서드를 호출한다
//	XMLPlatformUtils::Terminate();
}

//////////////////////////////////////////////////////////////////////////////

/// 
/// \param pURL 
//////////////////////////////////////////////////////////////////////////////
//void XMLParser::parseURL(const char* pURL)
//{
//	assert(pURL != NULL);
//	assert(m_pHandler != NULL);
//


//	//
//	// validation (기본값: true)
//	// namespaces (기본값: true)
//	// namespace-prefixes (기본값: false)
//	// validation/dynamic (기본값: false)
//	// reuse-grammar (기본값: false)
//	// schema (기본값: true)
//	// schema-full-checking (기본값: false)
//	// load-external-dtd (기본값: true)
//	// continue-after-fatal-error (기본값: false)
//	// validation-error-as-fatal (기본값: false)
//	//

//	// http://xml.apache.org/xerces-c/program-sax2.html#SAX2Features
//	SAX2XMLReader* pParser = XMLReaderFactory::createXMLReader();
//	pParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
//	pParser->setFeature(XMLUni::fgXercesSchema, true);
//	pParser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
//	pParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);
//	pParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
//	pParser->setFeature(XMLUni::fgXercesDynamic, true);
//	pParser->setContentHandler(m_pHandler);
//	pParser->setErrorHandler(m_pHandler);
//
//	try
//	{
//		pParser->parse(pURL);
//	}
//	catch (const XMLException& e)
//	{
//		XMLUtil::filelog(
//			"\nError during parsing! Exception Message: \n%s\n",
//			XMLUtil::WideCharToString(e.getMessage()));
//	}
//	catch (...)
//	{
//		XMLUtil::filelog(
//			"Unexpected exception during parsing!\n");
//	}
//
//	delete pParser;
//}

//////////////////////////////////////////////////////////////////////////////

/// 
/// \param buffer 
//////////////////////////////////////////////////////////////////////////////
char* XMLParser::parse(char* buffer, XMLTree *pTree, bool IsUseOnlyVector)
{
	assert(buffer != NULL && pTree != NULL);
//	assert(m_pHandler != NULL);
	char name[1024];
	char name2[1024];

	char *pXml = strchr( buffer, chXMLTagOpen );
	if( pXml == NULL )
		return NULL;

	if( memcmp( pXml, "<?xml", 5 ) == 0 )
		pXml = strchr( pXml+1, chXMLTagOpen );
		
//	// 닫는 태그
//	if( *(pXml+1) == chXMLTagPre ) // </닫음
//		return;

	// XML 노드 태그 이름 여는 부분
	pXml = pXml + strspn( pXml, chXMLTrimToken );
	size_t nameLen = strcspn( pXml, chXMLTrimToken );
	memcpy( name, pXml, nameLen );
	name[nameLen] = '\0';
	pTree->SetName( name );
	pXml += nameLen;

	// 속성 로드
	char *pCloseTag = strchr( pXml, chXMLTagClose );
	while( 1 )
	{
		pXml = pXml + strspn( pXml, chXMLTrimToken );
		
		if( pXml >= pCloseTag )
			break;

		size_t nameLen = strcspn( pXml, chXMLTrimToken );
		memcpy( name, pXml, nameLen );
		name[nameLen] = '\0';
		pXml += nameLen;


		pXml = pXml + strspn( pXml, chXMLTrimToken );
		//pXml = pXml + strspn( pXml, chXMLAttrToken )+1;

		size_t nameLen2 = strcspn( pXml, chXMLAttrToken );
		memcpy( name2, pXml, nameLen2 );
		name2[nameLen2] = '\0';
		pXml += nameLen2;
		pTree->AddAttribute( name, name2 );
	}
	
	pXml = pCloseTag;

	if( *(pCloseTag-1) == chXMLTagPre )
		return pXml;

	char *pChildTag = strchr( pXml, chXMLTagOpen );

	char *pTextFirst = pXml + strspn( pXml, chXMLTrimToken );
	if( pTextFirst < pChildTag )
	{
		nameLen = pChildTag - pTextFirst;

		while( nameLen > 0 )
		{
			if( pTextFirst[nameLen-1] != '\t' && pTextFirst[nameLen-1] != '\n' && pTextFirst[nameLen-1] != '\r' && pTextFirst[nameLen-1] != ' ')
				break;

			nameLen--;
		}
		if( nameLen > 0 )
		{
			memcpy( name, pTextFirst, nameLen );
			name[nameLen] = '\0';
			pTree->SetText( name );
		}
	}
	
	pXml = pChildTag;	
	
	// 자식 노드 로드
	while(*(pChildTag+1) != chXMLTagPre)
	{
		XMLTree *pChildTree = new XMLTree;
		pXml = parse( pChildTag, pChildTree, IsUseOnlyVector );
		// 2004, 7, 9, sobeit 추가 시작
		if(IsUseOnlyVector)
		{
			pTree->AddChildOnlyVector( pChildTree );
		}
		else
		{
		// 2004, 7, 9, sobeit 추가 끝
			pTree->AddChild( pChildTree );
		}
		pChildTag = strchr( pXml, chXMLTagOpen );
	}

	return strchr(pChildTag+1, chXMLTagOpen);



//	SAX2XMLReader* pParser = XMLReaderFactory::createXMLReader();
//	pParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
//	pParser->setFeature(XMLUni::fgXercesSchema, true);
//	pParser->setFeature(XMLUni::fgXercesSchemaFullChecking, false);
//	pParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, false);
//	pParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
//	pParser->setFeature(XMLUni::fgXercesDynamic, true);
//	pParser->setContentHandler(m_pHandler);
//	pParser->setErrorHandler(m_pHandler);
//
//	try
//	{
//		MemBufInputSource source(
//			(const XMLByte*)(buffer), (unsigned int)strlen(buffer), "", false);
//		pParser->parse(source);
//	}
//	catch (const XMLException& e)
//	{
//		XMLUtil::filelog(
//			"\nError during parsing! Exception Message: \n%s\n",
//			XMLUtil::WideCharToString(e.getMessage()));
//	}
//	catch (...)
//	{
//		XMLUtil::filelog(
//			"Unexpected exception during parsing!\n");
//	}
//
//	delete pParser;
}
