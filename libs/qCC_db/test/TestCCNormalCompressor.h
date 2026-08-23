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

#ifndef CC_TEST_NORMAL_COMPRESSOR_HEADER
#define CC_TEST_NORMAL_COMPRESSOR_HEADER

#include <QObject>
#include <QtTest/QtTest>

//! Unit tests for ccNormalCompressor
class TestCCNormalCompressor : public QObject
{
	Q_OBJECT
  private Q_SLOTS:
	//! A null vector should be compressed as the 'null normal' code
	void nullNormalIsCompressedAsNullCode() const;

	//! Decompressing the 'null normal' code should give a null vector
	void nullCodeIsDecompressedAsNullVector() const;

	//! Compressing then decompressing a normal should preserve its direction
	void compressionRoundTripPreservesDirection() const;

	//! All the axis-aligned directions should be preserved up to the quantization accuracy
	void axisAlignedNormalsAreAlmostPreserved() const;

	//! Compression should always output a valid code
	void compressedCodesAreValid() const;

	//! Inverting a code twice should give the initial code back
	void invertNormalIsSymmetrical() const;

	//! Inverting a code should invert the decompressed normal
	void invertNormalInvertsTheDirection() const;

	//! Inverting the 'null normal' code should be a no-op
	void invertNormalLeavesNullCodeUnchanged() const;

	//! The code of a normal and of its opposite should differ by the sector bits only
	void oppositeNormalsShareTheirQuantizationBits() const;
};

#endif // CC_TEST_NORMAL_COMPRESSOR_HEADER
