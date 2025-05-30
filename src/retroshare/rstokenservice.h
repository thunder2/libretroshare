/*******************************************************************************
 * libretroshare/src/retroshare: rstokenservice.h                              *
 *                                                                             *
 * libretroshare: retroshare core library                                      *
 *                                                                             *
 * Copyright (C) 2012  Chris Evi-Parker                                        *
 * Copyright (C) 2012  Robert Fernie <retroshare@lunamutt.com>                 *
 * Copyright (C) 2018-2019  Gioacchino Mazzurco <gio@eigenlab.org>             *
 *                                                                             *
 * This program is free software: you can redistribute it and/or modify        *
 * it under the terms of the GNU Lesser General Public License as              *
 * published by the Free Software Foundation, either version 3 of the          *
 * License, or (at your option) any later version.                             *
 *                                                                             *
 * This program is distributed in the hope that it will be useful,             *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                *
 * GNU Lesser General Public License for more details.                         *
 *                                                                             *
 * You should have received a copy of the GNU Lesser General Public License    *
 * along with this program. If not, see <https://www.gnu.org/licenses/>.       *
 *                                                                             *
 *******************************************************************************/
#pragma once

#include <inttypes.h>
#include <string>
#include <list>

#include "retroshare/rsgxsifacetypes.h"
#include "util/rsdeprecate.h"
#include "util/rsdebug.h"

// TODO CLEANUP: GXS_REQUEST_TYPE_* should be an inner enum of RsTokReqOptions
#define GXS_REQUEST_TYPE_GROUP_DATA			0x00010000
#define GXS_REQUEST_TYPE_GROUP_META			0x00020000
#define GXS_REQUEST_TYPE_GROUP_IDS			0x00040000
#define GXS_REQUEST_TYPE_MSG_DATA			0x00080000
#define GXS_REQUEST_TYPE_MSG_META			0x00100000
#define GXS_REQUEST_TYPE_MSG_IDS 			0x00200000

#define GXS_REQUEST_TYPE_MSG_RELATED_DATA		0x00400000
#define GXS_REQUEST_TYPE_MSG_RELATED_META		0x00800000
#define GXS_REQUEST_TYPE_MSG_RELATED_IDS 		0x01000000

#define GXS_REQUEST_TYPE_GROUP_STATS            0x01600000
#define GXS_REQUEST_TYPE_SERVICE_STATS          0x03200000
#define GXS_REQUEST_TYPE_GROUP_SERIALIZED_DATA	0x04000000


// TODO CLEANUP: RS_TOKREQOPT_MSG_* should be an inner enum of RsTokReqOptions
#define RS_TOKREQOPT_MSG_VERSIONS	0x0001		// MSGRELATED: Returns All MsgIds with OrigMsgId = MsgId.
#define RS_TOKREQOPT_MSG_ORIGMSG	0x0002		// MSGLIST: All Unique OrigMsgIds in a Group.
#define RS_TOKREQOPT_MSG_LATEST		0x0004		// MSGLIST: All Latest MsgIds in Group. MSGRELATED: Latest MsgIds for Input Msgs.
#define RS_TOKREQOPT_MSG_THREAD		0x0010		// MSGRELATED: All Msgs in Thread. MSGLIST: All Unique Thread Ids in Group.
#define RS_TOKREQOPT_MSG_PARENT		0x0020		// MSGRELATED: All Children Msgs.
#define RS_TOKREQOPT_MSG_AUTHOR		0x0040		// MSGLIST: Messages from this AuthorId


/* TODO CLEANUP: RS_TOKREQ_ANSTYPE_* values are meaningless and not used by
 * RsTokenService or its implementation, and may be arbitrarly defined by each
 * GXS client as they are of no usage, their use is deprecated, up until the
 * definitive cleanup is done new code must use RS_DEPRECATED_TOKREQ_ANSTYPE for
 * easier cleanup. */
#ifndef RS_NO_WARN_DEPRECATED
#	warning RS_TOKREQ_ANSTYPE_* macros are deprecated!
#endif
#define RS_DEPRECATED_TOKREQ_ANSTYPE 0x0000
#define RS_TOKREQ_ANSTYPE_LIST       0x0001
#define RS_TOKREQ_ANSTYPE_SUMMARY    0x0002
#define RS_TOKREQ_ANSTYPE_DATA       0x0003
#define RS_TOKREQ_ANSTYPE_ACK        0x0004


