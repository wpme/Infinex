// Copyright (c) 2017-2018 The Infinex Core developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef TRADE_H
#define TRADE_H

#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "tradepair.h"
#include "userconnection.h"

class CUserTrade;
class CUserTradeSetting;
class CUserTradeManager;

class CActualTrade;
class CActualTradeSetting;
class CActualTradeManager;

typedef std::map<int, std::shared_ptr<CUserTrade>> mUTIUT; //user trade id and trade details
typedef std::map<uint64_t, mUTIUT> mUTPIUTV; //price and user trade container
typedef std::map<std::string, mUTIUT> mUTPKUTV; //user public key and user trade container
extern std::map<int, mUTPIUTV> mapBidUserTradeByPrice;
extern std::map<int, mUTPIUTV> mapAskUserTradeByPrice;
extern std::map<int, mUTPKUTV> mapBidUserTradeByPubkey;
extern std::map<int, mUTPKUTV> mapAskUserTradeByPubkey;
extern std::map<int, CUserTradeSetting> mapUserTradeSetting;
extern std::map<int, std::set<std::string>> mapUserTradeHash;

extern CUserTradeManager userTradeManager;

typedef std::map<std::shared_ptr<int>, std::shared_ptr<CActualTrade>> mATIAT;
typedef std::map<std::shared_ptr<int>, mATIAT> mUTIATIAT;
typedef std::map<std::shared_ptr<std::string>, mATIAT> mUPKATIAT;
typedef std::pair<mUTIATIAT, mUPKATIAT> pUTIUPKATIAT;
typedef std::pair<mATIAT, pUTIUPKATIAT> pActualTrade;
typedef std::pair<CActualTradeSetting, std::set<std::string>> pairSettingSecurity; //trade pair setting & trade hash record
typedef std::pair<pairSettingSecurity, pActualTrade> ActualTradeContainer;
extern std::map<int, ActualTradeContainer> mapActualTrade;
extern std::map<int, std::vector<CActualTrade>> mapConflicTrade;
extern CActualTradeManager actualTradeManager;

class CUserTradeSetting
{
public:
	int nTradePairID;
	uint32_t nMaxSubmissionTimeDiff;
	uint16_t nToStoreLowerLimit;
	uint16_t nToStoreUpperLimit;
	int nLastUserTradeID;
	uint64_t nLastUserTradeTime;
	std::string nMNPubKey;
	bool nSyncInProgress;
	bool nIsInChargeOfProcessUserTrade;
	bool nIsInChargeOfMatchUserTrade;

	CUserTradeSetting() :
		nTradePairID(0),
		nMaxSubmissionTimeDiff(3000),
		nToStoreLowerLimit(50),
		nToStoreUpperLimit(100),
		nLastUserTradeID(0),
		nMNPubKey(""),
		nSyncInProgress(false),
		nIsInChargeOfProcessUserTrade(false),
		nIsInChargeOfMatchUserTrade(false)
	{}

	bool IsValidSubmissionTimeAndUpdate(uint64_t time);
};

class CUserTrade
{
private:
	std::vector<unsigned char> userVchSig;
	std::vector<unsigned char> mnVchSig;

public:
	int nTradePairID;
	uint64_t nPrice;
	uint64_t nQuantity;
	uint64_t nAmount;
	int nTradeFee; //this is here to compare with trade pair fee & apply to whichever lower fee (benefit user)	
	std::string nUserPubKey;
	uint64_t nTimeSubmit;
	std::string nUserHash;
	int nUserTradeID; //generated by MN
	std::string nMNPubKey;
	int64_t nBalanceQty;
	int64_t nBalanceAmount;
	uint64_t nLastUpdate;

	CUserTrade(int nTradePairID, uint64_t nPrice, uint64_t nQuantity, int nTradeFee, std::string nUserPubKey, uint64_t nTimeSubmit, std::string nUserHash) :
		nTradePairID(nTradePairID),
		nPrice(nPrice),
		nQuantity(nQuantity),
		nAmount(nPrice * nQuantity),
		nTradeFee(nTradeFee),
		nUserPubKey(nUserPubKey),
		nTimeSubmit(nTimeSubmit),
		nUserHash(nUserHash),
		nUserTradeID(0),
		nMNPubKey(""),
		nBalanceQty(nQuantity),
		nBalanceAmount(nAmount),
		nLastUpdate(0)
	{}

	CUserTrade(int nTradePairID, uint64_t nPrice, uint64_t nQuantity, uint64_t nAmount, int nTradeFee, std::string nUserPubKey, uint64_t nTimeSubmit, std::string nUserHash, int nUserTradeID, std::string nMNPubKey, int64_t nBalanceQty, int64_t nBalanceAmount, uint64_t nLastUpdate) :
		nTradePairID(nTradePairID),
		nPrice(nPrice),
		nQuantity(nQuantity),
		nAmount(nAmount),
		nTradeFee(nTradeFee),
		nUserPubKey(nUserPubKey),
		nTimeSubmit(nTimeSubmit),
		nUserHash(nUserHash),
		nUserTradeID(nUserTradeID),
		nMNPubKey(nMNPubKey),
		nBalanceQty(nBalanceQty),
		nBalanceAmount(nBalanceAmount),
		nLastUpdate(nLastUpdate)
	{}

	CUserTrade() :
		nTradePairID(0),
		nPrice(0),
		nQuantity(0),
		nAmount(0),
		nTradeFee(0),
		nUserPubKey(""),
		nTimeSubmit(0),
		nUserHash(""),
		nUserTradeID(0),
		nMNPubKey(""),
		nBalanceQty(0),
		nBalanceAmount(0),
		nLastUpdate(0)
	{}

