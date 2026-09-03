//////////////////////////////////////////////////////////////////////
// 
// 파일명: GuildMemberInfo.h 
// 작성자: 
// 설명:
// 
//////////////////////////////////////////////////////////////////////

#ifndef __GUILD_MEMBER_INFO_H__
#define __GUILD_MEMBER_INFO_H__

// include files
#include "Types.h"
#include "Exception.h"
#include "Packet.h"

//////////////////////////////////////////////////////////////////////
//
// class GuildMemberInfo;
//
// 클라이언트에 길드멤버 리스트를 보낸다.
//
//////////////////////////////////////////////////////////////////////

class GuildMemberInfo {

public :
	
	// constructor
	GuildMemberInfo () throw ();
	
	// destructor
	~GuildMemberInfo () throw ();

public :
	
	
    // 입력스트림(버퍼)으로부터 데이타를 읽어서 패킷을 초기화한다.
    void read (SocketInputStream & iStream) throw (ProtocolException, Error);
		    
    // 출력스트림(버퍼)으로 패킷의 바이너리 이미지를 보낸다.
    void write (SocketOutputStream & oStream) const throw (ProtocolException, Error);

	// 패킷의 바디 크기를 얻는다.
	// 최적화시, 미리 계산된 정수를 사용한다.
	PacketSize_t getSize () throw ();

	static uint getMaxSize() throw() {
		return ( szBYTE + 20 + szGuildMemberRank + szbool) * 220 + szBYTE + szServerID;
	}

	// 패킷 디버그 문자열을 얻는다.
	string toString () const throw ();

	// 이름 get/set
	string getName() const throw() { return m_Name; }
	void setName( const string& Name ) throw() { m_Name = Name; }

	// 길드 멤버 랭크 get/set
	GuildMemberRank_t getRank() const throw() { return m_Rank; }
	void setRank( GuildMemberRank_t rank ) throw() { m_Rank = rank; }

	// 길드 멤버 로그온 상태 get/set
	bool getLogOn() const throw() { return m_bLogOn; }
	void setLogOn( bool logOn ) throw() { m_bLogOn = logOn; }

	// 길드 멤버 존(현재 위치) get/set
	ServerID_t	getServerID()	const throw() { return m_ServerID; }
	void		setServerID( ServerID_t	ServerID)	throw() { m_ServerID = ServerID; }

private :

	// 이름
	string m_Name;

	// 길드 멤버 랭크
	GuildMemberRank_t m_Rank;


	// 길드 멤버 로그온 상태
	bool m_bLogOn;

	// 길드 멤버 존(현재 위치)
	ServerID_t	m_ServerID;

};

#endif
