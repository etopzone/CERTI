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

// FIXME Possible bug here !!
// TEST(FederateTest, SetHandleAcceptsNullHandle)
// {
//     Federate f("test", 1);
//     
//     ASSERT_EQ(1, f.getHandle());
//     
//     f.setHandle(0);
//     
//     ASSERT_EQ(0, f.getHandle());
// }

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

TEST(FederateTest, HandleGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_EQ(1, f.getHandle());
    
    f.new_setHandle(7);
    
    ASSERT_EQ(7, f.getHandle());
    
    f.new_setHandle(1);
    
    ASSERT_EQ(1, f.getHandle());
}

TEST(FederateTest, CannotSetNullHandle)
{
    Federate f("test", 1);
    
    ASSERT_THROW(f.new_setHandle(0), RTIinternalError);
    
    ASSERT_THROW(f.new_setHandle(1), RTIinternalError);
}

TEST(FederateTest, NameGetter)
{
    Federate f("name", 1);
    
    ASSERT_EQ("name", f.getName());
}

TEST(FederateTest, ConstrainedGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_FALSE(f.isConstrained());
    
    f.new_setConstrained(true);
    
    ASSERT_TRUE(f.isConstrained());
    
    f.new_setConstrained(false);
    
    ASSERT_FALSE(f.isConstrained());
}

TEST(FederateTest, CannotSetConstrainedTwice)
{
    Federate f("test", 1);
    
    f.new_setConstrained(true);
    
    ASSERT_THROW(f.new_setConstrained(true), RTIinternalError);
    
    f.new_setConstrained(false);
    
    ASSERT_THROW(f.new_setConstrained(false), RTIinternalError);
}

TEST(FederateTest, RegulatorGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_FALSE(f.isRegulator());
    
    f.new_setRegulator(true);
    
    ASSERT_TRUE(f.isRegulator());
    
    f.new_setRegulator(false);
    
    ASSERT_FALSE(f.isRegulator());
}

TEST(FederateTest, CannotSetRegulatorTwice)
{
    Federate f("test", 1);
    
    f.new_setRegulator(true);
    
    ASSERT_THROW(f.new_setRegulator(true), RTIinternalError);
    
    f.new_setRegulator(false);
    
    ASSERT_THROW(f.new_setRegulator(false), RTIinternalError);
}

TEST(FederateTest, UsingNERxGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_FALSE(f.isUsingNERx());
    
    f.setIsUsingNERx(true);
    
    ASSERT_TRUE(f.isUsingNERx());
    
    f.setIsUsingNERx(false);
    
    ASSERT_FALSE(f.isUsingNERx());
}

TEST(FederateTest, LastNERxValueGetterSetter)
{
    Federate f("test", 1);
    
    ::certi::FederationTime baseValue{};
    
    ASSERT_EQ(baseValue, f.getLastNERxValue());
    
    ::certi::FederationTime federation_time{0.1};
    
    f.setLastNERxValue(federation_time);
    
    ASSERT_EQ(federation_time, f.getLastNERxValue());
    
    f.setLastNERxValue(baseValue);
    
    ASSERT_EQ(baseValue, f.getLastNERxValue());
}

TEST(FederateTest, CRASGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_TRUE(f.isClassRelevanceAdvisorySwitch());
    
    f.new_setClassRelevanceAdvisorySwitch(false);
    
    ASSERT_FALSE(f.isClassRelevanceAdvisorySwitch());
    
    f.new_setClassRelevanceAdvisorySwitch(true);
    
    ASSERT_TRUE(f.isClassRelevanceAdvisorySwitch());
}

TEST(FederateTest, CannotSetCRASTwice)
{
    Federate f("test", 1);
    
    f.new_setClassRelevanceAdvisorySwitch(false);
    
    ASSERT_THROW(f.new_setClassRelevanceAdvisorySwitch(false), RTIinternalError);
    
    f.new_setClassRelevanceAdvisorySwitch(true);
    
    ASSERT_THROW(f.new_setClassRelevanceAdvisorySwitch(true), RTIinternalError);
}

TEST(FederateTest, IRASGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_TRUE(f.isInteractionRelevanceAdvisorySwitch());
    
    f.new_setInteractionRelevanceAdvisorySwitch(false);
    
    ASSERT_FALSE(f.isInteractionRelevanceAdvisorySwitch());
    
    f.new_setInteractionRelevanceAdvisorySwitch(true);
    
    ASSERT_TRUE(f.isInteractionRelevanceAdvisorySwitch());
}

TEST(FederateTest, CannotSetIRASTwice)
{
    Federate f("test", 1);
    
    f.new_setInteractionRelevanceAdvisorySwitch(false);
    
    ASSERT_THROW(f.new_setInteractionRelevanceAdvisorySwitch(false), RTIinternalError);
    
    f.new_setInteractionRelevanceAdvisorySwitch(true);
    
    ASSERT_THROW(f.new_setInteractionRelevanceAdvisorySwitch(true), RTIinternalError);
}

TEST(FederateTest, ARASGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_FALSE(f.isAttributeRelevanceAdvisorySwitch());
    
    f.new_setAttributeRelevanceAdvisorySwitch(true);
    
    ASSERT_TRUE(f.isAttributeRelevanceAdvisorySwitch());
    
    f.new_setAttributeRelevanceAdvisorySwitch(false);
    
    ASSERT_FALSE(f.isAttributeRelevanceAdvisorySwitch());
}

TEST(FederateTest, CannotSetARASTwice)
{
    Federate f("test", 1);
    
    f.new_setAttributeRelevanceAdvisorySwitch(true);
    
    ASSERT_THROW(f.new_setAttributeRelevanceAdvisorySwitch(true), RTIinternalError);
    
    f.new_setAttributeRelevanceAdvisorySwitch(false);
    
    ASSERT_THROW(f.new_setAttributeRelevanceAdvisorySwitch(false), RTIinternalError);
}

TEST(FederateTest, ASASGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_FALSE(f.isAttributeScopeAdvisorySwitch());
    
    f.new_setAttributeScopeAdvisorySwitch(true);
    
    ASSERT_TRUE(f.isAttributeScopeAdvisorySwitch());
    
    f.new_setAttributeScopeAdvisorySwitch(false);
    
    ASSERT_FALSE(f.isAttributeScopeAdvisorySwitch());
}

TEST(FederateTest, CannotSetASASTwice)
{
    Federate f("test", 1);
    
    f.new_setAttributeScopeAdvisorySwitch(true);
    
    ASSERT_THROW(f.new_setAttributeScopeAdvisorySwitch(true), RTIinternalError);
    
    f.new_setAttributeScopeAdvisorySwitch(false);
    
    ASSERT_THROW(f.new_setAttributeScopeAdvisorySwitch(false), RTIinternalError);
}

TEST(FederateTest, SavingGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_FALSE(f.isSaving());
    
    f.setSaving(true);
    
    ASSERT_TRUE(f.isSaving());
    
    f.setSaving(false);
    
    ASSERT_FALSE(f.isSaving());
}

TEST(FederateTest, RestoringGetterSetter)
{
    Federate f("test", 1);
    
    ASSERT_FALSE(f.isRestoring());
    
    f.setRestoring(true);
    
    ASSERT_TRUE(f.isRestoring());
    
    f.setRestoring(false);
    
    ASSERT_FALSE(f.isRestoring());
}