	bool VerifyUserSignature();
	bool VerifyMNSignature();
	bool MNSign();
	void RelayTo(CNode* node, CConnman& connman);
	void RelayToHandler(CConnman& connman);
};

class CUserTradeManager
{
private:
	std::vector<unsigned char> vchSig;
	uint64_t GetAdjustedTime();

public:
	bool IsTradePairInList(int TradePairID);
	bool IsSyncInProgress(int TradePairID);
	bool IsInChargeOfProcessUserTrade(int TradePairID);
	bool IsInChargeOfMatchUserTrade(int TradePairID);
	bool IsUserTradeInList(int TradePairID, std::string UserHash);
	void AddToUserTradeList(int TradePairID, std::string UserHash);
	bool IsProcessedUserTradeInList(CUserTrade UserTrade);
	int IsProcessedUserTradeInSequence(CUserTrade UserTrade);
	bool IsSubmittedBidValid(CUserTrade UserTrade, CTradePair TradePair);
	bool IsSubmittedAskValid(CUserTrade UserTrade, CTradePair TradePair);
	bool IsSubmittedBidAmountValid(CUserTrade userTrade, int nTradeFee);
	bool IsSubmittedAskAmountValid(CUserTrade userTrade, int nTradeFee);
	void UserSellRequest(CUserTrade userTrade);
	void UserBuyRequest(CUserTrade userTrade);
	void ProcessUserBuyRequest(CConnman& connman, CUserTrade& userTrade);
	void ProcessUserSellRequest(CConnman& connman, CUserTrade& userTrade);
	void InputMatchUserBuyRequest(CUserTrade userTrade);
	void InputMatchUserSellRequest(CUserTrade userTrade);
	uint64_t GetBidRequiredAmount(uint64_t Price, uint64_t Qty, int TradeFee);
	uint64_t GetAskExpectedAmount(uint64_t Price, uint64_t Qty, int TradeFee);
};

class CActualTradeSetting
{
public:
	int nTradePairID;
	bool nSecurityCheck;
	bool nSyncInProgress;
	uint32_t nLastTradeMaxPreTimeDistance;
	uint16_t nToStoreLowerLimit;
	uint16_t nToStoreUpperLimit;
	int nLastActualTradeID;
	std::string nLastHash;

	CActualTradeSetting(int nTradePairID, bool nSecurityCheck) :
		nTradePairID(nTradePairID),
		nSecurityCheck(nSecurityCheck),
		nSyncInProgress(false),
		nLastTradeMaxPreTimeDistance(3000),
		nToStoreLowerLimit(50),
		nToStoreUpperLimit(100),
		nLastActualTradeID(0),
		nLastHash("")
	{}

	bool GetSecurityCheck() { return nSecurityCheck; }
};

class CActualTrade
{
private:
	std::vector<unsigned char> vchSig;

public:
	int nActualTradeID;
	int nTradePairID;
	uint64_t nTradePrice;
	uint64_t nTradeQty;
	uint64_t nTradeAmount;
	std::string nUserPubKey1;
	std::string nUserPubKey2;
	int64_t nFee1; //during promo period, we can provide rebate instead of trade fee to user
	int nFee1CoinID;
	int64_t nFee2; //during promo period, we can provide rebate instead of trade fee to user
	int nFee2CoinID;
	std::string nMasternodeInspector;
	std::string nCurrentHash;
	uint64_t nTradeTime;

	CActualTrade(int nActualTradeID) :
		nActualTradeID(nActualTradeID)
	{}

	CActualTrade(std::string nUserPubKey1, std::string nUserPubKey2, int nTradePairID, uint64_t nTradePrice, uint64_t nTradeQty,
		uint64_t nTradeAmount, int64_t nFee1, int nFee1CoinID, int64_t nFee2, int nFee2CoinID,
		std::string nMasternodeInspector, uint64_t nTradeTime) :
		nUserPubKey1(nUserPubKey1),
		nUserPubKey2(nUserPubKey2),
		nTradePairID(nTradePairID),
		nTradePrice(nTradePrice),
		nTradeQty(nTradeQty),
		nTradeAmount(nTradeAmount),
		nFee1(nFee1),
		nFee1CoinID(nFee1CoinID),
		nFee2(nFee2),
		nFee2CoinID(nFee2CoinID),
		nMasternodeInspector(nMasternodeInspector),
		nTradeTime(nTradeTime)
	{}

	CActualTrade() :
		nUserPubKey1(""),
		nUserPubKey2(""),
		nTradePairID(0),
		nTradePrice(0),
		nTradeQty(0),
		nTradeAmount(0),
		nFee1(0),
		nFee1CoinID(0),
		nFee2(0),
		nFee2CoinID(0),
		nMasternodeInspector(""),
		nTradeTime(0)
	{}

	std::string GetHash();
	bool CheckSignature();
	bool Sign();
	bool InformActualTrade();
	bool InformConflictTrade(CNode* node);
};

class CActualTradeManager
{
private:
	bool RunSecurityCheck(int TradePairID, std::string Hash);

public:
	CActualTradeManager() {}
	bool SetSecurityCheck(int TradePairID, bool SecurityCheck);
	bool GetActualTrade(CNode* node, int ActualTradeID, int TradePairID);
	bool AddNewActualTrade(CActualTrade ActualTrade); //process by same node
	bool AddNewActualTrade(CNode* node, CActualTrade ActualTrade); //data from other node
	std::vector<std::string> FindDuplicateTrade(int TradePairID);
	void InitiateCompleteResync(int TradePairID);
	void InitialSync(int TradePairID);
	void InputNewTradePair(int TradePairID, bool SecurityCheck);
};

#endif