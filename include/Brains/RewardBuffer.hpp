//
//  RewardBuffer.hpp
//  BrainSim
//
//  Created by Sumner Hearth on 11/21/15.
//
//

#ifndef RewardBuffer_hpp
#define RewardBuffer_hpp

#include "Utility/NeuralUtility.hpp"
namespace neuronutility{

    // Only stores the input, must recalculate output
    // Less memory copying, can be implemented at the
    // Creature level
    class SlowRewardBuffer{
    public:
        ~SlowRewardBuffer(){destroy();};
        /* Initializes buffer, does not own layers */
        void init(NeuronLayer* layers, int numlayers,
                  int inputlen, int bufferlen);
        void destroy();
        // Copies the inputarray, does not own
        void log(int* inputarray);

        // Reward is X*factor, (X-1)*factor, ...
        void reward(int x, double factor);
        // Rewards defined by array rewards[0] is most recent
        void reward(double* rewards, int len);
    private:
        // Rewards input at loc by val
        void rewardAt(int loc,double val);
        // Used for rewarding only
        NeuronLayer* m_layers;
        int m_numlayers;
        int m_length = 0;
        int m_inputlen = 0;
        int m_head = 0;
        int m_size = 0;
        // Stored as [dirty,
        int** m_information;
    };
}

#endif /* RewardBuffer_hpp */
