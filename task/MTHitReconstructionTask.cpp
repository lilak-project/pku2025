#include "MTHitReconstructionTask.h"
#include "LKRun.h"
#include "LKListFileParser.h"

MTHitReconstructionTask::MTHitReconstructionTask()
:LKTask("MTHitReconstructionTask", "MTHitReconstructionTask")
{
}

bool MTHitReconstructionTask::Init() 
{
    fHitChannelArray = fRun -> GetBranchA("HitChannel");
    fHitArray = fRun -> RegisterBranchA("Hit","LKHit",20);

    fPar -> UpdatePar(fMappingFileName,"tpc_mapping_file");
    fPar -> UpdatePar(fMappingFormat,"tpc_mapping_format");

    LKListFileParser parser;
    parser.SetClass("GETChannelMapData");
    parser.ReadFile(fMappingFileName,fMappingFormat,true);
    auto dataArray = parser.GetDataArray();
    auto n = dataArray -> GetEntries();
    for (auto i=0; i<n; ++i) {
        auto chData = (GETChannelMapData*) dataArray -> At(i);
        GETChannelKey key { chData->cobo, chData->asad, chData->aget, chData->chan };
        fGETMap[key] = chData;
    }

    fChannelAnalyzer = new LKChannelAnalyzer();

    return true;
}

void MTHitReconstructionTask::Exec(Option_t*)
{
    fHitArray -> Clear("C");
    int countHits = 0;

    auto numChannels = fHitChannelArray -> GetEntries();
    for (auto iChannel=0; iChannel<numChannels; ++iChannel)
    {
        auto channel = (GETChannel*) fHitChannelArray -> At(iChannel);
        GETChannelKey key { short(channel->GetCobo()), short(channel->GetAsad()), short(channel->GetAget()), short(channel->GetChan()) };
        auto chData = fGETMap[key];
        if (chData==nullptr)
            continue; //TODO

        double xx = 0;
        double zz = 0;
        for (auto iv=0; iv<chData->n; ++iv) {
            xx += chData -> x[iv];
            zz += chData -> z[iv];
        }
        xx = xx/chData->n;
        zz = zz/chData->n;

        fChannelAnalyzer -> Analyze(channel->GetBufferArray());

        auto numHits = fChannelAnalyzer -> GetNumHits();
        for (auto iHit=0; iHit<numHits; ++iHit)
        {
            auto pedestal = fChannelAnalyzer -> GetPedestal();
            auto energy = fChannelAnalyzer -> GetAmplitude(0);
            auto time = fChannelAnalyzer -> GetTbHit(0);
            double energy0 = energy;

            auto hit = (LKHit*) fHitArray -> ConstructedAt(countHits);
            hit -> SetX(xx);
            hit -> SetZ(zz);
            hit -> SetCharge(energy);
            hit -> SetHitID(countHits);
            hit -> SetChannelID(iChannel);
            hit -> SetPedestal(pedestal);
            hit -> SetTb(time);
            countHits++;
        }
    }

    lk_info << "Found " << countHits << " hits" <<  endl;
}

bool MTHitReconstructionTask::EndOfRun()
{
    return true;
}
