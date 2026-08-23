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

#include "TestCCColorScale.h"

#include "ccColorScale.h"

// Qt
#include <QTemporaryDir>

// System
#include <cmath>

static void CompareColors(const ccColor::Rgb& actual, const ccColor::Rgb& expected, int tolerance = 0)
{
	QVERIFY(std::abs(static_cast<int>(actual.r) - static_cast<int>(expected.r)) <= tolerance);
	QVERIFY(std::abs(static_cast<int>(actual.g) - static_cast<int>(expected.g)) <= tolerance);
	QVERIFY(std::abs(static_cast<int>(actual.b) - static_cast<int>(expected.b)) <= tolerance);
}

//! Creates a black to white scale
static ccColorScale::Shared CreateGreyScale()
{
	ccColorScale::Shared scale = ccColorScale::Create("grey");
	scale->insert(ccColorScaleElement(0.0, Qt::black), false);
	scale->insert(ccColorScaleElement(1.0, Qt::white), true);
	return scale;
}

void TestCCColorScale::newScaleDefaultState() const
{
	ccColorScale::Shared scale = ccColorScale::Create("my scale");

	QVERIFY(scale);
	QCOMPARE(scale->getName(), QString("my scale"));
	QVERIFY(!scale->getUuid().isEmpty());
	QVERIFY(scale->isRelative());
	QVERIFY(!scale->isLocked());
	QCOMPARE(scale->stepCount(), 0);
	QVERIFY(scale->customLabels().empty());

	const QString previousUuid = scale->getUuid();
	scale->generateNewUuid();
	QVERIFY(scale->getUuid() != previousUuid);
}

void TestCCColorScale::stepsAreSortedOnUpdate() const
{
	ccColorScale::Shared scale = ccColorScale::Create("unsorted");
	scale->insert(ccColorScaleElement(1.0, Qt::white), false);
	scale->insert(ccColorScaleElement(0.5, Qt::red), false);
	scale->insert(ccColorScaleElement(0.0, Qt::black), false);
	scale->update();

	QCOMPARE(scale->stepCount(), 3);
	QCOMPARE(scale->step(0).getRelativePos(), 0.0);
	QCOMPARE(scale->step(1).getRelativePos(), 0.5);
	QCOMPARE(scale->step(2).getRelativePos(), 1.0);
}

void TestCCColorScale::colorsAreInterpolatedBetweenSteps() const
{
	ccColorScale::Shared scale = CreateGreyScale();

	QCOMPARE(scale->stepCount(), 2);

	CompareColors(*scale->getColorByRelativePos(0.0), ccColor::black);
	CompareColors(*scale->getColorByRelativePos(1.0), ccColor::white);
	CompareColors(*scale->getColorByRelativePos(0.5), ccColor::Rgb(127, 127, 127), 1);

	// the grey level should never decrease
	for (unsigned i = 1; i < ccColorScale::MAX_STEPS; ++i)
	{
		QVERIFY(scale->getColorByIndex(i).r >= scale->getColorByIndex(i - 1).r);
	}
}

void TestCCColorScale::colorsAreInterpolatedInsideEachInterval() const
{
	// black --> red (at 1/4) --> white
	ccColorScale::Shared scale = ccColorScale::Create("3 steps");
	scale->insert(ccColorScaleElement(0.0, Qt::black), false);
	scale->insert(ccColorScaleElement(0.25, Qt::red), false);
	scale->insert(ccColorScaleElement(1.0, Qt::white), true);

	CompareColors(*scale->getColorByRelativePos(0.25), ccColor::Rgb(255, 0, 0), 1);
	// half way inside the first interval: half of the red component, no green nor blue
	CompareColors(*scale->getColorByRelativePos(0.125), ccColor::Rgb(127, 0, 0), 1);
	// half way inside the second interval: full red, half of the green and blue components
	CompareColors(*scale->getColorByRelativePos(0.625), ccColor::Rgb(255, 127, 127), 1);
}

void TestCCColorScale::outOfRangePositionsReturnTheDefaultColor() const
{
	ccColorScale::Shared scale = CreateGreyScale();

	QVERIFY(scale->getColorByRelativePos(-0.001) == nullptr);
	QVERIFY(scale->getColorByRelativePos(1.001) == nullptr);

	const ccColor::Rgb outOfRangeColor(1, 2, 3);
	QCOMPARE(scale->getColorByRelativePos(-0.001, &outOfRangeColor), &outOfRangeColor);
	QCOMPARE(scale->getColorByRelativePos(1.001, ccColorScale::DEFAULT_STEPS, &outOfRangeColor), &outOfRangeColor);
}

