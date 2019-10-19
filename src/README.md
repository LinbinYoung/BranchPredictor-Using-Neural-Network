# BranchPredictor-Using-Neural-Network
* 状态机器（State Machine）1: Strongly Not Taken 2:  Weakly Not Taken 3: Weakly Taken 4: Strongly Taken
* GShare分支预测，pcIndex, globalHistoryBits, Global History Table |  Index = (pc ^ globalHistoryBit) & Global_History_Mask, 全局的状态对于此分支预测结果的影响是什么？全局的状态用GlobalHistoryBit来表示，针对于特定的分支则是用pc来表示。这个size为2^globalHistoryLen 的Table存储了不同全局状态下对于某一个分支器它的预测结果是什么。训练过程则是更新GlobalHistoryBits和对应Table中的中的状态，按照上边状态机中的转移法则。
* Tournament分支预测包含了Gshare法则但是也把分支自己的History考虑进去了。为了决定是用Local History 还是 Global History，Tournament方法中还有一个Choice Maker Table, 这是用Global History来Index的。同样做选择也是遵循状态转换的。
* Custom分支预测(感知机): 感知机以每一个branch的LocalHistoryBit为输入，输出结果为正则预测Taken否则为Not Taken. 每次训练如果预测值和实际输出结果不同，那么就decrement the weight 否则就 increase the weight。
