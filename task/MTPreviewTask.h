#ifndef MTPREVIEWTASK_HH
#define MTPREVIEWTASK_HH

#include "LKTask.h"
#include "TH2Poly.h"
#include "LKDrawing.h"
#include "GETChannel.h"
#include "LKDrawingGroup.h"

class MTPreviewTask : public LKTask
{
    private:
        TClonesArray* fHitArray = nullptr;
        TClonesArray* fHitChannelArray = nullptr;

        TH2D* fZT = nullptr;
        TH1D* fEnergy = nullptr;
        TH2Poly* fMM = nullptr;
        TH2Poly* fMMA = nullptr;
        TString fMappingFileName = "../common/MATE_4000ch_trianglePad_map_merged.txt";
        TString fMappingFormat = "id:cobo:asad:aget:chan:x0:z0:x1:z1:x2:z2:x3:z3";
        LKDrawingGroup* fTop = nullptr;
        bool fOmitDrawAtExe = false;

    public:
        MTPreviewTask();

        bool Init();
        void Exec(Option_t*);
        bool EndOfRun();

    ClassDef(MTPreviewTask, 1)
};

#endif