void TestCCColorScale::quantizedColorsMatchTheContinuousOnes() const
{
	ccColorScale::Shared scale = CreateGreyScale();

	constexpr unsigned steps = 8;
	for (unsigned i = 0; i < steps; ++i)
	{
		// all the positions inside a given interval should give the same color
		const double       intervalStart = static_cast<double>(i) / steps;
		const ccColor::Rgb expectedColor = *scale->getColorByRelativePos(intervalStart + 0.1 / steps, steps);
		CompareColors(*scale->getColorByRelativePos(intervalStart + 0.5 / steps, steps), expectedColor);
		CompareColors(*scale->getColorByRelativePos(intervalStart + 0.9 / steps, steps), expectedColor);

		// and this color is the 'continuous' one at the beginning of the interval
		CompareColors(expectedColor, *scale->getColorByRelativePos(intervalStart), 1);
	}
}

void TestCCColorScale::lockedScaleCannotBeModified() const
{
	ccColorScale::Shared scale = CreateGreyScale();
	scale->setLocked(true);
	QVERIFY(scale->isLocked());

	scale->insert(ccColorScaleElement(0.5, Qt::red));
	QCOMPARE(scale->stepCount(), 2);

	scale->remove(0);
	QCOMPARE(scale->stepCount(), 2);

	scale->clear();
	QCOMPARE(scale->stepCount(), 2);

	scale->setLocked(false);
	scale->insert(ccColorScaleElement(0.5, Qt::red));
	QCOMPARE(scale->stepCount(), 3);
}

void TestCCColorScale::removingAStepUpdatesTheColors() const
{
	ccColorScale::Shared scale = ccColorScale::Create("with a red step");
	scale->insert(ccColorScaleElement(0.0, Qt::black), false);
	scale->insert(ccColorScaleElement(0.5, Qt::red), false);
	scale->insert(ccColorScaleElement(1.0, Qt::black), true);

	CompareColors(*scale->getColorByRelativePos(0.5), ccColor::Rgb(255, 0, 0), 1);

	scale->remove(1);
	QCOMPARE(scale->stepCount(), 2);
	CompareColors(*scale->getColorByRelativePos(0.5), ccColor::black);
}

void TestCCColorScale::absoluteScaleConvertsValuesToRelativePositions() const
{
	ccColorScale::Shared scale = CreateGreyScale();
	scale->setAbsolute(10.0, 20.0);

	QVERIFY(!scale->isRelative());

	double minVal = 0.0;
	double maxVal = 0.0;
	scale->getAbsoluteBoundaries(minVal, maxVal);
	QCOMPARE(minVal, 10.0);
	QCOMPARE(maxVal, 20.0);

	QCOMPARE(scale->getRelativePosition(10.0), 0.0);
	QCOMPARE(scale->getRelativePosition(15.0), 0.5);
	QCOMPARE(scale->getRelativePosition(20.0), 1.0);

	CompareColors(*scale->getColorByValue(15.0), *scale->getColorByRelativePos(0.5));
	QVERIFY(scale->getColorByValue(9.99) == nullptr);
	QVERIFY(scale->getColorByValue(20.01) == nullptr);

	scale->setRelative();
	QVERIFY(scale->isRelative());
}

void TestCCColorScale::absoluteScaleWithNullRangeIsSafe() const
{
	ccColorScale::Shared scale = CreateGreyScale();
	scale->setAbsolute(5.0, 5.0);

	double minVal = 0.0;
	double maxVal = 0.0;
	scale->getAbsoluteBoundaries(minVal, maxVal);
	QCOMPARE(minVal, 5.0);
	QVERIFY(maxVal >= minVal);

	// no division by zero
	QVERIFY(std::isfinite(scale->getRelativePosition(5.0)));
}

