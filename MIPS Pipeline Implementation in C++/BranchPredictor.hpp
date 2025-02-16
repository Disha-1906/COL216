#ifndef _BRANCH_PREDICTOR_HPP_
#define _BRANCH_PREDICTOR_HPP_

#include <vector>
#include <bitset>
#include <cassert>

struct BranchPredictor {
    virtual bool predict(uint32_t pc) = 0;
    virtual void update(uint32_t pc, bool taken) = 0;
};

struct SaturatingBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> table;
    SaturatingBranchPredictor(int value) : table(1 << 14, value) {}
       
    bool predict(uint32_t pc) {

        // your code here
        int lsb_14=pc & 0x3FFF;
        if(table.at(lsb_14) == 00 || table.at(lsb_14) == 01){
            return false;
        }
        else{
            return true;
        }
        
    }
    void update(uint32_t pc, bool taken) {
        // your code here
        int lsb_14=pc & 0x3FFF;
        if(table.at(lsb_14)==00){
            if(taken == true){table.at(lsb_14)=01;}
            else{table.at(lsb_14)=00;}
        }
        else if(table.at(lsb_14)==01){
            if(taken == true){table.at(lsb_14)=10;}
            else{table.at(lsb_14)=00;}
        }
        else if(table.at(lsb_14)==10){
            if(taken == true){table.at(lsb_14)=11;}
            else{table.at(lsb_14)=01;}
        }
        else if(table.at(lsb_14)==11){
            if(taken == true){table.at(lsb_14)=11;}
            else{table.at(lsb_14)=10;}
        }
    }
};

struct BHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    BHRBranchPredictor(int value) : bhrTable(1 << 2, value), bhr(value) {}
    bool predict(uint32_t pc) {
        // your code here
        int index;
        if(bhr == 00){ index=0;}
        else if(bhr == 01){index=1;}
        else if(bhr == 10){index=2;}
        else if(bhr == 11){index=3;}
        if(bhrTable.at(index) == 00 || bhrTable.at(index) == 01){
            return false;
        }
        else{
            return true;
            }
    }

    void update(uint32_t pc, bool taken) {
        // your code here
        int index;
        if(bhr == 00){ index=0;}
        else if(bhr == 01){index=1;}
        else if(bhr == 10){index=2;}
        else if(bhr == 11){index=3;}
        if(bhrTable.at(index) == 00){
            if(taken == true){
                bhrTable.at(index) = 01;
                bhr[1]=bhr[0];
                bhr[0]=1;
            }
            else{
                bhrTable.at(index) = 00;
                bhr[1]=bhr[0];
                bhr[0]=0;
            }
        }
        if(bhrTable.at(index) == 01){
            if(taken == true){
                bhrTable.at(index) = 10;
                bhr[1]=bhr[0];
                bhr[0]=1;
            }
            else{
                bhrTable.at(index) = 00;
                bhr[1]=bhr[0];
                bhr[0]=0;
            }
        }
        if(bhrTable.at(index) == 10){
            if(taken == true){
                bhrTable.at(index) = 11;
                bhr[1]=bhr[0];
                bhr[0]=1;
            }
            else{
                bhrTable.at(index) = 01;
                bhr[1]=bhr[0];
                bhr[0]=0;
            }
        }
        if(bhrTable.at(index) == 11){
            if(taken == true){
                bhrTable.at(index) = 11;
                bhr[1]=bhr[0];
                bhr[0]=1;
            }
            else{
                bhrTable.at(index) = 10;
                bhr[1]=bhr[0];
                bhr[0]=0;
            }
        }

    }
};

struct SaturatingBHRBranchPredictor : public BranchPredictor {
    std::vector<std::bitset<2>> bhrTable;
    std::bitset<2> bhr;
    std::vector<std::bitset<2>> table;
    std::vector<std::bitset<2>> combination;
    SaturatingBHRBranchPredictor(int value, int size) : bhrTable(1 << 2, value), bhr(value), table(1 << 14, value), combination(size, value) {
        assert(size <= (1 << 16));
    }
    bool predict(uint32_t pc) {
        // your code here
        //-------------------BHR predicted value----------------------
        int index;
        bool pred_sc, pred_bhr;
        if(bhr == 00){ index=0;}
        else if(bhr == 01){index=1;}
        else if(bhr == 10){index=2;}
        else if(bhr == 11){index=3;}
        if(bhrTable.at(index) == 00 || bhrTable.at(index) == 01){
            pred_bhr= false;
        }
        else{
            pred_bhr=true;
            }
        //-------------------------------------------------------------
        //-------------------SC predicted value-------------------------
        int lsb_14=pc & 0x3FFF;
        if(table.at(lsb_14) == 00 || table.at(lsb_14) == 01){
            pred_sc= false;
        }
        else{
            pred_sc= true;
        }
        //--------------------------------------------------------------
        if(combination.at(lsb_14) == 01 || combination.at(lsb_14) == 00){
            return pred_bhr;
        }
        else{
            return pred_sc;
        }
    }

