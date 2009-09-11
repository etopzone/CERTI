//
// CERTI message description spec test file
// 
package my.hierarchy.pack

//
//  This is a multiline
//  comment which may contains markup
//  @desc here comes the EType
//
enum EType {
   FIRST_VAL = 0, // one may specify the value of the first enum symbol
   REAL_VAL1    , //! the real value 1
   REAL_VAL2    ,
   UNUSED
}

message MyMessage {} 

// blah
native ANativeType {
    language CXX [typedef int ANativeType]
}

native AnotherNativeType {
     language CXX [#include "AnotherNativeType.hh"]
}

native MessageType2 {
     language CXX []
}

message MyMessage2 { 
    repeated int32 whatever
}

message MyOtherMessage : merge MyMessage {
    required bool              constrained  // constrain 
	optional MessageType2       mytype
	required uint32            a { default=1 }
}