void TestCCColorScale::copyPreservesTheScaleParameters() const
{
	ccColorScale::Shared scale = CreateGreyScale();
	scale->insert(ccColorScaleElement(0.5, Qt::red));
	scale->setAbsolute(-1.0, 1.0);
	scale->setLocked(true);

	ccColorScale::LabelSet labels;
	labels.insert(ccColorScale::Label(0.0, "zero"));
	scale->setCustomLabels(labels);

	ccColorScale::Shared copy = scale->copy();
	QVERIFY(copy);
	QVERIFY(copy->getUuid() != scale->getUuid());
	QCOMPARE(copy->getName(), scale->getName());
	QCOMPARE(copy->stepCount(), scale->stepCount());
	QCOMPARE(copy->isRelative(), scale->isRelative());
	QCOMPARE(copy->isLocked(), scale->isLocked());
	QCOMPARE(copy->customLabels().size(), static_cast<std::size_t>(1));

	double minVal = 0.0;
	double maxVal = 0.0;
	copy->getAbsoluteBoundaries(minVal, maxVal);
	QCOMPARE(minVal, -1.0);
	QCOMPARE(maxVal, 1.0);

	for (int i = 0; i < scale->stepCount(); ++i)
	{
		QCOMPARE(copy->step(i).getRelativePos(), scale->step(i).getRelativePos());
		QCOMPARE(copy->step(i).getColor(), scale->step(i).getColor());
	}

	// the copy has its own steps
	copy->setLocked(false);
	copy->remove(1);
	QCOMPARE(copy->stepCount(), 2);
	QCOMPARE(scale->stepCount(), 3);
}

void TestCCColorScale::customLabelsAreSortedAndUnique() const
{
	ccColorScale::LabelSet labels;
	labels.insert(ccColorScale::Label(2.0, "two"));
	labels.insert(ccColorScale::Label(1.0, "one"));
	labels.insert(ccColorScale::Label(1.0, "one again")); // same value: should be ignored

	QCOMPARE(labels.size(), static_cast<std::size_t>(2));
	QCOMPARE(labels.begin()->value, 1.0);
	QCOMPARE(labels.begin()->text, QString("one"));
	QCOMPARE(labels.rbegin()->value, 2.0);

	ccColorScale::Shared scale = CreateGreyScale();
	scale->setCustomLabels(labels);
	QCOMPARE(scale->customLabels().size(), static_cast<std::size_t>(2));

	scale->customLabels().clear();
	QVERIFY(scale->customLabels().empty());
}

void TestCCColorScale::xmlRoundTripPreservesTheScale() const
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());
	const QString filename = tempDir.filePath("scale.xml");

	ccColorScale::Shared scale = ccColorScale::Create("saved scale");
	scale->insert(ccColorScaleElement(0.0, Qt::black), false);
	scale->insert(ccColorScaleElement(0.25, QColor(10, 20, 30)), false);
	scale->insert(ccColorScaleElement(1.0, Qt::white), true);
	scale->setAbsolute(-12.5, 37.5);

	ccColorScale::LabelSet labels;
	labels.insert(ccColorScale::Label(-12.5, "min"));
	labels.insert(ccColorScale::Label(37.5, "max"));
	scale->setCustomLabels(labels);

	QVERIFY(scale->saveAsXML(filename));

	ccColorScale::Shared loadedScale = ccColorScale::LoadFromXML(filename);
	QVERIFY(loadedScale);
	QCOMPARE(loadedScale->getName(), scale->getName());
	QCOMPARE(loadedScale->getUuid(), scale->getUuid());
	QCOMPARE(loadedScale->isRelative(), false);
	QCOMPARE(loadedScale->stepCount(), scale->stepCount());

	double minVal = 0.0;
	double maxVal = 0.0;
	loadedScale->getAbsoluteBoundaries(minVal, maxVal);
	QCOMPARE(minVal, -12.5);
	QCOMPARE(maxVal, 37.5);

	for (int i = 0; i < scale->stepCount(); ++i)
	{
		QCOMPARE(loadedScale->step(i).getRelativePos(), scale->step(i).getRelativePos());
		QCOMPARE(loadedScale->step(i).getColor(), scale->step(i).getColor());
	}

	QCOMPARE(loadedScale->customLabels().size(), static_cast<std::size_t>(2));
	QCOMPARE(loadedScale->customLabels().begin()->value, -12.5);
	QCOMPARE(loadedScale->customLabels().begin()->text, QString("min"));
}

QTEST_APPLESS_MAIN(TestCCColorScale)
