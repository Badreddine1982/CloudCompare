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

#ifndef CC_TEST_SCALAR_FIELD_HEADER
#define CC_TEST_SCALAR_FIELD_HEADER

#include <QObject>
#include <QtTest/QtTest>

//! Unit tests for ccScalarField (and ccScalarField::Range)
class TestCCScalarField : public QObject
{
	Q_OBJECT
  private Q_SLOTS:
	//! Range::setBounds should reset the start/stop values by default
	void rangeBoundsResetTheStartAndStopValues() const;

	//! Range::setBounds should be able to preserve the start/stop values
	void rangeBoundsCanPreserveTheStartAndStopValues() const;

	//! Range start/stop values should always stay inbound and ordered
	void rangeStartAndStopStayInboundAndOrdered() const;

	//! Range::range should never be null
	void rangeIsNeverNull() const;

	//! Values should be tested against the range boundaries
	void rangeTestsInboundAndInRangeValues() const;

	//! computeMinAndMax should update the display and saturation ranges
	void computeMinAndMaxUpdatesTheRanges() const;

	//! computeMinAndMax should ignore the NaN values
	void computeMinAndMaxIgnoresNaNValues() const;

	//! Setting the displayed range should clamp the input values
	void displayedRangeIsClamped() const;

	//! Setting the saturation range should clamp the input values
	void saturationRangeIsClamped() const;

	//! A symmetrical scale should use the biggest absolute value as saturation value
	void symmetricalScaleUsesAbsoluteValues() const;

	//! A logarithmic scale should have its own saturation range
	void logScaleHasItsOwnSaturationRange() const;

	//! Colors should be interpolated over the saturation range
	void colorsAreSpreadOverTheSaturationRange() const;

	//! Values outside of the displayed range should be hidden (or grey)
	void hiddenValuesDependOnTheDisplayRange() const;

	//! Parameters should be importable from another scalar field
	void parametersCanBeImported() const;

	//! Copying a scalar field should preserve its values and parameters
	void copyPreservesValuesAndParameters() const;
};

#endif // CC_TEST_SCALAR_FIELD_HEADER
