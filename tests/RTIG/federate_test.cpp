#include <gtest/gtest.h>

#include <RTIG/Federate.hh>

#include <libCERTI/Exception.hh>

using certi::rtig::Federate;
using certi::RTIinternalError;

TEST(FederateTest, ConstructorThrowsOnNullHandle)
{
    ASSERT_THROW(Federate("", "", ::certi::HLA_1_3, 0), RTIinternalError);
}

// FIXME Possible bug here !!
// TEST(FederateTest, SetHandleAcceptsNullHandle)
// {
//     Federate f("test", "", ::certi::HLA_1_3, 1);
//     
//     ASSERT_EQ(1, f.getHandle());
//     
//     f.setHandle(0);
//     
//     ASSERT_EQ(0, f.getHandle());
// }

TEST(FederateTest, BasicSyncLabelManipulation)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);

    const std::string label{"firstLabel"};

    ASSERT_FALSE(f.hasSynchronizationLabel(label));

    f.addSynchronizationLabel(label);

    ASSERT_TRUE(f.hasSynchronizationLabel(label));

    f.removeSynchronizationLabel(label);

    ASSERT_FALSE(f.hasSynchronizationLabel(label));
}

TEST(FederateTest, CannotAddSameLabelTwice)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    const std::string label{"firstLabel"};
    
    f.addSynchronizationLabel(label);
    
    ASSERT_THROW(f.addSynchronizationLabel(label), RTIinternalError);
}

TEST(FederateTest, CannotRemoveUnknownLabel)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_THROW(f.removeSynchronizationLabel("label"), RTIinternalError);
}

TEST(FederateTest, HandleGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_EQ(1u, f.getHandle());
    
    f.setHandle(7);
    
    ASSERT_EQ(7u, f.getHandle());
    
    f.setHandle(1);
    
    ASSERT_EQ(1u, f.getHandle());
}

TEST(FederateTest, CannotSetNullHandle)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_THROW(f.setHandle(0), RTIinternalError);
    
    ASSERT_THROW(f.setHandle(1), RTIinternalError);
}

TEST(FederateTest, NameGetter)
{
    Federate f("name", "", ::certi::HLA_1_3, 1);
    
    ASSERT_EQ("name", f.getName());
}

TEST(FederateTest, ConstrainedGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_FALSE(f.isConstrained());
    
    f.setConstrained(true);
    
    ASSERT_TRUE(f.isConstrained());
    
    f.setConstrained(false);
    
    ASSERT_FALSE(f.isConstrained());
}

TEST(FederateTest, CannotSetConstrainedTwice)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    f.setConstrained(true);
    
    ASSERT_THROW(f.setConstrained(true), RTIinternalError);
    
    f.setConstrained(false);
    
    ASSERT_THROW(f.setConstrained(false), RTIinternalError);
}

TEST(FederateTest, RegulatorGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_FALSE(f.isRegulator());
    
    f.setRegulator(true);
    
    ASSERT_TRUE(f.isRegulator());
    
    f.setRegulator(false);
    
    ASSERT_FALSE(f.isRegulator());
}

TEST(FederateTest, CannotSetRegulatorTwice)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    f.setRegulator(true);
    
    ASSERT_THROW(f.setRegulator(true), RTIinternalError);
    
    f.setRegulator(false);
    
    ASSERT_THROW(f.setRegulator(false), RTIinternalError);
}

TEST(FederateTest, UsingNERxGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_FALSE(f.isUsingNERx());
    
    f.setIsUsingNERx(true);
    
    ASSERT_TRUE(f.isUsingNERx());
    
    f.setIsUsingNERx(false);
    
    ASSERT_FALSE(f.isUsingNERx());
}

TEST(FederateTest, LastNERxValueGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
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
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_TRUE(f.isClassRelevanceAdvisorySwitch());
    
    f.setClassRelevanceAdvisorySwitch(false);
    
    ASSERT_FALSE(f.isClassRelevanceAdvisorySwitch());
    
    f.setClassRelevanceAdvisorySwitch(true);
    
    ASSERT_TRUE(f.isClassRelevanceAdvisorySwitch());
}

TEST(FederateTest, CannotSetCRASTwice)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    f.setClassRelevanceAdvisorySwitch(false);
    
    ASSERT_THROW(f.setClassRelevanceAdvisorySwitch(false), RTIinternalError);
    
    f.setClassRelevanceAdvisorySwitch(true);
    
    ASSERT_THROW(f.setClassRelevanceAdvisorySwitch(true), RTIinternalError);
}

TEST(FederateTest, IRASGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_TRUE(f.isInteractionRelevanceAdvisorySwitch());
    
    f.setInteractionRelevanceAdvisorySwitch(false);
    
    ASSERT_FALSE(f.isInteractionRelevanceAdvisorySwitch());
    
    f.setInteractionRelevanceAdvisorySwitch(true);
    
    ASSERT_TRUE(f.isInteractionRelevanceAdvisorySwitch());
}

TEST(FederateTest, CannotSetIRASTwice)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    f.setInteractionRelevanceAdvisorySwitch(false);
    
    ASSERT_THROW(f.setInteractionRelevanceAdvisorySwitch(false), RTIinternalError);
    
    f.setInteractionRelevanceAdvisorySwitch(true);
    
    ASSERT_THROW(f.setInteractionRelevanceAdvisorySwitch(true), RTIinternalError);
}

TEST(FederateTest, ARASGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_FALSE(f.isAttributeRelevanceAdvisorySwitch());
    
    f.setAttributeRelevanceAdvisorySwitch(true);
    
    ASSERT_TRUE(f.isAttributeRelevanceAdvisorySwitch());
    
    f.setAttributeRelevanceAdvisorySwitch(false);
    
    ASSERT_FALSE(f.isAttributeRelevanceAdvisorySwitch());
}

TEST(FederateTest, CannotSetARASTwice)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    f.setAttributeRelevanceAdvisorySwitch(true);
    
    ASSERT_THROW(f.setAttributeRelevanceAdvisorySwitch(true), RTIinternalError);
    
    f.setAttributeRelevanceAdvisorySwitch(false);
    
    ASSERT_THROW(f.setAttributeRelevanceAdvisorySwitch(false), RTIinternalError);
}

TEST(FederateTest, ASASGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_FALSE(f.isAttributeScopeAdvisorySwitch());
    
    f.setAttributeScopeAdvisorySwitch(true);
    
    ASSERT_TRUE(f.isAttributeScopeAdvisorySwitch());
    
    f.setAttributeScopeAdvisorySwitch(false);
    
    ASSERT_FALSE(f.isAttributeScopeAdvisorySwitch());
}

TEST(FederateTest, CannotSetASASTwice)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    f.setAttributeScopeAdvisorySwitch(true);
    
    ASSERT_THROW(f.setAttributeScopeAdvisorySwitch(true), RTIinternalError);
    
    f.setAttributeScopeAdvisorySwitch(false);
    
    ASSERT_THROW(f.setAttributeScopeAdvisorySwitch(false), RTIinternalError);
}

TEST(FederateTest, SavingGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_FALSE(f.isSaving());
    
    f.setSaving(true);
    
    ASSERT_TRUE(f.isSaving());
    
    f.setSaving(false);
    
    ASSERT_FALSE(f.isSaving());
}

TEST(FederateTest, RestoringGetterSetter)
{
    Federate f("test", "", ::certi::HLA_1_3, 1);
    
    ASSERT_FALSE(f.isRestoring());
    
    f.setRestoring(true);
    
    ASSERT_TRUE(f.isRestoring());
    
    f.setRestoring(false);
    
    ASSERT_FALSE(f.isRestoring());
}
