// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
#include "MessageFactory.h"
#include "mds/mdstypes.h"
// MDS
#include "messages/MPGStats.h"

#include "messages/MGenericMessage.h"

#include "messages/MPGStatsAck.h"

#include "messages/MStatfs.h"
#include "messages/MStatfsReply.h"

#include "messages/MGetPoolStats.h"
#include "messages/MGetPoolStatsReply.h"


#include "messages/MPoolOp.h"
#include "messages/MPoolOpReply.h"

#include "messages/PaxosServiceMessage.h"
#include "messages/MMonCommand.h"
#include "messages/MMonCommandAck.h"
#include "messages/MMonPaxos.h"

#include "messages/MMonProbe.h"
#include "messages/MMonJoin.h"
#include "messages/MMonElection.h"
#include "messages/MMonSync.h"
#include "messages/MMonScrub.h"

#include "messages/MLog.h"
#include "messages/MLogAck.h"

#include "messages/MPing.h"

#include "messages/MCommand.h"
#include "messages/MCommandReply.h"
#include "messages/MBackfillReserve.h"
#include "messages/MRecoveryReserve.h"

#include "messages/MRoute.h"
#include "messages/MForward.h"

#include "messages/MOSDBoot.h"
#include "messages/MOSDAlive.h"
#include "messages/MOSDPGTemp.h"
#include "messages/MOSDFailure.h"
#include "messages/MOSDMarkMeDown.h"
#include "messages/MOSDPing.h"
#include "messages/MOSDOp.h"
#include "messages/MOSDOpReply.h"
#include "messages/MOSDSubOp.h"
#include "messages/MOSDSubOpReply.h"
#include "messages/MOSDRepOp.h"
#include "messages/MOSDRepOpReply.h"
#include "messages/MOSDMap.h"
#include "messages/MMonGetOSDMap.h"

#include "messages/MOSDPGNotify.h"
#include "messages/MOSDPGQuery.h"
#include "messages/MOSDPGLog.h"
#include "messages/MOSDPGRemove.h"
#include "messages/MOSDPGInfo.h"
#include "messages/MOSDPGCreate.h"
#include "messages/MOSDPGTrim.h"
#include "messages/MOSDPGMissing.h"
#include "messages/MOSDScrub.h"
#include "messages/MOSDRepScrub.h"
#include "messages/MOSDPGScan.h"
#include "messages/MOSDPGBackfill.h"

#include "messages/MRemoveSnaps.h"

#include "messages/MMonMap.h"
#include "messages/MMonGetMap.h"
#include "messages/MMonGetVersion.h"
#include "messages/MMonGetVersionReply.h"
#include "messages/MMonHealth.h"
#include "messages/MMonMetadata.h"
#include "messages/MDataPing.h"
#include "messages/MAuth.h"
#include "messages/MAuthReply.h"
#include "messages/MMonSubscribe.h"
#include "messages/MMonSubscribeAck.h"
#include "messages/MMonGlobalID.h"
#include "messages/MClientSession.h"
#include "messages/MClientReconnect.h"
#include "messages/MClientRequest.h"
#include "messages/MClientRequestForward.h"
#include "messages/MClientReply.h"
#include "messages/MClientCaps.h"
#include "messages/MClientCapRelease.h"
#include "messages/MClientLease.h"
#include "messages/MClientSnap.h"
#include "messages/MClientQuota.h"

#include "messages/MMDSSlaveRequest.h"

#include "messages/MMDSMap.h"
#include "messages/MFSMap.h"
#include "messages/MFSMapUser.h"
#include "messages/MMDSBeacon.h"
#include "messages/MMDSLoadTargets.h"
#include "messages/MMDSResolve.h"
#include "messages/MMDSResolveAck.h"
#include "messages/MMDSCacheRejoin.h"
#include "messages/MMDSFindIno.h"
#include "messages/MMDSFindInoReply.h"
#include "messages/MMDSOpenIno.h"
#include "messages/MMDSOpenInoReply.h"

#include "messages/MDirUpdate.h"
#include "messages/MDiscover.h"
#include "messages/MDiscoverReply.h"

#include "messages/MMDSFragmentNotify.h"

#include "messages/MExportDirDiscover.h"
#include "messages/MExportDirDiscoverAck.h"
#include "messages/MExportDirCancel.h"
#include "messages/MExportDirPrep.h"
#include "messages/MExportDirPrepAck.h"
#include "messages/MExportDir.h"
#include "messages/MExportDirAck.h"
#include "messages/MExportDirNotify.h"
#include "messages/MExportDirNotifyAck.h"
#include "messages/MExportDirFinish.h"

#include "messages/MExportCaps.h"
#include "messages/MExportCapsAck.h"
#include "messages/MGatherCaps.h"


#include "messages/MDentryUnlink.h"
#include "messages/MDentryLink.h"

#include "messages/MHeartbeat.h"

#include "messages/MMDSTableRequest.h"

//#include "messages/MInodeUpdate.h"
#include "messages/MCacheExpire.h"
#include "messages/MInodeFileCaps.h"

#include "messages/MMgrBeacon.h"
#include "messages/MMgrMap.h"
#include "messages/MMgrDigest.h"
#include "messages/MMgrReport.h"
#include "messages/MMgrOpen.h"
#include "messages/MMgrConfigure.h"

#include "messages/MLock.h"

#include "messages/MWatchNotify.h"
#include "messages/MTimeCheck.h"

#include "common/config.h"

#include "messages/MOSDPGPush.h"
#include "messages/MOSDPGPushReply.h"
#include "messages/MOSDPGPull.h"

