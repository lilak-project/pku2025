LKRun *fRun;
TClonesArray *fHitChannelArray = nullptr;
Long64_t fNumEvents = 0;
Long64_t fCurrentEvent = -1;
LKDrawingGroup* top;
LKPulseAnalyzer* ana;

bool next(bool draw=true);

void psa()
{
    fRun = new LKRun();
    fRun -> AddPar("run_3_preview.mac");
    fRun -> Init();
    fNumEvents = fRun -> GetNumEvents();
    fHitChannelArray = fRun -> GetBranchA("HitChannel");
    cout << "Number of events: " << fNumEvents << endl;

    top = new LKDrawingGroup("top");
    ana = new LKPulseAnalyzer();

    next();
}

bool next(bool draw)
{
    fCurrentEvent++;
    if (fCurrentEvent>=fNumEvents)
        return false;

    fRun -> GetEntry(fCurrentEvent);
    auto numChannels = fHitChannelArray -> GetEntries();
    cout << fCurrentEvent << " " << numChannels << endl;

    if (draw)
        top -> SetDraw(false);

    for (auto iChannel=0; iChannel<numChannels; ++iChannel)
    {
        auto channel = (GETChannel*) fHitChannelArray -> At(iChannel);
        ana -> AddChannel(channel->GetBufferArray);
        if (draw) {
            if (top->GetNumDrawings()<iChannel+1)
                top -> CreateDrawing(Form("c%d",iChannel)) -> Add(channel->GetHist());
            else {
                auto drawing = top -> GetDrawing(iChannel);
                drawing -> SetDraw(true);
                auto hist = drawing -> GetMainHist();
                channel -> FillHist(hist);
            }
        }
    }

    if (draw)
        top -> Draw();

    return true;
}



bool DrawChannel(TVirtualPad* pad=(TVirtualPad*)nullptr);
