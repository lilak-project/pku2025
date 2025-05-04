void draw_padplane()
{
    TString mappingFileName = "../common/MATE_4000ch_trianglePad_map_merged.txt";

    auto hist = new TH2Poly("hist","MATE-TPC MM;x [cm];z [cm] (beam-axis)",100,-15,15,100,-1,30);
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
        auto bin = hist -> AddBin(data->n, xx, zz);
        hist -> SetBinContent(bin,data->id);
    }

    hist -> SetStats(0);
    hist -> SetMarkerSize(0.5);

    LKDrawing draw;
    draw.Add(hist,"col");
    draw.Add(hist,"l");
    draw.SetCanvasSize(1300,1300);
    draw.SetCanvasMargin(0.10,0.05,.10,.08);
    draw.Draw();

    draw.GetCanvas() -> SaveAs("figures/tpc_mm.png");
    draw.GetCanvas() -> SaveAs("figures/tpc_mm.eps");
}
