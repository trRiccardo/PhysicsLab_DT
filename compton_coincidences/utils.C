#include <iostream>
#include <vector>
#include <algorithm>

// debug
bool debug = true;

// input data structure
struct slimport_data_t {
	ULong64_t	timetag; //time stamp
	UInt_t		baseline;
	UShort_t	qshort; //integration with shorter time
	UShort_t	qlong; //integration with longer time
	UShort_t	pur;
	UShort_t	samples[4096];
};

// retrieve spectrum histogram for any channel 
TH1D* getHistoForChannelFromTree(const char *name_file, short chan, int numBins, double minX, double maxX) {
	
	// variables
	slimport_data_t indata;
	TFile *infile = new TFile(name_file);
	TTree *intree = (TTree*)infile->Get("acq_tree_0");
	TBranch *inbranch = intree->GetBranch(Form("acq_ch%d",chan));
	inbranch->SetAddress(&indata.timetag);
	TH1D *h_spectrum = new TH1D("h_spectrum","Total spectrum",numBins,minX,maxX);

	// histogram filling
	for (int i=0; i<inbranch->GetEntries(); i++) {
		inbranch->GetEntry(i);
		h_spectrum->Fill(indata.qlong);
	}

	// return
	return h_spectrum;

}

// calibrate an histogram
void CalibrateHisto(TH1D *h_uncal, float m, float q) {

	int max_bin = h_uncal->GetNbinsX(); // This method returns the number of bins in x of the histogram
	float max_kev = h_uncal->GetBinCenter(max_bin)*m + q;
	h_uncal->GetXaxis()->SetLimits(q,max_kev);
	if (m!=1 && q!=0) //This means that I actually changed the calibration!
	    h_uncal->SetXTitle("Energy (keV)");

}

// select tagger events of interest
std::vector<double> selectTagger(const char* name_file, TH1D* h, float m, float q) {

	// input data
	slimport_data_t iData;

	// set branch
	TFile* iFile = new TFile(name_file);
	TTree* iTree = (TTree*)iFile->Get( "acq_tree_0" );
	TBranch* iBranch = iTree->GetBranch( "acq_ch0" );	// tagger is channel 0
	iBranch->SetAddress(&iData.timetag);

	// select full energy peak and timestamps
	double calCharge = 0;
	std::vector<double> timestamps;

	for( int i = 0; i < iBranch->GetEntries(); i++ ) {
		// get charge and calibrate it
		iBranch->GetEntry(i);
		calCharge = iData.qlong * m + q;

		// if in the peak, get charge and its timestamp
		if( calCharge > 460 && calCharge < 560 ) {
			h->Fill(calCharge);
			timestamps.push_back(iData.timetag);
		}
	}

	return timestamps;

}

// get scattered electron energy
double getEnergy( double angle ) {

	double iEnergy = 511.;
	double val = 1. / ( 2. - TMath::Cos(angle) );
	double energy = iEnergy * ( 1. - val );

	return energy;

}

// select scatterer events of interest
std::vector<double> selectScatterer(const char* name_file, double angle, TH1D* h, 
									float m, float q) {

	// input data
	slimport_data_t iData;

	// set branch
	TFile* iFile = new TFile(name_file);
	TTree* iTree = (TTree*)iFile->Get( "acq_tree_0" );
	TBranch* iBranch = iTree->GetBranch( "acq_ch1" );	// scatterer is channel 1
	iBranch->SetAddress(&iData.timetag);

	// select full energy peak and timestamps
	double calCharge = 0;
	double expEnergy = getEnergy( angle );

	std::vector<double> timestamps;

	for( int i = 0; i < iBranch->GetEntries(); i++ ) {
		// get charge and calibrate it
		iBranch->GetEntry(i);
		calCharge = iData.qlong * m + q;

		/*
		 *	Retrieve timestamp only if the charge is in an appropriate
		 *	range around the expected energy. If the angle is 0, we 
		 *	expect E = 0, therefore we select events around 0 manually.
		 */

		if( angle == 0 ) {
			if( calCharge < 0. + 100. ) {
				h->Fill(calCharge);
				timestamps.push_back(iData.timetag);
			}
		} else {
			if( calCharge > 0.6*expEnergy && calCharge < 1.05*expEnergy ) {
				h->Fill(calCharge);
				timestamps.push_back(iData.timetag);
			}
		}
	}

	return timestamps;

}

// find common timestamps
std::vector<double> findCommon( std::vector<double> &v1, std::vector<double> &v2 ) {

	// sort input vectors
	std::sort( v1.begin(), v1.end() );
	std::sort( v2.begin(), v2.end() );

	// declare vector to store output
	std::vector<double> coincidences( v1.size() + v2.size(), 0.);
	std::vector<double>::iterator it, end;

	// find common entries with the set_intersection algorithm
	end = std::set_intersection( v1.begin(), v1.end(),
								 v2.begin(), v2.end(),
								 coincidences.begin() );

	// check whether the selection goes wrong
	bool check = std::all_of( coincidences.begin(), coincidences.end(),
							  [](double c) { return c == 0; } );
	if( check ) {
		std::cout << "Aborting: there are no coincidences." << std::endl;
	}

	std::sort( coincidences.begin(), coincidences.end() );

	return coincidences;

}

