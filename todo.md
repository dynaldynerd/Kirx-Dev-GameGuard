# TODO
1. fix any raw offset usage and convert them to struct type (not limited to packet/DQS query, but all class including class that derived from other class and the base class is trying to access the derived variable by using raw offset/array index + offset)
2. recreate any function that still had a v1,v2,v3 variable name to a sane variable name and modern C++
3. recreate function that still using goto and label
4. replace hex number to decimal when the hex make no sense, example: PushDQSData(0xFFFFFFFF) to PushDQSData(-1), case 0xA to case 10, etc
5. change compare a single char to a text instead, example : if(String[0] == 42) to if(String[0] == '*') if the string make more sense
6. use enum for any effect usage of _effect_parameter (this one had m_fEff_Rate[62], m_fEff_Plus[42], m_bEff_State[29], m_fEff_Have[83], each had their own effect usage, so we had to create enum for each item and array, for example m_fEff_Plus[ATTACK_INCREASE], where ATTACK_INCREASE value is for example 6 and is enum for increased attack)