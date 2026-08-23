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

#include "TestCCScalarField.h"

// qCC_db
#include <ccScalarField.h>

// CCCoreLib
#include <CCConst.h>

// system
#include <cmath>
#include <limits>

//! Grey color scale (black to white), which makes the expected colors easy to predict
static ccColorScale::Shared CreateGreyScale()
{
	ccColorScale::Shared scale = ccColorScale::Create("grey");
	scale->insert(ccColorScaleElement(0.0, Qt::black), false);
	scale->insert(ccColorScaleElement(1.0, Qt::white), true);

	return scale;
}

//! Creates a scalar field with the input values (and computes its min and max)
static ccScalarField* CreateScalarField(const std::vector<ScalarType>& values)
{
	ccScalarField* sf = new ccScalarField("test");
	for (ScalarType value : values)
	{
		sf->addElement(value);
	}
	sf->computeMinAndMax();

	return sf;
}

static void CompareColors(const ccColor::Rgb& actual, const ccColor::Rgb& expected, int tolerance = 0)
{
	QVERIFY(std::abs(static_cast<int>(actual.r) - static_cast<int>(expected.r)) <= tolerance);
	QVERIFY(std::abs(static_cast<int>(actual.g) - static_cast<int>(expected.g)) <= tolerance);
	QVERIFY(std::abs(static_cast<int>(actual.b) - static_cast<int>(expected.b)) <= tolerance);
}

void TestCCScalarField::rangeBoundsResetTheStartAndStopValues() const
{
	ccScalarField::Range range;
	range.setBounds(-5, 5);

	QCOMPARE(range.min(), -5.0f);
	QCOMPARE(range.max(), 5.0f);
	QCOMPARE(range.start(), -5.0f);
	QCOMPARE(range.stop(), 5.0f);
	QCOMPARE(range.range(), 10.0f);
	QCOMPARE(range.maxRange(), 10.0f);
}

void TestCCScalarField::rangeBoundsCanPreserveTheStartAndStopValues() const
{
	ccScalarField::Range range;
	range.setBounds(0, 10);
	range.setStart(2);
	range.setStop(8);

	// the start and stop values are kept, but clipped to the new boundaries
	range.setBounds(3, 7, false);

	QCOMPARE(range.start(), 3.0f);
	QCOMPARE(range.stop(), 7.0f);
	QCOMPARE(range.range(), 4.0f);
}

void TestCCScalarField::rangeStartAndStopStayInboundAndOrdered() const
{
	ccScalarField::Range range;
	range.setBounds(0, 10);

	range.setStop(5);

	// a start value above the maximum pushes the stop value
	range.setStart(20);
	QCOMPARE(range.start(), 10.0f);
	QCOMPARE(range.stop(), 10.0f);

	// a stop value below the minimum pulls the start value
	range.setStop(-5);
	QCOMPARE(range.stop(), 0.0f);
	QCOMPARE(range.start(), 0.0f);
}

void TestCCScalarField::rangeIsNeverNull() const
{
	ccScalarField::Range range;
	QVERIFY(range.range() > 0);

	// a flat range still has a strictly positive 'range'
	range.setBounds(1, 1);
	QCOMPARE(range.maxRange(), 0.0f);
	QVERIFY(range.range() > 0);
}

void TestCCScalarField::rangeTestsInboundAndInRangeValues() const
{
	ccScalarField::Range range;
	range.setBounds(0, 10);
	range.setStart(2);
	range.setStop(8);

	QVERIFY(range.isInbound(0));
	QVERIFY(range.isInbound(10));
	QVERIFY(!range.isInbound(-0.001f));
	QVERIFY(!range.isInbound(10.001f));

	QVERIFY(range.isInRange(2));
	QVERIFY(range.isInRange(5));
	QVERIFY(range.isInRange(8));
	QVERIFY(!range.isInRange(1));
	QVERIFY(!range.isInRange(9));

	// NaN values are never in range
	QVERIFY(!range.isInRange(std::numeric_limits<ScalarType>::quiet_NaN()));
}