/*!
 * This class provides useful generic support for GXS style services.
 * I expect much of this will be incorporated into the base GXS.
 */
struct RsTokReqOptions
{
	RsTokReqOptions() : mOptions(0), mStatusFilter(0), mStatusMask(0),
	    mMsgFlagMask(0), mMsgFlagFilter(0), mReqType(0), mSubscribeFilter(0),
	    mSubscribeMask(0), mBefore(0), mAfter(0),mPriority(GxsRequestPriority::NORMAL) {}

	/**
	 * Can be one or multiple RS_TOKREQOPT_*
	 * TODO: cleanup this should be made with proper flags instead of macros
	 */
	uint32_t mOptions;

	// Request specific matches with Group / Message Status.
	// Should be usable with any Options... applied afterwards.
	uint32_t mStatusFilter;
	uint32_t mStatusMask;

	// use
	uint32_t mMsgFlagMask, mMsgFlagFilter;

	/**
	 * Must be one of GXS_REQUEST_TYPE_*
	 * TODO: cleanup this should be made an enum instead of macros
	 */
	uint32_t mReqType;

	uint32_t mSubscribeFilter, mSubscribeMask; // Only for Groups.

	// Time range... again applied after Options.
	rstime_t   mBefore;
	rstime_t   mAfter;

    GxsRequestPriority mPriority;
};

/*!
 * A proxy class for requesting generic service data for GXS
 * This seperates the request mechanism from the actual retrieval of data
 */
class RsTokenService
{
public:

	enum GxsRequestStatus : uint8_t
	{
		FAILED    = 0,
		PENDING   = 1,
		PARTIAL   = 2,
		COMPLETE  = 3,
		DONE      = 4, /// Once all data has been retrived
        CANCELLED = 5,
        TO_REMOVE = 6
    };

	RsTokenService() {}
	virtual ~RsTokenService() {}

    /* Data Requests */

    /*!
     * Use this to request group related information
	 * @param token The token returned for the request, store this value to poll for request completion
     * @param ansType The type of result (e.g. group data, meta, ids)
     * @param opts Additional option that affect outcome of request. Please see specific services, for valid values
     * @param groupIds group id to request info for
     * @return
     */
    virtual bool requestGroupInfo(uint32_t &token, uint32_t ansType, const RsTokReqOptions &opts, const std::list<RsGxsGroupId> &groupIds) = 0;

    /*!
     * Use this to request all group related info
	 * @param token The token returned for the request, store this value to poll for request completion
     * @param ansType The type of result (e.g. group data, meta, ids)
     * @param opts Additional option that affect outcome of request. Please see specific services, for valid values
     * @return
     */
    virtual bool requestGroupInfo(uint32_t &token, uint32_t ansType, const RsTokReqOptions &opts) = 0;

    /*!
	 * Use this to get msg related information, store this value to poll for request completion
     * @param token The token returned for the request
     * @param ansType The type of result wanted
     * @param opts Additional option that affect outcome of request. Please see specific services, for valid values
     * @param groupIds The ids of the groups to get, second entry of map empty to query for all msgs
     * @return true if request successful false otherwise
     */
    virtual bool requestMsgInfo(uint32_t &token, uint32_t ansType, const RsTokReqOptions &opts, const GxsMsgReq& msgIds) = 0;

    /*!
	 * Use this to get msg related information, store this value to poll for request completion
     * @param token The token returned for the request
     * @param ansType The type of result wanted
     * @param opts Additional option that affect outcome of request. Please see specific services, for valid values
     * @param groupIds The ids of the groups to get, this retrieves all the msgs info for each grpId in list
     * @return true if request successful false otherwise
     */
    virtual bool requestMsgInfo(uint32_t &token, uint32_t ansType, const RsTokReqOptions &opts, const std::list<RsGxsGroupId>& grpIds) = 0;

