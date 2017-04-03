#include "warming.h"

#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>


#include "Pipeline.h"
#include "Parser.h"
#include "Scratch.h"
#include "temperature.h"
#include "Semaphores.h"
#include "FileOperator.h"

using namespace std;

void getCoolingCurve(unsigned n_cpu, vector<unsigned long> wcets){
    string path = "./cooling_curve.xml";

    for (unsigned k = 0; k < n_cpu; ++k)
    {
      vector<unsigned long> toffs;
      unsigned long toff = 2000;
      do{
        toffs.push_back(toff);
        toff += 1000;
      }while(toff < 160000);
      vector<unsigned long> tons(toffs.size(), wcets[k]); 

      stringstream name;
      string savepath = "./offlineResults/";
      name << savepath << "stage_"<< k+1 <<"_cooling";
      if (system(("mkdir -p " + savepath).data() ))
      {
        cout << "error creating folder " << endl;
      }
      getOneStageCurve(toffs, tons, k, path, name.str());

    }
}

void getWarmingCurve(unsigned n_cpu){
	string path = "./warming_curve.xml";
    unsigned slope_step = 5;
    string savepath = "./offlineResults/";
    if (system(("mkdir -p " + savepath).data() ))
      {
        cout << "error creating folder " << endl;
      }

    for(unsigned k = 0; k < n_cpu; k++){
        for(unsigned i=1; i<95; i = i + slope_step){

            double activek = (double)i/100;

            stringstream name;
            name << savepath << "stage_"<<k+1<<"_slope_"<<i;

            vector<unsigned long> toffs;
            vector<unsigned long> tons;
            unsigned long toff  = 2000;
            unsigned long ton;
            do{
                ton = (unsigned long) (activek/(1-activek)*(double)toff + (double)1000/(1-activek));
                toffs.push_back(toff);
                tons.push_back(ton);

                if (toff<=10000)
                    toff = toff + 1000;
                else if (toff<30000)
                    toff = toff + 2000;
                else if (toff<70000)
                    toff = toff + 4000;
                else if (toff<150000)
                    toff = toff + 8000;
                else if (toff<310000)
                    toff = toff + 16000;
                else
                  break;

            }while(toff < 300000);
            getOneStageCurve(toffs, tons, k, path, name.str());

        }
    }
}


void getOneStageCurve(const vector<unsigned long> & toffs, const vector<unsigned long> & tons, 
  const unsigned cpuId, const string path, const string filename){
    if (toffs.size() != tons.size()){
      cout << "toff size doesnot match ton size" << endl;
      return;
    }

    vector<double> Tvstoff;
    vector<double> vectoff;

    double peakT;

    for (unsigned i = 0; i < toffs.size(); ++i)
    {
      unsigned long toff = toffs[i] - 1000;
      unsigned long ton = tons[i] + 1000;
      stringstream name2;
      name2 << filename << "_toff_"<< toff;

      Pipeline* p = new Pipeline(path);
      Scratch::setName(name2.str());
      p->initialize();
      vector<unsigned> worker_cpu;
      worker_cpu.push_back(cpuId);
      p->setWorkerCPU(worker_cpu);

      vector<unsigned long> vton;
      vector<unsigned long> vtoff;
      vton.push_back(ton);
      vtoff.push_back(toff);
      p->setPTMs(vton, vtoff);
      Scratch::setSavingFile(false);
      

      Semaphores::print_sem.wait_sem();
      cout<< filename << "|toff: "<< toff<<endl;
      Semaphores::print_sem.post_sem();
      peakT = p->simulate();

      vectoff.push_back((double)toff);
      Tvstoff.push_back((double)peakT);

      delete p;

      sleep(15);
    }

    string savename = filename + ".csv";
    appendToFile(savename, Tvstoff);
    appendToFile(savename, vectoff);

}



// void saveVectorToFile(vector<double> data, string filename){
//   ofstream file;

//   /************ SAVING _temprature_trace *********/
//   file.open((filename + ".csv").data() , std::ofstream::out | std::ofstream::app);
//   stringstream out;
//   for(unsigned int c=0; c< data.size();c++) {

//     if ( c != data.size()-1 )
//       out << data[c] << "," ;
//     else
//       out << data[c];

//   }
//   file << out.str() << endl;
//   file.close();


//   //Change the owner and permissions of generated files
//   //system(("chown hsf:hsf " + filePrefix + "_*.csv").data() );
//   if (system(("chmod 666 " + filename + ".csv").data() ))
//   {
//     cout << "error saving file " << endl;
//   }

// }










    // do{
    //   stringstream name2;
    //   name2 << "stage_"<<k+1<<"_slope_"<<i;
    //   int peakT;
    //   Pipeline* p = new Pipeline(path);
    //   name2 << "toff_"<<toff;
    //   Scratch::setName(name2.str());
    //       // unsigned n_cpu = p->getNCPU();
    //   p->initialize();
    //   vector<unsigned> worker_cpu;
    //   worker_cpu.push_back(k);
    //       //worker_cpu.push_back(k+4);
    //   p->setWorkerCPU(worker_cpu);

    //   double ton = activek/(1-activek)*toff + (double)1000/(1-activek)+1000;
    //   vector<unsigned long> vton;
    //   vector<unsigned long> vtoff;
    //       // vton.push_back((unsigned)ton);
    //   vton.push_back((unsigned)ton);
    //   vtoff.push_back(toff-1000);
    //       // vtoff.push_back(toff-1000);
    //   p->setPTMs(vton, vtoff);


    //   Semaphores::print_sem.wait_sem();
    //   cout<<"stage: "<<k<<" |slope: "<<i<<" |toff: "<<toff<<endl;
    //   Semaphores::print_sem.post_sem();
    //   peakT = p->simulate();

    //   vectoff.push_back((double)toff);
    //   Tvstoff.push_back((double)peakT);

    //   delete p;

    //   if (toff<=10000)
    //     toff = toff + 1000;
    //   else if (toff<30000)
    //     toff = toff + 2000;
    //   else if (toff<70000)
    //     toff = toff + 4000;
    //   else if (toff<150000)
    //     toff = toff + 8000;
    //   else if (toff<310000)
    //     toff = toff + 16000;
    //   else
    //     break;
    //   sleep(5);

    // }while(toff < 300000);
    // saveVectorToFile(Tvstoff, name.str());
    // saveVectorToFile(vectoff, name.str());
