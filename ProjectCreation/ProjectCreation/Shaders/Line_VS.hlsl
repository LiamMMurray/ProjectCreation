#include "MVPBuffer.hlsl"
struct OUTPUT_VERTEX
{
        float4 Pos : SV_POSITION;
        uint   ColorID : COLOR;
};

struct INPUT_VERTEX
{
        float4 Pos : POSITION;
        uint   ColorID : COLOR;
};

OUTPUT_VERTEX
main(INPUT_VERTEX vIn)
{
        OUTPUT_VERTEX output = (OUTPUT_VERTEX)0;
        output.Pos           = vIn.Pos;
        output.ColorID       = vIn.ColorID;

        return output;
}