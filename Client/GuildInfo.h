//////////////////////////////////////////////////////////////////////
// 
// Filename    : GuildInfo.h 
// Written By  : 
// Description :
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GUILD_INFO_H__
#define __GUILD_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class GuildInfo;
//
// 클라이언트에 길드 리스트를 보낸다.
//
//////////////////////////////////////////////////////////////////////

class GuildInfo {

public :
	
	// constructor
	GuildInfo () throw ();
	
	// destructor
	~GuildInfo () throw ();

public :
	
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	// 패킷의 바디 크기를 얻는다.
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () throw ();

	static uint getMaxSize() throw() {
		//return ( szGuildID + szBYTE + 30 + szBYTE + 20 + szBYTE + szBYTE + 11 ) * 256 + szBYTE;
		return szGuildID + szBYTE + 30 + szBYTE + 20 + szBYTE + szBYTE + 11;
	}

	// 패킷 디버그 문자열을 얻는다.
	std::string toString () const throw ();

	// GuildID get/set
	GuildID_t getGuildID() const throw() { return m_GuildID; }
	void setGuildID( GuildID_t GuildID ) throw() { m_GuildID = GuildID; }

	// 길드 이름 get/set
	std::string getGuildName() const throw() { return m_GuildName; }
	void setGuildName( const std::string& GuildName ) throw() { m_GuildName = GuildName; }

	// 길드 마스터 get/set
	std::string getGuildMaster() const throw() { return m_GuildMaster; }
	void setGuildMaster( const std::string& GuildMaster ) throw() { m_GuildMaster = GuildMaster; }

	// 길드 멤버 카운트 get/set
	BYTE getGuildMemberCount() const throw() { return m_GuildMemberCount; }
	void setGuildMemberCount( BYTE GuildMemberCount ) throw() { m_GuildMemberCount = GuildMemberCount; }

	// 길드 만료일 get/set
	std::string getGuildExpireDate() const throw() { return m_GuildExpireDate; }
	void setGuildExpireDate( const std::string& GuildExpireDate ) throw() { m_GuildExpireDate = GuildExpireDate; }


private :

	// 길드 아이디
	GuildID_t m_GuildID;

	// 길드 이름
	std::string m_GuildName;

	// 길드 마스터
	std::string m_GuildMaster;

	// 길드 멤버 카운트
	BYTE m_GuildMemberCount;

	// 길드 Expire Date
	std::string m_GuildExpireDate;

};

#endif
