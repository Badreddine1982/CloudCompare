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

#ifndef CC_TEST_COLOR_SCALE_HEADER
#define CC_TEST_COLOR_SCALE_HEADER

#include <QObject>
#include <QtTest/QtTest>

//! Unit tests for ccColorScale (and ccColorScaleElement)
class TestCCColorScale : public QObject
{
	Q_OBJECT
  private Q_SLOTS:
	//! A newly created scale should be relative, unlocked and have a valid UUID
	void newScaleDefaultState() const;

	//! Steps should be sorted by relative position when the scale is updated
	void stepsAreSortedOnUpdate() const;

	//! Colors should be linearly interpolated between two steps
	void colorsAreInterpolatedBetweenSteps() const;

	//! Colors should be interpolated inside each interval of a multi-step scale
	void colorsAreInterpolatedInsideEachInterval() const;

	//! Out of range relative positions should return the 'out of range' color
	void outOfRangePositionsReturnTheDefaultColor() const;

	//! The 'steps' version of getColorByRelativePos should quantize the output
	void quantizedColorsMatchTheContinuousOnes() const;

	//! A locked scale should not be modifiable
	void lockedScaleCannotBeModified() const;

	//! Removing a step should update the interpolated colors
	void removingAStepUpdatesTheColors() const;

	//! An absolute scale should convert values to relative positions
	void absoluteScaleConvertsValuesToRelativePositions() const;

	//! An absolute scale should never have a null range
	void absoluteScaleWithNullRangeIsSafe() const;

	//! Copying a scale should preserve its parameters (but not its UUID)
	void copyPreservesTheScaleParameters() const;

	//! Custom labels should be sorted and unique
	void customLabelsAreSortedAndUnique() const;

	//! Saving and re-loading a scale as XML should preserve it
	void xmlRoundTripPreservesTheScale() const;
};

#endif // CC_TEST_COLOR_SCALE_HEADER
