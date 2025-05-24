#include "MTSelectHitChannelsTask.h"
#include "LKRun.h"

MTSelectHitChannelsTask::MTSelectHitChannelsTask()
:LKTask("MTSelectHitChannelsTask", "MTSelectHitChannelsTask")
{
}

bool MTSelectHitChannelsTask::Init() 
{
    fRawDataArray = fRun -> GetBranchA("RawData");
    fHitChannelArray = fRun -> RegisterBranchA("HitChannel","GETChannel",20);

    fChannel11 = new GETChannel();
    fChannel22 = new GETChannel();
    fChannel45 = new GETChannel();
    fChannel56 = new GETChannel();

    return true;
}

void MTSelectHitChannelsTask::Exec(Option_t*)
{
    fHitChannelArray -> Clear("C");

    auto numChannels = fRawDataArray -> GetEntries();

    for (auto iChannel=0; iChannel<numChannels; ++iChannel)
    {
        auto channel = (GETChannel*) fRawDataArray -> At(iChannel);
        auto chan = channel -> GetChan();
        if      (chan==11) channel -> Copy(*fChannel11);
        else if (chan==22) channel -> Copy(*fChannel22);
        else if (chan==45) channel -> Copy(*fChannel45);
        else if (chan==56) channel -> Copy(*fChannel56);
    }

    int countHitChannels = 0;

    for (auto iChannel=0; iChannel<numChannels; ++iChannel)
    {
        auto channel = (GETChannel*) fRawDataArray -> At(iChannel);
        auto chan = channel -> GetChan();
        if (chan==11 || chan==22 || chan==45 || chan==56)
            continue;

        GETChannel* channelFPN = nullptr;
        if      (chan < 15) channelFPN = fChannel11;
        else if (chan < 33) channelFPN = fChannel22;
        else if (chan < 50) channelFPN = fChannel45;
        else                channelFPN = fChannel56;

        auto channelHit = (GETChannel*) fHitChannelArray -> ConstructedAt(countHitChannels++);
        channel -> Copy(*channelHit);

        channelHit -> SubtractArray(channelFPN->GetBufferArray());
    }

    lk_info << "Found " << countHitChannels << " fired channels" << endl;
}

bool MTSelectHitChannelsTask::EndOfRun()
{
    return true;
}
