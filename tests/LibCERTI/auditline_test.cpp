#include <gtest/gtest.h>

#include <ctime>
#include <sstream>
#include <string>

#include "libCERTI/AuditLine.hh"

using ::certi::AuditLine;

TEST(AuditLineTest, CtorDefaultsToAllZeros)
{
    AuditLine l;
    auto time_at_construction = std::to_string(std::time(nullptr));
    
    std::stringstream ss;
    l.write(ss);
    
    EXPECT_EQ(time_at_construction + "\t0\t0\t0\t0\t0\t\n",
              ss.str());
}

/** Ensure the output follows the legend :
 * 
 * <ul>
 *  <li>date</li>
 *  <li>federation</li>
 *  <li>federate</li>
 *  <li>type</li>
 *  <li>level</li>
 *  <li>status</li>
 *  <li>comment</li>
 * </ul>
 */
TEST(AuditLineTest, OutputIsInTheRightOrder)
{
    AuditLine l {
        AuditLine::Type(3),
        AuditLine::Level(4),
        AuditLine::Status(static_cast<::certi::Exception::Type>(5)),
        "reason"
    };
    auto time_at_construction = std::to_string(std::time(nullptr));
    
    l.setFederation(1);
    l.setFederate(2);
    
    std::stringstream ss;
    l.write(ss);
    
    ASSERT_EQ(std::to_string(std::time(nullptr)) + "\t1\t2\t3\t4\t5\treason\n",
              ss.str());
}

TEST(AuditLineTest, SettersImpactOutput)
{
    AuditLine l;
    auto time_at_construction = std::to_string(std::time(nullptr));
    
    auto level = AuditLine::Level(1);
    
    l.addComment("reason");
    l.setLevel(level);
    
    l.setFederation(2);
    l.setFederate(3);
    
    l.end(AuditLine::Status(static_cast<::certi::Exception::Type>(4)), "end");
    
    std::stringstream ss;
    l.write(ss);
    
    ASSERT_EQ(level.get(), l.getLevel().get());
    ASSERT_EQ(AuditLine::Status(static_cast<::certi::Exception::Type>(4)).get(), l.getStatus().get());
    
    EXPECT_EQ(time_at_construction + "\t2\t3\t0\t1\t4\treasonend\n",
              ss.str());
}
