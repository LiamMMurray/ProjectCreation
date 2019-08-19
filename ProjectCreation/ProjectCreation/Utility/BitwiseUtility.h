#pragma once
inline namespace BitwiseUtility
{
        // nextPowerOf2 is from the link below
        // https://www.geeksforgeeks.org/smallest-power-of-2-greater-than-or-equal-to-n/
        // Added an if check to return if n is already a power of 2
        inline constexpr unsigned int nextPowerOf2(unsigned int n)
        {
                if (!(n & (n - 1)))
                        return n;
                n--;
                n |= n >> 1;
                n |= n >> 2;
                n |= n >> 4;
                n |= n >> 8;
                n |= n >> 16;
                n++;
                return n;
                // This code is contributed by SHUBHAMSINGH10
        }

        //  gets the next power of 2 that is a factor of N which is assumed to be a power of 2.
        inline constexpr unsigned int nextPwrOf2FactorOfOtherPowerOf2(unsigned input, unsigned otherPowerOf2)
        {
                return nextPowerOf2(input + (otherPowerOf2 & input - 1));
        }
} // namespace BitwiseUtility