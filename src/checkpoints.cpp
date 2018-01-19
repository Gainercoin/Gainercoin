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
		(2549, uint256("0x8ca7e5ceed02b708efe32c0713465174a6349e14b11e8b8f1f0f8f3e3330315c"))
		(4821, uint256("0xae179fa65e4f2f000ebdd81c3d3538e8042435e2e7de1bd0dddfabac7d9edb3e"))
		(5716, uint256("0x6df0ac972421d40f6880147dcdbc62053eec486e6c076e27e22e055d2cb55b50"))
		(8236, uint256("0x3c2f848e309315ed9f2e0a09f3a3d6a600f2145ab70446073edf4cc6878d085b"))
		(11548, uint256("0xa5a35f71f8b796445ab3fdb7ca622d789751385fd5d914577d0e976f0c03b7d7"))
		(14566, uint256("0x2b66bc36c6a801f920317c93ff7c2bc28c68d576721bfa6157abfad8cada3910"))
		(15826, uint256("0xba730a1b50d35a6fdb39f5f3e91e062075f55beecf3ac8397a3dfb17aaaa48cf"))
		(16497, uint256("0x22f4c9ac944c9c5c42a4b3f7394d9116e5aa5d116b083f04e231bbc674b2579c"))
		(16498, uint256("0x1ac7f51b03910641aaa1c5e70ef01419e26d640608845bfc1fa2c5836742b492"))
		(16499, uint256("0x0c4f7519dd78ea461c9534adfa793252437137c7553786fb3a2af289647a3f6f"))
		(16500, uint256("0xa1199bc0a477cf691b9ef78725d0097be8af82e7d717a5e6a3aae85bcd3ed27c"))
		(16501, uint256("0xe9d00580e7f36d4211a6102c043bd2b583440801cde8d97f597bc9dbb0632a38"))
		(16502, uint256("0x198a135b2fb816e4553ce88e9872ed829803c7565c789c7d69ab4abf6b701445"))
		(16503, uint256("0x9ed26a5a96a7ae3ffb4bb90c03b60396ac0bfb41bf3d7abf67d17f3f3f931ad9"))
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