    void update(uint32_t pc, bool taken) {
        // your code here

        //---------------------Sc predicted value------------------------------------
        int lsb_14=pc & 0x3FFF;
        bool pred_sc, pred_bhr;
        if(table.at(lsb_14) == 00 || table.at(lsb_14) == 01){
            pred_sc= false;
        }
        else{
            pred_sc= true;
        }
        //----------------------------------------------------------------------------
        //----------------------BHR predicted valUE---------------------------------
        int index;
        if(bhr == 00){ index=0;}
        else if(bhr == 01){index=1;}
        else if(bhr == 10){index=2;}
        else if(bhr == 11){index=3;}
        if(bhrTable.at(index) == 00 || bhrTable.at(index) == 01){
            pred_bhr= false;
        }
        else{
            pred_bhr=true;
            }
        //---------------------------------------------------------------------------
        //--------------------BHR update-----------------------------------------------------
        if(bhr == 00){ index=0;}
        else if(bhr == 01){index=1;}
        else if(bhr == 10){index=2;}
        else if(bhr == 11){index=3;}
        if(bhrTable.at(index) == 00){
            if(taken == true){
                bhrTable.at(index) = 01;
                bhr[1]=bhr[0];
                bhr[0]=1;
            }
            else{
                bhrTable.at(index) = 00;
                bhr[1]=bhr[0];
                bhr[0]=0;
            }
        }
        else if(bhrTable.at(index) == 01){
            if(taken == true){
                bhrTable.at(index) = 10;
                bhr[1]=bhr[0];
                bhr[0]=1;
            }
            else{
                bhrTable.at(index) = 00;
                bhr[1]=bhr[0];
                bhr[0]=0;
            }
        }
        else if(bhrTable.at(index) == 10){
            if(taken == true){
                bhrTable.at(index) = 11;
                bhr[1]=bhr[0];
                bhr[0]=1;
            }
            else{
                bhrTable.at(index) = 01;
                bhr[1]=bhr[0];
                bhr[0]=0;
            }
        }
        else if(bhrTable.at(index) == 11){
            if(taken == true){
                bhrTable.at(index) = 11;
                bhr[1]=bhr[0];
                bhr[0]=1;
            }
            else{
                bhrTable.at(index) = 10;
                bhr[1]=bhr[0];
                bhr[0]=0;
            }
        }
        //-------------------------------------------------------------------------------------
        //----------------------Sc update------------------------------------------------------
        if(table.at(lsb_14)==00){
            if(taken == true){table.at(lsb_14)=01;}
            else{table.at(lsb_14)=00;}
        }
        else if(table.at(lsb_14)==01){
            if(taken == true){table.at(lsb_14)=10;}
            else{table.at(lsb_14)=00;}
        }
        else if(table.at(lsb_14)==10){
            if(taken == true){table.at(lsb_14)=11;}
            else{table.at(lsb_14)=01;}
        }
        else if(table.at(lsb_14)==11){
            if(taken == true){table.at(lsb_14)=11;}
            else{table.at(lsb_14)=10;}
        }
        //-------------------------------------------------------------------------------------
        //-------------------------------Combination update--------------------------------------
        if(combination.at(lsb_14)==00){
            if((pred_sc == taken) && (pred_bhr != taken)){
                combination.at(lsb_14)==01;
            }
            else if((pred_sc != taken) && (pred_bhr == taken)){
                combination.at(lsb_14)==00;
            }
            }
        else if(combination.at(lsb_14)==01){
            if((pred_sc == taken) && (pred_bhr != taken)){
                combination.at(lsb_14)==10;
            }
            else if((pred_sc != taken) && (pred_bhr == taken)){
                combination.at(lsb_14)==00;
            }
            }
        else if(combination.at(lsb_14)==10){
            if((pred_sc == taken) && (pred_bhr != taken)){
                combination.at(lsb_14)==11;
            }
            else if((pred_sc != taken) && (pred_bhr == taken)){
                combination.at(lsb_14)==01;
            }
            }
        else if(combination.at(lsb_14)==11){
            if((pred_sc == taken) && (pred_bhr != taken)){
                combination.at(lsb_14)==11;
            }
            else if((pred_sc != taken) && (pred_bhr == taken)){
                combination.at(lsb_14)==10;
            }
            }
        }
        //-------------------------------------------------------------------------------------
    };
#endif
