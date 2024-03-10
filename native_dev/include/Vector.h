struct Vec4{
    float                                                     x, y, z, w;
    constexpr Vec4()                                        : x(0.0f), y(0.0f), z(0.0f), w(0.0f) { }
    constexpr Vec4(float _x, float _y, float _z, float _w)  : x(_x), y(_y), z(_z), w(_w) { }
};