// Credit - Lari Norri

// This file defines HLSL specific functions & structures shared across shaders.

// New "Intrinsics" defined by this file:

// wrap : wraps a 1 to 4 component vector value between a specified range. (ex: UV wrapping)

// Start Intrinsic defenitions

// will generate proper function over-loads ala shader intrinsics
#define WRAP(numC)                                                                                             \
        vector<float, numC> wrap(vector<float, numC> _val, vector<float, numC> _min, vector<float, numC> _max) \
        {                                                                                                      \
                _val -= _min;                                                                                  \
                _val = _val / (_max - _min) + 1.192092896e-07F;                                                \
                _val -= floor(_val);                                                                           \
                return _val * (_max - _min) + _min;                                                            \
        }

// "wrap" operation which ensures a vector is "wrapped" within an upper and lower bound.
// "_min" and "_max" are INCLUSIVE. Wrapping is preformed per-component
WRAP(1)
WRAP(2)
WRAP(3)
WRAP(4)
