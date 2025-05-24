#ifndef MTHITRECONSTRUCTIONTASK_HH
#define MTHITRECONSTRUCTIONTASK_HH

#include "LKTask.h"
#include "TH2Poly.h"
#include "LKDrawing.h"
#include "GETChannel.h"
#include "GETChannelMapData.h"
#include "LKChannelAnalyzer.h"

#include <unordered_map>
#include <tuple>

class MTHitReconstructionTask : public LKTask
{
    private:
        TClonesArray* fHitChannelArray = nullptr;
        TClonesArray* fHitArray = nullptr;

        TString fMappingFileName = "../common/MATE_4000ch_trianglePad_map_merged.txt";
        TString fMappingFormat = "id:cobo:asad:aget:chan:x0:z0:x1:z1:x2:z2:x3:z3";

        std::unordered_map<GETChannelKey, GETChannelMapData*> fGETMap;

        LKChannelAnalyzer *fChannelAnalyzer = nullptr;

    public:
        MTHitReconstructionTask();

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    ClassDef(MTHitReconstructionTask, 1)
};

#endif
