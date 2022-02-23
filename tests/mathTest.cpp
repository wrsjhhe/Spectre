#include "gtest/gtest.h"
#include "SpectreDefs.h"
#include "DirectXMath.h"

USING_NAMESPACE(DirectX)

TEST(TestMat, matrix)
{
    XMMATRIX mat1(0., 0., 0., 0.,
        0., 0., 0., 0.,
        0., 0., 0., 0.,
        1., 0., 0., 0.);

    XMMATRIX mat2(0., 0., 0., 0.,
        0., 0., 0., 0.,
        0., 0., 0., 0.,
        2., 0., 0., 0.);

    mat1 += mat2;

    EXPECT_TRUE(mat1.r[3].m128_f32[0] == 3.);
}