//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Linbin Yang";
const char *studentID   = "A53277054";
const char *email       = "sheeplinbean@163.com";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

#include "custom.c"

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//


uint32_t Global_History_gshare;
uint8_t* PHT_gshare;
uint32_t PHT_SIZE_gshare;
uint32_t gmask_gshare;

uint32_t Global_History;
uint8_t* PHT_tor;
uint32_t PHT_SIZE;
uint32_t gmask_tor;

uint32_t* LHT;
uint32_t LHT_SIZE;
uint32_t pcmask;

uint8_t* LPT;
uint32_t LPT_SIZE;
uint32_t lmask;

uint8_t* CPT;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor

void initialized_STATIC(){

}

void initialized_GSHARE(){
    Global_History_gshare = 0;
    PHT_SIZE_gshare = 1 << ghistoryBits;
    gmask_gshare = PHT_SIZE_gshare - 1;
    PHT_gshare = (uint8_t*)malloc(sizeof(uint8_t)*PHT_SIZE_gshare);
    for (int i = 0; i < PHT_SIZE_gshare; i++){
        PHT_gshare[i] = 1;
    }
}

void initialized_TOURNAMENT(){
    Global_History = 0;
    PHT_SIZE = 1 << ghistoryBits;
    gmask_tor = PHT_SIZE - 1;
    PHT_tor = (uint8_t*)malloc(sizeof(uint8_t)*PHT_SIZE);
    for (int i = 0; i < PHT_SIZE; i++){
        PHT_tor[i] = 1;  
    }
  
    LHT_SIZE = 1 << pcIndexBits;
    pcmask = LHT_SIZE - 1;
    LHT = (uint32_t*)malloc(sizeof(uint32_t)*LHT_SIZE);
    for (int i = 0; i < LHT_SIZE; i++){
        LHT[i] = 0;
    }
  
    LPT_SIZE = 1 << lhistoryBits;
    lmask = LPT_SIZE - 1;
    LPT = (uint8_t*)malloc(sizeof(uint8_t)*LPT_SIZE);
    for (int i = 0; i < LPT_SIZE; i++){
        LPT[i] = 1;
    }
  
    CPT = (uint8_t*)malloc(sizeof(uint8_t)*PHT_SIZE);
    for (int i = 0; i < PHT_SIZE; i++){
        CPT[i] = 1;
    }
}

void init_predictor()
{
  switch (bpType){
    case STATIC:
      initialized_STATIC();
    case GSHARE:
      initialized_GSHARE();
    case TOURNAMENT:
      initialized_TOURNAMENT();
    case CUSTOM:
      initialized_CUSTOM();
    default:
      break;
  }
  return;
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//

uint8_t GSHARE_predictor(uint32_t pc){
    uint32_t index = (pc ^ Global_History_gshare) & gmask_gshare;
    uint8_t res = PHT_gshare[index];
    if (res > 1){
        return TAKEN;
    }else{
        return NOTTAKEN;
    }
}

uint8_t Choice_maker(){
    uint32_t index_c = Global_History & gmask_tor;
    uint8_t res_choice = CPT[index_c];
    if (res_choice < 2){
        // 0, 1
        return 0;
    }else{
        // 2, 3
        return 1;
    }
}

uint8_t Get_Local_Res(uint32_t pc){
    uint32_t histroy_index = pc & pcmask;
    uint32_t history_record = LHT[histroy_index] & lmask;
    uint8_t local_res = LPT[history_record];
    return local_res;
}

uint8_t Get_Global_Res(){
    uint32_t index = Global_History & gmask_tor;
    uint8_t global_res = PHT_tor[index];
    return global_res;
}

uint8_t TOURNAMENT_predictor(uint32_t pc){
    uint8_t local_res = Get_Local_Res(pc);
    uint8_t global_res = Get_Global_Res();
    uint8_t our_choice = Choice_maker();
    if (our_choice == 0){
        if (global_res > 1){
            return TAKEN;             
        }else{
            return NOTTAKEN;
        }
    }else{
        if (local_res > 1){
            return TAKEN;
        }else{
            return NOTTAKEN;
        }
    }
}

uint8_t make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
      return GSHARE_predictor(pc);
    case TOURNAMENT:
      return TOURNAMENT_predictor(pc);
    case CUSTOM:
      return CUSTOM_predictor(pc);
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void train_STATIC(uint32_t pc, uint8_t outcome){

}

void train_GSHARE(uint32_t pc, uint8_t outcome){
    uint32_t index = (pc ^ Global_History_gshare) & gmask_gshare;
    if (outcome == TAKEN){
        if (PHT_gshare[index] != 3){
            PHT_gshare[index] = PHT_gshare[index] + 1;
        }  
    }else{
        if (PHT_gshare[index] != 0){
            PHT_gshare[index] = PHT_gshare[index] - 1;
        }
    }
    Global_History_gshare = (Global_History_gshare << 1 | outcome) & gmask_gshare;
}


void train_TOURNAMENT(uint32_t pc, uint8_t outcome){
    uint32_t histroy_index = pc & pcmask;
    uint32_t history_record = LHT[histroy_index] & lmask;
    uint8_t local_res = LPT[history_record];
    uint32_t index = Global_History & gmask_tor;
    uint8_t global_res = PHT_tor[index];
    uint8_t lpre = 0;
    uint8_t gpre = 0;
    if (global_res > 1){
        gpre = TAKEN;
    }else{
        gpre = NOTTAKEN;
    }
    if (local_res > 1){
        lpre = TAKEN;
    }else{
        lpre = NOTTAKEN;
    }
    if (outcome == TAKEN){
        if (LPT[history_record] != 3){
            LPT[history_record] = LPT[history_record] + 1;
        }
    }else{
        if (LPT[history_record] != 0){
            LPT[history_record] = LPT[history_record] - 1;
        }
    }
    if (gpre == outcome && lpre != outcome){
        if (CPT[index] != 0){
            CPT[index] = CPT[index] - 1;
        }
    }else if(gpre != outcome && lpre == outcome){
        if (CPT[index] != 3){
            CPT[index] = CPT[index] + 1;
        }
    }
    if (outcome == TAKEN){
        if (PHT_tor[index] != 3){
            PHT_tor[index] = PHT_tor[index] + 1;
        }
    }else{
        if (PHT_tor[index] != 0){
            PHT_tor[index] = PHT_tor[index] - 1;
        }
    }
    //update history record
    history_record = (history_record << 1 | outcome) & lmask;
    LHT[histroy_index] = history_record;
    Global_History = (Global_History << 1 | outcome) & gmask_tor;
}



void train_predictor(uint32_t pc, uint8_t outcome)
{
  switch (bpType){
    case STATIC:
      train_STATIC(pc, outcome);
    case GSHARE:
      train_GSHARE(pc, outcome);
    case TOURNAMENT:
      train_TOURNAMENT(pc, outcome);
    case CUSTOM:
      train_CUSTOM(pc, outcome);
    default:
      break;
  }
  return;
}