    /*!
     * For requesting msgs related to a given msg id within a group
     * @param token The token returned for the request
     * @param ansType The type of result wanted
     * @param opts Additional option that affect outcome of request. Please see specific services, for valid values
     * @param groupIds The ids of the groups to get, second entry of map empty to query for all msgs
     * @return true if request successful false otherwise
     */
    virtual bool requestMsgRelatedInfo(uint32_t &token, uint32_t ansType, const RsTokReqOptions &opts, const std::vector<RsGxsGrpMsgIdPair>& msgIds) = 0;

    /*!
     * This request statistics on amount of data held
     * number of groups
     * number of groups subscribed
     * number of messages
     * size of db store
     * total size of messages
     * total size of groups
     * @param token
     */
    virtual void requestServiceStatistic(uint32_t& token, const RsTokReqOptions &opts) = 0;

	/*!
	 * To request statistic on a group
	 * @param token set to value to be redeemed to get statistic
	 * @param grpId the id of the group
	 */
    virtual void requestGroupStatistic(uint32_t& token, const RsGxsGroupId& grpId, const RsTokReqOptions &opts) = 0;


    /* Poll */

    /*!
     * Request the status of ongoing request.
     * Please use this for polling as much cheaper
     * than polling the specific service as they might
     * not return intermediate status information
     * @param token value of token to check status for
     * @return the current status of request
     */
	virtual GxsRequestStatus requestStatus(const uint32_t token) = 0;

	/*!
	 * @brief Cancel Request
	 * If this function returns false, it may be that the request has completed
	 * already. Useful for very expensive request.
	 * @param token the token of the request to cancel
	 * @return false if unusuccessful in cancelling request, true if successful
	 */
	virtual bool cancelRequest(const uint32_t &token) = 0;

#ifdef TO_REMOVE
	/**
	 * Block caller while request is being processed.
	 * Useful for blocking API implementation.
	 * @param[in] token token associated to the request caller is waiting for
	 * @param[in] maxWait maximum waiting time in milliseconds
	 * @param[in] checkEvery time in millisecond between status checks
	 */
	RsTokenService::GxsRequestStatus waitToken(
	        uint32_t token,
	        std::chrono::milliseconds maxWait = std::chrono::milliseconds(10000),
	        std::chrono::milliseconds checkEvery = std::chrono::milliseconds(20),
            bool auto_delete_if_unsuccessful=true)
	{
#if defined(__ANDROID__) && (__ANDROID_API__ < 24)
		auto wkStartime = std::chrono::steady_clock::now();
		int maxWorkAroundCnt = 10;
LLwaitTokenBeginLabel:
#endif
		auto timeout = std::chrono::steady_clock::now() + maxWait;
		auto st = requestStatus(token);
		while( !(st == RsTokenService::FAILED || st >= RsTokenService::COMPLETE) && std::chrono::steady_clock::now() < timeout )
		{
			std::this_thread::sleep_for(checkEvery);
			st = requestStatus(token);
		}
        if(st != RsTokenService::COMPLETE && auto_delete_if_unsuccessful)
            cancelRequest(token);

#if defined(__ANDROID__) && (__ANDROID_API__ < 24)
		/* Work around for very slow/old android devices, we don't expect this
		 * to be necessary on newer devices. If it take unreasonably long
		 * something worser is already happening elsewere and we return anyway.
		 */
		if( st > RsTokenService::FAILED && st < RsTokenService::COMPLETE
		        && maxWorkAroundCnt-- > 0 )
		{
			maxWait *= 10;
			checkEvery *= 3;
			Dbg3() << __PRETTY_FUNCTION__ << " Slow Android device "
			       << " workaround st: " << st
			       << " maxWorkAroundCnt: " << maxWorkAroundCnt
			       << " maxWait: " << maxWait.count()
			       << " checkEvery: " << checkEvery.count() << std::endl;
			goto LLwaitTokenBeginLabel;
		}
		Dbg3() << __PRETTY_FUNCTION__ << " lasted: "
		       << std::chrono::duration_cast<std::chrono::milliseconds>(
		              std::chrono::steady_clock::now() - wkStartime ).count()
		       << "ms" << std::endl;

#endif

		return st;
	}
#endif

	RS_SET_CONTEXT_DEBUG_LEVEL(2)
};