void TestCCScalarField::computeMinAndMaxUpdatesTheRanges() const
{
	ccScalarField* sf = CreateScalarField({1, 2, 3, 4, 5});

	QCOMPARE(sf->currentSize(), 5u);
	QCOMPARE(sf->getMin(), 1.0f);
	QCOMPARE(sf->getMax(), 5.0f);

	QCOMPARE(sf->displayRange().min(), 1.0f);
	QCOMPARE(sf->displayRange().max(), 5.0f);
	QCOMPARE(sf->displayRange().start(), 1.0f);
	QCOMPARE(sf->displayRange().stop(), 5.0f);

	// with the default (relative) color scale, the saturation range matches the SF boundaries
	QCOMPARE(sf->saturationRange().min(), 1.0f);
	QCOMPARE(sf->saturationRange().max(), 5.0f);

	// the histogram should account for all the (valid) values
	const ccScalarField::Histogram& histogram = sf->getHistogram();
	QVERIFY(!histogram.empty());
	unsigned sum = 0;
	for (unsigned binSize : histogram)
	{
		sum += binSize;
	}
	QCOMPARE(sum, 5u);
	QVERIFY(histogram.maxValue > 0);

	sf->release();
}

void TestCCScalarField::computeMinAndMaxIgnoresNaNValues() const
{
	ccScalarField* sf = CreateScalarField({3, ccScalarField::NaN(), 1, 5, ccScalarField::NaN()});

	QCOMPARE(sf->currentSize(), 5u);
	QCOMPARE(sf->countValidValues(), static_cast<std::size_t>(3));
	QCOMPARE(sf->getMin(), 1.0f);
	QCOMPARE(sf->getMax(), 5.0f);

	// NaN values are not part of the histogram
	unsigned sum = 0;
	for (unsigned binSize : sf->getHistogram())
	{
		sum += binSize;
	}
	QCOMPARE(sum, 3u);

	sf->release();
}

void TestCCScalarField::displayedRangeIsClamped() const
{
	ccScalarField* sf = CreateScalarField({0, 10});

	sf->setMinDisplayed(-100);
	QCOMPARE(sf->displayRange().start(), 0.0f);

	sf->setMaxDisplayed(100);
	QCOMPARE(sf->displayRange().stop(), 10.0f);

	sf->setMinDisplayed(4);
	sf->setMaxDisplayed(6);
	QCOMPARE(sf->displayRange().start(), 4.0f);
	QCOMPARE(sf->displayRange().stop(), 6.0f);
	QCOMPARE(sf->displayRange().range(), 2.0f);

	sf->release();
}

void TestCCScalarField::saturationRangeIsClamped() const
{
	ccScalarField* sf = CreateScalarField({0, 10});

	sf->setSaturationStart(-100);
	QCOMPARE(sf->saturationRange().start(), 0.0f);

	sf->setSaturationStop(100);
	QCOMPARE(sf->saturationRange().stop(), 10.0f);

	sf->setSaturationStart(2);
	sf->setSaturationStop(8);
	QCOMPARE(sf->saturationRange().start(), 2.0f);
	QCOMPARE(sf->saturationRange().stop(), 8.0f);

	sf->release();
}

void TestCCScalarField::symmetricalScaleUsesAbsoluteValues() const
{
	ccScalarField* sf = CreateScalarField({-10, -3, 0, 4});
	sf->setColorScale(CreateGreyScale());
	sf->setColorRampSteps(ccColorScale::MAX_STEPS);
	sf->setSymmetricalScale(true);

	// the saturation range is now defined by the absolute values
	QCOMPARE(sf->saturationRange().min(), 0.0f);
	QCOMPARE(sf->saturationRange().max(), 10.0f);

	// zero is at the middle of a symmetrical scale
	CompareColors(*sf->getColor(0), ccColor::Rgb(127, 127, 127), 4);
	// the most negative value is at the very beginning of the scale
	CompareColors(*sf->getColor(-10), ccColor::black);

	sf->release();
}

void TestCCScalarField::logScaleHasItsOwnSaturationRange() const
{
	ccScalarField* sf = CreateScalarField({1, 10, 1000});

	// the log saturation range is always maintained
	QCOMPARE(sf->logSaturationRange().min(), 0.0f);
	QCOMPARE(sf->logSaturationRange().max(), 3.0f);
	QCOMPARE(sf->saturationRange().min(), 1.0f);

	// once the log scale is enabled, 'saturationRange' returns the log one
	sf->setLogScale(true);
	QVERIFY(sf->logScale());
	QCOMPARE(sf->saturationRange().min(), 0.0f);
	QCOMPARE(sf->saturationRange().max(), 3.0f);

	// and the saturation setters apply to the log range
	sf->setSaturationStart(1);
	QCOMPARE(sf->logSaturationRange().start(), 1.0f);

	sf->setLogScale(false);
	QCOMPARE(sf->saturationRange().start(), 1.0f);

	sf->release();
}