#include "messages/MOSDECSubOpWrite.h"
#include "messages/MOSDECSubOpWriteReply.h"
#include "messages/MOSDECSubOpRead.h"
#include "messages/MOSDECSubOpReadReply.h"

#include "messages/MOSDPGUpdateLogMissing.h"
#include "messages/MOSDPGUpdateLogMissingReply.h"


Message* MonMessageFactory::create(int type)
{
  switch (type) {
  case MSG_MON_COMMAND:               return new MMonCommand;
  case MSG_MON_COMMAND_ACK:           return new MMonCommandAck;
  case MSG_MON_PAXOS:                 return new MMonPaxos;
  case MSG_MON_PROBE:                 return new MMonProbe;
  case MSG_MON_JOIN:                  return new MMonJoin;
  case MSG_MON_ELECTION:              return new MMonElection;
  case MSG_MON_SYNC:                  return new MMonSync;
  case MSG_MON_SCRUB:                 return new MMonScrub;
  case CEPH_MSG_MON_MAP:              return new MMonMap;
  case CEPH_MSG_MON_GET_MAP:          return new MMonGetMap;
  case CEPH_MSG_MON_GET_OSDMAP:       return new MMonGetOSDMap;
  case CEPH_MSG_MON_GET_VERSION:      return new MMonGetVersion();
  case CEPH_MSG_MON_GET_VERSION_REPLY: return new MMonGetVersionReply();
  case CEPH_MSG_MON_METADATA:         return new MMonMetadata();

  case MSG_TIMECHECK:                 return new MTimeCheck();
  case MSG_MON_HEALTH:                return new MMonHealth();
  case MSG_ROUTE:                     return new MRoute;
  case MSG_FORWARD:                   return new MForward;

  case CEPH_MSG_AUTH:                 return new MAuth;
  case CEPH_MSG_AUTH_REPLY:           return new MAuthReply;
  case MSG_MON_GLOBAL_ID:             return new MMonGlobalID;

  case MSG_LOG:                       return new MLog;
  case MSG_LOGACK:                    return new MLogAck;

  case CEPH_MSG_MON_SUBSCRIBE:
    return new MMonSubscribe;

  case MSG_PGSTATS:
    return new MPGStats;

  case MSG_OSD_BOOT:
    return new MOSDBoot();

  case MSG_OSD_ALIVE:
    return new MOSDAlive();

  case MSG_OSD_PGTEMP:
    return new MOSDPGTemp;

    // MDS
  case MSG_MDS_HEARTBEAT:             return new MHeartbeat;
  case MSG_MDS_BEACON:                return new MMDSBeacon;
  case CEPH_MSG_MDS_MAP:              return new MMDSMap();
  case CEPH_MSG_OSD_MAP:              return new MOSDMap;
  case MSG_MDS_SLAVE_REQUEST:         return new MMDSSlaveRequest;
  case CEPH_MSG_FS_MAP:               return new MFSMap;
  case CEPH_MSG_FS_MAP_USER:          return new MFSMapUser;

  default: return nullptr;
  }
}

Message* MonClientMessageFactory::create(int type)
{
  switch (type) {
  case MSG_MON_COMMAND:               return new MMonCommand;
  case MSG_MON_COMMAND_ACK:           return new MMonCommandAck;
  case MSG_MON_PAXOS:                 return new MMonPaxos;
  case MSG_MON_PROBE:                 return new MMonProbe;
  case MSG_MON_JOIN:                  return new MMonJoin;
  case MSG_MON_ELECTION:              return new MMonElection;
  case MSG_MON_SYNC:                  return new MMonSync;
  case MSG_MON_SCRUB:                 return new MMonScrub;
  case CEPH_MSG_MON_MAP:              return new MMonMap;
  case CEPH_MSG_MON_GET_MAP:          return new MMonGetMap;
  case CEPH_MSG_MON_GET_OSDMAP:       return new MMonGetOSDMap;
  case CEPH_MSG_MON_GET_VERSION:      return new MMonGetVersion();
  case CEPH_MSG_MON_GET_VERSION_REPLY: return new MMonGetVersionReply();
  case CEPH_MSG_MON_METADATA:         return new MMonMetadata();

  case MSG_TIMECHECK:                 return new MTimeCheck();
  case MSG_MON_HEALTH:                return new MMonHealth();
  case MSG_ROUTE:                     return new MRoute;
  case MSG_FORWARD:                   return new MForward;

  case CEPH_MSG_AUTH:                 return new MAuth;
  case CEPH_MSG_AUTH_REPLY:           return new MAuthReply;
  case MSG_MON_GLOBAL_ID:             return new MMonGlobalID;

  case MSG_LOG:                       return new MLog;
  case MSG_LOGACK:                    return new MLogAck;
  case MSG_MGR_MAP:                   return new MMgrMap;
  case CEPH_MSG_MON_SUBSCRIBE:
    return new MMonSubscribe;

  case MSG_PGSTATS:
    return new MPGStats;

  case MSG_OSD_BOOT:
    return new MOSDBoot();

  case MSG_OSD_ALIVE:
    return new MOSDAlive();

  case MSG_OSD_PGTEMP:
    return new MOSDPGTemp;

    // MDS
  case MSG_MDS_HEARTBEAT:             return new MHeartbeat;
  case MSG_MDS_BEACON:                return new MMDSBeacon;
  case CEPH_MSG_MDS_MAP:              return new MMDSMap();
  case CEPH_MSG_OSD_MAP:              return new MOSDMap;
  case MSG_MDS_SLAVE_REQUEST:         return new MMDSSlaveRequest;
  case CEPH_MSG_FS_MAP:               return new MFSMap;
  case CEPH_MSG_FS_MAP_USER:          return new MFSMapUser;
  default: return nullptr;
  }
}
