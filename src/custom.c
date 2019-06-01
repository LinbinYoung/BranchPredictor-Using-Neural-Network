#include <stdio.h>

uint32_t Global_History_custom;
uint8_t* PHT_tor_custom;
uint32_t PHT_SIZE_custom;
uint32_t gmask_tor_custom;

uint32_t* LHT_custom;
uint32_t LHT_SIZE_custom;
uint32_t pcmask_custom;
uint32_t lmask_custom;

uint8_t* CPT_custom;
int32_t** WEIGHT;

void initialized_CUSTOM(){
    // Compared with tournament, the network methods adopt
    Global_History_custom = 0;
    PHT_SIZE_custom = 1 << ghistoryBits;
    gmask_tor_custom = PHT_SIZE_custom - 1;
    lmask_custom = (1 << lhistoryBits) - 1;
    PHT_tor_custom = (uint8_t*)malloc(sizeof(uint8_t)*PHT_SIZE_custom);
    for (int i = 0; i < PHT_SIZE_custom; i++){
        PHT_tor_custom[i] = 1;  
    }

    LHT_SIZE_custom = 1 << pcIndexBits;
    pcmask_custom = LHT_SIZE_custom - 1;
    LHT_custom = (uint32_t*)malloc(sizeof(uint32_t)*LHT_SIZE_custom);
    for (int i = 0; i < LHT_SIZE_custom; i++){
        LHT_custom[i] = 0;
    }

    CPT_custom = (uint8_t*)malloc(sizeof(uint8_t)*PHT_SIZE_custom);
    for (int i = 0; i < PHT_SIZE_custom; i++){
        CPT_custom[i] = 2;
    }

    //intialized the two-dimension weight array
    WEIGHT = (int32_t**)malloc(sizeof(int32_t*)*LHT_SIZE_custom);
    for (int i = 0; i < LHT_SIZE_custom; i ++){
        WEIGHT[i] = (int32_t*)malloc(sizeof(int32_t)*(lhistoryBits + 1));
    }//end for
}

uint8_t Choice_maker_custom(){
    uint32_t index_c = Global_History_custom & gmask_tor_custom;
    uint8_t res_choice = CPT_custom[index_c];
    if (res_choice < 2){
        // 0, 1
        return 0;
    }else{
        // 2, 3
        return 1;
    }
}

uint8_t Get_Global_Res_custom(){
    uint32_t index = Global_History_custom & gmask_tor_custom;
    uint8_t global_res = PHT_tor_custom[index];
    return global_res;
}

int32_t Perceptron_predict(uint32_t pc){
    uint32_t index_c = pc & pcmask_custom;
    int32_t* weight_for_branch = WEIGHT[index_c];
    uint32_t history_record = LHT_custom[index_c] & lmask_custom;
    int32_t sum = weight_for_branch[0];
    for (int i = 0; i < lhistoryBits; i ++){
        int32_t res = (history_record >> i) & 1;
        if (res == 0){
            res = -1;
        }
        sum = sum + weight_for_branch[i+1] * res;
    }
    return sum;
}

uint8_t CUSTOM_predictor(uint32_t pc){
    uint8_t global_res = Get_Global_Res_custom();
    uint8_t our_choice = Choice_maker_custom();
    int32_t sum = Perceptron_predict(pc);
    if (our_choice == 0){
        if (global_res > 1){
            return TAKEN;
        }else{
            return NOTTAKEN;
        }
    }else{
        if (sum > 0){
            return TAKEN;
        }else{
            return NOTTAKEN;
        }
    }
}

void train_CUSTOM(uint32_t pc, uint8_t outcome){
    //get all the necessary indexs and predictions before any modification
    uint32_t index = Global_History_custom & gmask_tor_custom;
    uint8_t global_res = PHT_tor_custom[index];
    uint32_t index_c = pc & pcmask_custom;
    int32_t* weight_for_branch = WEIGHT[index_c];
    uint32_t history_record = LHT_custom[index_c] & lmask_custom;
    int32_t sum = weight_for_branch[0];
    for (int i = 0; i < lhistoryBits; i ++){
        int32_t res = (history_record >> i) & 1;
        if (res == 0){
            res = -1;
        }
        sum = sum + weight_for_branch[i+1] * res;
    }
    uint8_t lres;
    uint8_t gres;
    if (global_res > 1){
        gres = TAKEN;
    }else{
        gres = NOTTAKEN;
    }
    if (sum > 0){
        lres = TAKEN;
    }else{
        lres = NOTTAKEN;
    }
    //update wieight
    int32_t actual_outcome = 1;
    if (outcome == 0){
        actual_outcome = -1;
    }
    if (outcome != lres){
        weight_for_branch[0] = weight_for_branch[0] + actual_outcome*1;
        for (int i = 0; i < lhistoryBits; i ++){
            int32_t his = 1;
            uint32_t record = (history_record >> i) & 1;
            if (record == 0){
                his = -1;
            }
            weight_for_branch[i+1] = weight_for_branch[i+1] + actual_outcome*his;
        }
        WEIGHT[index_c] = weight_for_branch;
    }
    //update CPT
    if (lres != outcome && gres == outcome){
        if (CPT_custom[index] != 0){
            CPT_custom[index] = CPT_custom[index] - 1;
        }
    }else if (lres == outcome && gres != outcome){
        if (CPT_custom[index] != 3){
            CPT_custom[index] = CPT_custom[index] + 1;
        }
    }
    //update PHT_or
    if (outcome == TAKEN){
        if (PHT_tor_custom[index] != 3){
            PHT_tor_custom[index] = PHT_tor_custom[index] + 1;
        }
    }else{
        if (PHT_tor_custom[index] != 0){
            PHT_tor_custom[index] = PHT_tor_custom[index] - 1;
        }
    }
    // update record
    history_record = (history_record << 1 | outcome) & lmask_custom;
    LHT_custom[index_c] = history_record;
    Global_History_custom = (Global_History_custom << 1 | outcome) & gmask_tor_custom;
}
