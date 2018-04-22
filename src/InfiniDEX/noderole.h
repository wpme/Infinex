// Copyright (c) 2017-2018 The Infinex Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NODEROLE_H
#define NODEROLE_H

#include <map>
#include <vector>
#include "net.h"
#include "utilstrencodings.h"

enum infinidex_node_role_enum {
	INFINIDEX_NOTHING = 0,
	INFINIDEX_USER = 1,
	INFINIDEX_BID_BOOK_BROADCAST = 2,
	INFINIDEX_ASK_BOOK_BROADCAST = 3,
	INFINIDEX_USER_HISTORY_PROVIDER = 4,
	INFINIDEX_MARKET_HISTORY_PROVIDER = 5,
	INFINIDEX_CHART_DATA_PROVIDER = 6,
	INFINIDEX_TRADE_PROCESSOR = 7,
	INFINIDEX_WALLET_ADDRESS = 8,
	INFINIDEX_BALANCE_HANDLER = 9,
	INFINIDEX_WITHDRAW_INFO = 10,	
	INFINIDEX_DEPOSIT_INFO = 11,
	INFINIDEX_TRUSTED_NODE = 12,
	INFINIDEX_MARKET_OVERVIEW_PROCESSOR = 13,
	INFINIDEX_MARKET_OVERVIEW_PROVIDER = 14
};

class CNodeRole;
class CNodeRoleManager;
class CPendingProcess;

typedef std::map<int, std::shared_ptr<CNodeRole>> NodeRoleWithID;
typedef std::map<int, NodeRoleWithID> mapGlobalNodeRolesByRole;
extern std::map<int, mapGlobalNodeRolesByRole> mapGlobalNodeRolesByTradePairID;
extern std::map<char, mapGlobalNodeRolesByRole> mapGlobalNodeRolesByChar;
extern NodeRoleWithID mapGlobalNodeRoles;
extern std::vector<CNodeRole> completeNodeRoles;
extern std::map<int, NodeRoleWithID> mapLocalNodeRoles;
extern CNodeRoleManager nodeRoleManager;
extern std::string DEXKey;
extern std::string dexMasterPrivKey;
extern std::string MNPubKey;
extern CPendingProcess pendingProcessStatus;

class CPendingProcess
{
public:
	bool UserTrade;
	bool CancelTrade;
	
	CPendingProcess() :
		UserTrade(false),
		CancelTrade(false)
	{}
};

class CNodeRole
{
private:
	std::vector<unsigned char> vchSig;

public:
	int NodeRoleID;
	int TradePairID;
	int CoinID;
	char Char;
	int NodeRole;
	std::string NodeIP;
	std::string NodePubKey;
	uint64_t StartTime;
	uint64_t EndTime;
	bool IsActive;
	bool IsBackup;
	int ToReplaceNodeRoleID;
	uint64_t LastUpdateTime;

	CNodeRole() :
		NodeRoleID(0),
		TradePairID(0),
		CoinID(0),
		Char(),
		NodeRole(INFINIDEX_NOTHING),
		NodeIP(""),
		NodePubKey(""),
		StartTime(0),
		EndTime(0),
		IsActive(false),
		IsBackup(false),
		ToReplaceNodeRoleID(0),
		LastUpdateTime(0)
	{}

	ADD_SERIALIZE_METHODS;
	template <typename Stream, typename Operation>
	inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
		READWRITE(NodeRoleID);
		READWRITE(TradePairID);
		READWRITE(CoinID);
		READWRITE(Char);
		READWRITE(NodeRole);
		READWRITE(NodeIP);
		READWRITE(NodePubKey);
		READWRITE(StartTime);
		READWRITE(EndTime);
		READWRITE(IsActive);
		READWRITE(IsBackup);
		READWRITE(ToReplaceNodeRoleID);
		READWRITE(LastUpdateTime);
		READWRITE(vchSig);
	}

	bool VerifySignature();
	bool DEXSign(std::string dexSignKey);
	void Broadcast(CConnman& connman);
};

class CNodeRoleManager
{
public:
	void InitialInit();
	bool SetDEXPrivKey(std::string dexPrivKey);
	bool IsValidInChargeOfUserTrade(std::string MNPubkey, uint64_t time, int TradePairID);
	bool IsValidInChargeOfUserBalance(std::string MNPubKey, uint64_t time, std::string UserPubKey);
};

#endif