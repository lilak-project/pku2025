#ifndef MTSELECTHITCHANNELSTASK_HH
#define MTSELECTHITCHANNELSTASK_HH

#include "LKTask.h"
#include "GETChannel.h"
#include "LKChannelAnalyzer.h"

class MTSelectHitChannelsTask : public LKTask
{
    private:
        TClonesArray* fRawDataArray = nullptr;
        TClonesArray* fHitChannelArray = nullptr;

        GETChannel* fChannel11 = nullptr;
        GETChannel* fChannel22 = nullptr;
        GETChannel* fChannel45 = nullptr;
        GETChannel* fChannel56 = nullptr;

    public:
        MTSelectHitChannelsTask();

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    ClassDef(MTSelectHitChannelsTask, 1)
};

#endif