void TestCCScalarField::colorsAreSpreadOverTheSaturationRange() const
{
	ccScalarField* sf = CreateScalarField({0, 10});
	sf->setColorScale(CreateGreyScale());
	sf->setColorRampSteps(ccColorScale::MAX_STEPS);

	CompareColors(*sf->getColor(0), ccColor::black);
	CompareColors(*sf->getColor(10), ccColor::white, 4);
	CompareColors(*sf->getColor(5), ccColor::Rgb(127, 127, 127), 4);

	// values below/above the saturation range are saturated
	sf->setSaturationStart(2);
	sf->setSaturationStop(8);
	CompareColors(*sf->getColor(1), ccColor::black);
	CompareColors(*sf->getColor(9), ccColor::white, 4);
	CompareColors(*sf->getColor(5), ccColor::Rgb(127, 127, 127), 4);

	sf->release();
}

void TestCCScalarField::hiddenValuesDependOnTheDisplayRange() const
{
	ccScalarField* sf = CreateScalarField({0, 10});
	sf->setColorScale(CreateGreyScale());
	sf->setMinDisplayed(2);
	sf->setMaxDisplayed(8);

	// by default the values outside of the displayed range are shown in grey
	QVERIFY(sf->areNaNValuesShownInGrey());
	CompareColors(*sf->getColor(0), ccColor::lightGreyRGB);
	QVERIFY(!sf->mayHaveHiddenValues());

	// otherwise they are simply hidden (i.e. no color at all)
	sf->showNaNValuesInGrey(false);
	QVERIFY(sf->getColor(0) == nullptr);
	QVERIFY(sf->getColor(ccScalarField::NaN()) == nullptr);
	QVERIFY(sf->mayHaveHiddenValues());

	// values inside the displayed range are always visible
	QVERIFY(sf->getColor(5) != nullptr);

	sf->release();
}

void TestCCScalarField::parametersCanBeImported() const
{
	ccScalarField* source = CreateScalarField({0, 10});
	source->setColorScale(CreateGreyScale());
	source->setColorRampSteps(64);
	source->showNaNValuesInGrey(false);
	source->alwaysShowZero(true);
	source->setMinDisplayed(2);
	source->setMaxDisplayed(8);
	source->setSaturationStart(3);
	source->setSaturationStop(7);

	ccScalarField* target = CreateScalarField({0, 10});
	target->importParametersFrom(source);

	QCOMPARE(target->getColorScale(), source->getColorScale());
	QCOMPARE(target->getColorRampSteps(), 64u);
	QCOMPARE(target->areNaNValuesShownInGrey(), false);
	QCOMPARE(target->isZeroAlwaysShown(), true);
	QCOMPARE(target->logScale(), false);
	QCOMPARE(target->symmetricalScale(), false);
	QCOMPARE(target->displayRange().start(), 2.0f);
	QCOMPARE(target->displayRange().stop(), 8.0f);
	QCOMPARE(target->saturationRange().start(), 3.0f);
	QCOMPARE(target->saturationRange().stop(), 7.0f);

	// but the values themselves are left untouched
	QCOMPARE(target->currentSize(), 2u);

	source->release();
	target->release();
}

void TestCCScalarField::copyPreservesValuesAndParameters() const
{
	ccScalarField* source = CreateScalarField({0, 5, 10});
	source->setColorScale(CreateGreyScale());
	source->setColorRampSteps(32);
	source->setMinDisplayed(1);
	source->setMaxDisplayed(9);

	ccScalarField* copy = new ccScalarField(*source);

	QCOMPARE(copy->getName(), source->getName());
	QCOMPARE(copy->currentSize(), source->currentSize());
	for (unsigned i = 0; i < source->currentSize(); ++i)
	{
		QCOMPARE(copy->getValue(i), source->getValue(i));
	}

	QCOMPARE(copy->getColorScale(), source->getColorScale());
	QCOMPARE(copy->getColorRampSteps(), 32u);
	QCOMPARE(copy->displayRange().start(), 1.0f);
	QCOMPARE(copy->displayRange().stop(), 9.0f);

	source->release();
	copy->release();
}

QTEST_MAIN(TestCCScalarField)
