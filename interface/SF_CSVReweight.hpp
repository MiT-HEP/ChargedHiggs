#ifndef SF_CSVReweight_H
#define SF_CSVReweight_H

#include "interface/SF.hpp"
#include "interface/CSVHelper.hpp"

#include <memory>

class SF_CSVReweight : public SF_Asymm
{
    private:
        std::unique_ptr<CSVHelper> helper_;

        std::vector<double> pt_;
        std::vector<double> eta_;
        std::vector<double> csv_;
        std::vector<int>    flavor_;

        double csvWgtHF,csvWgtLF,csvWgtCF;

    public:
        SF_CSVReweight(){}
        void print(){};
        void init();
        void set();
        void add_to_sf( float pt, float eta, float csv, int flavor);
        const string name() const override {return "SF_CSVReweight";}
        void clearEvent() override { pt_.clear(); eta_.clear(); csv_.clear(); flavor_.clear(); }

};

// Local Variables:
// mode:c++
// indent-tabs-mode:nil
// tab-width:4
// c-basic-offset:4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4 

#endif
