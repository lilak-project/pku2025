TH2Poly* fmm = nullptr;

TH2Poly* get_mm(bool fill_id=false)
{
    if (fmm!=nullptr)
        return fmm;

    TString mappingFileName = "../common/MATE_4000ch_trianglePad_map_merged.txt";

    fmm = new TH2Poly("mate_tpc_mm","MATE-TPC MM;x [cm];z [cm] (beam-axis)",100,-15,15,100,-1,30);
    double xx[8] = {0};
    double zz[8] = {0};

    LKListFileParser parser;
    parser.SetClass("CAACMapData");
    parser.ReadFile(mappingFileName,"id:cobo:asad:aget:chan:x0:z0:x1:z1:x2:z2:x3:z3",true);
    auto dataArray = parser.GetDataArray();
    auto n = dataArray -> GetEntries();
    for (auto i=0; i<n; ++i)
    {
        auto data = (CAACMapData*) dataArray -> At(i);
        for (auto iv=0; iv<data->n; ++iv) {
            xx[iv] = data -> x[iv];
            zz[iv] = data -> z[iv];
        }
        auto bin = fmm -> AddBin(data->n, xx, zz);
        if (fill_id) fmm -> SetBinContent(bin,data->id);
    }

    fmm -> SetStats(0);
    fmm -> SetMarkerSize(0.5);

    return fmm;
}

LKDrawing* draw_mm(bool save=true, bool fill_id=false)
{
    get_mm(fill_id);

    auto draw = new LKDrawing();
    draw -> Add(fmm,"col");
    draw -> Add(fmm,"l");
    draw -> SetCanvasSize(1300,1300);
    draw -> SetCanvasMargin(0.10,0.05,.10,.08);
    draw -> Draw();

    if (save) {
        draw -> GetCanvas() -> SaveAs("figures/tpc_mm.png");
        draw -> GetCanvas() -> SaveAs("figures/tpc_mm.eps");
    }

    return draw;
}
