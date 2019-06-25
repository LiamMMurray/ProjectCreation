
struct VS_OUTPUT
{
        uint id : ID;
};
VS_OUTPUT main(uint id : SV_VERTEXID)
{
        VS_OUTPUT output = (VS_OUTPUT)0;
        output.id        = id;
        return output;
}