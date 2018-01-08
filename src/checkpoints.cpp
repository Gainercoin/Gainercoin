// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 5000;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    //
    // What makes a good checkpoint block?
    // + Is surrounded by blocks with reasonable timestamps
    //   (no blocks before with a timestamp after, none after with
    //    timestamp before)
    // + Contains no strange transactions
    //
    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
		(0, uint256("0x0000d9badf5d39afaa47451111a931672baaa3ce9bbbfb9165f414b9e6e69d61"))
		(1, uint256("0x5eb206c544981db73e4d90c8d346f2be9a15ac51e6074eb432855b6497d26d74"))
		(2, uint256("0x3a82bae794ef2c92369671112a3ac9cd8f22e0f1473b5b8fd8b4e4ac206b2694"))
		(3, uint256("0xebb84fa178eb8e46ca7b89efb3430b6ff56f0d19b51ef625e518912b941d6722"))
		(5, uint256("0x612f7520718080a2d571138613b30535e6fb16b6b7fa43b1ede8ecd7e356a9e1"))
		(16, uint256("0x33896e574a763ce525404378d6ddd1ee8aa45c8c20adb8ab657bf842d1bff805"))
		(25, uint256("0x843e73753c99b0580343ea4c145a3f921664d3dc9ae2ca3f35fb5892027a914c"))
		(29, uint256("0xd7bae3b07190b09f8dac77b16074b2623baf18e66549712df01cd0a4c1eec7b9"))
		(32, uint256("0xac93b8b08a07458aff6038dcc26aba4c6eec50eadc65edd217cb9872d2c5d48c"))
		(56, uint256("0x056e3109775c6e176556de5c4a70529928c91260394e44d40b39cabda18e150d"))
		(95, uint256("0x6c2bae321038a2abad5b6b6025375de3636b4293f479322cb848c9cb18c8c1b9"))
		(126, uint256("0x95786cf07d9e9010c52aeebfb8e8985b434ca8cbea87df0a3c400937539be077"))
		(199, uint256("0xb5f52495ae7a73c7c4b9efc156b92b305c65d7022952afc7ed0f62b885136e22"))
		(549, uint256("0x1082b9f6ca3177077fbd5b814da50736ddcc5675fe7d0103deac57d1764f07e5"))
		(943, uint256("0x6f6b67f3e91cf3a61598aec1eccb2769a6e374e6b8d492c4e09f12e41f7757aa"))
		(1353, uint256("0x674fab53e704dbfce2bdb632f6750ed95b49e7d435cb6e140392e9fd292d901d"))
		(1987, uint256("0x182f18b30b15deecbda1b5010be2cad52bca1542d13a1627f7a7bed495ad1005"))
		(2427, uint256("0x26d0f0f2cb3432490012022c1d06f0c0b969b4be1d4f31d563e9dd935e6b49f4"))
		(3172, uint256("0xca4693e83fe29cb9cd0b39055ab0b3fc5e219925e27691c18c952cc9400d4f95"))
		(3767, uint256("0x32894ac8ff07b945cbdb26b8f4110822d3b394e606adde0d4e0a481075c7a7e0"))
    ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();
        if (nHeight <= pindexSync->nHeight){
            return false;
        }
        return true;
    }
}
