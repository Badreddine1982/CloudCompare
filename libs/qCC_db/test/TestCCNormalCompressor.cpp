// ##########################################################################
// #                                                                        #
// #                              CLOUDCOMPARE                              #
// #                                                                        #
// #  This program is free software; you can redistribute it and/or modify  #
// #  it under the terms of the GNU General Public License as published by  #
// #  the Free Software Foundation; version 2 or later of the License.      #
// #                                                                        #
// #  This program is distributed in the hope that it will be useful,       #
// #  but WITHOUT ANY WARRANTY; without even the implied warranty of        #
// #  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          #
// #  GNU General Public License for more details.                          #
// #                                                                        #
// ##########################################################################

#include "TestCCNormalCompressor.h"

#include "ccNormalCompressor.h"

// CCCoreLib
#include <CCConst.h>
#include <CCGeom.h>

// System
#include <cmath>
#include <vector>

//! Maximum angular error (in degrees) introduced by the compression
static constexpr double MAX_ANGULAR_ERROR_DEG = 0.5;

//! Number of directions used by the round trip tests
static constexpr unsigned SAMPLE_COUNT = 32;

static unsigned Compress(const CCVector3& N)
{
	return ccNormalCompressor::Compress(N.u);
}

static CCVector3 Decompress(unsigned code)
{
	CCVector3 N(0, 0, 0);
	ccNormalCompressor::Decompress(code, N.u);
	return N;
}

//! Returns a set of directions spread over the unit sphere
static std::vector<CCVector3> SampleDirections()
{
	std::vector<CCVector3> directions;
	directions.reserve(SAMPLE_COUNT * SAMPLE_COUNT);

	for (unsigned i = 0; i < SAMPLE_COUNT; ++i)
	{
		const double theta = M_PI * (i + 0.5) / SAMPLE_COUNT; // in ]0 ; pi[
		for (unsigned j = 0; j < SAMPLE_COUNT; ++j)
		{
			const double phi = 2.0 * M_PI * j / SAMPLE_COUNT;
			directions.emplace_back(static_cast<PointCoordinateType>(std::sin(theta) * std::cos(phi)),
			                        static_cast<PointCoordinateType>(std::sin(theta) * std::sin(phi)),
			                        static_cast<PointCoordinateType>(std::cos(theta)));
		}
	}

	return directions;
}

void TestCCNormalCompressor::nullNormalIsCompressedAsNullCode() const
{
	QCOMPARE(Compress(CCVector3(0, 0, 0)), static_cast<unsigned>(ccNormalCompressor::NULL_NORM_CODE));
}

void TestCCNormalCompressor::nullCodeIsDecompressedAsNullVector() const
{
	const CCVector3 N = Decompress(ccNormalCompressor::NULL_NORM_CODE);
	QCOMPARE(N.x, 0.0f);
	QCOMPARE(N.y, 0.0f);
	QCOMPARE(N.z, 0.0f);
}

void TestCCNormalCompressor::compressionRoundTripPreservesDirection() const
{
	const double minDotProduct = std::cos(MAX_ANGULAR_ERROR_DEG * M_PI / 180.0);

	for (const CCVector3& N : SampleDirections())
	{
		CCVector3 decompressed = Decompress(Compress(N));
		QVERIFY(decompressed.norm() > 0);
		decompressed.normalize();

		const double dot = N.dot(decompressed);
		QVERIFY2(dot >= minDotProduct,
		         qPrintable(QString("direction (%1 ; %2 ; %3) was decompressed as (%4 ; %5 ; %6)")
		                        .arg(N.x)
		                        .arg(N.y)
		                        .arg(N.z)
		                        .arg(decompressed.x)
		                        .arg(decompressed.y)
		                        .arg(decompressed.z)));
	}
}

void TestCCNormalCompressor::axisAlignedNormalsAreAlmostPreserved() const
{
	const CCVector3 axes[6]{CCVector3(1, 0, 0),
	                        CCVector3(-1, 0, 0),
	                        CCVector3(0, 1, 0),
	                        CCVector3(0, -1, 0),
	                        CCVector3(0, 0, 1),
	                        CCVector3(0, 0, -1)};

	// the compression is lossy: even the axis aligned normals are only preserved
	// up to the quantization accuracy
	constexpr PointCoordinateType maxComponentError = static_cast<PointCoordinateType>(1.0e-2);

	for (const CCVector3& N : axes)
	{
		CCVector3 decompressed = Decompress(Compress(N));
		decompressed.normalize();
		QVERIFY(std::abs(decompressed.x - N.x) < maxComponentError);
		QVERIFY(std::abs(decompressed.y - N.y) < maxComponentError);
		QVERIFY(std::abs(decompressed.z - N.z) < maxComponentError);
	}
}

void TestCCNormalCompressor::compressedCodesAreValid() const
{
	for (const CCVector3& N : SampleDirections())
	{
		QVERIFY(Compress(N) <= static_cast<unsigned>(ccNormalCompressor::MAX_VALID_NORM_CODE));
	}
}

void TestCCNormalCompressor::invertNormalIsSymmetrical() const
{
	for (const CCVector3& N : SampleDirections())
	{
		const CompressedNormType initialCode = static_cast<CompressedNormType>(Compress(N));

		CompressedNormType code = initialCode;
		ccNormalCompressor::InvertNormal(code);
		QVERIFY(code != initialCode);

		ccNormalCompressor::InvertNormal(code);
		QCOMPARE(code, initialCode);
	}
}

void TestCCNormalCompressor::invertNormalInvertsTheDirection() const
{
	for (const CCVector3& N : SampleDirections())
	{
		CompressedNormType code   = static_cast<CompressedNormType>(Compress(N));
		const CCVector3    before = Decompress(code);

		ccNormalCompressor::InvertNormal(code);
		const CCVector3 after = Decompress(code);

		QCOMPARE(after.x, -before.x);
		QCOMPARE(after.y, -before.y);
		QCOMPARE(after.z, -before.z);
	}
}

void TestCCNormalCompressor::invertNormalLeavesNullCodeUnchanged() const
{
	CompressedNormType code = static_cast<CompressedNormType>(ccNormalCompressor::NULL_NORM_CODE);
	ccNormalCompressor::InvertNormal(code);
	QCOMPARE(code, static_cast<CompressedNormType>(ccNormalCompressor::NULL_NORM_CODE));
}

void TestCCNormalCompressor::oppositeNormalsShareTheirQuantizationBits() const
{
	//! Mask of the quantization bits (i.e. all the bits but the 3 'sector' ones)
	constexpr unsigned quantizationMask = (1u << (2 * ccNormalCompressor::QUANTIZE_LEVEL)) - 1;

	for (const CCVector3& N : SampleDirections())
	{
		const unsigned code         = Compress(N);
		const unsigned oppositeCode = Compress(-N);
		QCOMPARE(oppositeCode & quantizationMask, code & quantizationMask);
		QVERIFY(oppositeCode != code);
	}
}

QTEST_APPLESS_MAIN(TestCCNormalCompressor)
