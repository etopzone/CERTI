#include <gtest/gtest.h>

#include <iostream>
#include <limits>

#include <RTIG/Federate.hh>

using certi::rtig::Federate;
using certi::RTIinternalError;

TEST(FederateTest, ConstructorThrowsOnNullHandle)
{
    ASSERT_THROW(Federate("", 0), RTIinternalError);
}

TEST(FederateTest, BasicSyncLabelManipulation)
{
    Federate f("test", 1);

    const std::string label{"firstLabel"};

    ASSERT_FALSE(f.isSynchronizationLabel(label));

    f.addSynchronizationLabel(label);

    ASSERT_TRUE(f.isSynchronizationLabel(label));

    f.removeSynchronizationLabel(label);

    ASSERT_FALSE(f.isSynchronizationLabel(label));
}

TEST(FederateTest, CannotAddSameLabelTwice)
{
    Federate f("test", 1);
    
    const std::string label{"firstLabel"};
    
    f.addSynchronizationLabel(label);
    
    ASSERT_THROW(f.addSynchronizationLabel(label), RTIinternalError);
}

TEST(FederateTest, CannotRemoveUnknownLabel)
{
    Federate f("test", 1);
    
    ASSERT_THROW(f.removeSynchronizationLabel("label"), RTIinternalError);
}
