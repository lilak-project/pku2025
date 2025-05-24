LKRun *run;
void next()
{
    run -> ExecuteNextEvent();
}

void ha()
{
    run = new LKRun();
    run -> AddInputFile("../macros/data_reco/pku2025_0221.reco.root");
    run -> Add( new MTHitReconstructionTask());
    //run -> Add( new MTPreviewTask());
    run -> Init();
    //run -> ExecuteEvent(0);
    run -> Run(2);
}
