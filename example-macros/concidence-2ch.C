/*
This macro has to be executed only if you are acquiring with just 2 channels.
For any problems or doubts contact Franco Galtarossa (franco.galtarossa@lnl.infn.it).
*/

struct slimport_data_t {
    ULong64_t	timetag;
    UInt_t	baseline;
    UShort_t	qshort;
    UShort_t	qlong;
    UShort_t	pur;
    UShort_t	samples[4096];
};

void fix_coinc(const char *name_infile, const char *name_outfile){
    
    slimport_data_t indataA, indataB;
    int offsetA=0, offsetB=0, ent=0;
    int tolerance = 25; // in timestamp units
    TFile *infile = new TFile(name_infile);
    TTree *intree = (TTree*)infile->Get("acq_tree_0");
    
//If you are using channels in a different order change here!
    TBranch *inbranchA = intree->GetBranch("acq_ch0");
    TBranch *inbranchB = intree->GetBranch("acq_ch1");
    inbranchA->SetAddress(&indataA.timetag);
    inbranchB->SetAddress(&indataB.timetag);
                 
    while(ent+offsetA<inbranchA->GetEntries() && ent+offsetB<inbranchB->GetEntries()){
         inbranchA->GetEntry(ent+offsetA);
         inbranchB->GetEntry(ent+offsetB);
         if(indataA.timetag < indataB.timetag - tolerance){
	     offsetA++;
	     continue;
	 } else if(indataA.timetag > indataB.timetag + tolerance){
	     offsetB++;
	     continue;
	 }
                                                         
         ent++;                                                                                         

//Here you can do your analysis
                                          
    }

}