// select detector events of interests
void selectDetector( const char* name_file, TH1D* h, float m, float q,
					 std::vector<double> tagger, std::vector<double> scatterer ) {

	// get coincidences
	std::vector<double> coincidences = findCommon( tagger, scatterer );
	
	/* 
	 *	Since the measurements are collected in FOLD 3, an event
	 *	is collected only if tagger, scatterer and detector are in
	 *	coincidence. Therefore, if we select the event of interest 
	 *	in tagger and scatterer by timestamp, we can the retrieve
	 *	the corresponding detector events and plot them.
	 */	

	// input data
	slimport_data_t iData;

	// set branch
	TFile* iFile = new TFile(name_file);
	TTree* iTree = (TTree*)iFile->Get( "acq_tree_0" );
	TBranch* iBranch = iTree->GetBranch( "acq_ch2" );	// detector is channel 2
	iBranch->SetAddress(&iData.timetag);

	// select the events of interest
	double calCharge = 0;

	for( int i = 0; i < iBranch->GetEntries(); i++ ) {
		// get charge and calibrate it
		iBranch->GetEntry(i);
		calCharge = iData.qlong * m + q;

		// check the coincidence and fill the histogram
		if( std::binary_search(coincidences.begin(), coincidences.end(), 
			iData.timetag) ) {
			h->Fill(calCharge);
		}
	}

	return;

}

// create 2d histogram of detector versus scatterer
TH2F* createHisto2d( const char* name_file, int angle, float m_s, float q_s, float m_d, float q_d,
					 std::vector<double> tagger, std::vector<double> scatterer ) {

	// keep track of analysis status (debug)
	if( debug )
		std::cout << "I am at angle " << angle << std::endl;

	// input file
	TFile* iFile = new TFile(name_file);
	TTree* iTree = (TTree*)iFile->Get( "acq_tree_0" );

	// 2d histogram
	TH2F* h = new TH2F( "h", Form("2d histogram at angle %i", angle), 100, 0, 700,
						100, 0, 700 );
	h->GetXaxis()->SetTitle("Scatterer (keV)");
	h->GetYaxis()->SetTitle("Detector(keV)");

	// input scatterer
	slimport_data_t iData_s;
	TBranch* iBranch_s = iTree->GetBranch( "acq_ch1" );	// scatterer is channel 1
	iBranch_s->SetAddress(&iData_s.timetag);
	double calCharge_s = 0;
	double expEnergy = getEnergy( angle );	// for scatterer energy selection

	// input detector
	slimport_data_t iData_d;
	TBranch* iBranch_d = iTree->GetBranch( "acq_ch2" );	// detector is channle 2
	iBranch_d->SetAddress(&iData_d.timetag);
	double calCharge_d = 0;

	// coincidences between tagger and scatterer for detector selection
	std::vector<double> coincidences = findCommon( tagger, scatterer );	

	/*
	 *	Scan the detector branch and retrieve coincidences with the 
	 *	binary_search algorithm. If a coincidence is found, scan also
	 *	the scatterer branch and select the events of interest.
	 *	Fill the histogram if the sum of energies is 511 keV within
	 *	a tolerance range.
	 */

	// set tolerance to 12%
	double tolerance = 0.12;
	double minEnergy = (1 - tolerance) * 511.;
	double maxEnergy = (1 + tolerance) * 511.;
	double sum = 0;

	// scan detector
	for( int i = 0; i < 5001; i++ ) { // for( int i = 0; i < iBranch_d->GetEntries(); i++ ) {
		
		// keep track of analysis status (debug)
		if( debug ) {
			if( i % 1000 == 0 )
				std::cout << "Detector iteration: " << i << std::endl;	
		}

		// get entries and timestamps from detector
		iBranch_d->GetEntry(i);

		// continue only if a coincidence is found
		if( std::binary_search(coincidences.begin(), coincidences.end(), 
			iData_d.timetag) ) {

			// scan scatterer
			for( int j = 0; j < 5001; j++ ) { // for( int j = 0; j < iBranch_s->GetEntries(); i++ ) {

				// keep track of analysis status (debug)
				if( debug ) {
					if( j % 1000 == 0 )
						std::cout << "Scatterer iteration: " << j << std::endl;	
				}

				// get entries and charge from scatterer
				iBranch_s->GetEntry(j);
				calCharge_s = iData_s.qlong * m_s + q_s;

				// continue only if the scatterer energy is of interest
				if( angle == 0 ) {
					if( calCharge_s < 0. + 100. ) {
						// if the sum passes the check, fill histogram
						calCharge_d = iData_d.qlong * m_s + q_s;
						sum = calCharge_s + calCharge_d;
						if( (sum > minEnergy) && (sum < maxEnergy) ) {
							h->Fill(calCharge_s, calCharge_d);
						}
					}
				} else {
					if( calCharge_s > 0.6*expEnergy && calCharge_s < 1.05*expEnergy ) {
						// if the sum passes the check, fill histogram
						calCharge_d = iData_d.qlong * m_s + q_s;
						sum = calCharge_s + calCharge_d;
						if( (sum > minEnergy) && (sum < maxEnergy) ) {
							h->Fill(calCharge_s, calCharge_d);
						}
					}	
				}

			}

		}
	}

	return h;

}