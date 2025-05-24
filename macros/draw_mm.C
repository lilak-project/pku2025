TH2Poly* fMM = nullptr;

TH2Poly* get_mm(bool fill_id=false)
{
    if (fMM!=nullptr)
        return fMM;

    TString mappingFileName = "../common/MATE_4000ch_trianglePad_map_merged.txt";

    fMM = new TH2Poly("mate_tpc_mm","MATE-TPC MM;x [cm];z [cm] (beam-axis)",100,-15,15,100,-1,30);
    double xx[8] = {0};
    double zz[8] = {0};

    LKListFileParser parser;
    parser.SetClass("GETChannelMapData");
    parser.ReadFile(mappingFileName,"id:cobo:asad:aget:chan:x0:z0:x1:z1:x2:z2:x3:z3",true);
    auto dataArray = parser.GetDataArray();
    auto n = dataArray -> GetEntries();
    for (auto i=0; i<n; ++i)
    {
        auto data = (GETChannelMapData*) dataArray -> At(i);
        for (auto iv=0; iv<data->n; ++iv) {
            xx[iv] = data -> x[iv];
            zz[iv] = data -> z[iv];
        }
        cout << i << " " << data -> x[0] << " " << data -> x[1] << " " << data -> x[2] << endl;
        auto bin = fMM -> AddBin(data->n, xx, zz);
        if (fill_id)
            //if (data->id<100)
                //fMM -> SetBinContent(bin,data->id);
                //fMM -> SetBinContent(bin,data->cobo+1);
                fMM -> SetBinContent(bin,data->asad+1);
    }

    fMM -> SetStats(0);
    fMM -> SetMarkerSize(0.5);

    return fMM;
}

LKDrawing* draw_mm(bool save=true, bool fill_id=true)
{
    get_mm(fill_id);

    auto draw = new LKDrawing();
    //draw -> Add(fMM,"1");
    draw -> Add(fMM,"col0lz");
    //draw -> Add(fMM,"l");
    draw -> SetCanvasSize(1300,1300);
    draw -> SetCanvasMargin(0.10,0.05,.10,.08);
    draw -> Draw();

    if (save) {
        draw -> GetCanvas() -> SaveAs("figures/tpc_mm.png");
        draw -> GetCanvas() -> SaveAs("figures/tpc_mm.eps");
    }

    return draw;
}
